#include <iostream>
#include <string>

#include <csignal>

#include "PathPlanning.h"
#include "core/include/Logger.hpp"


using namespace boost::posix_time;
using namespace std;

ACTIVITY_REGISTER (PathPlanning);

ACTIVITY_START
/*
 *
 * check for move grid!!!!changed the front expansion
 *
 *
 * */

/*********** Math functions ***********/
using namespace KMath;

PathPlanning::PathPlanning(Blackboard &b) :
	IActivity(b), vprof("PathPlanning")
{
	;
}

void PathPlanning::UserInit() {
	_blk.updateSubscription ("sensors", msgentry::SUBSCRIBE_ON_TOPIC);
	_blk.updateSubscription ("pathplanning", msgentry::SUBSCRIBE_ON_TOPIC);
	
	//Setup path map
	int radiusRings = atoi(Configurator::Instance().findValueForKey("pathPlanningConfig.GridRingsInRadius").c_str());
	int ringCells = atoi(Configurator::Instance().findValueForKey("pathPlanningConfig.GridRingsInRadius").c_str());
	float realMetters = atof(Configurator::Instance().findValueForKey("pathPlanningConfig.GridRadiusInMeters").c_str());
	pathMap.setupGrid (radiusRings, ringCells, realMetters);
	
	//Setup small map
	float smallRealMetters = atof(Configurator::Instance().findValueForKey("pathPlanningConfig.SmallGridRadiusInMeters").c_str());
	smallPathMap.setupGrid (radiusRings, ringCells, smallRealMetters);
	
	//Setup sonars
	leftSonars = new float[KDeviceLists::US_SIZE];
	rightSonars = new float[KDeviceLists::US_SIZE];
	
	currentTime = boost::posix_time::microsec_clock::universal_time();

	//Initialize astar properties
	aStarDirections = atoi(Configurator::Instance().findValueForKey("pathPlanningConfig.AStarDirections").c_str());
	aStarTransformation = 2 * M_PI / (float) aStarDirections;
	aStarCircleCells = pathMap.getRingCells();
	aStarRadiusCells = pathMap.getRadiusCells();
	//Allocate the 3d arrays for astar
	currentValues.setup (aStarDirections, pathMap.getRadiusCells(), pathMap.getRingCells());
	directions.setup (aStarDirections, pathMap.getRadiusCells(), pathMap.getRingCells());
	openNodes.reserve (atoi(Configurator::Instance().findValueForKey("pathPlanningConfig.AStarQueueReservation").c_str())*sizeof(node*));
	pathFromAStar.reserve(atoi(Configurator::Instance().findValueForKey("pathPlanningConfig.AStarPathVectorReservation").c_str())*sizeof(coords));

	Reset();
	LogEntry(LogLevel::Info, GetName()) << "PathPlanning Initialized";
}

void PathPlanning::Reset() {
	hasTarget = false;	
	firstOdometryData = true;
	
	gameMode = atoi(Configurator::Instance().findValueForKey("teamConfig.game_mode").c_str()) == 1 ? true : false;
	if(!gameMode){	
		for (int i = 0; i < pathMap.getRingCells(); i++){
			for (int j = 0; j < pathMap.getRadiusCells(); j++) {
				gridInfoMessage.add_gridcells(0.0f);
			}
		}
	}
	firstTime = true;
	SonarsMinDist = atof(Configurator::Instance().findValueForKey("pathPlanningConfig.SonarsMinDist").c_str());
	SonarsMaxDist = atof(Configurator::Instance().findValueForKey("pathPlanningConfig.SonarsMaxDist").c_str());
	SonarsConeInDegs = atof(Configurator::Instance().findValueForKey("pathPlanningConfig.SonarsConeInDegs").c_str());
	updateObstacles = atoi(Configurator::Instance().findValueForKey("pathPlanningConfig.ObstaclesUpdate").c_str()) == 1? true : false;
	checkForTargetInObstacle = atoi(Configurator::Instance().findValueForKey("pathPlanningConfig.CheckForTargetInObstacle").c_str()) == 1? true : false;
}

int PathPlanning::Execute() {

	//update the grid with the new sonar values
	while(currentTime < boost::posix_time::microsec_clock::universal_time()){
		currentTime = currentTime + boost::posix_time::milliseconds(100);
		rpm = _blk.readData<RobotPositionMessage> ("sensors", msgentry::HOST_ID_LOCAL_HOST, NULL, &currentTime);
		processOdometryData();
		pathMap.updateCells();
		allsm =  _blk.readData<AllSensorValuesMessage> ("sensors", msgentry::HOST_ID_LOCAL_HOST, NULL, &currentTime);

		if (updateObstacles && allsm != 0) {
			for (int j = 0; j < KDeviceLists::US_SIZE; j++) {
				leftSonars[j] = allsm->sensordata (KDeviceLists::L_US + j).sensorvalue();
				rightSonars[j] = allsm->sensordata (KDeviceLists::R_US + j).sensorvalue();
			}
			//printSonarValues();
			if (leftSonars[0] < SonarsMinDist) {
				pathMap.updateGrid (0.15f, 0.4363f, KMath::TO_RAD (SonarsConeInDegs), false);
			} else if (leftSonars[0] > SonarsMaxDist) {
				pathMap.softUpdateGrid(2.0f, 0.4363f, KMath::TO_RAD(SonarsConeInDegs), false);
			} else {
				pathMap.updateGrid (leftSonars[0], 0.4363f, KMath::TO_RAD (SonarsConeInDegs), false);
			}

			if (rightSonars[0] < SonarsMinDist) {
				pathMap.updateGrid (0.15f, KMath::wrapTo0_2Pi (-0.4363f), KMath::TO_RAD (SonarsConeInDegs), false);
			} else if (rightSonars[0] > SonarsMaxDist) {
				pathMap.softUpdateGrid(2.0f, KMath::wrapTo0_2Pi(-0.4363f), KMath::TO_RAD(SonarsConeInDegs), false);
			} else {
				pathMap.updateGrid (rightSonars[0], KMath::wrapTo0_2Pi (-0.4363f), KMath::TO_RAD (SonarsConeInDegs), false);
			}
		}
		
		
	}
	
	pprm = _blk.readSignal<PathPlanningRequestMessage> ("pathplanning");
	

	if(pprm != 0){
		aStarUseSmallGrid = pprm->forceuseofsmallmap();
		if(fabs(pprm->targetx()) <= smallPathMap.getRealMetters() && fabs(pprm->targety()) <= smallPathMap.getRealMetters()){
			aStarUseSmallGrid = true;
		}else{
			aStarUseSmallGrid = false;
		}
		
		if(pprm->targetx()==0 && pprm->targety()==0){
			aStarTargetR = 255;
			aStarTargetC = 255;
		}else if(!aStarUseSmallGrid){
			aStarTargetR = (KMath::toPolarD(pprm->targetx(), pprm->targety())*pathMap.getRadiusCells())/pathMap.getRealMetters() - 1;
			if(aStarTargetR >= pathMap.getRadiusCells()){
				aStarTargetR = pathMap.getRadiusCells()-1;
			}
			aStarTargetC = (KMath::wrapTo0_2Pi(KMath::toPolarT(pprm->targetx(), pprm->targety()))*pathMap.getRingCells())/(2*M_PI);
		}else{
			aStarTargetR = (KMath::toPolarD(pprm->targetx(), pprm->targety())*smallPathMap.getRadiusCells())/smallPathMap.getRealMetters() - 1;
			if(aStarTargetR >= smallPathMap.getRadiusCells()){
				aStarTargetR = smallPathMap.getRadiusCells()-1;
			}
			aStarTargetC = (KMath::wrapTo0_2Pi(KMath::toPolarT(pprm->targetx(), pprm->targety()))*smallPathMap.getRingCells())/(2*M_PI);
		}
		aStarTargetZ = KMath::wrapTo0_2Pi(pprm->targetorientation())/aStarTransformation;
		hasTarget = true;

		aStar();	
		commitMovement();
	}else{
		hasTarget = false;
	}
	
	if(!gameMode){
		publishGridInfo();
	}
#ifdef KPROFILING_ENABLED
	vprof.generate_report(10);
#endif

	return 0;
}

void PathPlanning::processOdometryData(){
	if(rpm != 0){
		newOdometryX = rpm->sensordata(KDeviceLists::ROBOT_X).sensorvalue();
		newOdometryY = rpm->sensordata(KDeviceLists::ROBOT_Y).sensorvalue();
		newOdometryZ = rpm->sensordata(KDeviceLists::ROBOT_ANGLE).sensorvalue();
		if (firstOdometryData){
			oldOdometryX = newOdometryX;
			oldOdometryY = newOdometryX;
			oldOdometryZ = newOdometryZ;
		}else{
			float diffX = newOdometryX - oldOdometryX;
			float diffY = newOdometryY - oldOdometryY;
			float diffZ = anglediff(newOdometryZ, oldOdometryZ);
			pathMap.translateGrid(diffX,diffY);
			pathMap.rotateGrid(diffZ);
			smallPathMap.translateGrid(diffX,diffY);
			smallPathMap.rotateGrid(diffZ);
		}
	}
}

void PathPlanning::printSonarValues() {
	//for(int i = 0; i < KDeviceLists::US_SIZE; i++)
	LogEntry(LogLevel::Info, GetName()) << "leftSensor: " << leftSonars[0] << " rightSensor: " << rightSonars[0];
}

void PathPlanning::publishGridInfo() {
	if(!aStarUseSmallGrid){
		gridInfoMessage.set_cellsradius (pathMap.getRadiusCells() );
		gridInfoMessage.set_cellsring (pathMap.getRingCells() );
		gridInfoMessage.set_realgridlength (pathMap.getRealMetters() );
	}else{
		gridInfoMessage.set_cellsradius (smallPathMap.getRadiusCells() );
		gridInfoMessage.set_cellsring (smallPathMap.getRingCells() );
		gridInfoMessage.set_realgridlength (smallPathMap.getRealMetters() );
	}
	gridInfoMessage.set_usingsmallmap(aStarUseSmallGrid);
	
	if(hasTarget){
		gridInfoMessage.set_pathlength (pathFromAStar.size() );
	}else{
		gridInfoMessage.set_pathlength (0);
	}

	gridInfoMessage.clear_pathstepsring();
	gridInfoMessage.clear_pathstepssector();
	gridInfoMessage.clear_pathstepsorientation();

	gridInfoMessage.clear_targetring();
	gridInfoMessage.clear_targetsector();
	gridInfoMessage.clear_targetorientation();

	if(!aStarUseSmallGrid){
		for (int i = 0; i < pathMap.getRadiusCells(); i++){
			for (int j = 0; j < pathMap.getRingCells(); j++) {
				gridInfoMessage.set_gridcells (i * pathMap.getRingCells() + j, (pathMap (i, j) + 1) / 2);
			}
		}
	}else{
		for (int i = 0; i < smallPathMap.getRadiusCells(); i++){
			for (int j = 0; j < smallPathMap.getRingCells(); j++) {
				gridInfoMessage.set_gridcells (i * smallPathMap.getRingCells() + j, (smallPathMap (i, j) + 1) / 2);
			}
		}
	}

	if(hasTarget){
		gridInfoMessage.set_targetring (aStarTargetR);
		gridInfoMessage.set_targetsector (aStarTargetC);
		gridInfoMessage.set_targetorientation (aStarTargetZ * aStarTransformation);
		int i=0;
		for(unsigned int j = 0; j<pathFromAStar.size(); j++){
			gridInfoMessage.add_pathstepsring (0.0f);
			gridInfoMessage.add_pathstepssector (0.0f);
			gridInfoMessage.add_pathstepsorientation (0.0f);
		}
		for(int j = pathFromAStar.size()-1; j>=0; j--){
			gridInfoMessage.set_pathstepsring (i, pathFromAStar.at(j).x);
			gridInfoMessage.set_pathstepssector (i, pathFromAStar.at(j).y);
			gridInfoMessage.set_pathstepsorientation (i, pathFromAStar.at(j).z);
			i++;
		}
	}

	_blk.publishSignal (gridInfoMessage, "pathplanning");
}

void PathPlanning::commitMovement(){
	//The path from A* has the revert path, so the start is in the end of the vector :)
	if(pathFromAStar.size()==0){		
		velocityWalk(0.0f,0.0f,0.0f,1.0f);	
		LogEntry(LogLevel::Info, GetName()) << "PathPlanning Commit return";
		return;
	}
	
	int lastElement = pathFromAStar.size()-1;
	if((pathFromAStar.at(lastElement).z > aStarDirections/8 && pathFromAStar.at(lastElement).z < aStarDirections-aStarDirections/8) || pathFromAStar.size() == 1){
		float phiSpeed;
		if(pathFromAStar.size()==1){
			phiSpeed = KMath::sign(KMath::wrapToPi(pathFromAStar.at(lastElement).z*aStarTransformation)/M_PI);
		}else{
			phiSpeed = KMath::sign(KMath::wrapToPi(pathFromAStar.at(lastElement-1).z*aStarTransformation)/M_PI);
		}
		velocityWalk(0.0f,0.0f,phiSpeed,1.0f);
		LogEntry(LogLevel::Info, GetName()) << "PathPlanning Commit Move 1 phiSpeed = " << phiSpeed;
	}else{
		//Take the second step and calculate the movement base on that
		float cartX, cartY;
		if(!aStarUseSmallGrid){
			cartX = KMath::toCartesianX (pathFromAStar.at(lastElement-1).x * pathMap.getMoveSteps() + pathMap.getMoveSteps() / 2, pathFromAStar.at(lastElement-1).y * pathMap.getTurnsteps());
			cartY = KMath::toCartesianY (pathFromAStar.at(lastElement-1).x * pathMap.getMoveSteps() + pathMap.getMoveSteps() / 2, pathFromAStar.at(lastElement-1).y * pathMap.getTurnsteps());
		}else{
			cartX = KMath::toCartesianX (pathFromAStar.at(lastElement-1).x * smallPathMap.getMoveSteps() + smallPathMap.getMoveSteps() / 2, pathFromAStar.at(lastElement-1).y * smallPathMap.getTurnsteps());
			cartY = KMath::toCartesianY (pathFromAStar.at(lastElement-1).x * smallPathMap.getMoveSteps() + smallPathMap.getMoveSteps() / 2, pathFromAStar.at(lastElement-1).y * smallPathMap.getTurnsteps());
		}
		
		float xSpeed, ySpeed, phiSpeed;
		float factor = 1.0f;
		if(pathFromAStar.size() < 3){
			factor = pathFromAStar.size()/3.0f;		
		}
		if(fabs(cartX) > fabs(cartY)){
			xSpeed = KMath::sign(cartX);
			ySpeed = KMath::sign(cartY)*fabs(cartY/cartX);
		}else{
			ySpeed = KMath::sign(cartY);
			xSpeed = KMath::sign(cartX)*fabs(cartX/cartY);
		}
		phiSpeed = KMath::sign(KMath::wrapToPi(pathFromAStar.at(lastElement-1).z*aStarTransformation)/M_PI);
		if(aStarUseSmallGrid){
			factor = (float)pathFromAStar.size()/(float)smallPathMap.getRadiusCells();
			factor = factor > 1 ? 1 : factor;
		}
		xSpeed *= factor;
		ySpeed *= factor;	
		
		velocityWalk(xSpeed,ySpeed,phiSpeed/3,1.0f);
		LogEntry(LogLevel::Info, GetName()) << "PathPlanning Commit Move 2 factor = " << factor << " xSpeed = " << xSpeed << " ySpeed = " << ySpeed << " phiSpeed = " << phiSpeed << " smallGrid = " << aStarUseSmallGrid;
	}
}

void PathPlanning::velocityWalk(float ix, float iy, float it, float f) {
	while(wmot.parameter_size()!=4){
		wmot.add_parameter(0.0f);
	}
	wmot.set_command("setWalkTargetVelocity");
	wmot.set_parameter(0, ix);
	wmot.set_parameter(1, iy);
	wmot.set_parameter(2, it);
	wmot.set_parameter(3, f);
	_blk.publishSignal(wmot, "motion");
}
/*--------------------------- aStar functions -------------------------------*/

void PathPlanning::aStar () {
	//KPROF_SCOPE(vprof, "astar");
	//Return imidietly if the target is unreachable
	if (!targetReachable (aStarTargetR, aStarTargetC) ) {
		pathFromAStar.clear();
		return;
	}
	LogEntry(LogLevel::Info, GetName()) << "Ring = " << aStarTargetR << " Cell = " << aStarTargetC << " phi = " << aStarTargetZ;

	currentValues.init();
	directions.init();
	
	int currentX, currentY, currentZ, newX, newY, newZ;
	//Initialization just to be sure
	currentX = currentY = currentZ = newX = newY = newZ = 255;
	
	if(!aStarUseSmallGrid){
		aStarRealTargetX = pathMap.getRealX (aStarTargetR, aStarTargetC);
		aStarRealTargetY = pathMap.getRealY (aStarTargetR, aStarTargetC);
	}else{
		aStarRealTargetX = smallPathMap.getRealX (aStarTargetR, aStarTargetC);
		aStarRealTargetY = smallPathMap.getRealY (aStarTargetR, aStarTargetC);
	}
	float infinity = std::numeric_limits<float>::infinity();

	node *popNode;
	node *newNode;
	
	for (int i = 0; i < aStarDirections; i++) {
		popNode = pool.malloc();
		setupNode (popNode, 255, 255, i, 0.0f);
		updateG (popNode, 255, 255, 0, 255, 255, i);
		updateH (popNode);
		openNodes.push(popNode);
	}

	while (openNodes.size() != 0) {
		do{
			popNode = openNodes.top();
			openNodes.pop();
			currentX = popNode->x;
			currentY = popNode->y;
			currentZ = popNode->z;
			//cout << currentX << " " << currentY << " " << currentZ << " " << popNode->h <<  endl;
		}while(currentX != 255 && currentValues.getElement(currentZ, currentX, currentY) == infinity);

		if (currentX != 255 && currentY != 255) {
			currentValues.setElement (currentZ, currentX, currentY, infinity );
		}

		if (currentX == aStarTargetR && currentY == aStarTargetC && currentZ == aStarTargetZ) {
			pool.free(popNode);
			break;
		}

		for (int dy = -1; dy <= 1; dy++) {
			newY = currentY + dy;

			if (newY == -1) {
				newY = aStarCircleCells - 1;
			} else if (newY >= aStarCircleCells ) {
				newY = 0;
			}

			for (int dx = -1; dx <= 1; dx++) {
				if (currentX == 255 && currentY == 255) {
					newX = 0;

					if (dx == 1 && dy == 0) {
						newY = 0;
					} else if (dx == -1 && dy == 0) {
						newY = aStarCircleCells / 2;
					} else if (dx == -1 && dy == 1) {
						newY = aStarCircleCells / 2 - aStarCircleCells / 8;
					} else if (dx == -1 && dy == -1) {
						newY = aStarCircleCells / 2 + aStarCircleCells / 8;
					} else if (dx == 0 && dy == 1) {
						newY = aStarCircleCells / 4;
					} else if (dx == 0 && dy == -1) {
						newY = 3 * aStarCircleCells / 4;
					} else if (dx == 1 && dy == 1) {
						newY = aStarCircleCells / 8;
					} else if (dx == 1 && dy == -1) {
						newY = 3 * aStarCircleCells / 4 + aStarCircleCells / 8;
					} else {
						newX = -1;
					}
				} else {
					newX = currentX + dx;
				}
				if(!aStarUseSmallGrid){
					if (newX < 0 || newX >= aStarRadiusCells || (pathMap (newX, newY) > 0.3f && (checkForTargetInObstacle || newX != aStarTargetR || newY != aStarTargetC))){
						continue;
					}
				}else{
					if (newX < 0 || newX >= aStarRadiusCells || (smallPathMap (newX, newY) > 0.3f && (checkForTargetInObstacle || newX != aStarTargetR || newY != aStarTargetC))){
						continue;
					}
				}

				for (int dz = -1; dz <= 1; dz++) {
					newZ = currentZ + dz;

					if (newZ < 0) {
						newZ += aStarDirections;
					} else if (newZ >= aStarDirections) {
						newZ -= aStarDirections;
					}

					if (currentValues.getElement (newZ, newX, newY) != infinity ) {
						newNode = pool.malloc();
						setupNode (newNode, newX, newY, newZ, popNode->g);
						updateG (newNode, currentX, currentY, currentZ, newX, newY, newZ);
						updateH (newNode);
						if (currentValues.getElement(newZ, newX, newY) == 0) {
							currentValues.setElement (newZ, newX, newY, newNode->h);
							openNodes.push(newNode);
							directions.setElement (newZ, newX, newY, (currentX << 16) | (currentY << 8) | (currentZ) );
						} else if (currentValues.getElement(newZ, newX, newY) > newNode->h) {
							openNodes.push(newNode);
							currentValues.setElement (newZ, newX, newY, newNode->h);
							directions.setElement (newZ, newX, newY, (currentX << 16) | (currentY << 8) | (currentZ) );
						} else {
							pool.free(newNode);
						}
					}
				}
			}
		}

		pool.free(popNode);
	}
	pathFromAStar.clear();
	if (openNodes.size() != 0) {
		coords pathCoords;
		pathCoords.x = currentX;
		pathCoords.y = currentY;
		pathCoords.z = currentZ;
		pathFromAStar.push_back(pathCoords);

		while (currentX != 255 || currentY != 255) {
			int tempDir = directions.getElement (currentZ, currentX, currentY);
			currentX = (tempDir >> 16) & 255;
			currentY = (tempDir >> 8) & 255;
			currentZ = (tempDir) & 255;
			pathCoords.x = currentX;
			pathCoords.y = currentY;
			pathCoords.z = currentZ;
			pathFromAStar.push_back(pathCoords);
		}
	}
	LogEntry(LogLevel::Info, GetName()) << "Astar pathSize: " << pathFromAStar.size();


	while(openNodes.empty() != true) {
		pool.free(openNodes.top());
		openNodes.pop();
	}
}

inline void PathPlanning::setupNode (node *n, int x, int y, int z, float g) {
	//KPROF_SCOPE(vprof, "setup");
	n->x = x;
	n->y = y;
	n->z = z;
	n->g = g;
}

inline void PathPlanning::updateG (node *n, int oldX, int oldY, int oldZ, int newX, int newY, int newZ) {
	//KPROF_SCOPE(vprof, "updateG");
	float tempValue = 0.0f;
	float realXdiff, realYdiff;
	if(!aStarUseSmallGrid){
		realXdiff = pathMap.getRealX (newX, newY) - pathMap.getRealX (oldX, oldY);
		realYdiff = pathMap.getRealY (newX, newY) - pathMap.getRealY (oldX, oldY);
	}else{
		realXdiff = smallPathMap.getRealX (newX, newY) - smallPathMap.getRealX (oldX, oldY);
		realYdiff = smallPathMap.getRealY (newX, newY) - smallPathMap.getRealY (oldX, oldY);
	}

	float dist;
	if (oldX != newX || oldY != newY) {
		float movingAngle = KMath::fast_atan2f (realYdiff, realXdiff);
		float difference = fabs (KMath::wrapTo0_2Pi (movingAngle)/aStarTransformation - oldZ);
		difference = difference > aStarDirections / 2 ? aStarDirections - difference : difference;
		tempValue = difference;
		dist = sqrtf(realXdiff*realXdiff + realYdiff*realYdiff);
	}else{
		dist = 0;
	}

	int orientChange = abs (newZ - oldZ);
	orientChange = orientChange > aStarDirections / 2 ? aStarDirections - orientChange : orientChange;
	n->g += dist*10 + tempValue + orientChange*2;
}

inline void PathPlanning::updateH (node *n) {
	//KPROF_SCOPE(vprof, "updateH");
	float tempValue = 0.0f;
	float realXdiff;
	float realYdiff;
	if(!aStarUseSmallGrid){
		realXdiff = aStarRealTargetX - pathMap.getRealX (n->x, n->y);
		realYdiff = aStarRealTargetY - pathMap.getRealY (n->x, n->y);
	}else{
		realXdiff = aStarRealTargetX - smallPathMap.getRealX (n->x, n->y);
		realYdiff = aStarRealTargetY - smallPathMap.getRealY (n->x, n->y);
	}
	if (n->x != aStarTargetR || n->y != aStarTargetC) {
		float movingAngle = KMath::fast_atan2f (realYdiff, realXdiff);
		float difference = fabs (KMath::wrapTo0_2Pi (movingAngle) / aStarTransformation - n->z);
		difference = difference > aStarDirections / 2 ? aStarDirections - difference : difference;
		tempValue = difference;
	}

	int orientChange = abs (aStarTargetZ - n->z);
	orientChange = orientChange > aStarDirections / 2 ? aStarDirections - orientChange : orientChange;
	n->h =n->g+ sqrtf (realXdiff * realXdiff + realYdiff * realYdiff)*10 + tempValue + orientChange*2;
}

bool PathPlanning::targetReachable (int x, int y) {
	if(aStarUseSmallGrid){
		return true;
	}
	//Apla elegxw se mia seira an einai reachable to target
	int y_plus_one = (y + 1 == aStarCircleCells ) ? 0 : y + 1;
	int y_minus_one = (y - 1 == -1) ? aStarCircleCells - 1 : y - 1;
	return (pathMap (x, y) <= 0.3f || !checkForTargetInObstacle) && ( (x < (pathMap.getRadiusCells() - 1) && (pathMap (x + 1, y) <= 0.3f || pathMap (x + 1, y_plus_one) <= 0.3f || pathMap (x + 1, y - 1) <= 0.3f) )
		    || pathMap (x, y_plus_one) <= 0.3f
		    || (x > 0 && (pathMap (x - 1, y_plus_one) <= 0.3f || pathMap (x - 1, y) <= 0.3f || (x < (pathMap.getRadiusCells() - 1) && pathMap (x + 1, y_minus_one) <= 0.3f)) )
		    || pathMap (x, y_minus_one) <= 0.3f);

}
ACTIVITY_END

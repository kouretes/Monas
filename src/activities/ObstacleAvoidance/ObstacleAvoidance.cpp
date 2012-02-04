#include "ObstacleAvoidance.h"
#include "tools/toString.h"
#include "tools/XMLConfig.h"
#include "tools/logger.h"
#include <iostream>
#include <string>

using namespace boost::posix_time;

namespace {
	ActivityRegistrar<ObstacleAvoidance>::Type temp("ObstacleAvoidance");
}

ObstacleAvoidance::ObstacleAvoidance() {
}




/*
 * 
 * check for move grid!!!!changed the fornt expansion
 * 
 * 
 * */


void ObstacleAvoidance::UserInit() {

	_blk->updateSubscription("sensors", msgentry::SUBSCRIBE_ON_TOPIC);
	_blk->updateSubscription("behavior", msgentry::SUBSCRIBE_ON_TOPIC);
	_blk->updateSubscription("obstacle", msgentry::SUBSCRIBE_ON_TOPIC);
	_blk->updateSubscription("buttonevents", msgentry::SUBSCRIBE_ON_TOPIC);

	Initialize();
	
	//ypoloipa metakinhsewn
	resX=0.0;
	resY=0.0;
	resAngle=0.0;
	shiftConsole = 100;  //shift==metatopish gia probolh grid sthn ekastote konsola
	countLeft=0;
	countRight=0;
	SonarFailCounter = 0; // counter gia fail twn sonars
	firstTimeInitializeOdometry=1;		//xrhsh gia prwth anafora se odometry metrhseis
	current = 0;		//current PolarGrid - used for correct obstacle move

	lastAge = microsec_clock::universal_time();
	debugTimer = microsec_clock::universal_time();
	lastwalk = microsec_clock::universal_time();

	//gia ta mnmata empodiwn
	mprosta=false;
	dexia= false;
	aristera = false;
	mprostaDist = 0.0;
	dexiaDist = 0.0;
	aristeraDist = 0.0;
	mprostaCert = 0.0;
	dexiaCert = 0.0;
	aristeraCert = 0.0;
	obstacleFound = 0;
	
	
	//syntetagmenes stoxou path planning
	goalX=0;
	goalY=0;
	targetX=0.0;
	targetY = 0.0;
	goalOrientation = 0;
	targetOrientation = 0.0;
	
	pathSize = 0;
	//boh8htikos pinakas gia update
	for(int i=0;i<SOnARsNum;i++)
		empty[i]=EMPTY;

	//initialize obstacle message
	obavm.add_direction(0);
	obavm.add_direction(0);
	obavm.add_direction(0);

	obavm.add_distance(0);
	obavm.add_distance(0);
	obavm.add_distance(0);

	obavm.add_certainty(0);
	obavm.add_certainty(0);
	obavm.add_certainty(0);
	
	//initialize gridinfo message
	for(int i=0;i<M+robotCells;i++)
		for(int j=0;j<N;j++)
			gridInfoMessage.add_gridcells(0);
			
	for(int i=0;i<RobotDirections;i++)
		gridInfoMessage.add_odometrymeasure(0);
	
	//gia gui initialization
	white = cvScalar(255,255,255);
	blue = cvScalar(255, 0, 0);
	paintColor = grey;
	nCurvePts=4;
	nCurves=1;
	isCurveClosed=1;
	lineWidth=8;
	colorValue =0;
	
	counterPath=0;
	debugCounter=0;
	turnCounter = 0;
	
	cX=0.0; cY=0.0; ct=0.0;
	bd=0.0;
	
	wmot = new MotionWalkMessage();
	wmot->add_parameter(0.0f);
	wmot->add_parameter(0.0f);
	wmot->add_parameter(0.0f);
	wmot->add_parameter(0.0f);
	
	initGrid();		//arxikopoihsh twn pinakwn
	calculateUpdateCells();
	if (cvHighgui)
	{
		Logger::Instance().WriteMsg("ObstacleAvoidance", "Enable highgui", Logger::Info);
		img = cvCreateImage(cvSize(200, 200), IPL_DEPTH_8U, 3);
		cvNamedWindow("img", CV_WINDOW_AUTOSIZE);
	}
}
int ObstacleAvoidance::Execute() {
	read_messages();
	
	//testPath(7, 8, 1);
	
	//return 0;
	//xrhsh gia elegxo ean to array me tis times twn sonars einai 0 = sfalma
	countLeft=0;
	countRight=0;

	double Right[SOnARsNum], Left[SOnARsNum];
	//xrhsh gia elegxo ean to array me tis times twn sonars einai 0 = sfalma
	countLeft=0;
	countRight=0;

	//(RobotPositionSensor Message) find current robot position
	if(rpm !=0){
		if(debugModeCout) 
			Logger::Instance().WriteMsg("ObstacleAvoidance", "PosX: " + _toString(PosX.sensorvalue()) + " PosY: " + _toString(PosY.sensorvalue()) + " Angle: " + _toString(Angle.sensorvalue()) , Logger::Info);

		PosX = rpm->sensordata(KDeviceLists::ROBOT_X);
		PosY = rpm->sensordata(KDeviceLists::ROBOT_Y);
		Angle = rpm->sensordata(KDeviceLists::ROBOT_ANGLE);
		if (firstTimeInitializeOdometry ==1){
			RobotPosition[0]=PosX.sensorvalue();
			RobotPosition[1]=PosY.sensorvalue();
			RobotPosition[2]=Angle.sensorvalue();
			firstTimeInitializeOdometry=0;
		}
		else{
			if(debugWithMovement) 
				findNewPosition();
			//targetX kai tagetY apo mnm behavior
			/*if(debugFullPathPlanning){
				goalX = int(targetX/distance);
				goalY = int(targetY/N);
				initPathPlanning(goalX, goalY);
			}*/
		}
	}
	
	//update the grid with the new sonar values
	if (asvm != 0) {
		for (int j=SOnARsNum-1;j>=0;j--){
			LeftValue[j] = asvm->sensordata(KDeviceLists::L_US+j);
			Left[j] = LeftValue[j].sensorvalue();
			RightValue[j] = asvm->sensordata(KDeviceLists::R_US+j);
			Right[j] = RightValue[j].sensorvalue();
			countLeft = countLeft+LeftValue[j].sensorvalue();
			countRight = countRight + RightValue[j].sensorvalue() ;
		}
		if(debugModeCout) 
			printSonarValues();
		if(countLeft == 0 && countRight == 0){
			SonarFailCounter++;
			if(SonarFailCounter > 10)
				Logger::Instance().WriteMsg("ObstacleAvoidance", "sonars fail", Logger::Info);
		}
		else
			updateGrida(countLeft!=0?Left:empty, countRight!=0?Right:empty);
	}
	
	
	//path planning messsage
	if(pprm !=0){
		/*targetX = pprm->gotox();
		targetY = pprm->gotoy();
		targetOrientation = pprm->gotoangle();
		targetY = (atan2(targetY, targetX)*ToDegrees)/RotationAngle;
		targetX = targetX/distance;
		astar3Neighbours(int(targetX), int(targetY), targetOrientation);*/
		double waypointX, waypointY, waypointT, distance2Goal;
		double targetZeroOrientation = 0.0;
		targetX = pprm->gotox(); //*ToCm;
		targetY = pprm->gotoy(); //*ToCm;
		targetOrientation = pprm->gotoangle();
		distance2Goal = sqrt((targetX)*(targetX) + (targetY)*(targetY));
		if(debugModeCout)
			Logger::Instance().WriteMsg("ObstacleAvoidance", " targetX : " + _toString(targetX) + " y: " + _toString(targetY) + " or: " + _toString(targetOrientation), Logger::Info );
		if( distance2Goal > 0.0){
			//eykleidia apostash kai orientation an mikrotero apla kalv velocityWalk
			targetY = wrapTo0_2Pi(atan2(targetY, targetX)+0.5*RotationAngleRad+M_PI_2); //*ToDegrees)/RotationAngle;
			//targetY = ((targetY > 0 ? targetY : (2*M_PI + targetY)) * ToDegrees) / RotationAngle;
			targetY = (targetY * ToDegrees) / RotationAngle;
			targetX = distance2Goal/distanceM;
			if (targetX > M ) targetX = M;
			targetZeroOrientation = wrapTo0_2Pi((int(targetY)-0.5)*RotationAngleRad - M_PI_2);
			//Logger::Instance().WriteMsg("ObstacleAvoidance", " orient leme " + _toString(targetOrientation), Logger::Info);
			targetOrientation = wrapTo0_2Pi(targetOrientation+(0.5*M_PI_4) - targetZeroOrientation)/M_PI_4;
			if(debugModeCout)
				Logger::Instance().WriteMsg("ObstacleAvoidance", " astarPx " + _toString((int)targetX) + " astarPy " + _toString((int)targetY) + " astarOr " + _toString((int)targetOrientation), Logger::Info );
			//astarSimple(int(targetX), int(targetY));
			if(debugModeCout)
				Logger::Instance().WriteMsg("ObstacleAvoidance", " targetZoreOrientation " + _toString(targetZeroOrientation) + " targetOrientation " + _toString(targetOrientation), Logger::Info );
			astar13Neighbours(int(targetX), int(targetY),  int(targetOrientation));
			//if(debugModeCout) {
				//Logger::Instance().WriteMsg("ObstacleAvoidance", "astar11 x: " + _toString(targetX) + " y: " + _toString(targetY) + " or: " + _toString((int)targetOrientation) , Logger::Info);
				//for (int l=0; l < 20; l++)
					//Logger::Instance().WriteMsg("ObstacleAvoidance", "waypoint x: " + _toString(waypointx[l]) + " y: " + _toString(waypointy[l]) + " or: " + _toString(waypointOr[l]), Logger::Info);
			//}
			if(waypointx[1] == 1){ 		//peristrofikh kinhsh
				waypointX = 0.0;
				waypointY = 0.0;
				waypointT =(waypointy[1]>FRONT)?M_PI_4:-M_PI_4;
			}else if(waypointy[1]>FRONT+1 || waypointy[1]<FRONT-1){		//plagia bhmata
				waypointX = cellCenterX[waypointx[1]][waypointy[1]];
				waypointY = cellCenterY[waypointx[1]][waypointy[1]];
				waypointT = 0.0;
			}else{
				waypointX = cellCenterX[waypointx[1]][waypointy[1]];
				waypointY = cellCenterY[waypointx[1]][waypointy[1]];
				waypointT = wrapToPi(waypointOr[1]*M_PI_2);
			}
			if(debugModeCout)
				Logger::Instance().WriteMsg("ObstacleAvoidance", "waypoint x: " + _toString(waypointx[1]) + " y: " + _toString(waypointy[1]) + " or: " + _toString(waypointOr[1]) , Logger::Info);
		}
		else{
			waypointX = targetX;
			waypointY = targetY;
			waypointT = targetOrientation;
		}
		for (int l=0;l<20;l++){
			waypointx[l] = 0;
			waypointy[l] = 0;
			waypointOr[l] = 0;
		}
		if(debugModeCout)
			Logger::Instance().WriteMsg("ObstacleAvoidance", "coord waypoint x: " + _toString(waypointX) + " y: " + _toString(waypointY) + " or: " + _toString(waypointT) , Logger::Info);
		callVelocityWalk(waypointX, waypointY, waypointT, distance2Goal);
	}
	
	
	//values from messages for grid
	/*if (beamVals != 0){
		for( int i =robotCells; i<M+robotCells;i++)
			for(int j=0; j< N; j++)
				PolarGrid[current][i][j] = beamVals->beams(i*N+j);
	}
	*/
	//for path planning for specific time
	if (wm != 0 && debugPathPlannignDirect) {
		if(debugModeCout) 
			Logger::Instance().WriteMsg("ObstacleAvoidance", "motion message", Logger::Info);
		if (wm->command() == "setWalkTargetVelocity" && wm->parameter_size() == 5) {
			velocityParam1 = wm->parameter(0); //x
			velocityParam2 = wm->parameter(1); //y
			velocityParam3 = wm->parameter(2); //theta
			velocityParam4 = wm->parameter(3); //freq
			velocityParam5 = wm->parameter(4); //time
			if(debugModeCout)
				Logger::Instance().WriteMsg("ObstacleAvoidance", "velocityP1" + _toString(velocityParam1) + " velocityP2" + _toString(velocityParam2) + 
					" velocityP3" + _toString(velocityParam3) + " velocityP4" + _toString(velocityParam4) +" velocityP5" + _toString(velocityParam5), Logger::Info);
			
			xDirection = velocityParam1*WALK_MAX_STEP_X;
			yDirection = velocityParam2*WALK_MAX_STEP_Y;
			thetaDirection = velocityParam3*WALK_MAX_STEP_THETA;
			
			velocityAngle = atan2(yDirection, xDirection)*ToDegrees + velosityShift;
			velocityAngle = velocityAngle < 0 ? angleDegrees + velocityAngle : velocityAngle;
				
			//velocityDistance = sqrt(pow(xDirection, 2.0)+ pow(yDirection, 2.0)) ;
			velocityDistance = velocityDistance * velocityParam5;
			
			if(debugModeCout)
				Logger::Instance().WriteMsg("ObstacleAvoidance", "x" + _toString(xDirection) + " y" + _toString(yDirection) + 
					" theta" + _toString(thetaDirection) + " angle" + _toString(velocityAngle) +" dist" + _toString(velocityDistance), Logger::Info);
			
			obstacleFound = checkForObstacle(velocityAngle, velocityDistance);
			
			if(debugModeCout) Logger::Instance().WriteMsg("ObstacleAvoidance", "obstacleFound " + _toString(obstacleFound), Logger::Info);
		}
		else
			Logger::Instance().WriteMsg("ObstacleAvoidance", "Not enough parameters found [time missing??]", Logger::Info);
	}
	
	boost::shared_ptr<const ButtonMessage> bm=_blk->readSignal<ButtonMessage>("buttonevents");
    if(bm.get()!= NULL){
        int lbump=bm->data(KDeviceLists::L_BUMPER_L)+bm->data(KDeviceLists::L_BUMPER_R);
        int rbump=bm->data(KDeviceLists::R_BUMPER_L)+bm->data(KDeviceLists::R_BUMPER_R);
        if(debugModeCout) 
			Logger::Instance().WriteMsg("ObstacleAvoidance", "lbump " + _toString(lbump) + "rbump " + _toString(rbump), Logger::Info);
		if(lbump >= 1 && rbump >= 1 )
			for(int i = RIGHT; i <= LEFT; i++)
				PolarGrid[current][2][i] = 1.0;
		else if(lbump >= 1 )
			for(int i = FRONT; i < LEFT; i++)
				PolarGrid[current][2][i] = 1.0;
		else if(rbump >= 1 )
			for(int i = RIGHT; i < FRONT+1; i++)
				PolarGrid[current][2][i] = 1.0;
    }
	
	//aging grid every 'ageTimerSeconds' seconds (currently 3)
	if (microsec_clock::universal_time()-lastAge >= seconds(ageTimerSeconds)){
		ageGrid();
		lastAge = microsec_clock::universal_time();
		if(debugModeCout) 
			Logger::Instance().WriteMsg("ObstacleAvoidance", "aging", Logger::Info);
		//publishObstacleMessage();
		//publishMotionMessage(0);
	}

	if (cvHighgui)
		cvDrawGrid();

	publishObstacleMessage();
	//publishGridInfo();
	//pairnei orisma initn???
	if(debugModePublishMotion)
		publishMotionMessage(FRONT);
	return 0;
}

void ObstacleAvoidance::testPath(int m, int n, int o){
	initGrid();
	astar3Neighbours(m, n, o);
}


void ObstacleAvoidance::printSonarValues(){
	for(int i=0;i<SOnARsNum;i++)
		if(debugModeCout) Logger::Instance().WriteMsg("ObstacleAvoidance", "leftSensor: " + _toString(LeftValue[i].sensorvalue()) + " rightSensor: " + _toString(RightValue[i].sensorvalue()) , Logger::Info);

	if(debugModeCout) Logger::Instance().WriteMsg("ObstacleAvoidance", "debugCounter: " + _toString(debugCounter), Logger::Info);
	debugCounter++;
}

int ObstacleAvoidance::checkForObstacle(float checkAngle, float checkDistance){
	int check=int(checkAngle/RotationAngle);
	int dist = int(checkDistance*distance);
	if(debugModeCout) 
		Logger::Instance().WriteMsg("ObstacleAvoidance", "angle: " + _toString(check) + " distance " + _toString(dist), Logger::Info);
	for(int i=robotCells;i<dist+robotCells;i++){
		if(PolarGrid[current][i][check] >= HighObstaclePossibility){
			initPathPlanning(M, FRONT);
			return i;
		}
		else continue;
	}
	return 0;					
}

void ObstacleAvoidance::read_messages() {
	asvm =  _blk->readData<AllSensorValuesMessage>("sensors");
	rpm =  _blk->readData<RobotPositionMessage>("sensors");
	pprm = _blk->readSignal<PathPlanningRequestMessage>("obstacle");
	wm = _blk->readSignal<MotionWalkMessage>("obstacle");
	//sonarData = _blk->readData<SonarsData>("obstacle");
	//beamVals = _blk->readData<beam>("obstacle");
	//targetX = _blk->in_msg_nb<RobotPositionSensorMessage>("targetX", "Behavior");
	//targetY = _blk->in_msg_nb<RobotPositionSensorMessage>("targetY", "Behavior");
	//Logger::Instance().WriteMsg("ObstacleAvoidance", "read messages " , Logger::ExtraExtraInfo);
}

void ObstacleAvoidance::publishMotionMessage(int initn){
	float x=1.0, y=0.0, z=0.0, s=0.0, t=10;
	
	/* if(initm != pathx[g-1]) {
		s = initn*RotationAngle;
		if(s<45 && s>=0){
			x =initn*0.2; 
			y =-1;
		}else if(s>45 && s<=90){
			x =1;
			y = (initn-4.5)*0.2-1;
		}else if(s>90 && s<=135){
			x =1;
			y =0.2*(initn-9);
		}else if(s>135 && s<=180){
			x =(initn-13.5)*0.2 -1; 
			y =1;
		}
		wmot = new MotionWalkMessage();
		wmot->set_command("setWalkTargetVelocity");
		wmot->add_parameter(x);
		wmot->add_parameter(y);
		wmot->add_parameter(z);
		wmot->add_parameter(s);
		if(debugModeCout)
			Logger::Instance().WriteMsg("ObstacleAvoidance","Sending Command  to motion: setWalkTargetVelocity. x: " + _toString(x) + " y: " + _toString(y), Logger::ExtraInfo);
		_blk->publishSignal(*wmot,"obstacle");
		break;
	  }*/
	s = initn*RotationAngle;
	if(s<45 && s>=0){
		x =initn*0.2; 
		y =-1;
	}else if(s>45 && s<=90){
		x =1;
		y = (initn-4.5)*0.2-1;
	}else if(s>90 && s<=135){
		x =1;
		y =0.2*(initn-9);
	}else if(s>135 && s<=180){
		x =(initn-13.5)*0.2 -1; 
		y =1;
	}

	
	wmot->set_command("setWalkTargetVelocity");
	wmot->add_parameter(x);
	wmot->add_parameter(y);
	wmot->add_parameter(z);
	wmot->add_parameter(s);
	wmot->add_parameter(t);
	if(debugModeCout)
		Logger::Instance().WriteMsg("ObstacleAvoidance","Sending Command: setWalkTargetVelocity. x: " + _toString(x) + " y: " + _toString(y), Logger::ExtraInfo);
	_blk->publishSignal(*wmot,"motion");
}

void ObstacleAvoidance::publishObstacleMessage(){
	if(debugModeCout){
		Logger::Instance().WriteMsg("ObstacleAvoidance", "aristera: " + _toString(aristera)+ " dist: " + _toString(aristeraDist) + " cert: "+_toString(aristeraCert) , Logger::ExtraExtraInfo);
		Logger::Instance().WriteMsg("ObstacleAvoidance", "mprosta: " + _toString(mprosta) + " dist: " + _toString(mprostaDist) + " cert: "+_toString(mprostaCert) , Logger::ExtraExtraInfo);
		Logger::Instance().WriteMsg("ObstacleAvoidance", "dexia: " + _toString(dexia) + " dist: " + _toString(dexiaDist) + " cert: "+_toString(dexiaCert) , Logger::ExtraExtraInfo);
	}

	obavm.set_direction(0, aristera?1:0);
	obavm.set_direction(1, mprosta?1:0);
	obavm.set_direction(2, dexia?1:0);

	obavm.set_distance(0, aristera?aristeraDist:RAND_MAX);
	obavm.set_distance(1, mprosta?mprostaDist:RAND_MAX);
	obavm.set_distance(2, dexia?dexiaDist:RAND_MAX);

//	obavm.set_distance(0, aristera?aristeraDist*ToMeters:RAND_MAX);
//	obavm.set_distance(1, mprosta?mprostaDist*ToMeters:RAND_MAX);
//	obavm.set_distance(2, dexia?dexiaDist*ToMeters:RAND_MAX);

	obavm.set_certainty(0, aristera?aristeraCert:0.0);
	obavm.set_certainty(1, mprosta?mprostaCert:0.0);
	obavm.set_certainty(2, dexia?dexiaCert:0.0);
	_blk->publishState(obavm, "obstacle");

}

void ObstacleAvoidance::publishGridInfo(){
	for(int i=0;i<M+robotCells;i++)
		for(int j=0;j<N;j++)
			gridInfoMessage.set_gridcells(i*N+j, PolarGrid[current][i][j]);
	
	for(int i=0;i<RobotDirections;i++)
		gridInfoMessage.set_odometrymeasure(i, RobotPosition[i]);
	
	_blk->publishSignal(gridInfoMessage, "obstacle");
}

/*
void ObstacleAvoidance::gridInformationMessage(){
	for(int i=robotCells;i<M+robotCells;i++)
		for(int j=0;j<N;j++)
			BeamMessage.set_beams(i*N+j, PolarGrid[current][i][j]);
}*/

void ObstacleAvoidance::Initialize() {
	XMLConfig config(ArchConfig::Instance().GetConfigPrefix() + "/obstacle.xml");
	
	if (config.IsLoadedSuccessfully()) {
		bool found = true;
		///Parameters
		found &= config.QueryElement("cvHighgui", cvHighgui);
		//found &= config.QueryElement("SonarAngleRange", SonarAngleRange);
//		found &= config.QueryElement("RobotDirections", RobotDirections);
//		found &= config.QueryElement("ToCm", ToCm);
		found &= config.QueryElement("HighObstaclePossibility", HighObstaclePossibility);
		found &= config.QueryElement("agePossibility", agePossibility);
		found &= config.QueryElement("TooClose", TooClose);
		found &= config.QueryElement("MinimumDistanceValue", MinimumDistanceValue);
		found &= config.QueryElement("MinimumValidValue", MinimumValidValue);
		//found &= config.QueryElement("NoKnowledge", NoKnowledge);
		//path planning
		found &= config.QueryElement("discount", discount);
		found &= config.QueryElement("reward", reward);
		found &= config.QueryElement("goal", goal);
		found &= config.QueryElement("obstacle", obstacle);
		found &= config.QueryElement("ITERATIONS", ITERATIONS);
		
		found &= config.QueryElement("CELLS", CELLS);
		found &= config.QueryElement("UsePossibilityDown", UsePossibilityDown);
		found &= config.QueryElement("UsePossibilityUp", UsePossibilityUp);
		found &= config.QueryElement("PossibilityDiscount", PossibilityDiscount);
		found &= config.QueryElement("PossibilityDiscountNoObst", PossibilityDiscountNoObst);
		found &= config.QueryElement("WATCHDIRECTION", WATCHDIRECTION);
		found &= config.QueryElement("TOLERANCE", TOLERANCE);
		found &= config.QueryElement("FrontDeviation", FrontDeviation);
		found &= config.QueryElement("SideDeviation", SideDeviation);
		found &= config.QueryElement("MinPossibility", MinPossibility);
		found &= config.QueryElement("MaxPossibility", MaxPossibility);
		found &= config.QueryElement("ObstaclePossibility", ObstaclePossibility);
		found &= config.QueryElement("MinimumValidSonarValue", MinimumValidSonarValue);
		found &= config.QueryElement("velosityShift", velosityShift);
		
		found &= config.QueryElement("ageTimerSeconds", ageTimerSeconds);
		//debuging variables for selective running of codel
		found &= config.QueryElement("debugModeCout", debugModeCout);
		found &= config.QueryElement("debugInstantResponce", debugInstantResponce);
		found &= config.QueryElement("debugWithMovement", debugWithMovement);
		found &= config.QueryElement("debugPathPlannignDirect", debugPathPlannignDirect);
		found &= config.QueryElement("debugFullPathPlanning", debugFullPathPlanning);
		found &= config.QueryElement("debugModePublishMotion", debugModePublishMotion);
		//found &= config.QueryElement("angleDegrees", angleDegrees);
		found &= config.QueryElement("robotCells", robotCells);
		found &= config.QueryElement("obstacleCost", obstacleCost);
		found &= config.QueryElement("rotationCost", rotationCost);
		
		Logger::Instance().WriteMsg("ObstacleAvoidance", "Initialize no knowledge: " + _toString(NoKnowledge) , Logger::ExtraExtraInfo);

		if (found) 
			Logger::Instance().WriteMsg("ObstacleAvoidance","All obstacle parameters loaded successfully" , Logger::ExtraExtraInfo);
		else 
			Logger::Instance().WriteMsg("ObstacleAvoidance", "Cant Find an attribute in the xml config file " , Logger::ExtraExtraInfo);
	} else 
		Logger::Instance().WriteMsg("ObstacleAvoidance","Cant Find xml config file ", Logger::ExtraExtraInfo);
}

void ObstacleAvoidance::initPolarGrid(){
	//initialize Polar grid
	for(int currentGrid=0;currentGrid<=1;currentGrid++)
		for(int i=0;i<=M+2+robotCells;i++)
			for(int j=0;j<N;j++){
				if (i==0 || i==1) {
					PolarGrid[currentGrid][i][j] = 0.0;
				}
				else
					PolarGrid[currentGrid][i][j] = NoKnowledge;
			}
			
	/*for(int n=5;n<=10;n++)
		PolarGrid[current][5][n] = 1.0;
	for(int n=6;n<=8;n++)
		PolarGrid[current][4][n] = 1.0;*/
}

void ObstacleAvoidance::initGrid(){
	//oi syntetagmenes pou antistoixoun sta 4 shmeia pou orizoun to keli
	findCoordinates();

	//initialize the variables of the robot position
	for (int i=0;i<RobotDirections;i++)
		RobotPosition[i]=EMPTY;

	//initialize arrays with sonar values
	for (int i=0;i<SOnARsNum;i++){
		rightArray[i]=EMPTY;
		leftArray[i]=EMPTY;
	}

	//initialize Polar grid
	initPolarGrid();

	//initialize Movegrid
	initMoveGrid();	
	
	
	initEuclidean();
	
	//initialize matrix with euklidean distances between cells for astar
	//EuclideanLookup();
		
	//initialize matrix with euklidean distances between cells for astar11
	//EuclideanOrientationLookup();
}

void ObstacleAvoidance::initMoveGrid(){

}

double ObstacleAvoidance::myRound(double value){
	return floor(value+0.5);
}

//age all other cells besides those that were resently updated
void ObstacleAvoidance::ageRestGrid(){
	for(int i=robotCells;i<M+robotCells;i++)
		for (int j=0;j<N;j++){
			if(j >= LEFT && j <=RIGHT) continue;
			PolarGrid[current][i][j] = (PolarGrid[current][i][j]-NoKnowledge)*agePossibility + NoKnowledge;
		}
}

//age all other cells besides those that were resently updated
void ObstacleAvoidance::ageSpecGrid(int x, int y){
	PolarGrid[current][x][y] = (PolarGrid[current][x][y]-NoKnowledge)*agePossibility + NoKnowledge;
}

void ObstacleAvoidance::checkNeighbour(int fromM, int fromN, int toM, int toN, vector<int> &updateCellsM, vector<int> &updateCellsN){
	double xDiff, yDiff, dist;
	
	if(toM > M) return;
	if(changed[toM][toN] == 1) return;
	xDiff = cellCenterX[toM][toN] - cellCenterX[fromM][fromN];
	yDiff = cellCenterY[toM][toN] - cellCenterY[fromM][fromN];
	dist = sqrt(xDiff*xDiff + yDiff*yDiff);
	
	if(dist <= 0.15){
		updateCellsM.push_back(toM);
		updateCellsN.push_back(toN);
	}
	changed[toM][toN] = 1;
	initIndexes(toM, toN);
	for(int i=0;i<NEIGHBOURS;i++){
		if(indexx[i] <=1) continue;
		checkNeighbour(fromM, fromN, indexx[i], indexy[i], updateCellsM, updateCellsN);
	}
}


void ObstacleAvoidance::calculateUpdateCells(){
	int middle;
	vector<int> updateCellsM;
	vector<int> updateCellsN;
	
	for(int left = 0 ; left <=1 ; left ++){
		for(int i=robotCells;i<=M;i++){
			initChanged();
			updateCellsM.clear();
			updateCellsN.clear();
			middle = (left == 0)?MIDDLE_RIGHT:MIDDLE_LEFT;
			initIndexes(i, middle);
			
			for(int j=0;j<NEIGHBOURS;j++){
				initIndexes(i, middle);
				if(indexx[j] <=1) continue;
				checkNeighbour(i, middle, indexx[j], indexy[j], updateCellsM, updateCellsN);
			}
			if(left == 0){
				RightM.insert(std::pair<int, vector<int> >(i, updateCellsM)); 
				RightN.insert(std::pair<int, vector<int> >(i, updateCellsN)); 
			}
			else{
				LeftM.insert(std::pair<int, vector<int> >(i, updateCellsM)); 
				LeftN.insert(std::pair<int, vector<int> >(i, updateCellsN)); 
			}
			
		}
	}
}


void ObstacleAvoidance::updateFront(){
	for(int i=robotCells;i<3+robotCells;i++)
		if(abs(PolarGrid[current][i][LEFT] - PolarGrid[current][i][RIGHT]) <= TOLERANCE) 
			for (int k=FRONT;k<=FRONT+1;k++) // TODO: use the correct range
				PolarGrid[current][i][k] = (PolarGrid[current][i][LEFT] + PolarGrid[current][i][RIGHT])/2;
}

void ObstacleAvoidance::updateGrid(double (&left)[SOnARsNum], double (&right)[SOnARsNum]){//left, right
	int index, v=0;
	double temp[2], possibilityUp, gridValue, ind=0.0;
	mprosta=false;
	dexia= false;
	aristera = false;
	
	//Gia periptwsh eleipsis empodiou -> meiwsh pi8anothtas yparxis empodiou
	//Logger::Instance().WriteMsg("ObstacleAvoidance", "countLeft " + _toString(countLeft) +  " countRight " + _toString(countRight) , Logger::ExtraExtraInfo);
	
	//in case of no obstacle reduce possibility in the front 
	if (right[0] >= TooFar){//countRight >= 19 || countRight ==0  ){
		for(int i=robotCells;i<M+robotCells;i++)
			for(int j=RIGHT ;j<=FRONT  ;j++){ //13:17
				PolarGrid[current][i][j] = fmaxf( PolarGrid[current][i][j]*PossibilityDiscountNoObst, MinPossibility );
			}
	}
	if (left[0] >= TooFar){//countLeft >= 19 || countLeft ==0 ){
		for(int i=robotCells;i<M+robotCells;i++)
			for(int j=FRONT ;j<=LEFT   ;j++){ //17:21
				PolarGrid[current][i][j]= fmaxf( PolarGrid[current][i][j]*PossibilityDiscountNoObst, MinPossibility ); 
			}
	}
	
	for (int i=SOnARsNum-1;i>=0;i--){
		left[i] = fmaxf(left[i], MinimumValidValue);
		right[i] = fmaxf(right[i], MinimumValidValue);
		left[i] = ( (left[i] < TooFar) && (left[i] > TooClose) ) ? left[i] : EMPTY; //left
		right[i] = ( (right[i] < TooFar) && (right[i] > TooClose) ) ? right[i] : EMPTY; //right
		if ( (left[i] == EMPTY) && (right[i] == EMPTY) ) continue;
		temp[0] = left[i];
		temp[1] = right[i];
		int fromY = 0;
		int toY = 0;
		
		for(int k=0;k<=1;k++){
			ind = temp[k];
			if (ind == EMPTY) continue;
			index = (int) ( ((temp[k]*ToCm)/distance) - ((MinimumValidValue*ToCm)/distance) + robotCells ); //-2 giati den mas endiaferei mexri ta 20 cm
			if(index < 0) continue;
			
			if(k){
				fromY = RIGHT;	//4
				toY = FRONT-1; //7
			}
			else{
				fromY = FRONT+2;//10
				toY = LEFT;	//13
			}
			
			for(int j=fromY; j<=toY; j++){
				gridValue = PolarGrid[current][index][j];
				PolarGrid[current][index][j] = fminf( PolarGrid[current][index][j]*ObstaclePossibility, MaxPossibility );
				
				v=1;
				possibilityUp = UsePossibilityUp;
				while (index-v >= 0 || index+v < M ){
					//if (index-v>=0)
						//PolarGrid[current][index-v][j] = fmaxf( PolarGrid[current][index-v][j]*UsePossibilityDown, MinPossibility );
					if (index+v<M){
						if (v >= 2) 
							ageSpecGrid(index+v, j);
						else
							PolarGrid[current][index+v][j] = fminf( PolarGrid[current][index+v][j]*possibilityUp, MaxPossibility );
					}
					v++;
					possibilityUp *= PossibilityDiscount;
				}
			}
		}
	}

	updateFront();
	
	for(int i=robotCells;i<M+robotCells;i++){
		mprosta = PolarGrid[current][i][FRONT] > 0.6?true:false;
		mprostaDist = mprosta == true ? ((i+1) * distance / ((double) ToCm)) : 0.0;
		mprostaCert = mprosta == true ? PolarGrid[current][i][FRONT] : 0.0;
		if (mprosta) break;
	}

	for(int i=robotCells;i<M+robotCells;i++){
		dexia = PolarGrid[current][i][RIGHT+1] > 0.6?true:false;
		dexiaDist = dexia == true ? ((i+1) * distance / ((double) ToCm)) : 0.0;
		dexiaCert = dexia == true ? PolarGrid[current][i][RIGHT+1] : 0.0;  // TODO: get the central ray
		if (dexia) break;
	}
		
	for(int i=robotCells;i<M+robotCells;i++){
		aristera = PolarGrid[current][i][LEFT-1] > 0.6?true:false;
		aristeraDist = aristera == true ? ((i+1) * distance / ((double) ToCm)) : 0.0;
		aristeraCert = aristera == true ? PolarGrid[current][i][LEFT-1] : 0.0;  // TODO: get the central ray
		if (aristera) break;
	}
	
	/*if(debugModeCout){
		Logger::Instance().WriteMsg("ObstacleAvoidance", "mprosta " + _toString(mprostaCert) , Logger::ExtraExtraInfo);
		Logger::Instance().WriteMsg("ObstacleAvoidance", "dexia " + _toString(dexiaCert)  , Logger::ExtraExtraInfo);
		Logger::Instance().WriteMsg("ObstacleAvoidance", "aristera " + _toString(aristeraCert) , Logger::ExtraExtraInfo);
	}*/
	//printSonarValues();
	if (cvHighgui){
		cvDrawGrid();
	}
}

void ObstacleAvoidance::reduceFrontPossibility(double (&left)[SOnARsNum], double (&right)[SOnARsNum]){
	if (right[0] >= TooFar){//countRight >= 19 || countRight ==0  ){
		for(int i=robotCells;i<M+robotCells;i++)
			for(int j=RIGHT ;j<=FRONT  ;j++){ //13:17
				PolarGrid[current][i][j] = fmaxf( PolarGrid[current][i][j]*PossibilityDiscountNoObst, MinPossibility );
			}
	}
	if (left[0] >= TooFar){//countLeft >= 19 || countLeft ==0 ){
		for(int i=robotCells;i<M+robotCells;i++)
			for(int j=FRONT ;j<=LEFT   ;j++){ //17:21
				PolarGrid[current][i][j]= fmaxf( PolarGrid[current][i][j]*PossibilityDiscountNoObst, MinPossibility ); 
			}
	}
}

void ObstacleAvoidance::updateGrida(double (&left)[SOnARsNum], double (&right)[SOnARsNum]){//left, right
	int index, updateM, updateN, toSize;
	double temp[2];
	double ind=0.0;
	updateCellByIndex::iterator it ;
	
	//if(debugModeCout)
		//Logger::Instance().WriteMsg("ObstacleAvoidance", "update ", Logger::ExtraExtraInfo);
	
	reduceFrontPossibility(left, right);
	
	for (int i=SOnARsNum-1;i>=0;i--){
		left[i] = left[i] == MinimumValidSonarValue?MinimumValidValue:left[i] ;
		right[i] = right[i] == MinimumValidSonarValue?MinimumValidValue:right[i] ;
		left[i] = (left[i] < TooFar && left[i] > TooClose)?left[i]:0.0; //left
		right[i] = (right[i] < TooFar && right[i] > TooClose)?right[i]:0.0; //right
		if(left[i] == 0.0 && right[i] == 0.0) continue;
		temp[0] = left[i];
		temp[1] = right[i];
		
		for(int left=0;left<=1;left++){
			ind = temp[left];
			if(ind== 0) continue;
			index=(int)(temp[left]*(ToCm/distance) -(30/distance))+robotCells;//-2 giati den mas endiaferei mexri ta 20 cm
			if(index < 0) continue;
			toSize = (left == 0)?RightM[index].size():LeftM[index].size();
			for(int l=0;l<toSize;l++){
				updateM = (left == 0)?RightM[index].at(l):LeftM[index].at(l);
				updateN = (left == 0)?RightN[index].at(l):LeftN[index].at(l);
				PolarGrid[current][updateM][updateN]=PolarGrid[current][updateM][updateN]*1.5>=MaxPossibility?MaxPossibility:PolarGrid[current][updateM][updateN]*1.1;
			}
		}
	}
	if (cvHighgui)
		cvDrawGrid();
}

void ObstacleAvoidance::ageGrid(){
	for (int i=robotCells;i<M+robotCells;i++)
		for(int j=0;j<N;j++)
			PolarGrid[current][i][j] = (PolarGrid[current][i][j]-NoKnowledge)*agePossibility + NoKnowledge;
}

void ObstacleAvoidance::findNewPosition(){
	double temp[3]={EMPTY, EMPTY, EMPTY};

	temp[0] = (PosX.sensorvalue()-RobotPosition[0]);
	temp[1] = (PosY.sensorvalue()-RobotPosition[1]);
	temp[2] = angleDiff(Angle.sensorvalue(), RobotPosition[2]);//(Angle.sensorvalue()-RobotPosition[2]);
	//Logger::Instance().WriteMsg("ObstacleAvoidance", "x: " + _toString(temp[0]) + "   y: " + _toString(temp[1]) + "   theta: " + _toString(temp[2]), Logger::ExtraExtraInfo);
	//opou distance bale 15
	if (fabs(temp[2]) > RotationAngleRad) {
		rotateGrid(temp[2]);
	}
	if(fabs(temp[0] )> distanceM || fabs(temp[1]) > distanceM){
		//Logger::Instance().WriteMsg("ObstacleAvoidance", "temp2: " + _toString(temp[2]) , Logger::ExtraExtraInfo);
		straightMoveGrid(temp[0]*FrontDeviation, temp[1]*SideDeviation, 0);//, temp[2]);
	}
}

void ObstacleAvoidance::rotateGrid(double angle){
	double temp[M+robotCells][N];
	int rotateCells = (int) myRound(angle/RotationAngleRad);
	int change[N];
	//Logger::Instance().WriteMsg("ObstacleAvoidance", "rotateangle " + _toString(rotateCells), Logger::ExtraExtraInfo);
	for (int i=robotCells;i<M+robotCells;i++){
		for(int j=0;j<N;j++) change[j] = 0;//init change for every ring
		for(int j=0;j<N;j++){
		  temp[i][(j+rotateCells)%N] = PolarGrid[current][i][(j+rotateCells)%N]; 
		  if(j+rotateCells < 0) rotateCells=N+rotateCells;
		  if(change[j]==0){
			PolarGrid[current][i][(j+rotateCells)%N] = PolarGrid[current][i][j]; 
			change[(j+rotateCells)%N] = 1;
		  }
		  else{
			PolarGrid[current][i][(j+rotateCells)%N] = temp[i][j];
			change[(j+rotateCells)%N] = 1;
		  }
		}
	}
	RobotPosition[2] = Angle.sensorvalue(); 
	//Logger::Instance().WriteMsg("ObstacleAvoidance", "---------------------------------------------------------", Logger::Info);
}

void ObstacleAvoidance::findCoordinates(){
	double extraAngle;
	extraAngle = ((N/2)%2 == 0)?RotationAngleRad/2.0:0.0;
	for (int i=0;i<M+2+robotCells;i++){
		for (int j=0;j<N;j++){
			guiX[i][j] = round(i*distance * sin(RotationAngle*j*ToRad)) + shiftConsole;
			guiY[i][j] = shiftConsole - round(i*distance * cos(RotationAngle*j*ToRad)) ;
			if(i<M+1+robotCells){
				cellCenterX[i][j] = (i+0.5)*distanceM * sin(RotationAngle*(j+0.5)*ToRad-extraAngle);
				cellCenterY[i][j] = -(i+0.5)*distanceM * cos(RotationAngle*(j+0.5)*ToRad-extraAngle);
			}
			//Logger::Instance().WriteMsg("ObstacleAvoidance", " i " + _toString(i) + " j " + _toString(j) + " coord x " + _toString(guiX[i][j]) + " y " + _toString(guiY[i][j])  + " cellcenter x " + _toString(cellCenterX[i][j]) + " y " + _toString(cellCenterY[i][j]), Logger::ExtraExtraInfo);
		}
	}
}

void ObstacleAvoidance::reset(){
	initPolarGrid();
	firstTimeInitializeOdometry = 1;
}

void ObstacleAvoidance::initChanged(){
	for(int i=0;i<M;i++)
		for(int j=0;j<N;j++)
			changed[i][j]=0;
}

double ObstacleAvoidance::angleDiff(double a1, double a2){
	return wrapToPi(wrapToPi(a1+M_PI-a2) - M_PI);
}

double ObstacleAvoidance::wrapToPi(double angle){
	while (angle > M_PI)
		angle -= 2.0 * M_PI;
	while (angle < -M_PI)
		angle += 2.0 * M_PI;
	return (angle);
}


double ObstacleAvoidance::wrapTo0_2Pi(double angle)
{
	while (angle > 2.0 * M_PI)
		angle -= 2.0 * M_PI;
	while (angle < 0.0)
		angle += 2.0 * M_PI;
	return angle;
}

double ObstacleAvoidance::angleDiff2(double a1, double a2){
	double diff = a1*ToDegrees-a2*ToDegrees;
	while(diff < -angleDegrees/2) diff +=angleDegrees;
	while(diff > angleDegrees/2) diff -=angleDegrees;
	return diff/ToDegrees;
}

void ObstacleAvoidance::straightMoveGrid(double deltaFront,double deltaSide, double deltatheta){
	double px, py,r, theta, temp, xnew, ynew, rnew, thetanew=0.0;
	int indexm=0, indexn=0;//, indey=0, cellnum=0; int smooth =0;
	if(debugModeCout)
		Logger::Instance().WriteMsg("ObstacleAvoidance", "move front " + _toString(deltaFront) + "move side " + _toString(deltaSide)+ "move theta " + _toString(deltatheta), Logger::ExtraExtraInfo);
	//initialize changed
	current = 1-current;
	//Logger::Instance().WriteMsg("ObstacleAvoidance", "current " + _toString(current), Logger::Info);
	//initChanged();
	
	//initialize new polar grid
	for(int i=0;i<=M+robotCells;i++)
		for(int j=0;j<N;j++){
			if (robotCells == 2 && (i==0 || i==1)) {
				PolarGrid[current][i][j] = 0.0;
			}
			else if(i < M+robotCells-1)
				PolarGrid[current][i][j] = NoKnowledge;//PolarGrid[1-current][i][j];//
			else
				PolarGrid[current][i][j] = NoKnowledge;
		}
	
	for(int i=0;i<=M+robotCells;i++)
		for(int j=0;j<N;j++){
			temp = PolarGrid[1-current][i][j];
			//if (fabs(PolarGrid[1-current][i][j]-NoKnowledge) >= TOLERANCE){// && changed[i*N+j]==0){ //check only the cells with obstacles.
		
			px = cellCenterX[i][j];
			py = cellCenterY[i][j];
			
			r = sqrt(px*px+ py*py) ;//M
			theta = angleDiff(atan2(px, py), double(RobotPosition[2]));//N

			xnew= r*sin(theta+ 2*RobotPosition[2]) - deltaFront;
			ynew= -r*cos(theta+ 2*RobotPosition[2]) + deltaSide;
			
			rnew = sqrt(xnew*xnew + ynew*ynew);
			thetanew = angleDiff(atan2(xnew, ynew), double(Angle.sensorvalue()))*ToDegrees;
			thetanew = thetanew < 0 ? angleDegrees + thetanew : thetanew;
			
			indexn = int(thetanew/RotationAngle);
			indexm = int(rnew/distanceM);//-1; // -1 gia swsto deikth
			
			
			if(indexn>=0 && indexm >=robotCells  && indexn < N && indexm < M+robotCells-1){
				PolarGrid[current][indexm][indexn] = temp;
				changed[indexm][indexn] = 1;
				initIndexes(indexm, indexn);
				//for(int l=3;l<=5;l++){
					//in case of moving forwards obstacles 'expanding'
					//if((indexm < i) && (indexn <= LEFT) && (indexn >= RIGHT)){
						//PolarGrid[current][indexx[l]][indexy[l]] = temp*0.8;
					//}	
				//}
			}
		}
	
	RobotPosition[0] = PosX.sensorvalue();
	RobotPosition[1] = PosY.sensorvalue();
	RobotPosition[2] = Angle.sensorvalue(); 

	/*if(distanceFront) {
		if (distanceSide)
			smooth=2;
		else smooth=1;
	}
	else if(distanceSide) smooth=3;
	
	smoothGrid(smooth);*/
	//smoothGrid(1);
	if (cvHighgui)
		cvDrawGrid();
}



///////////////////////////value iteration/////////////////////////////

void ObstacleAvoidance::initPossibilities(){
  possibilities[0]=0.635; //mprosta
  possibilities[1]=0.1; //ma
  possibilities[2]=0.075;  //a
  possibilities[3]=0.005; //pa
  possibilities[4]=0.005; //pisw
  possibilities[5]=0.005; //pd
  possibilities[6]=0.075;  //d
  possibilities[7]=0.1; //md
  possibilities[8]=0.0;  //idio
}

//8 neighbour cells
void ObstacleAvoidance::initIndexes(int mm, int nn){
	indexx[0] = mm!=M?mm+1:mm; //mprosta
	indexx[1] = mm!=M?mm+1:mm; //ma
	indexx[2] = mm;				 //a
	indexx[3] = mm!=0?mm-1:mm;	 //pa
	indexx[4] = mm!=0?mm-1:mm;	 //pisw
	indexx[5] = mm!=0?mm-1:mm;	 //pd
	indexx[6] = mm;				 //d
	indexx[7] = mm!=M?mm+1:mm; //md
	indexx[8] = mm;				 //idio

	indexy[0] = nn;				 //mprosta
	indexy[1] = nn!=N-1?nn+1:0;	 //ma
	indexy[2] = nn!=N-1?nn+1:0;	 //a
	indexy[3] = nn!=N-1?nn+1:0;	 //pa
	indexy[4] = nn ;			 //pisw
	indexy[5] = nn!=0?nn-1:N-1;	 //pd
	indexy[6] = nn!=0?nn-1:N-1;	 //d
	indexy[7] = nn!=0?nn-1:N-1;	 //md
	indexy[8] = nn;				 //idio
}

void ObstacleAvoidance::iterationGridDir(int mm, int nn){
	initIndexes(mm, nn);
	for(int i=0;i<NEIGHBOURS+1;i++)
		value[i] = IterationGrid[indexx[i]][indexy[i]];
}

void ObstacleAvoidance::bestPath(int goalx, int goaly){
	
	int initm=robotCells, initn=FRONT, i=1,g=0;
	double bigger=-10, temp;
	int pathx[20], pathy[20];
	//briskw prwto eley8ero keli gia kinhsh an oxi (0,17)
	while(PolarGrid[current][initm][initn] > 0.75){
		if (PolarGrid[current][initm][initn+i] < 0.6){
		  initn=initn+i;
		  break;
		}
		if (PolarGrid[current][initm][initn-i] < 0.6){
		  initn=initn-i;
		  break;
		}
		initn++;
		i+=2;
	}
	//Logger::Instance().WriteMsg("ObstacleAvoidance", "initm " + _toString(initm) +  " initn " + _toString(initn) , Logger::ExtraExtraInfo);
	pathx[g] = initm;
	pathy[g] = initn;

  initIndexes(initm, initn);
  do{

	g++;

	bigger = IterationGrid[indexx[0]][indexy[0]]; //bigger prwto mprosta
    initm = indexx[0];
    initn = indexy[0];


	  for (i=1;i<NEIGHBOURS;i++){
		  temp=IterationGrid[indexx[i]][indexy[i]];//value[i];

		  if (temp>bigger){
			  initm = indexx[i];
			  initn = indexy[i];
			  bigger=temp;
		  }
	  }
	  if(debugModeCout) Logger::Instance().WriteMsg("ObstacleAvoidance", "initm " + _toString(initm) +  " initn " + _toString(initn) , Logger::ExtraExtraInfo);
	  pathx[g] = initm;
	  pathy[g] = initn;
	  initIndexes(initm, initn);
	  
	 /* if(initm != pathx[g-1]) {
		publishMotionMessage(initn);
		break;
	  }*/
	  //if(initm == pathx[g-1] && initn == pathy[g-1]) break;
	}while(initm != goalx || initn!=goaly);
	for (i=0;i<g;i++)
		IterationGrid[pathx[i]][pathy[i]] = PathChosen;
	IterationGrid[goalx][goaly] = PathChosen;
	if (cvHighgui)
		drawIterationGrid();
}

void ObstacleAvoidance::fillIterationGrid(int goalx, int goaly){
	if (cvHighgui)
		cvDrawGrid();
	int count=0, it=0;
	double sum=0.0, sumold = 0.0, max=0.0;
	initPossibilities();
	//for(int it=0;it<30;it++){
	do{
		it++;
		count =0;
		for(int mm=robotCells;mm<=M+robotCells;mm++){
		  for(int nn=0;nn<N;nn++){
		  //nn=21;mm=5;
			sum=EMPTY;
			sumold=EMPTY;
			max=EMPTY;
			iterationGridDir(mm, nn); //bazw sta value tis times twn gyrw keliwn tou (mm, nn)
			initIndexes(mm, nn);
			if (PolarGrid[current][mm][nn] > HighObstaclePossibility){
				count++;
				IterationGrid[mm][nn] = obstacle;
			}
			else if(mm==goalx && nn==goaly){
				IterationGrid[mm][nn] = goal;
				count++;
			}
			else{
			  for(int a=0;a<NEIGHBOURS;a++){
				sum=0.0;
				for (int k=0;k<WATCHDIRECTION;k++){
					if(mm==0){
						if(k==3 || k==4 || k==5)
							sum = sum + IterationGrid[indexx[8]][indexy[8]]*possibilities[(k-a>0?k-a:NEIGHBOURS+k-a)%NEIGHBOURS];
						else
							sum = sum + IterationGrid[indexx[k]][indexy[k]]*possibilities[(k-a>0?k-a:NEIGHBOURS+k-a)%NEIGHBOURS];
					}
					else if(mm==M){
						if(k==0 || k==1 || k==7)
							sum = sum + IterationGrid[indexx[8]][indexy[8]]*possibilities[(k-a>0?k-a:NEIGHBOURS+k-a)%NEIGHBOURS];
						else
							sum = sum + IterationGrid[indexx[k]][indexy[k]]*possibilities[(k-a>0?k-a:NEIGHBOURS+k-a)%NEIGHBOURS];
					}
					else
						sum = sum + IterationGrid[indexx[k]][indexy[k]] * possibilities[((k-a>=0)?(k-a):(NEIGHBOURS+k-a))%NEIGHBOURS];
				}
				max = (sum>sumold)?sum:sumold;
				sumold = max;
			  }
			 // drawIterationGrid();
			  if (fabs(IterationGrid[mm][nn] - reward-max) < 0.1){
				count++; 
				//cout << "count " << count << endl;	
			  }
			  if(count > 280) {
					if(debugModeCout) Logger::Instance().WriteMsg("ObstacleAvoidance", "returning..." + _toString(it), Logger::ExtraExtraInfo);
					return;
				}	
			  IterationGrid[mm][nn] = reward + max;
			}
		  }
		}
	}while(1);
	if (cvHighgui)
		drawIterationGrid();
}


void ObstacleAvoidance::examplePathPlanningInit(){
	PolarGrid[current][6][6] = 1.0;
	PolarGrid[current][6][7] = 1.0;
	PolarGrid[current][6][8] = 1.0;
	PolarGrid[current][6][9] = 1.0;
	PolarGrid[current][6][10] = 1.0;
	PolarGrid[current][6][11] = 1.0;
	PolarGrid[current][6][12] = 1.0;
	PolarGrid[current][6][13] = 1.0;
}

void ObstacleAvoidance::initPathPlanning(int goalx, int goaly){
	//uncomment for path planning example
	//examplePathPlanningInit();

	if (cvHighgui)
		cvDrawGrid();

	//initialize iterationGrid to be same as PolarGrid
	for (int i=robotCells;i<=M+robotCells;i++)
		for (int j=0;j<N;j++)
			IterationGrid[i][j] = PolarGrid[current][i][j] ;
	IterationGrid[goalx][goaly] = goal;
	fillIterationGrid(goalx, goaly);
	if (cvHighgui)
		drawIterationGrid();
	bestPath(goalx, goaly);
}


void ObstacleAvoidance::cvDrawGrid(){
	CvPoint* curveArr={curve1};
	for (int k=robotCells;k<M+robotCells;k++){
		for(int l=0;l<N;l++){
			curve1[0].x = guiX[k][l];      curve1[0].y = guiY[k][l];
			curve1[1].x = guiX[(k+1)][l];  curve1[1].y = guiY[(k+1)][l];
			
			if(l < N-1){
				curve1[2].x = guiX[(k+1)][1+l];	curve1[2].y = guiY[(k+1)][1+l];
				curve1[3].x = guiX[k][1+l];    	curve1[3].y = guiY[k][1+l];
			}
			else{
				curve1[2].x =guiX[(k+1)][l];     curve1[2].y = guiY[(k+1)][l];
				curve1[3].x =guiX[k][0];         curve1[3].y = guiY[k][0];
			}
			colorValue = int(colorMax-PolarGrid[current][k][l]*colorMax);
			paintColor = cvScalar(colorValue, colorValue, colorValue);
			if(PolarGrid[current][k][l] == 2.0)
				paintColor = blue;
			cvFillPoly( img, &curveArr,&nCurvePts, nCurves, paintColor);
			cvPolyLine( img,&curveArr,&nCurvePts, nCurves, 1, white, isCurveClosed, lineWidth, 0 );

		}
	}
	//for the blue waypoints
	for (int i=0; i<20; i++){
		if(waypointx[i] == 0) break;
		int k = waypointx[i];
		int l = waypointy[i]; 
		curve1[0].x = guiX[k][l];      curve1[0].y = guiY[k][l];
		curve1[1].x = guiX[(k+1)][l];  curve1[1].y = guiY[(k+1)][l];
		if(l < N-1){
			curve1[2].x = guiX[(k+1)][1+l];	curve1[2].y = guiY[(k+1)][1+l];
			curve1[3].x = guiX[k][1+l];    	curve1[3].y = guiY[k][1+l];
		}
		else{
			curve1[2].x =guiX[(k+1)][0];     curve1[2].y = guiY[(k+1)][0];
			curve1[3].x =guiX[k][0];         curve1[3].y = guiY[k][0];
		}
		paintColor = blue;
		cvFillPoly( img, &curveArr,&nCurvePts, nCurves, paintColor);
		cvPolyLine( img,&curveArr,&nCurvePts, nCurves, 1, white, isCurveClosed, lineWidth, 0 );

		//cout << " " << waypointx[i] << " " << waypointy[i] << " " << waypointOr[i] << endl;
	}
	
	//draw the arrow
	angle = 90;
	center = cvPoint( 100,100 );  
	fromP= cvPoint( center.x , center.y + 10 ); 
	toP = cvPoint( center.x, center.y - 10 );   
	leftP = cvPoint(center.x - 10, center.y );  
	rightP = cvPoint( center.x+10, center.y );  
	cvLine( img, fromP, toP, blue, 2, CV_AA, 0);
	cvLine( img, leftP, toP, blue, 2, CV_AA, 0);
	cvLine( img, rightP, toP, blue, 2, CV_AA, 0);
    //cvLine( img, center, cvPoint( cvRound( center.x + magnitude*cos(angle*CV_PI/180)),cvRound( center.y - magnitude*sin(angle*CV_PI/180))), blue, 2, CV_AA, 0 );  
     
	
	cvNamedWindow("img", CV_WINDOW_AUTOSIZE);
	cvShowImage("img", img);
	cvWaitKey(10);
	return;

}


void ObstacleAvoidance::drawIterationGrid(){
	CvPoint* curveArr={curve1};
	for (int k=0;k<M;k++){
		for(int l=0;l<N;l++){
			curve1[0].x = guiX[k][l];      curve1[0].y = guiY[k][l];
			curve1[1].x = guiX[(k+1)][l];  curve1[1].y = guiY[(k+1)][l];
			
			if(l < N-1){
				curve1[2].x = guiX[(k+1)][1+l];	curve1[2].y = guiY[(k+1)][1+l];
				curve1[3].x = guiX[k][1+l];    	curve1[3].y = guiY[k][1+l];
			}
			else{
				curve1[2].x =guiX[(k+1)][l];     curve1[2].y = guiY[(k+1)][l];
				curve1[3].x =guiX[k][0];         curve1[3].y = guiY[k][0];
			}
			colorValue = int(colorMax-IterationGrid[k][l]*colorMax);
			if(IterationGrid[k][l] == 2.0)
				paintColor = blue;
			else paintColor = cvScalar(colorValue, colorValue, colorValue);
			cvFillPoly( img, &curveArr,&nCurvePts, nCurves, paintColor);
			cvPolyLine( img,&curveArr,&nCurvePts, nCurves, 1, white, isCurveClosed, lineWidth, 0 );
		}
	}

	//draw the arrow
	angle = 90;
	center = cvPoint( 100,100 );  
	fromP= cvPoint( center.x , center.y + 10 ); 
	toP = cvPoint( center.x, center.y - 10 );   
	leftP = cvPoint(center.x - 10, center.y );  
	rightP = cvPoint( center.x+10, center.y );  
	cvLine( img, fromP, toP, blue, 2, CV_AA, 0);
	cvLine( img, leftP, toP, blue, 2, CV_AA, 0);
	cvLine( img, rightP, toP, blue, 2, CV_AA, 0);
    //cvLine( img, center, cvPoint( cvRound( center.x + magnitude*cos(angle*CV_PI/180)),cvRound( center.y - magnitude*sin(angle*CV_PI/180))), blue, 2, CV_AA, 0 );  
     

	cvNamedWindow("img", CV_WINDOW_AUTOSIZE);
	cvShowImage("img", img);
	cvWaitKey(0);
}


/////////////////////////astar////////////////////////////////////////
/*void ObstacleAvoidance::EuclideanLookup(){
	int fromx, fromy, tox, toy;
	for(int i=0;i<((M+4)*N);i++)
		for(int j=0;j<((M+4)*N);j++){
			fromx = cellCenterX[i];
			fromy = cellCenterY[i];
			tox = cellCenterX[j];
			toy = cellCenterY[j];
			euclidean[i][j] = sqrt((fromx-tox)*(fromx-tox) + (fromy-toy)*(fromy-toy));// + fabs(atan2(toy-fromy, tox-fromx));
			//out << " from " << i << " to " << j << " eucl " << euclidean[i][j] << " aney " << sqrt((fromx-tox)*(fromx-tox) + (fromy-toy)*(fromy-toy))<<endl;
		}
		
}


void ObstacleAvoidance::EuclideanOrientationLookup(){
	int fromx, fromy, tox, toy;
	for(int i=0;i<((M+4)*N);i++)
		for(int j=0;j<((M+4)*N);j++){
			fromx = cellCenterX[i];
			fromy = cellCenterY[i];
			tox = cellCenterX[j];
			toy = cellCenterY[j];
			euclideanOrientation[i][j] = sqrt((fromx-tox)*(fromx-tox) + (fromy-toy)*(fromy-toy) + (M_PI/4*M_PI/4) );
			//out << " from " << i << " to " << j << " eucl " << euclidean[i][j] << " aney " << sqrt((fromx-tox)*(fromx-tox) + (fromy-toy)*(fromy-toy))<<endl;
		}
		
}
*/
void ObstacleAvoidance::reconstructPathNew(int ring, int sector, int orientt){
	if(debugModeCout)
		Logger::Instance().WriteMsg("ObstacleAvoidance", " cellnew m " + _toString(ring) + " n " + _toString(sector) + " " + " orient " + _toString(orientt) + " parent  " + _toString(parentM[ring][sector][orientt]) + " "  + _toString(parentN[ring][sector][orientt]), Logger::ExtraExtraInfo);
	if(parentM[ring][sector][orientt] > -1 && parentN[ring][sector][orientt] > -1)
		reconstructPathNew(parentM[ring][sector][orientt], parentN[ring][sector][orientt], orient[ring][sector][orientt]);

	if(counterPath > 20 ) return;
	waypointx[counterPath] = ring;
	waypointy[counterPath] = sector;
	waypointOr[counterPath] = orientt;
	counterPath++;
}

void ObstacleAvoidance::reconstructPath(int ring, int sector){
	if(debugModeCout)
		Logger::Instance().WriteMsg("ObstacleAvoidance", " cellpath m " + _toString(ring) + " n " + _toString(sector), Logger::ExtraExtraInfo);
	if(parentM[ring][sector][0] > -1)
		reconstructPath(parentM[ring][sector][0], parentN[ring][sector][0]);
	
	//cout <<  int(currentcell/N)  << " "  << int(currentcell%N) << " " << orient[currentcell] << endl;
	/*if(counterPath > 1 ) return;
	nextM[counterPath] = ring;
	nextN[counterPath] = sector;
	*/
	if(counterPath > 20 ) return;
	waypointx[counterPath] = ring;
	waypointy[counterPath] = sector;
	counterPath++;
}

list<ObstacleAvoidance::OpenListNode> ObstacleAvoidance::insertInOpenList(ObstacleAvoidance::OpenListNode anode, list<ObstacleAvoidance::OpenListNode> &openList){
	list<ObstacleAvoidance::OpenListNode>::iterator i;
	for(i=openList.begin(); i!=openList.end();i++)
		if(anode.fn < (*i).fn)
			break;
	
	openList.insert(i, anode);	
	//for(i=openList.begin(); i!=openList.end();i++)
		//Logger::Instance().WriteMsg("ObstacleAvoidance", " insert... " + _toString( (*i).fn) , Logger::ExtraExtraInfo);
	return openList;
}


void ObstacleAvoidance::generateSuccessor(int Ring, int Sector, int Orientation,
 double gn, double hn, OpenListNode currentNode, list<OpenListNode> &openList){
	OpenListNode temp;
	//Logger::Instance().WriteMsg("ObstacleAvoidance", " generate m " + _toString(Ring) + " n " + _toString(Sector) + " " + " orient " + _toString(Orientation) +
		//						" gn  " + _toString(gn) + " hn "  + _toString(hn), Logger::ExtraExtraInfo);

	//in closed
	if(whatList[Ring][Sector][Orientation] == -1 )
		return;
	
	//if not in open then add in open
	if(whatList[Ring][Sector][Orientation] == 0){
		temp.ring = Ring;
		temp.sector = Sector;
		temp.gn = gn;
		temp.hn = hn;
		temp.fn = gn + hn; 
		parentM[Ring][Sector][Orientation] =currentNode.ring;
		parentN[Ring][Sector][Orientation] =currentNode.sector;
		temp.orientation = Orientation;
		orient[Ring][Sector][Orientation]= currentNode.orientation;
		insertInOpenList(temp, openList);
		whatList[Ring][Sector][Orientation] = 1;
	}
	else{
		list<OpenListNode>::iterator it1;
		for (it1=openList.begin(); it1!=openList.end(); ++it1)
			if(((*it1).ring == Ring) && ((*it1).sector == Sector) && ((*it1).orientation == Orientation) )
				break;
		if(debugFullPathPlanning)
			cout << " was in open " << " M " << ((*it1).ring)  << " N " << ((*it1).sector) << " ori " << (*it1).orientation << endl;
		if((*it1).gn > gn){
			openList.erase(it1);
			temp.ring = Ring;
			temp.sector = Sector;
			temp.gn = gn;
			temp.hn = hn;
			temp.fn = gn + hn; 
			parentM[Ring][Sector][Orientation] =currentNode.ring;
			parentN[Ring][Sector][Orientation] =currentNode.sector;
			temp.orientation = Orientation;
			orient[Ring][Sector][Orientation] = currentNode.orientation;
			insertInOpenList(temp, openList);
			whatList[Ring][Sector][Orientation] = 1;
		}
	}	
}


void ObstacleAvoidance::initAstarArrays(){
	for(int i=0;i<M+3;i++)
		for(int j=0;j<N;j++)
			for(int k=0; k< NEIGHBOURS; k++){
				whatList[i][j][k] = 0;
				parentM[i][j][k] = -1;
				parentN[i][j][k] = -1;
				orient[i][j][k] = 0;
			}
}

void ObstacleAvoidance::createNewNode(int initindem, int initinden, int gn, int hn, int orientation, list<OpenListNode> &openList){
	OpenListNode temp;
	temp.ring = initindem;
	temp.sector = initinden;
	temp.gn = gn;							//cost to move from start point to the given cell following the generated path
	temp.hn = hn; ////estimated cost from given cell to goal
	temp.fn = gn + hn;
	temp.orientation = orientation;
	openList.push_front(temp);
	whatList[initindem][initinden][0] = 1;
}

void ObstacleAvoidance::astar13Neighbours(int goalx, int goaly, int goalorient){
	if(debugModeCout) Logger::Instance().WriteMsg("ObstacleAvoidance", " 13 neighbours astar... ", Logger::ExtraExtraInfo);
	
	int startAngle = 0, nodes=0, neighbourSector, neighbourRing, currentm, currentn;
	int initinden = FRONT, initindem = 1, pathFound = 0, nodeOrientation;
	double  newg, hn;
	OpenListNode currentNode;
	list<OpenListNode> openList;
	
	initAstarArrays();
	
	for (int orr=0;orr<NEIGHBOURS;orr++){
		orient[initindem][initinden][orr] = 0;
		parentM[initindem][initinden][orr] = -1;
		parentN[initindem][initinden][orr] = -1;
	}
	
	hn = euclidean[initindem][initinden][goalx][goaly];
	createNewNode(initindem, initinden, 0, hn, 0, openList);
	
	while(!openList.empty()){
		currentNode = openList.front();
		currentm = currentNode.ring;  //int(currentNode.cell / N);
		currentn = currentNode.sector; //int(currentNode.cell% N);
		nodes++;
		if((currentm == goalx) && (currentn == goaly) && (currentNode.orientation == goalorient)){ //    ){  //
			pathFound = 1;
			if(parentM[goalx][goaly][goalorient] == -1 && parentN[goalx][goaly][goalorient] == -1) {
				waypointx[1] = goalx;
				waypointy[1] = goaly;
				waypointOr[1] = goalorient;
				return;
			}
			//printParentM();
			reconstructPathNew(currentm, currentn, currentNode.orientation);
			counterPath = 0;
			if (cvHighgui)
				cvDrawGrid();
			return;
		}
		
		//remove from openlist
		openList.pop_front();
		
		//insert to closed list
		whatList[currentm][currentn][currentNode.orientation] = -1;
				
		//expand neighbours
		initIndexes(currentm, currentn);
		startAngle= currentNode.orientation;
		
		//generate the rotating steps
		neighbourRing = currentm;

		//left
		neighbourSector = (currentm == 1)?(currentn+1)%N:currentn;
		nodeOrientation = (currentm == 1)?startAngle:(startAngle+1)%NEIGHBOURS;
		hn =  euclidean[neighbourRing][neighbourSector][goalx][goaly];
		newg = currentNode.gn;
		newg += euclidean[currentNode.ring][currentNode.sector][neighbourRing][neighbourSector];
		newg += (currentm == 1)?RotationAngleRad/M_PI_4*rotationCost:rotationCost;
		generateSuccessor(neighbourRing, neighbourSector, nodeOrientation, newg, hn, currentNode, openList);
		//cout << "1" << " m " << neighbourRing << " n " << neighbourSector << " or " << nodeOrientation << endl;
		//right
		neighbourSector = (currentm == 1)?(currentn-1+N)%N:currentn;
		nodeOrientation = (currentm == 1)?startAngle:(startAngle-1+N)%NEIGHBOURS;	
		hn =  euclidean[neighbourRing][neighbourSector][goalx][goaly];
		newg = currentNode.gn;
		newg += euclidean[currentNode.ring][currentNode.sector][neighbourRing][neighbourSector];
		newg += (currentm == 1)?RotationAngleRad/M_PI_4*rotationCost:rotationCost;
		generateSuccessor(neighbourRing, neighbourSector, nodeOrientation, newg, hn, currentNode, openList);
		//cout << "2" << " m " << neighbourRing << " n " << neighbourSector << " or " << nodeOrientation << endl;
		//v=3;
		for (int  j=0;j<NEIGHBOURS;j++){
			if (indexx[j] > M) continue;
			
			//generate the three neighbours 
			if((j==startAngle) || (j==(startAngle+1)%NEIGHBOURS) || (j==(startAngle-1+NEIGHBOURS)%NEIGHBOURS) ){
				if (indexx[j] <= 1) continue;
				for(int l = -1; l<=1; l++){	
					nodeOrientation = (j+l)%NEIGHBOURS;
					if(nodeOrientation < 0) nodeOrientation = (nodeOrientation+NEIGHBOURS);
					neighbourRing = indexx[j];
					neighbourSector = indexy[j];
					newg = currentNode.gn;
					newg += euclidean[currentNode.ring][currentNode.sector][neighbourRing][neighbourSector];
					newg += 0.067;
		
					if(PolarGrid[current][neighbourRing][neighbourSector] >= 0.8)
						continue;					
					hn =  euclidean[neighbourRing][neighbourSector][goalx][goaly];
					
					//cout << v << " m " << neighbourRing << " n " << neighbourSector << " or " << nodeOrientation << endl;
					generateSuccessor(neighbourRing, neighbourSector, nodeOrientation, newg, hn, currentNode, openList);	
					//v++;
				}
			}
			/*
			//generate side steps
			if((j==(startAngle+2)%NEIGHBOURS) || (j==(startAngle-2+NEIGHBOURS)%NEIGHBOURS)){
				//cout << "JJJJJJK " << startAngle << endl;
				if (currentm == 1) {
					neighbourRing = 2;
					if (j==2) {	//left	
						neighbourSector = SIDE_LEFT;
						nodeOrientation = 6;
					}
					if (j==6) {	//right
						neighbourSector = SIDE_RIGHT;
						nodeOrientation = 2;
					}
					
				}
				else{
					neighbourRing = indexx[j];
					neighbourSector = indexy[j];
					nodeOrientation = startAngle;
				}
				newg = currentNode.gn;
				newg += euclidean[currentNode.ring][currentNode.sector][neighbourRing][neighbourSector];
				newg += 2.0;
				if(PolarGrid[current][neighbourRing][neighbourSector] >= 0.8)
					continue;				
				hn =  euclidean[neighbourRing][neighbourSector][goalx][goaly];
				//cout << v << " m " << neighbourRing << " n " << neighbourSector << " or " << nodeOrientation << endl;
				generateSuccessor(neighbourRing, neighbourSector, nodeOrientation, newg, hn, currentNode, openList);
				//v++;
			}
			*/  
		}
	}
}


void ObstacleAvoidance::astar3Neighbours(int goalx, int goaly, int goalorient){
	if(debugModeCout){
		Logger::Instance().WriteMsg("ObstacleAvoidance", " Threeee neighbours astar... ", Logger::ExtraExtraInfo);
		Logger::Instance().WriteMsg("ObstacleAvoidance", " goalm " + _toString(goalx) + " n " + _toString(goaly) + " or " + _toString(goalorient), Logger::ExtraExtraInfo);
	}
	int startAngle = 0, nodes=0, neighbourSector, neighbourRing, currentm, currentn;
	int initinden = FRONT, initindem = 1, pathFound = 0;
	double  newg;
	OpenListNode temp, currentNode;
	list<OpenListNode> openList;
	
	for(int i=0;i<M+3;i++)
		for(int j=0;j<N;j++)
			for(int k=0; k< WATCHDIRECTION; k++){
				whatList[i][j][k] = 0;
				parentM[i][j][k] = -1;
				parentN[i][j][k] = -1;
				orient[i][j][k] = 0;
			}
	
	// goalIndex=goalx*N+goaly;
	
	
	temp.ring = initindem;
	temp.sector = initinden;
	temp.gn = 0;							//cost to move from start point to the given cell following the generated path
	temp.hn = euclidean[temp.ring][temp.sector][goalx][goaly];  ////estimated cost from given cell to goal
	temp.fn = temp.gn + temp.hn;
	temp.orientation = 0;
	for (int orr=0;orr<NEIGHBOURS;orr++){
		orient[initindem][initinden][orr] = 0;
		parentM[initindem][initinden][orr] = -1;
		parentN[initindem][initinden][orr] = -1;
	}
	openList.push_front(temp);
	whatList[initindem][initinden][0] = 1;
	
	while(!openList.empty()){
		currentNode = openList.front();
		currentm = currentNode.ring; 
		currentn = currentNode.sector; 
		//if(debugFullPathPlanning)
			//Logger::Instance().WriteMsg("ObstacleAvoidance", "CURRENT m: " + _toString(currentNode.ring) + " n " + _toString(currentNode.sector) + " me cost g" + _toString(currentNode.gn) + " f " + _toString(currentNode.fn) + " kai orient " _toString(currentNode.orientation), Logger::ExtraExtraInfo);
		if(debugFullPathPlanning)
			nodes++;
		if((currentm == goalx) && (currentn == goaly) && (currentNode.orientation == goalorient)){ //    ){  //
			pathFound = 1;
			/*for(int i=0;i<M+3;i++)
				for(int j=0;j<N;j++)
					for(int k=0; k< WATCHDIRECTION; k++){
						if(parent[i][j][k] > -1)
							Logger::Instance().WriteMsg("ObstacleAvoidance", "three i " + _toString(i) + " j " + _toString(j) + " k " + _toString(k) + " parent: " + _toString(parent[i][j][k]/N) + " " + _toString(parent[i][j][k]%N) + " " + _toString(orient[(int)parent[i][j][k]/N][(int)parent[i][j][k]%N][k]), Logger::ExtraExtraInfo);
					}*/
			//if target == first cell
			if(parentM[goalx][goaly][goalorient] == -1 && parentN[goalx][goaly][goalorient] == -1) {
				waypointx[1] = goalx;
				waypointy[1] = goaly;
				waypointOr[1] = goalorient;
				return;
			}
			reconstructPathNew(currentm, currentn, currentNode.orientation);
			counterPath = 0;
			if (cvHighgui)
				cvDrawGrid();
			return;
		}
		
		//remove from openlist
		openList.pop_front();
		
		//insert to closed list
		whatList[currentm][currentn][currentNode.orientation] = -1;
				
		//expand neighbours
		initIndexes(currentm, currentn);
		startAngle= currentNode.orientation;
		for (int  j=0;j<NEIGHBOURS;j++){
			if((j!=startAngle) && (j!=(startAngle+1)%NEIGHBOURS) && (j!=(startAngle-1+NEIGHBOURS)%NEIGHBOURS) )continue;
			if (indexx[j] <= 1) continue;
			if (indexx[j] > M) continue;
			
			neighbourRing = indexx[j];
			neighbourSector = indexy[j];
			
			//in closed
			if(whatList[neighbourRing][neighbourSector][j] == -1 )
				continue;
			newg = currentNode.gn + euclidean[currentNode.ring][currentNode.sector][neighbourRing][neighbourSector]; //
			if(PolarGrid[current][neighbourRing][neighbourSector] >= 0.8){
				newg += obstacleCost;
				continue;
			}
			
			//if not in open then add in open
			if(whatList[neighbourRing][neighbourSector][j] == 0){
				temp.ring = neighbourRing;
				temp.sector = neighbourSector;
				temp.gn = newg;
				temp.hn =  euclidean[temp.ring][temp.sector][goalx][goaly]; //	//estimated cost from given cell to goal
				temp.fn = temp.gn + temp.hn; 
				parentM[neighbourRing][neighbourSector][j] =currentNode.ring;
				parentN[neighbourRing][neighbourSector][j] =currentNode.sector;
				temp.orientation = j;
				orient[neighbourRing][neighbourSector][j]= currentNode.orientation;
				insertInOpenList(temp, openList);
				whatList[neighbourRing][neighbourSector][j] = 1;
			}
			else{
				list<OpenListNode>::iterator it1;
				for (it1=openList.begin(); it1!=openList.end(); ++it1)
					if(((*it1).ring == neighbourRing) && ((*it1).sector == neighbourSector) && ((*it1).orientation == j) )
						break;
				if(debugFullPathPlanning)
					cout << " was in open " << " M " << ((*it1).ring)  << " N " << ((*it1).sector) << " ori " << (*it1).orientation << endl;
				if((*it1).gn > newg){
					openList.erase(it1);
					temp.ring = neighbourRing;
					temp.sector = neighbourSector;
					temp.gn = newg;
					temp.hn =  euclidean[temp.ring][temp.sector][goalx][goaly]; //	//estimated cost from given cell to goal
					temp.fn = temp.gn + temp.hn; 
					parentM[neighbourRing][neighbourSector][j] =currentNode.ring;
					parentN[neighbourRing][neighbourSector][j] =currentNode.sector;
					temp.orientation = j;
					orient[neighbourRing][neighbourSector][j] = currentNode.orientation;
					insertInOpenList(temp, openList);
					whatList[neighbourRing][neighbourSector][j] = 1;
				}
			}	
		}
	}
	///if(pathFound == 0)
		//astarSimple(goalx, goaly);
}


void ObstacleAvoidance::astar5Neighbours(int goalx, int goaly, int goalorient){
	cout << " five neighbours " << endl;
	int startAngle = 0, nodes=0, indexNeighbour=0, neighbourSector, neighbourRing, currentm, currentn;
	int initinden = FRONT, initindem = 1, pathFound = 0, indexCell=0;
	double  newg;
	OpenListNode temp, currentNode;
	list<OpenListNode> openList;
	
	for(int i=0;i<M+3;i++)
		for(int j=0;j<N;j++)
			for(int k=0; k< WATCHDIRECTION; k++){
				whatList[i][j][k] = 0;
				parentM[i][j][k] = -1;
				parentN[i][j][k] = -1;
				orient[i][j][k] = 0;
			}

	temp.ring = initindem;
	temp.sector = initinden;
	temp.gn = 0;							//cost to move from start point to the given cell following the generated path
	temp.hn = euclidean[temp.ring][temp.sector][goalx][goaly];  ////estimated cost from given cell to goal
	temp.fn = temp.gn + temp.hn;
	temp.orientation = 0;
	for (int orr=0;orr<NEIGHBOURS;orr++){
		orient[initindem][initinden][orr] = 0;
		parentM[initindem][initinden][orr] = -1;
		parentN[initindem][initinden][orr] = -1;
	}
	openList.push_front(temp);
	whatList[initindem][initinden][0] = 1;
	
	while(!openList.empty()){
		currentNode = openList.front();
		currentm = currentNode.ring;  
		currentn = currentNode.sector; 
		//if(debugFullPathPlanning)
			//Logger::Instance().WriteMsg("ObstacleAvoidance", "CURRENT m: " + _toString(currentNode.ring) + " n " + _toString(currentNode.sector) + " me cost g" + _toString(currentNode.gn) + " f " + _toString(currentNode.fn) + " kai orient " _toString(currentNode.orientation), Logger::ExtraExtraInfo);
		nodes++;
		if((currentm == goalx) && (currentn == goaly) && (currentNode.orientation == goalorient)){ //    ){  //
			pathFound = 1;
			//if target == first cell
			if(parentM[goalx][goaly][goalorient] == -1 && parentN[goalx][goaly][goalorient] == -1) {
				waypointx[1] = goalx;
				waypointy[1] = goaly;
				waypointOr[1] = goalorient;
				return;
			}
			reconstructPathNew(currentm, currentn, currentNode.orientation);
			counterPath = 0;
			if (cvHighgui)
				cvDrawGrid();
			return;
		}
		
		//remove from openlist
		openList.pop_front();
		
		//insert to closed list
		whatList[currentm][currentn][currentNode.orientation] = -1;
				
		//expand neighbours
		initIndexes(currentm, currentn);
		startAngle= currentNode.orientation;
		for (int  j=0;j<NEIGHBOURS+2;j++){
			initIndexes(currentm, currentn);
			if (j < NEIGHBOURS){
				if((j!=startAngle) && (j!=(startAngle+1)%NEIGHBOURS) && (j!=(startAngle-1+NEIGHBOURS)%NEIGHBOURS) )continue;
				if (indexx[j] < 1) continue;
				if (indexx[j] > M) continue;
			}
			
			if(j >= NEIGHBOURS){
				if (j==NEIGHBOURS){
					indexCell = 6;
				}
				if (j==9){
					indexCell = 2;
				}
			}
			else indexCell = j;
			
			
			
			indexNeighbour = indexx[indexCell]*N+indexy[indexCell];
			neighbourRing = indexx[indexCell];
			neighbourSector = indexy[indexCell];

			indexCell = (j >= NEIGHBOURS) ? startAngle : j ;
			
			//in closed
			if(whatList[neighbourRing][neighbourSector][indexCell] == -1 )
				continue;
			newg = currentNode.gn + euclidean[currentNode.ring][currentNode.sector][indexx[indexCell]][indexy[indexCell]]; //
			if(PolarGrid[current][neighbourRing][neighbourSector] >= 0.8)
				newg += obstacleCost;
			
			if ((PolarGrid[current][indexx[startAngle]][indexy[startAngle]] >= 0.8) && (j >= NEIGHBOURS)){
				initIndexes(neighbourRing, neighbourSector);
				newg *= 0.95;
			}
			
			//if not in open then add in open
			if(whatList[neighbourRing][neighbourSector][indexCell] != 1){
				temp.ring = neighbourRing;
				temp.sector = neighbourSector;
				temp.gn = newg;
				temp.hn =  euclidean[temp.ring][temp.sector][goalx][goaly]; //	//estimated cost from given cell to goal
				temp.fn = temp.gn + temp.hn; 
				parentM[neighbourRing][neighbourSector][indexCell] =currentNode.ring;
				parentN[neighbourRing][neighbourSector][indexCell] =currentNode.sector;
				temp.orientation = indexCell;
				orient[neighbourRing][neighbourSector][indexCell]= currentNode.orientation;
				insertInOpenList(temp, openList);
				whatList[neighbourRing][neighbourSector][indexCell] = 1;
			}
			else{
				list<OpenListNode>::iterator it1;
				for (it1=openList.begin(); it1!=openList.end(); ++it1)
					if(((*it1).ring == neighbourRing) && ((*it1).sector == neighbourSector) && ((*it1).orientation == indexCell) )
						break;
				if((*it1).gn > newg){
					openList.erase(it1);
					temp.ring = neighbourRing;
					temp.sector = neighbourSector;
					temp.gn = newg;
					temp.hn =  euclidean[temp.ring][temp.sector][goalx][goaly]; //	//estimated cost from given cell to goal
					temp.fn = temp.gn + temp.hn; 
					parentM[neighbourRing][neighbourSector][indexCell] =currentNode.ring;
					parentN[neighbourRing][neighbourSector][indexCell] =currentNode.sector;
					temp.orientation = indexCell;
					orient[neighbourRing][neighbourSector][indexCell] = currentNode.orientation;
					insertInOpenList(temp, openList);
					whatList[neighbourRing][neighbourSector][indexCell] = 1;
				}
			}	
		}
	}
	if(pathFound == 0)
		astarSimple(goalx, goaly);
}


void ObstacleAvoidance::astar11Neighbours(int goalx, int goaly, int goalorient){
	cout << " eleven neighbours " << endl;
	int startAngle = 0, nodes=0, indexNeighbour=0, neighbourSector, neighbourRing, currentm, currentn;
	int initinden = FRONT, initindem = 1, pathFound = 0, onlyTurn = 0;
	int preGoalx=0, preGoaly=0, tempOrient =0;
	double  newg;
	OpenListNode temp, currentNode;
	list<OpenListNode> openList;
	
	for(int i=0;i<M+3;i++)
		for(int j=0;j<N;j++)
			for(int k=0; k< WATCHDIRECTION; k++){
				whatList[i][j][k] = 0;
				parentM[i][j][k] = -1;
				parentN[i][j][k] = -1;
				orient[i][j][k] = 0;
			}
	
	//first node in openlist
	temp.ring = initindem;
	temp.sector = initinden;
	temp.gn = 0;							//cost to move from start point to the given cell following the generated path
	temp.hn = euclideanOrientation[temp.ring][temp.sector][goalx][goaly];  ////estimated cost from given cell to goal
	temp.fn = temp.gn + temp.hn;
	temp.orientation = 0;
	for (int orr=0;orr<NEIGHBOURS;orr++){
		orient[initindem][initinden][orr] = 0;
		parentM[initindem][initinden][orr] = -1;
		parentN[initindem][initinden][orr] = -1;
	}
	openList.push_front(temp);
	whatList[initindem][initinden][0] = 1;
	
	while(!openList.empty()){
		currentNode = openList.front();
		currentm = currentNode.ring;  
		currentn = currentNode.sector; 
		//if(debugFullPathPlanning)
		//Logger::Instance().WriteMsg("ObstacleAvoidance", "CURRENT m: " + _toString(currentNode.ring) + " n " + _toString(currentNode.sector) + " me cost g" + _toString(currentNode.gn) + " f " + _toString(currentNode.fn) + " kai orient " _toString(currentNode.orientation), Logger::ExtraExtraInfo);
		nodes++;
		if((currentm == preGoalx) && (currentn == preGoaly) && (currentNode.orientation == goalorient)){ //    ){  //
			pathFound = 1;
			//if target == first cell
			if(parentM[preGoalx][preGoaly][goalorient] == -1 && parentN[preGoalx][preGoaly][goalorient] == -1) {
				waypointx[1] = goalx;
				waypointy[1] = goaly;
				waypointOr[1] = goalorient;
				return;
			}
			reconstructPathNew(currentm, currentn, currentNode.orientation);
			counterPath = 0;
			if (cvHighgui)
				cvDrawGrid();
			return;
		}
		
		//remove from openlist
		openList.pop_front();
		
		//insert to closed list
		whatList[currentm][currentn][currentNode.orientation] = -1;
		
		//expand neighbours
		initIndexes(currentm, currentn);
		startAngle= currentNode.orientation;
		//out << " eimai m: " << currentNode.ring << " n " << currentNode.sector << " me cost " << currentNode.gn << endl;
		
		
		for (int  j=0;j<=NEIGHBOURS+1;j++){
			if (j==NEIGHBOURS) {
				onlyTurn = (startAngle+1)%NEIGHBOURS;
				indexNeighbour = indexx[NEIGHBOURS]*N+indexy[NEIGHBOURS];
				neighbourRing = currentm;
				neighbourSector = currentn;				
			}
			else if (j==NEIGHBOURS+1) {
				onlyTurn = (startAngle-1+NEIGHBOURS)%NEIGHBOURS;
				indexNeighbour = indexx[NEIGHBOURS]*N+indexy[NEIGHBOURS];
				neighbourRing = currentm;
				neighbourSector = currentn;
			}
			else {
				if((j!=startAngle) && (j!=(startAngle+1)%NEIGHBOURS) && (j!=(startAngle-1+NEIGHBOURS)%NEIGHBOURS)  )continue;
				if (indexx[j] <= 1) continue;
				if (indexx[j] > M) continue;
				indexNeighbour = indexx[j]*N+indexy[j];
				neighbourRing = indexx[j];
				neighbourSector = indexy[j];				
			}
			
			//if (indexx[j] <= 1) continue;
			//if (indexx[j] > M) continue;
			
			//out << " j : " << j << endl;
			
			//in closed
			if(whatList[neighbourRing][neighbourSector][(j>=NEIGHBOURS)?onlyTurn:j] == -1 )
				continue;
			newg = currentNode.gn + euclideanOrientation[currentNode.ring][currentNode.sector][indexx[j]][indexy[j]]; //
			if(PolarGrid[current][neighbourRing][neighbourSector] >= 0.8)
				newg += obstacleCost;
			
			//if not in open then add in open
			if(whatList[neighbourRing][neighbourSector][(j>=NEIGHBOURS)?onlyTurn:j] != 1){
				temp.ring = neighbourRing;
				temp.sector = neighbourSector;
				temp.gn = newg;
				temp.hn =  euclideanOrientation[temp.ring][temp.sector][goalx][goaly]; //	//estimated cost from given cell to goal
				temp.fn = temp.gn + temp.hn; 
				if (j >= NEIGHBOURS)
					tempOrient = onlyTurn;
				else 
					tempOrient = j;
				parentM[neighbourRing][neighbourSector][tempOrient] =currentNode.ring;
				parentN[neighbourRing][neighbourSector][tempOrient] =currentNode.sector;
				temp.orientation = tempOrient;
				orient[neighbourRing][neighbourSector][tempOrient]= currentNode.orientation;
				insertInOpenList(temp, openList);
				whatList[neighbourRing][neighbourSector][tempOrient] = 1;

				//out << "add in open m: " << temp.ring << " n " << temp.sector << " cost " << temp.gn << endl;
			}
			else{
				//out << " was in open " << endl;
				list<OpenListNode>::iterator it1;
				for (it1=openList.begin(); it1!=openList.end(); ++it1)
					if(((*it1).ring == neighbourRing) && ((*it1).sector == neighbourSector) && ((*it1).orientation == (j>=NEIGHBOURS)?onlyTurn:j) )
						break;
				if((*it1).gn > newg){
					//out << " kai paw : " << ((*it1).ring) << " " <<  ((*it1).sector) << " me cost prin " << ((*it1).gn) << " logo m: " << (parent[(*it1).ring]) << " n " << (parent[(*it1).sector]) ;
					openList.erase(it1);
					temp.ring = neighbourRing;
					temp.sector = neighbourSector;
					temp.gn = newg;
					temp.hn =  euclideanOrientation[temp.ring][temp.sector][goalx][goaly]; //	//estimated cost from given cell to goal
					temp.fn = temp.gn + temp.hn; 
	
					if (j >= NEIGHBOURS)
						tempOrient = onlyTurn;
					else 
						tempOrient = j;
					parentM[neighbourRing][neighbourSector][tempOrient] =currentNode.ring;
					parentN[neighbourRing][neighbourSector][tempOrient] =currentNode.sector;
					temp.orientation = tempOrient;
					orient[neighbourRing][neighbourSector][tempOrient] = currentNode.orientation;
					insertInOpenList(temp, openList);
					whatList[neighbourRing][neighbourSector][tempOrient] = 1;
					//out << " kai cost meta " << ((*it1).gn) << " kai cell m: " << (((*it1).ring)) << " n " << (((*it1).sector)) << endl;
				}
			}	
		}
	}
}

void ObstacleAvoidance::initEuclidean(){
	double fromx, fromy, tox, toy;
	for(int i=0;i<(M+3);i++)
		for(int k=0;k<N;k++)
			for(int j=0;j<(M+3);j++)
				for(int l=0;l<N;l++){
					if(i<=1 && j<=1){
						euclidean[i][k][j][l] = 0.0;
						continue;
					}
					fromx = (i<=1)?0.0:cellCenterX[i][k];
					fromy = (i<=1)?0.0:cellCenterY[i][k];
					tox = (j<=1)?0.0:cellCenterX[j][l];
					toy = (j<=1)?0.0:cellCenterY[j][l];
					euclidean[i][k][j][l] = sqrt((fromx-tox)*(fromx-tox) + (fromy-toy)*(fromy-toy));// + fabs(atan2(toy-fromy, tox-fromx));
					//euclideanOrientation[i][k][j][l] = sqrt((fromx-tox)*(fromx-tox) + (fromy-toy)*(fromy-toy) + (M_PI/4*M_PI/4) );
					//Logger::Instance().WriteMsg("ObstacleAvoidance", "from m: " + _toString(i) +" n: " + _toString(k) +" to m: " + _toString(j) + " n " + _toString(l) + " eucl " + _toString(euclidean[i][k][j][l]) + " euclidenaor " + _toString(euclideanOrientation[i][k][j][l]), Logger::ExtraExtraInfo);
				}
}

void ObstacleAvoidance::astarSimple(int goalx, int goaly){
	cout << " simple neighbours " << endl;
	int startAngle = 0, nodes=0, neighbourSector, neighbourRing, currentm, currentn;
	int initinden = FRONT, initindem = 1, pathFound = 0;
	double  newg;
	OpenListNode temp, currentNode;
	list<OpenListNode> openList;
	
	for(int i=0;i<M+3;i++)
		for(int j=0;j<N;j++){
			int k=0;
			//for(int k=0; k< WATCHDIRECTION; k++){
				whatList[i][j][k] = 0;
				parentM[i][j][k] = -1;
				parentN[i][j][k] = -1;
				orient[i][j][k] = 0;
			}
	
	
	temp.ring = initindem;
	temp.sector = initinden;
	temp.gn = 0;							//cost to move from start point to the given cell following the generated path
	temp.hn = euclidean[initindem][initinden][goalx][goaly];  ////estimated cost from given cell to goal
	temp.fn = temp.gn + temp.hn;
	temp.orientation = 0;
	//for (int orr=0;orr<NEIGHBOURS;orr++){
		orient[initindem][initinden][0] = 0;
		parentM[initindem][initinden][0] = -1;
		parentN[initindem][initinden][0] = -1;
	//}
	openList.push_front(temp);
	whatList[initindem][initinden][0] = 1;
	
	//for (int foo=0;foo<10;foo++){
	while(!openList.empty()){
		//cout << "foo " << foo << endl;
		currentNode = openList.front();
		currentm = currentNode.ring;  
		currentn = currentNode.sector; 
		//cout << "current m " << currentm << " n " << currentn << endl;
		//if(debugFullPathPlanning)
			//Logger::Instance().WriteMsg("ObstacleAvoidance", "CURRENT m: " + _toString(currentNode.ring) + " n " + _toString(currentNode.sector) + " me cost g" + _toString(currentNode.gn) + " f " + _toString(currentNode.fn) + " kai orient " _toString(currentNode.orientation), Logger::ExtraExtraInfo);
		nodes++;
		if((currentm == goalx) && (currentn == goaly) ){ //    ){  //
			pathFound = 1;
			cout  << "FOUND!!!! " << endl;
			reconstructPath(currentm, currentn);
			counterPath = 0;
			if (cvHighgui)
				cvDrawGrid();
			return;
		}
		
		//remove from openlist
		openList.pop_front();
		
		//insert to closed list
		whatList[currentm][currentn][0] = -1;
				
		//expand neighbours
		initIndexes(currentm, currentn);
		startAngle= 0;
		for (int  j=0;j<NEIGHBOURS;j++){
		//	if((j!=startAngle) && (j!=(startAngle+1)%NEIGHBOURS) && (j!=(startAngle-1+NEIGHBOURS)%NEIGHBOURS) )continue;
			if (indexx[j] <= 1) continue;
			if (indexx[j] > M) continue;
		//	indexNeighbour = indexx[j]*N+indexy[j];
			neighbourRing = indexx[j];
			neighbourSector = indexy[j];
			//in closed
			if(whatList[neighbourRing][neighbourSector][0] == -1 )
				continue;
			newg = currentNode.gn + euclidean[currentNode.ring][currentNode.sector][indexx[j]][indexy[j]]; //
			if(PolarGrid[current][neighbourRing][neighbourSector] >= 0.8)
				newg += obstacleCost;
					
			//if not in open then add in open
			if(whatList[neighbourRing][neighbourSector][0] != 1){
				temp.ring = neighbourRing;
				temp.sector = neighbourSector;
				temp.gn = newg;
				temp.hn =  euclidean[temp.ring][temp.sector][goalx][goaly]; //	//estimated cost from given cell to goal				
				temp.fn = temp.gn + temp.hn; 
				parentM[neighbourRing][neighbourSector][0] =currentNode.ring;
				parentN[neighbourRing][neighbourSector][0] =currentNode.sector;
				temp.orientation = 0;
				orient[neighbourRing][neighbourSector][0]= currentNode.orientation;
				insertInOpenList(temp, openList);
				whatList[neighbourRing][neighbourSector][0] = 1;
			}
			else{
				list<OpenListNode>::iterator it1;
				for (it1=openList.begin(); it1!=openList.end(); ++it1)
					if(((*it1).ring == neighbourRing) && ((*it1).sector == neighbourSector) )
						break;
				if((*it1).gn > newg){
					openList.erase(it1);
					temp.ring = neighbourRing;
					temp.sector = neighbourSector;
					temp.gn = newg;
					temp.hn =  euclidean[temp.ring][temp.sector][goalx][goaly]; //	//estimated cost from given cell to goal
					temp.fn = temp.gn + temp.hn; 
					parentM[neighbourRing][neighbourSector][0] =currentNode.ring;
					parentN[neighbourRing][neighbourSector][0] =currentNode.sector;
					temp.orientation = 0;
					orient[neighbourRing][neighbourSector][0] = currentNode.orientation;
					insertInOpenList(temp, openList);
					whatList[neighbourRing][neighbourSector][0] = 1;
				}
			}	
		}
	}
	cout << " OUT!! " << endl;
	//if(pathFound == 0)
		//astarPlain(goalx, goaly);
}


void ObstacleAvoidance::astarPlain(int goalx, int goaly){
	if(debugModeCout)
		Logger::Instance().WriteMsg("ObstacleAvoidance", "Plain astar no orientation... ", Logger::ExtraExtraInfo);

	int startAngle = 0, indexNeighbour=0, neighborRing, neighborSector, currentm, currentn;
	int initinden = FRONT, initindem = 1;
	double newg;
	OpenListNode temp, currentNode, temp1;
list<OpenListNode> openList;

	if(debugModeCout)
		Logger::Instance().WriteMsg("ObstacleAvoidance", "ASTAR PLAIN "  , Logger::ExtraExtraInfo);
	for (int i=0;i<(M+3)*N;i++)
		for(int j=0; j< WATCHDIRECTION; j++){
			whatList[i][j][0] = 0;
			parentM[i][j][0] = -1;
			parentN[i][j][0] = -1;
			orient[i][j][0] = 0;
		}
	
	temp.ring = initindem;
	temp.sector = initinden;
	temp.gn = 0;							//cost to move from start point to the given cell following the generated path
	temp.hn =euclidean[temp.ring][temp.sector][goalx][goaly];	//estimated cost from given cell to goal
	temp.fn = temp.gn + temp.hn;
	parentM[initindem][initinden][0] =-1;
	parentN[initindem][initinden][0] =-1;
	temp.orientation =0;
	openList.push_front(temp);
	whatList[initindem][initinden][0] = 1;
	
	while(!openList.empty()){
		currentNode = openList.front(); 
		currentm = currentNode.ring; 
		currentn = currentNode.sector; 
		if(currentm == goalx && currentn == goaly){
			//for(int i=0;i<M+3;i++)
				//for(int j=0;j<N;j++)
					//if(parent[i][j][0] > 0)
						//Logger::Instance().WriteMsg("ObstacleAvoidance", "plain i " + _toString(i) + " j " + _toString(j) + " k " + _toString(0) + " parent: " + _toString(parent[i][j][0]/N) + " " + _toString(parent[i][j][0]%N) + " " + _toString(orient[(int)parent[i][j][0]/N][(int)parent[i][j][0]%N][0]), Logger::ExtraExtraInfo);
			reconstructPath(currentm, currentn);
			//counterPath = 0;
			if (cvHighgui)
				cvDrawGrid();
			return;
		}
		//remove from openlist
		openList.pop_front();
		
		//insert to closed list
		whatList[currentm][currentn][0] = -1;
		
		//expand neighbours
		initIndexes(currentm, currentn);
		startAngle= currentNode.orientation;
		if(debugFullPathPlanning)
			Logger::Instance().WriteMsg("ObstacleAvoidance", "eimai m:  " + _toString(currentNode.ring) + " n " + _toString(currentNode.sector) + " me cost " + _toString( currentNode.gn), Logger::ExtraExtraInfo);
			
		for (int  j=0;j<NEIGHBOURS;j++){
			
			//if( j!= (( startAngle+1 < NEIGHBOURS )?startAngle+1:NEIGHBOURS-startAngle-1) &&
				//j!= (( startAngle-1 >= 0 )?startAngle-1:NEIGHBOURS+startAngle-1) &&
				//j!=  startAngle )  continue;
			if (indexx[j] <= 1) continue;
			if (indexx[j] > M) continue;
			indexNeighbour = indexx[j]*N+indexy[j];
			neighborRing = indexx[j];
			neighborSector = indexy[j];
			//in closed or not walkable
			
			if(whatList[neighborRing][neighborSector][0] == -1 )//|| PolarGrid[current][indexx[j]][indexy[j]] >= 0.8)
				continue;
			newg = currentNode.gn +euclidean[currentNode.ring][currentNode.sector][indexx[j]][indexy[j]]; 
			if(PolarGrid[current][neighborRing][neighborSector] >= 0.8)
				newg += obstacleCost;
			//if not in open then add in open
			
			if(whatList[neighborRing][neighborSector][0] != 1){
				temp.ring = neighborRing;
				temp.sector = neighborSector;
				temp.gn = euclidean[currentNode.ring][currentNode.sector][indexx[j]][indexy[j]]; 
				if(PolarGrid[current][neighborRing][neighborSector] >= 0.8)
					temp.gn += obstacleCost;
				temp.hn = euclidean[indexx[j]][indexy[j]][goalx][goaly]; 	//estimated cost from given cell to goal
				temp.fn = temp.gn + temp.hn; 
				temp.orientation = j;
				insertInOpenList(temp, openList);
				parentM[neighborRing][neighborSector][0] =currentNode.ring;
				parentN[neighborRing][neighborSector][0] =currentNode.sector;
				whatList[neighborRing][neighborSector][0] = 1;
				if(debugFullPathPlanning)
					Logger::Instance().WriteMsg("ObstacleAvoidance", "add in open m: " + _toString(temp.ring) + " n " + _toString(temp.sector) + " me cost " + _toString(temp.gn), Logger::ExtraExtraInfo);
			}
			else{
				if(debugFullPathPlanning)
					Logger::Instance().WriteMsg("ObstacleAvoidance"," was in open ", Logger::ExtraExtraInfo);
				list<OpenListNode>::iterator it1;
				for (it1=openList.begin(); it1!=openList.end(); ++it1)
					if((*it1).ring == neighborRing && (*it1).sector == neighborSector)
						break;
				temp1 = (*it1);
				if(temp1.gn > newg){
					//if(debugFullPathPlanning)
						//Logger::Instance().WriteMsg("ObstacleAvoidance", " kai paw : " + _toString( temp1.ring ) + " " + + _toString( temp1.sector ) + " me cost prin " + _toString(temp1.gn ) + " logo m: " + _toString(parentM[temp1.ring][temp1.sector][0]) +  " n " + _toString(parentN[temp1.ring][temp1.sector][0]%N), Logger::ExtraExtraInfo);
					parentM[temp1.ring][temp1.sector][0] =currentNode.ring;
					parentN[temp1.ring][temp1.sector][0] =currentNode.sector;
					temp1.gn = newg;
					temp1.fn = temp1.hn + temp1.gn;
					//if(debugFullPathPlanning)
						//Logger::Instance().WriteMsg("ObstacleAvoidance", " kai cost meta : " + _toString( temp1.gn) + " kai cell m: " + _toString(temp1.ring) + " n: " + _toString(temp1.sector), Logger::ExtraExtraInfo);
					openList.erase(it1);
					insertInOpenList(temp1, openList);
					whatList[temp1.ring][temp1.sector][0] = 1;
				}
			}
			
		}
	}
}


void ObstacleAvoidance::velocityWalk(double ix, double iy, double it, double f)
{
	if(debugModeCout)
		Logger::Instance().WriteMsg("ObstacleAvoidance", "velocity walk x" + _toString(ix) + " y " + _toString(iy) + " t " + _toString(it) + " f " + _toString(f) , Logger::ExtraExtraInfo);
	
	double x, y, t;
	x = ix;
	y = iy;
	t = it;
	
	wmot->set_command("setWalkTargetVelocity");
	
	if ( (x==0.0) && (y==0.0) && (t==0.0) ) {
		cX = 0.0;
		cY = 0.0;
		ct = 0.0;
		
	} 
	else {
		if( lastwalk + milliseconds(200) > microsec_clock::universal_time() )
			return;

		x = x>+1.0?+1.0:x;
		x = x<-1.0?-1.0:x;
		y = y>+1.0?+1.0:y;
		y = y<-1.0?-1.0:y;
		t = t>+1.0?+1.0:t;
		t = t<-1.0?-1.0:t;
		cX = 0.25*cX+0.75*x;
		cY = 0.25*cY+0.75*y;
		ct = 0.25*ct+0.75*t;
		
	}

	wmot->set_parameter(0, cX);
	wmot->set_parameter(1, cY);
	wmot->set_parameter(2, ct);
	wmot->set_parameter(3, f);
	if(debugModeCout)
		Logger::Instance().WriteMsg("ObstacleAvoidance", "publish: cx" + _toString(cX) + " cy " + _toString(cY) + " ct " + _toString(ct) +  " f " + _toString(f)  , Logger::ExtraExtraInfo);

	_blk->publishSignal(*wmot, "motion");

	lastwalk = microsec_clock::universal_time();
}

void ObstacleAvoidance::callVelocityWalk(double waypointX, double waypointY, double waypointT, double distance2Goal){
	static double X=0.0, Y=0.0, t=0.0, f=1.0, gain = 1.0;
	double maxd = fmaxf(fabs(waypointX),fabs(waypointY));
	if (maxd == 0.0 ) maxd = 1.0; 
	f    = fminf(1.0, 0.4+(distance2Goal/0.4));
	gain = fminf(1.0, 0.0+(distance2Goal/0.4));
	X = gain*(waypointX)/maxd;
	Y = gain*(waypointY)/maxd;
	t = gain * (waypointT/M_PI);
	Logger::Instance().WriteMsg("ObstacleAvoidance", "x: " + _toString(X) + " y " + _toString(Y) + " t " + _toString(t) + " f " + _toString(f), Logger::ExtraExtraInfo);
	velocityWalk(X,Y,t,f);
}



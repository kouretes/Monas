#include "ObstacleAvoidance.h"
#include "tools/toString.h"
#include "tools/XMLConfig.h"
#include "tools/logger.h"
#include <iostream>
#include <string>

#include <pthread.h>
#include <netinet/in.h>
#include <csignal>
#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>

using namespace boost::posix_time;

ACTIVITY_REGISTER(ObstacleAvoidance);

/*
 *
 * check for move grid!!!!changed the fornt expansion
 *
 *
 * */

/*********** Math functions ***********/

using namespace KMath;

void ObstacleAvoidance::UserInit()
{
	_blk.updateSubscription("sensors", msgentry::SUBSCRIBE_ON_TOPIC);
	_blk.updateSubscription("behavior", msgentry::SUBSCRIBE_ON_TOPIC);
	_blk.updateSubscription("obstacle", msgentry::SUBSCRIBE_ON_TOPIC);
	_blk.updateSubscription("buttonevents", msgentry::SUBSCRIBE_ON_TOPIC);
	lastAge = microsec_clock::universal_time();
	lastwalk = microsec_clock::universal_time();
	present = 0;
	past = 1;
	/****** path planning *******/
	pathCounter = 0;
	relativeMode = 0;
	/******** odometry *******/
	targetX = 0.0;
	targetY = 0.0;
	targetA = 0.0;
	odometryX = 0.0;
	odometryY = 0.0;
	odometryA = 0.0;
	initializeOdometry = true;
	/******** debug ********/
	countLeft = 0;
	countRight = 0;
	SonarFailCounter = 0; // counter gia fail twn sonars
	debugCounter = 0;
	debugModeCout = 0;

	//boh8htikos pinakas gia update
	for(int i = 0; i < KDeviceLists::US_SIZE; i++)
		empty[i] = 0.0;

	/********* Messages *********/
	frontObstacle = false;
	rightObstacle = false;
	leftObstacle = false;
	frontDist = 0.0;
	rightDist = 0.0;
	leftDist = 0.0;
	frontCert = 0.0;
	rightCert = 0.0;
	leftCert = 0.0;
	obavm.add_direction(0);
	obavm.add_direction(0);
	obavm.add_direction(0);
	obavm.add_distance(0);
	obavm.add_distance(0);
	obavm.add_distance(0);
	obavm.add_certainty(0);
	obavm.add_certainty(0);
	obavm.add_certainty(0);

	for(int i = 0; i < TotalRings; i++)
		for(int j = 0; j < N; j++)
			gridInfoMessage.add_gridcells(0);

	for(int i = 0; i < NumOfTargetCoordinates; i++)
		gridInfoMessage.add_targetcoordinates(0);

	for(int i = 0; i < PathLength; i++)
	{
		gridInfoMessage.add_pathstepsring(0);
		gridInfoMessage.add_pathstepssector(0);
		gridInfoMessage.add_pathstepsorientation(0);
	}

	pprm_rel = new PathPlanningRequestMessage();
	wmot = new MotionWalkMessage();
	wmot->add_parameter(0.0f);
	wmot->add_parameter(0.0f);
	wmot->add_parameter(0.0f);
	wmot->add_parameter(0.0f);
	cX = 0.0;
	cY = 0.0;
	ct = 0.0;
	bd = 0.0;
	/******** Initialize **********/
	initGrid();		//arxikopoihsh twn pinakwn
	Initialize();
	/*if (cvHighgui)
	{
		Logger::Instance().WriteMsg("ObstacleAvoidance", "Enable highgui", Logger::Info);
		img = cvCreateImage(cvSize(ImgSize, ImgSize), IPL_DEPTH_8U, 3);
		cvNamedWindow("img", CV_WINDOW_AUTOSIZE);
	}*/
}

void ObstacleAvoidance::Reset(){
	originX = robotX;
	originY = robotY;
	originA = robotA;
	relativeMode = 1;
	targetX_0 = testPathX;
	targetY_0 = testPathY;
	targetA_0 = -M_PI_2;
	double targetD_0 = sqrt((targetX_0) * (targetX_0) + (targetY_0) * (targetY_0));
	double targetT_0 = atan2(targetY_0, targetX_0);
	targetX_0 = targetD_0 * cos(targetT_0 + originA);
	targetY_0 = targetD_0 * sin(targetT_0 + originA);
	targetA_0 = wrapToPi(targetA_0 + originA);
}

int ObstacleAvoidance::Execute()
{
	read_messages();
	//xrhsh gia elegxo ean to array me tis times twn sonars einai 0 = sfalma
	countLeft = 0;
	countRight = 0;
	double Right[KDeviceLists::US_SIZE], Left[KDeviceLists::US_SIZE];
	initPaths();

	//(RobotPositionSensor Message) find current robot position
	if(rpm != 0)
	{
		PosX = rpm->sensordata(KDeviceLists::ROBOT_X);
		PosY = rpm->sensordata(KDeviceLists::ROBOT_Y);
		Angle = rpm->sensordata(KDeviceLists::ROBOT_ANGLE);

		if(debugModeCout)
			Logger::Instance().WriteMsg("ObstacleAvoidance", "PosX: " + _toString(PosX.sensorvalue()) + " PosY: " + _toString(PosY.sensorvalue()) + " Angle: " + _toString(Angle.sensorvalue()) , Logger::Info);

		odometryX = PosX.sensorvalue();
		odometryY = PosY.sensorvalue();
		odometryA = Angle.sensorvalue();

		if (initializeOdometry)
		{
			initializeOdometry = false;
			robotX = odometryX;
			robotY = odometryY;
			robotA = odometryA;

			if (debugRelativeMode == 1)
			{
				Reset();
				pathPlanningRequestRelative(0.0, 0.0, 0.0);
			}
		}
		else
			moveRobot();
	}

	//update the grid with the new sonar values
	if (asvm != 0)
	{
		for (int j = KDeviceLists::US_SIZE - 1; j >= 0; j--)
		{
			LeftValue[j] = asvm->sensordata(KDeviceLists::L_US + j);
			Left[j] = LeftValue[j].sensorvalue();
			RightValue[j] = asvm->sensordata(KDeviceLists::R_US + j);
			Right[j] = RightValue[j].sensorvalue();
			countLeft = countLeft + LeftValue[j].sensorvalue();
			countRight = countRight + RightValue[j].sensorvalue() ;
		}

		if(debugModeCout)
			printSonarValues();

		if(countLeft == 0 && countRight == 0)
		{
			SonarFailCounter++;

			if(SonarFailCounter > 10)
				Logger::Instance().WriteMsg("ObstacleAvoidance", "sonars fail", Logger::Info);
		}
		else
			updateGrid(countLeft != 0 ? Left : empty, countRight != 0 ? Right : empty);

		chooseCloserObstacle();
	}

	//path planning messsage
	if(pprm != 0)
	{
		double walkToX, walkToY, walkToT, distance2Goal;
		double targetZeroOrientation = 0.0;
		double deltaRelative[3] = {0.0, 0.0, 0.0};
		double r, theta;
		double deltaX, deltaY, deltaA, targetD, targetT;

		if (relativeMode == 1 && (pprm->mode()).compare("absolute") == 0)
			relativeMode = 0;

		if (debugRelativeMode == 1)
		{
			deltaX = (PosX.sensorvalue() - originX);
			deltaY = (PosY.sensorvalue() - originY);
			deltaA = anglediff2(Angle.sensorvalue(), originA);
			targetX = targetX_0 - deltaX;
			targetY = targetY_0 - deltaY;
			targetD = sqrt((targetX) * (targetX) + (targetY) * (targetY));
			targetT = anglediff2(atan2(targetY, targetX), deltaA + originA);
			targetX = targetD * cos(targetT);
			targetY = targetD * sin(targetT);
			targetA = anglediff2(targetA_0, deltaA + originA);
			Logger::Instance().WriteMsg("ObstacleAvoidance", " delta X: " + _toString(deltaX) + " Y: " + _toString(deltaY) + " A: " + _toString(deltaA) + " D: " + _toString(deltaA) + " T: " + _toString(deltaA), Logger::Info );
			pathPlanningRequestRelative(0.0, 0.0, 0.0);
		}
		else
		{
			targetX = pprm->gotox();
			targetY = pprm->gotoy();
			targetA = pprm->gotoangle();
		}

		distance2Goal = sqrt((targetX) * (targetX) + (targetY) * (targetY));

		if(debugModeCout)
			Logger::Instance().WriteMsg("ObstacleAvoidance", " targetX: " + _toString(targetX) + " targetY: " + _toString(targetY) + " targetA: " + _toString(targetA), Logger::Info );

		if( distance2Goal > GoalDistanceTolerance && fabs(anglediff2(targetA, robotA)) > GoalAngleTolerance)   //|| targetA > M_PI_4 ){
		{
			//eykleidia apostash kai orientation an mikrotero apla kalv velocityWalk
			targetR = XYtoR(targetX, targetY);
			targetS = XYtoS(targetX, targetY);
			double targetZeroOrientation = wrapTo0_2Pi((targetS + 0.5) * SectorAngleRad - M_PI_2);
			targetO = int( wrapTo0_2Pi( anglediff(targetA, targetZeroOrientation) + 0.5 * M_PI_4 ) / M_PI_4 );

			if (targetR > OuterRing)
			{
				targetR = OuterRing;
				targetO =  0;
			}

			if (targetR < InnerRing)
				targetR = InnerRing;

			if (targetR == InnerRing)
				targetO = 0;

			//Logger::Instance().WriteMsg("ObstacleAvoidance",  " targetR " + _toString(targetR) + " targetS " + _toString(targetS) + " targetO " + _toString(targetO), Logger::Info );
			//Logger::Instance().WriteMsg("ObstacleAvoidance",  " targetR " + _toString(int(targetR)) + " targetS " + _toString(int(targetS)) + " targetO " + _toString(int(targetO)), Logger::Info );
			astar13Neighbours(int(targetR), int(targetS),  int(targetO));
			motionController(distance2Goal);
		}
		else
		{
			if (relativeMode == 1)
			{
				walkToX = 0.0;
				walkToY = 0.0;
				walkToT = 0.0;
			}
			else
			{
				walkToX = targetX;
				walkToY = targetY;
				walkToT = targetA;
			}

			if(debugModeCout)
				Logger::Instance().WriteMsg("ObstacleAvoidance", "coord waypoint x: " + _toString(walkToX) + " y: " + _toString(walkToY) + " or: " + _toString(walkToT) , Logger::Info);

			callVelocityWalk(walkToX, walkToY, walkToT, distance2Goal);
		}
	}
	else
	{
		targetX = 0.0;
		targetY = 0.0;
		targetA = 0.0;
	}

	//aging grid every 'ageTimerSeconds' seconds (currently 3)
	if (microsec_clock::universal_time() - lastAge >= seconds(ageTimerSeconds))
	{
		ageGrid();
		lastAge = microsec_clock::universal_time();

		if(debugModeCout)
			Logger::Instance().WriteMsg("ObstacleAvoidance", "aging!!", Logger::Info);

	}

	publishObstacleMessage();
	publishGridInfo();
	return 0;
}

int ObstacleAvoidance::DtoR(double d)
{
	if (d < 0)
		return InnerRing;
	else
		return int( d / RingDistance ) + 1; // +1 is used to skip the InnerRing
}

int ObstacleAvoidance::TtoS(double theta)
{
	return int( wrapTo0_2Pi(theta + M_PI_2 + SectorShiftRad) / SectorAngleRad );
}

double ObstacleAvoidance::RtoD(int r)
{
	if (r == InnerRing)
		return 0.0;
	else
		return (r - 1) * RingDistance;
}

double ObstacleAvoidance::StoT(int s)
{
	return wrapToPi(s * SectorAngleRad - M_PI_2 - SectorShiftRad);
}

int ObstacleAvoidance::XYtoR(double x, double y)
{
	return DtoR( toPolarD(x, y) );
}

int ObstacleAvoidance::XYtoS(double x, double y)
{
	return TtoS( toPolarT(x, y) );
}

int ObstacleAvoidance::wrapTo(int n, int MAXN)
{
	while (n < 0) n += MAXN;

	return (n % MAXN);
}

void ObstacleAvoidance::chooseCloserObstacle()
{
	for(int i = 0; i < TotalRings; i++)
	{
		frontObstacle = PolarGrid[present][i][FRONT] > ObstacleThreshold ? true : false;
		frontDist = frontObstacle == true ? ((i + 1) * distanceM) : 0.0;
		frontCert = frontObstacle == true ? PolarGrid[present][i][FRONT] : 0.0;

		if (frontObstacle) break;
	}

	for(int i = 0; i < TotalRings; i++)
	{
		rightObstacle = PolarGrid[present][i][RIGHT] > ObstacleThreshold ? true : false;
		rightDist = rightObstacle == true ? ((i + 1) * distanceM ) : 0.0;
		rightCert = rightObstacle == true ? PolarGrid[present][i][RIGHT] : 0.0;  // TODO: get the central ray

		if (rightObstacle) break;
	}

	for(int i = 0; i < TotalRings; i++)
	{
		leftObstacle = PolarGrid[present][i][LEFT] > ObstacleThreshold ? true : false;
		leftDist = leftObstacle == true ? ((i + 1) * distanceM) : 0.0;
		leftCert = leftObstacle == true ? PolarGrid[present][i][LEFT] : 0.0;  // TODO: get the central ray

		if (leftObstacle) break;
	}
}

void ObstacleAvoidance::printSonarValues()
{
	for(int i = 0; i < KDeviceLists::US_SIZE; i++)
		Logger::Instance().WriteMsg("ObstacleAvoidance", "leftSensor: " + _toString(LeftValue[i].sensorvalue()) + " rightSensor: " + _toString(RightValue[i].sensorvalue()) , Logger::Info);

	Logger::Instance().WriteMsg("ObstacleAvoidance", "debugCounter: " + _toString(debugCounter), Logger::Info);
	debugCounter++;
}

/************** Map Update **************/

void ObstacleAvoidance::mapObstacle(double distance, double theta, double radius)
{
	int minr, maxr, mins, maxs;
	double x, y;

	if ( distance > MapRadius ) return;

	x = toCartesianX(distance, theta);
	y = toCartesianY(distance, theta);
	minr = DtoR(distance - radius);
	minr = max(minr, InnerRing + 1);
	maxr = DtoR(distance + radius);
	maxr = min(maxr, OuterRing - 1);
	mins = TtoS(theta - radius / distance);
	maxs = TtoS(theta + radius / distance);

	if (mins > maxs) maxs += N;

	for (int r = minr; r <= maxr; r++)
		for (int s = mins; s <= maxs; s++)
			if ( toPolarD(cellCenterX[r][wrapTo(s, N)] - x, cellCenterY[r][wrapTo(s, N)] - y) <= radius )
				PolarGrid[present][r][wrapTo(s, N)] = fmin( PolarGrid[present][r][wrapTo(s, N)] * OccupancyIncreaseFactor, MaxOccupancy );
}

void ObstacleAvoidance::mapFreeSpace(double distance, double theta)
{
	int maxr, mins, maxs;
	double x, y;
	x = toCartesianX(distance, theta);
	y = toCartesianY(distance, theta);
	maxr = DtoR(distance);

	if (maxr < InnerRing + 1) return;

	maxr = min(maxr, OuterRing - 1);
	mins = TtoS(theta - 0.5 * SonarAngleRangeRad);
	maxs = TtoS(theta + 0.5 * SonarAngleRangeRad);

	if (mins > maxs) maxs += N;

	for (int r = InnerRing + 1; r <= maxr; r++)
		for (int s = mins; s <= maxs; s++)
			if ( toPolarD(cellCenterX[r][wrapTo(s, N)] - x, cellCenterY[r][wrapTo(s, N)] - y) > ObstacleRadius )
				PolarGrid[present][r][wrapTo(s, N)] = fmax( PolarGrid[present][r][wrapTo(s, N)] * OccupancyDecreaseFactor, MinOccupancy );
}

void ObstacleAvoidance::updateGrid(double* left, double* right)
{
	if (left[0] != 0 )
	{
		if (left[0] <= SonarDistanceRange)
			mapObstacle( left[0] + SonarDistanceShift, +SonarAngleShiftRad, ObstacleRadius );

		if (left[0] <= SonarMinReading)
		{
			//mapObstacle( ObstacleRadius, +SonarAngleShiftRad, ObstacleRadius );
			mapObstacle( CloseObstacleCenter, +SonarAngleShiftRad, CloseObstacleRadius );
		}
		else
			mapFreeSpace( left[0] + SonarDistanceShift, +SonarAngleShiftRad );
	}
	else
		Logger::Instance().WriteMsg("ObstacleAvoidance", "left sonar fail", Logger::ExtraExtraInfo);

	if (right[0] != 0)
	{
		if (right[0] <= SonarDistanceRange)
			mapObstacle( right[0] + SonarDistanceShift, -SonarAngleShiftRad, ObstacleRadius );

		if (right[0] <= SonarMinReading)
		{
			mapObstacle( CloseObstacleCenter, -SonarAngleShiftRad, CloseObstacleRadius );
			//			mapObstacle( ObstacleRadius, -SonarAngleShiftRad, ObstacleRadius );
		}
		else
			mapFreeSpace( right[0] + SonarDistanceShift, -SonarAngleShiftRad );
	}
	else
		Logger::Instance().WriteMsg("ObstacleAvoidance", "right sonar fail", Logger::ExtraExtraInfo);
}

void ObstacleAvoidance::ageGrid()
{
	for (int r = InnerRing + 1; r < OuterRing; r++)
		for (int s = 0; s < N; s++)
			PolarGrid[present][r][s] = (PolarGrid[present][r][s] - NoKnowledge) * AgeFactor + NoKnowledge;
}

/************** Map Transformation **************/

void ObstacleAvoidance::rotateGrid(double angle)
{
	int rotateSectors = int(angle / SectorAngleRad);
	//cout << "   rotate (degrees): " << rad2deg(angle) << "   rotate (sectors): " << rotateSectors << endl;
	past = present;
	present = 1 - present;

	for (int r = 0; r < TotalRings; r++)
		for (int s = 0; s < N; s++)
			PolarGrid[present][r][s] = PolarGrid[past][r][wrapTo(s + rotateSectors, N)];
}

void ObstacleAvoidance::translateGrid(double deltaX, double deltaY)
{
	double xnew, ynew, xold, yold;
	int rnew, snew;
	past = present;
	present = 1 - present;

	// initialization
	for (int r = 0; r < TotalRings; r++)
		for (int s = 0; s < N; s++)
		{
			changed[r][s] = 0;

			if (r == InnerRing)
				PolarGrid[present][r][s] = 0.0;
			else if (r < OuterRing)
				PolarGrid[present][r][s] = PolarGrid[past][r][s];
			else
				PolarGrid[present][r][s] = NoKnowledge;
		}

	// translation
	for (int r = 0; r < TotalRings; r++)
	{
		for (int s = 0; s < N; s++)
		{
			xold = cellCenterX[r][s];
			yold = cellCenterY[r][s];
			xnew = xold - deltaX;
			ynew = yold - deltaY;
			rnew = XYtoR(xnew, ynew);
			snew = XYtoS(xnew, ynew);

			if ( (rnew > InnerRing) && (rnew < OuterRing) )
			{
				if (changed[rnew][snew] == 0)
				{
					PolarGrid[present][rnew][snew] = PolarGrid[past][r][s];
					changed[rnew][snew] = 1;
				}
				else
				{
					PolarGrid[present][rnew][snew] += PolarGrid[past][r][s];
					changed[rnew][snew]++;
				}
			}
		}
	}

	// averaging
	for (int r = InnerRing + 1; r < OuterRing; r++)
		for (int s = 0; s < N; s++)
			if (changed[r][s] > 0)
			{
				PolarGrid[present][r][s] /= changed[r][s];
				changed[r][s] = 0;
			}
}

void ObstacleAvoidance::moveRobot()
{
	double diffX, diffY, diffA;
	//cout << "Robot     : " << robotX << " " << robotY << " " << robotA << endl;
	//cout << "Odometry  : " << odometryX << " " << odometryY << " " << odometryA << endl;
	diffX = odometryX - robotX;
	diffY = odometryY - robotY;
	diffA = anglediff(odometryA, robotA);
	//cout << "DIFF: " << diffX << " " << diffY << " " << diffA << endl;
	double diffD = sqrt(diffX * diffX + diffY * diffY);
	double diffT = atan2(diffY, diffX);
	diffX = diffD * cos(diffT - robotA);
	diffY = diffD * sin(diffT - robotA);
	//cout << "DIFF: " << diffX << " " << diffY << " " << diffA << " " << diffD << " " << diffT << endl;

	if ( fabs(diffD) >= RingDistance )
	{
		//	cout << "TRANSLATION!" << endl;
		translateGrid(diffX, diffY);
		robotX += diffX * cos(robotA);
		robotY += diffX * sin(robotA);
		robotX += diffY * cos(robotA + M_PI_2);
		robotY += diffY * sin(robotA + M_PI_2);
		// could be simply: robotX = odometryX; robotY = odometryY;
	}

	if ( fabs(diffA) >= SectorAngleRad )
	{
		//	cout << "ROTATION!" << endl;
		rotateGrid(diffA);
		robotA += int(diffA / SectorAngleRad) * SectorAngleRad;
	}
}

/************** Path Planning **************/

void ObstacleAvoidance::initAstar()
{
	for (int r = 0; r < TotalRings; r++)
		for (int s = 0; s < N; s++)
			for (int k = 0; k < NEIGHBOURS; k++)
			{
				whatList[r][s][k] = 0;
				parentM[r][s][k] = -1;
				parentN[r][s][k] = -1;
				parentO[r][s][k] = 0;
			}
}

void ObstacleAvoidance::insertInOpenList(OpenListNode anode, list<OpenListNode> &openList)
{
	list<OpenListNode>::iterator i;

	for (i = openList.begin(); i != openList.end(); i++)
		if (anode.fn < (*i).fn)
			break;

	openList.insert(i, anode);
}

void ObstacleAvoidance::generateNode(OpenListNode next, int pr, int ps, int po, list<OpenListNode> &openList)
{
	if (whatList[next.ring][next.sector][next.orientation] == -1) // in closed
		return;

	if (whatList[next.ring][next.sector][next.orientation] == 1)   // in open
	{
		list<OpenListNode>::iterator it;

		for (it = openList.begin(); it != openList.end(); it++)
			if ( ((*it).ring == next.ring) && ((*it).sector == next.sector) && ((*it).orientation == next.orientation) )
				break;

		if ( (*it).gn <= next.gn )
			return;
		else
			openList.erase(it);
	}

	parentM[next.ring][next.sector][next.orientation] = pr;
	parentN[next.ring][next.sector][next.orientation] = ps;
	parentO[next.ring][next.sector][next.orientation] = po;
	insertInOpenList(next, openList);
	whatList[next.ring][next.sector][next.orientation] = 1;
}

void ObstacleAvoidance::findNeighbours(int r, int s)
{
	neighbourRing[0] = (r < OuterRing) ? r + 1 : -1; 	//f
	neighbourRing[1] = (r < OuterRing) ? r + 1 : -1; 	//fl
	neighbourRing[2] = r;				 				//l
	neighbourRing[3] = (r > InnerRing + 1) ? r - 1 : -1;	 	//bl
	neighbourRing[4] = (r > InnerRing + 1) ? r - 1 : -1;	 	//b
	neighbourRing[5] = (r > InnerRing + 1) ? r - 1 : -1;	 	//br
	neighbourRing[6] = r;				 				//r
	neighbourRing[7] = (r < OuterRing) ? r + 1 : -1; 	//fr
	neighbourRing[8] = r;				 				//self
	neighbourSector[0] = s;			//f
	neighbourSector[1] = wrapTo(s + 1, N);	//fl
	neighbourSector[2] = wrapTo(s + 1, N);	//l
	neighbourSector[3] = wrapTo(s + 1, N);	//bl
	neighbourSector[4] = s ;			//b
	neighbourSector[5] = wrapTo(s - 1, N);	//br
	neighbourSector[6] = wrapTo(s - 1, N);	//r
	neighbourSector[7] = wrapTo(s - 1, N);	//fr
	neighbourSector[8] = s;			//self
}

void ObstacleAvoidance::astar13Neighbours(int goalm, int goaln, int goalo)
{
	//Logger::Instance().WriteMsg("ObstacleAvoidance", "*** A* input: r" + _toString(goalm) + " s " + _toString(goaln) + " o " + _toString(goalo) , Logger::ExtraExtraInfo);
	int nodes = 0;
	int startSector = FRONT, startRing = InnerRing;
	double deltaTheta, sideCost;
	list<OpenListNode> openList;
	OpenListNode current, next;
	initAstar();
	next.ring = startRing;
	next.sector = startSector;
	next.orientation = 0;
	next.angle = next.sector * SectorAngleRad;
	next.gn = 0.0;
	next.hn = euclidean[startRing][startSector][goalm][goaln];
	next.fn = next.gn + next.hn;
	generateNode(next, -1, -1, 0, openList);

	while ( !openList.empty() )
	{
		nodes++;
		current = openList.front();
		openList.pop_front();
		whatList[current.ring][current.sector][current.orientation] = -1;

		//Logger::Instance().WriteMsg("ObstacleAvoidance", "Expanding m" + _toString(current.ring) + " n " + _toString(current.sector) + " o " + _toString(current.orientation) + " f " + _toString(current.fn) , Logger::ExtraExtraInfo);
		if ( (current.ring == goalm) && (current.sector == goaln) && (current.orientation == goalo) )
		{
			pathCounter = 0;
			reconstructPath(current.ring, current.sector, current.orientation);
			return;
		}

		// expand current node
		findNeighbours(current.ring, current.sector);

		if (current.ring == InnerRing)
		{
			// left rotation in place
			next.ring = current.ring;
			next.sector = (current.ring == InnerRing) ? wrapTo(current.sector + 1, N) : current.sector;
			next.orientation = (current.ring == InnerRing) ? current.orientation : wrapTo(current.orientation + 1, NEIGHBOURS);
			next.angle = next.sector * SectorAngleRad + next.orientation * M_PI_4;
			deltaTheta = fabs( anglediff(next.angle, current.angle) );
			next.gn = current.gn;
			next.gn += (current.ring == InnerRing) ? RotationCostFactor * SectorAngleRad / (2.0 * M_PI) : RotationCostFactor * deltaTheta / (2.0 * M_PI);
			next.hn = euclidean[next.ring][next.sector][goalm][goaln];
			next.fn = next.gn + next.hn;
			//out << "L" << " m " << next.ring << " n " << next.sector << " o " << next.orientation << " g " << next.gn << " f " << next.fn << " parent m " << current.ring << " n " << current.sector << endl;
			generateNode(next, current.ring, current.sector, current.orientation, openList);
			// right rotation in place
			next.ring = current.ring;
			next.sector = (current.ring == InnerRing) ? wrapTo(current.sector - 1, N) : current.sector;
			next.orientation = (current.ring == InnerRing) ? current.orientation : wrapTo(current.orientation - 1, NEIGHBOURS);
			next.angle = next.sector * SectorAngleRad + next.orientation * M_PI_4;
			deltaTheta = fabs( anglediff(next.angle, current.angle) );
			next.gn = current.gn;
			next.gn += (current.ring == InnerRing) ? RotationCostFactor * SectorAngleRad / (2.0 * M_PI) : RotationCostFactor * deltaTheta / (2.0 * M_PI);
			next.hn = euclidean[next.ring][next.sector][goalm][goaln];
			next.fn = next.gn + next.hn;
			//out << "R" << " m " << next.ring << " n " << next.sector << " o " << next.orientation << " g " << next.gn << " f " << next.fn << " parent m " << current.ring << " n " << current.sector << endl;
			generateNode(next, current.ring, current.sector, current.orientation, openList);
		}

		// if (goalm == 1) continue; // MGL: ayti i synthiki alitheia giati mpike;

		for (int j = 0; j < NEIGHBOURS; j++)
		{
			if (neighbourRing[j] == -1) continue;

			// forward translation with and without rotation
			if ( (j == current.orientation) || (j == wrapTo(current.orientation + 1, NEIGHBOURS)) || (j == wrapTo(current.orientation - 1, NEIGHBOURS)) )
			{
				for (int l = -1; l <= +1; l++)
				{
					next.ring = neighbourRing[j];
					next.sector = neighbourSector[j];
					next.orientation = wrapTo(current.orientation + l, NEIGHBOURS);
					next.angle = next.sector * SectorAngleRad + next.orientation * M_PI_4;
					next.gn = current.gn;

					if (PolarGrid[present][next.ring][next.sector] >= ObstacleThreshold)
						next.gn += ObstacleCostFactor * PolarGrid[present][next.ring][next.sector];

					sideCost = ( (next.orientation != j) && (j != current.orientation) ) ? SemiSideCostFactor : 1.0;
					next.gn += sideCost * euclidean[current.ring][current.sector][next.ring][next.sector];
					deltaTheta = fabs( anglediff(next.angle, current.angle) );
					next.gn += deltaTheta / (2.0 * M_PI);
					next.hn =  euclidean[next.ring][next.sector][goalm][goaln];
					next.fn = next.gn + next.hn;
					//out << "F" << " m " << next.ring << " n " << next.sector << " o " << next.orientation << " g " << next.gn << " f " << next.fn << " parent m " << current.ring << " n " << current.sector << endl;
					generateNode(next, current.ring, current.sector, current.orientation, openList);
				}
			}

			// side steps
			if ( (j == wrapTo(current.orientation + 2, NEIGHBOURS)) || (j == wrapTo(current.orientation - 2, NEIGHBOURS)) )
			{
				if (current.ring == InnerRing)
				{
					next.ring = InnerRing + 1;

					if ( j == wrapTo(current.orientation + 2, NEIGHBOURS) )  	//left
					{
						next.sector = wrapTo(current.sector - FRONT + SIDE_LEFT, N);
						next.orientation = 6;
					}

					if ( j == wrapTo(current.orientation - 2, NEIGHBOURS) )  	//right
					{
						next.sector = wrapTo(current.sector - FRONT + SIDE_RIGHT, N);
						next.orientation = 2;
					}
				}
				else
				{
					next.ring = neighbourRing[j];
					next.sector = neighbourSector[j];
					next.orientation = current.orientation;
				}

				next.angle = next.sector * SectorAngleRad + next.orientation * M_PI_4;
				next.gn = current.gn;

				if (PolarGrid[present][next.ring][next.sector] >= ObstacleThreshold)
					next.gn += ObstacleCostFactor * PolarGrid[present][next.ring][next.sector];

				sideCost = SideStepCostFactor;
				next.gn += sideCost * euclidean[current.ring][current.sector][next.ring][next.sector];
				deltaTheta = fabs(anglediff(next.angle, current.angle));
				next.gn += deltaTheta / (2.0 * M_PI);
				next.hn =  euclidean[next.ring][next.sector][goalm][goaln];
				next.fn = next.gn + next.hn;
				//out << "S" << " m " << next.ring << " n " << next.sector << " o " << next.orientation << " g " << next.gn << " f " << next.fn << " parent m " << current.ring << " n " << current.sector << endl;
				generateNode(next, current.ring, current.sector, current.orientation, openList);
			}
		}
	}

	//cout << "*** A* input: " << "x " << goalm << " y " << goaln << " o " << goalo << endl;
}

void ObstacleAvoidance::reconstructPath(int ring, int sector, int orientation)
{
	//Logger::Instance().WriteMsg("ObstacleAvoidance", "PATH m " + _toString(ring) + " n " + _toString(sector) + " o " + _toString(orientation), Logger::ExtraExtraInfo);
	if (parentM[ring][sector][orientation] != -1 && parentN[ring][sector][orientation] != -1)
		reconstructPath( parentM[ring][sector][orientation], parentN[ring][sector][orientation], parentO[ring][sector][orientation] );

	if (pathCounter >= PathLength) return;

	pathR[pathCounter] = ring;
	pathS[pathCounter] = sector;
	pathO[pathCounter] = orientation;
	pathCounter++;
}

/************** Messages **************/

void ObstacleAvoidance::read_messages()
{
	asvm =  _blk.readData<AllSensorValuesMessage>("sensors");
	rpm =  _blk.readData<RobotPositionMessage>("sensors");
	pprm = _blk.readSignal<PathPlanningRequestMessage>("obstacle");
}

void ObstacleAvoidance::publishObstacleMessage()
{
	if(debugModeCout)
	{
		Logger::Instance().WriteMsg("ObstacleAvoidance", "aristera: " + _toString(leftObstacle) + " dist: " + _toString(leftDist) + " cert: " + _toString(leftCert) , Logger::ExtraExtraInfo);
		Logger::Instance().WriteMsg("ObstacleAvoidance", "mprosta: " + _toString(frontObstacle) + " dist: " + _toString(frontDist) + " cert: " + _toString(frontCert) , Logger::ExtraExtraInfo);
		Logger::Instance().WriteMsg("ObstacleAvoidance", "dexia: " + _toString(rightObstacle) + " dist: " + _toString(rightDist) + " cert: " + _toString(rightCert) , Logger::ExtraExtraInfo);
	}

	obavm.set_direction(0, leftObstacle ? 1 : 0);
	obavm.set_direction(1, frontObstacle ? 1 : 0);
	obavm.set_direction(2, rightObstacle ? 1 : 0);
	obavm.set_distance(0, leftObstacle ? leftDist : SonarDistanceRange);
	obavm.set_distance(1, frontObstacle ? frontDist : SonarDistanceRange);
	obavm.set_distance(2, rightObstacle ? rightDist : SonarDistanceRange);
	obavm.set_certainty(0, leftObstacle ? leftCert : 0.0);
	obavm.set_certainty(1, frontObstacle ? frontCert : 0.0);
	obavm.set_certainty(2, rightObstacle ? rightCert : 0.0);
	_blk.publishState(obavm, "obstacle");
}

void ObstacleAvoidance::publishGridInfo()
{
	for(int i = 0; i < TotalRings; i++)
		for(int j = 0; j < N; j++)
			gridInfoMessage.set_gridcells(i * N + j, PolarGrid[present][i][j]);

	gridInfoMessage.set_targetcoordinates(0, targetX);
	gridInfoMessage.set_targetcoordinates(1, targetY);
	gridInfoMessage.set_targetcoordinates(2, targetA);

	for (int step = 0; step < PathLength; step++)
	{
		gridInfoMessage.set_pathstepsring(step, pathR[step]);
		gridInfoMessage.set_pathstepssector(step, pathS[step]);
		gridInfoMessage.set_pathstepsorientation(step, pathO[step]);
		//Logger::Instance().WriteMsg("ObstacleAvoidance", " R " + _toString(pathR[step]) + " S " + _toString(pathS[step]), Logger::Info);
	}

	_blk.publishSignal(gridInfoMessage, "obstacle");
}

void ObstacleAvoidance::pathPlanningRequestRelative(float target_x, float target_y, float target_phi)
{
	pprm_rel->set_gotox(target_x);
	pprm_rel->set_gotoy(target_y);
	pprm_rel->set_gotoangle(target_phi);
	pprm_rel->set_mode("relative");
	_blk.publishSignal(*pprm_rel, "obstacle");
}

/************** Motion **************/

void ObstacleAvoidance::motionController(double distance2Goal)
{
	double walkToX, walkToY, walkToT;

	//Logger::Instance().WriteMsg("ObstacleAvoidance", " wayM " + _toString(pathR[1]) + " wayN " + _toString(pathS[1]), Logger::Info);
	if (pathCounter != 0)
	{
		if(pathCounter == 1)
		{
			walkToX = 0.0;
			walkToY = 0.0;
			walkToT = 0.0;
			//Logger::Instance().WriteMsg("ObstacleAvoidance", "EFTASAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA! " , Logger::Info);
		}
		else if(pathR[1] == InnerRing)  		//peristrofikh kinhsh
		{
			walkToX = 0.0;
			walkToY = 0.0;
			int c = 1;

			while (c < pathCounter - 1 && pathR[c + 1] == 1)
				c++;

			walkToT = (pathS[1] > FRONT) ? c * RotationAngleRad * RotationMoveFactor : -c * RotationAngleRad * RotationMoveFactor;
			//Logger::Instance().WriteMsg("ObstacleAvoidance", "Turn! " + _toString(c), Logger::Info);
		}
		else if(pathS[1] > FRONT + 1 || pathS[1] < FRONT - 1) 		//plagia bhmata
		{
			walkToX = 0.0;
			walkToY = cellCenterY[pathR[1]][pathS[1]];
			walkToT = 0.0;
			//Logger::Instance().WriteMsg("ObstacleAvoidance", "Side! " , Logger::Info);
		}
		else
		{
			if (PolarGrid[present][pathR[1]][pathS[1]] > 0.8)
			{
				walkToX = 0.0;
				walkToY = 0.0;
				walkToT = 0.0;
			}
			else
			{
				walkToX = cellCenterX[pathR[1]][pathS[1]];
				walkToY = cellCenterY[pathR[1]][pathS[1]];
				walkToT = anglediff2((pathS[1] * RotationAngleRad + pathO[1] * M_PI_4), (pathS[0] * RotationAngleRad + pathO[0] * M_PI_4));
				//Logger::Instance().WriteMsg("ObstacleAvoidance", "Front! ", Logger::Info);
			}
		}

		if(debugModeCout)
			Logger::Instance().WriteMsg("ObstacleAvoidance", "coord waypoint x: " + _toString(walkToX) + " y: " + _toString(walkToY) + " or: " + _toString(walkToT) , Logger::Info);

		callVelocityWalk(walkToX, walkToY, walkToT, distance2Goal);
	}
}

void ObstacleAvoidance::velocityWalk(double ix, double iy, double it, double f)
{
	double x, y, t;
	x = ix;
	y = iy;
	t = it;
	wmot->set_command("setWalkTargetVelocity");
	x = x > +1.0 ? +1.0 : x;
	x = x < -1.0 ? -1.0 : x;
	y = y > +1.0 ? +1.0 : y;
	y = y < -1.0 ? -1.0 : y;
	t = t > +1.0 ? +1.0 : t;
	t = t < -1.0 ? -1.0 : t;
	wmot->set_parameter(0, x);
	wmot->set_parameter(1, y);
	wmot->set_parameter(2, t);
	wmot->set_parameter(3, f);

	if(debugModeCout)
		Logger::Instance().WriteMsg("ObstacleAvoidance", "publish: cx " + _toString(cX) + " cy " + _toString(cY) + " ct " + _toString(ct) +  " f " + _toString(f)  , Logger::ExtraExtraInfo);

	_blk.publishSignal(*wmot, "motion");
	lastwalk = microsec_clock::universal_time();
}

void ObstacleAvoidance::callVelocityWalk(double walkToX, double walkToY, double walkToT, double distance2Goal)
{
	static double X = 0.0, Y = 0.0, t = 0.0, f = 1.0, gain = 1.0;
	double maxd = fmaxf(fabs(walkToX), fabs(walkToY));

	if (maxd == 0.0 ) maxd = 1.0;

	f    = fminf(1.0, 0.4 + (distance2Goal / 0.4));
	gain = fminf(1.0, 0.0 + (distance2Goal / 0.4));
	X = gain * (walkToX) / maxd;
	Y = gain * (walkToY) / maxd;
	t = gain * (walkToT / M_PI);
	//Logger::Instance().WriteMsg("ObstacleAvoidance", "x: " + _toString(X) + " y " + _toString(Y) + " t " + _toString(t) + " f " + _toString(f), Logger::ExtraExtraInfo);
	velocityWalk(X, Y, t, f);
}

/************** Initialization **************/

void ObstacleAvoidance::Initialize()
{
	XMLConfig config(ArchConfig::Instance().GetConfigPrefix() + "/obstacle.xml");

	if (config.IsLoadedSuccessfully())
	{
		bool found = true;
		found &= config.QueryElement("FrontDeviation", FrontDeviation);
		found &= config.QueryElement("SideDeviation", SideDeviation);
		found &= config.QueryElement("ageTimerSeconds", ageTimerSeconds);
		found &= config.QueryElement("debugModeCout", debugModeCout);
		found &= config.QueryElement("debugRelativeMode", debugRelativeMode);
		found &= config.QueryElement("testPathX", testPathX);
		found &= config.QueryElement("testPathY", testPathY);
		found &= config.QueryElement("testPathA", testPathA);
		found &= config.QueryElement("AgeFactor", AgeFactor);
		found &= config.QueryElement("OccupancyDecreaseFactor", OccupancyDecreaseFactor);
		found &= config.QueryElement("OccupancyIncreaseFactor", OccupancyIncreaseFactor);
		found &= config.QueryElement("MinOccupancy", MinOccupancy);
		found &= config.QueryElement("MaxOccupancy", MaxOccupancy);
		found &= config.QueryElement("ObstacleThreshold", ObstacleThreshold);
		found &= config.QueryElement("ObstacleCostFactor", ObstacleCostFactor);
		found &= config.QueryElement("RotationCostFactor", RotationCostFactor);
		found &= config.QueryElement("RotationMoveFactor", RotationMoveFactor);
		found &= config.QueryElement("SideStepCostFactor", SideStepCostFactor);
		found &= config.QueryElement("SemiSideCostFactor", SemiSideCostFactor);
		found &= config.QueryElement("SonarDistanceRange", SonarDistanceRange);
		found &= config.QueryElement("SonarMinReading", SonarMinReading);
		found &= config.QueryElement("CloseObstacleRadius", CloseObstacleRadius);
		found &= config.QueryElement("CloseObstacleCenter", CloseObstacleCenter);
		found &= config.QueryElement("GoalDistanceTolerance", GoalDistanceTolerance);
		found &= config.QueryElement("GoalAngleTolerance", GoalAngleTolerance);
		
		
		Logger::Instance().WriteMsg("ObstacleAvoidance", "Initialize no knowledge: " + _toString(NoKnowledge) , Logger::ExtraExtraInfo);

		if (found)
			Logger::Instance().WriteMsg("ObstacleAvoidance", "All obstacle parameters loaded successfully" , Logger::ExtraExtraInfo);
		else
			Logger::Instance().WriteMsg("ObstacleAvoidance", "Cant Find an attribute in the xml config file " , Logger::ExtraExtraInfo);
	}
	else
		Logger::Instance().WriteMsg("ObstacleAvoidance", "Cant Find xml config file ", Logger::ExtraExtraInfo);
}

void ObstacleAvoidance::initGrid()
{
	initCoordinates();
	robotX = 0.0;
	robotY = 0.0;
	robotA = 0.0;
	initPaths();
	initPolarGrid();
	initEuclidean();
}

void ObstacleAvoidance::initPaths()
{
	for (int i = 0; i < PathLength; i++)
	{
		pathR[i] = -1;
		pathS[i] = -1;
		pathO[i] = -1;
	}
}

void ObstacleAvoidance::initPolarGrid()
{
	for (int k = 0; k < 2; k++)
		for (int r = 0; r < TotalRings; r++)
			for (int s = 0; s < N; s++)
				if (r == InnerRing)
					PolarGrid[k][r][s] = 0.0;
				else
					PolarGrid[k][r][s] = NoKnowledge;
}

void ObstacleAvoidance::initCoordinates()
{
	for (int r = 0; r < TotalRings; r++)
		for (int s = 0; s < N; s++)
			if (r == InnerRing)
			{
				cellCenterX[r][s] = 0.0;
				cellCenterY[r][s] = 0.0;
			}
			else
			{
				cellCenterX[r][s] = toCartesianX( RtoD(r) + 0.5 * RingDistance, StoT(s) + 0.5 * SectorAngleRad );
				cellCenterY[r][s] = toCartesianY( RtoD(r) + 0.5 * RingDistance, StoT(s) + 0.5 * SectorAngleRad );
			}
}

void ObstacleAvoidance::initEuclidean()
{
	for (int i = 0; i < TotalRings; i++)
		for (int k = 0; k < N; k++)
			for (int j = 0; j < TotalRings; j++)
				for (int l = 0; l < N; l++)
					euclidean[i][k][j][l] = toPolarD( cellCenterX[j][l] - cellCenterX[i][k], cellCenterY[j][l] - cellCenterY[i][k] );
}

void ObstacleAvoidance::initChanged()
{
	for(int i = 0; i < M; i++)
		for(int j = 0; j < N; j++)
			changed[i][j] = 0;
}




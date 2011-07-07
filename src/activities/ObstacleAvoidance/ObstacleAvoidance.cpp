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

	_blk->subscribeTo("sensors", 0);
	_blk->subscribeTo("behavior", 0);
	_blk->subscribeTo("obstacle", 0);

	Initialize();
	
	//obavm = new ObstacleMessage();

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
	
	//gia gui initialization
	white = cvScalar(255,255,255);
	blue = cvScalar(255, 0, 0);
	paintColor = grey;
	nCurvePts=4;
	nCurves=1;
	isCurveClosed=1;
	lineWidth=8;
	colorValue =0;
	
	
	debugCounter=0;
	
	initGrid();		//arxikopoihsh twn pinakwn
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
			updateGrid(countLeft!=0?Left:empty, countRight!=0?Right:empty);
	}
	
	
	//path planning messsage
	if(pprm !=0){
		targetX = pprm->gotox();
		targetY = pprm->gotoy();
		targetOrientation = pprm->gotoangle();
		targetY = (atan2(targetY, targetX)*ToDegrees)/RotationAngle;
		targetX = targetX/distance;
		astar3Neighbours(int(targetX), int(targetY), targetOrientation);
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
	
	//aging grid every 'ageTimerSeconds' seconds (currently 3)
	if (microsec_clock::universal_time()-lastAge >= seconds(ageTimerSeconds)){
		ageGrid();
		lastAge = microsec_clock::universal_time();
		if(debugModeCout) Logger::Instance().WriteMsg("ObstacleAvoidance", "aging", Logger::Info);
		//publishObstacleMessage();
		//publishMotionMessage(0);
	}

	if (cvHighgui)
		cvDrawGrid();

	publishObstacleMessage();
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
		Logger::Instance().WriteMsg("ObstacleAvoidance", "leftSensor: " + _toString(LeftValue[i].sensorvalue()) + " rightSensor: " + _toString(RightValue[i].sensorvalue()) , Logger::Info);

	Logger::Instance().WriteMsg("ObstacleAvoidance", "debugCounter: " + _toString(debugCounter), Logger::Info);
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
	pprm = _blk->readData<PathPlanningRequestMessage>("behavior");
	//sonarData = _blk->readData<SonarsData>("obstacle");
	//beamVals = _blk->readData<beam>("obstacle");
	
	wm = _blk->readSignal<MotionWalkMessage>("obstacle");
	//targetX = _blk->in_msg_nb<RobotPositionSensorMessage>("targetX", "Behavior");
	//targetY = _blk->in_msg_nb<RobotPositionSensorMessage>("targetY", "Behavior");
	Logger::Instance().WriteMsg("ObstacleAvoidance", "read messages " , Logger::ExtraExtraInfo);
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

	wmot = new MotionWalkMessage();
	wmot->set_command("setWalkTargetVelocity");
	wmot->add_parameter(x);
	wmot->add_parameter(y);
	wmot->add_parameter(z);
	wmot->add_parameter(s);
	wmot->add_parameter(t);
	if(debugModeCout)
		Logger::Instance().WriteMsg("ObstacleAvoidance","Sending Command: setWalkTargetVelocity. x: " + _toString(x) + " y: " + _toString(y), Logger::ExtraInfo);
	_blk->publishSignal(*wmot,"obstacle");
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

	obavm.set_distance(0, aristera?
	:0);
	obavm.set_distance(1, mprosta?mprostaDist:0);
	obavm.set_distance(2, dexia?dexiaDist:0);

	obavm.set_certainty(0, aristera?aristeraCert:0);
	obavm.set_certainty(1, mprosta?mprostaCert:0);
	obavm.set_certainty(2, dexia?dexiaCert:0);
	_blk->publishSignal(obavm, "behavior");

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
		found &= config.QueryElement("SonarAngleRange", SonarAngleRange);
		found &= config.QueryElement("RobotDirections", RobotDirections);
		found &= config.QueryElement("ToCm", ToCm);
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
		found &= config.QueryElement("angleDegrees", angleDegrees);
		found &= config.QueryElement("robotCells", robotCells);
		found &= config.QueryElement("obstacleCost", obstacleCost);
		Logger::Instance().WriteMsg("ObstacleAvoidance", "Initialize no knowledge: " + _toString(NoKnowledge) , Logger::ExtraExtraInfo);

		if (found) 
			cout << "All Localization parameters loaded successfully" << endl;
		else 
			cerr << "Cant Find an attribute in the xml config file " << endl;
	} else 
		cerr << "Cant Find xml config file " << endl;
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
			
	for(int n=5;n<=10;n++)
		PolarGrid[current][5][n] = 1.0;
	for(int n=6;n<=8;n++)
		PolarGrid[current][4][n] = 1.0;
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
	
	//initialize matrix with euklidean distances between cells for astar
	EuclideanLookup();
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

void ObstacleAvoidance::smoothGrid(int smooth){

}

void ObstacleAvoidance::updateFront(){
	for(int i=robotCells;i<3+robotCells;i++)
		if(abs(PolarGrid[current][i][LEFT] - PolarGrid[current][i][RIGHT]) <= TOLERANCE) 
			for(int k =FRONT-1;k<=FRONT+1;k++)
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
				PolarGrid[current][i][j]=(PolarGrid[current][i][j]*PossibilityDiscountNoObst<=MinPossibility)?MinPossibility:PolarGrid[current][i][j]*PossibilityDiscountNoObst;
			}
	}
	if (left[0] >= TooFar){//countLeft >= 19 || countLeft ==0 ){
		for(int i=robotCells;i<M+robotCells;i++)
			for(int j=FRONT  ;j<=LEFT ;j++){ //17:21
				PolarGrid[current][i][j]=PolarGrid[current][i][j]*PossibilityDiscountNoObst<=MinPossibility?MinPossibility:(PolarGrid[current][i][j]*PossibilityDiscountNoObst); 
			}
	}
	
	for (int i=SOnARsNum-1;i>=0;i--){
		left[i] = left[i] <= MinimumValidValue?MinimumValidValue:left[i] ;
		right[i] = right[i] <= MinimumValidValue?MinimumValidValue:right[i] ;
		left[i] = (left[i] < TooFar && left[i] > TooClose)?left[i]:EMPTY; //left
		right[i] = (right[i] < TooFar && right[i] > TooClose)?right[i]:EMPTY; //right
		if(left[i] == EMPTY && right[i] == EMPTY) continue;
		temp[0] = left[i];
		temp[1] = right[i];
		int fromY = 0;
		int toY = 0;
		
		for(int left=0;left<=1;left++){
			ind = temp[left];
			if(ind== 0.0) continue;
			index=(int)(temp[left]*(ToCm/distance) -(30/distance)) +robotCells;//-2 giati den mas endiaferei mexri ta 20 cm
			if(index < 0) continue;
			
			if(left){
				fromY = RIGHT;	//4
				toY = FRONT-1; //7
			}
			else{
				fromY = FRONT+2;//10
				toY = LEFT;	//13
			}
			
			
			for(int j=fromY; j<=toY; j++){
				gridValue = PolarGrid[current][index][j];
				PolarGrid[current][index][j]=PolarGrid[current][index][j]*ObstaclePossibility>=MaxPossibility?MaxPossibility:PolarGrid[current][index][j]*ObstaclePossibility;
				
				v=1;
				possibilityUp = UsePossibilityUp;
				while (index-v >= 0 || index+v < M ){
					if (index-v>=0)
						PolarGrid[current][index-v][j]=(PolarGrid[current][index-v][j]*UsePossibilityDown<=MinPossibility)?MinPossibility:PolarGrid[current][index-v][j]*UsePossibilityDown;
					if (index+v<M){
						if (v >= 2) 
							ageSpecGrid(index+v, j);
						else
							PolarGrid[current][index+v][j]=PolarGrid[current][index+v][j]*possibilityUp>=MaxPossibility?MaxPossibility:PolarGrid[current][index+v][j]*possibilityUp;
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
		mprostaDist = mprosta == true?(i+1):0;
		mprostaCert = mprosta == true?PolarGrid[current][i][FRONT]:0;
		break;
	}
		

	for(int i=robotCells;i<M+robotCells;i++){
		dexia = PolarGrid[current][i][RIGHT+1] > 0.6?true:false;
		dexiaDist = dexia == true?(i+1):0;
		dexiaCert = dexia == true?PolarGrid[current][i][RIGHT+1]:0;
		break;
	}
		
	for(int i=robotCells;i<M+robotCells;i++){
		aristera = PolarGrid[current][i][LEFT-1] > 0.6?true:false;
		aristeraDist = aristera == true?(i+1)*distance:0;
		aristeraCert = aristera == true?PolarGrid[current][i][LEFT-1]:0;
		break;
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


void ObstacleAvoidance::ageGrid(){
	for (int i=robotCells;i<M+robotCells;i++)
		for(int j=0;j<N;j++)
			PolarGrid[current][i][j] = (PolarGrid[current][i][j]-NoKnowledge)*agePossibility + NoKnowledge;
}

void ObstacleAvoidance::findNewPosition(){
	double temp[3]={EMPTY, EMPTY, EMPTY};

	temp[0] = (PosX.sensorvalue()-RobotPosition[0])*ToCm;
	temp[1] = (PosY.sensorvalue()-RobotPosition[1])*ToCm;
	temp[2] = angleDiff(Angle.sensorvalue(), RobotPosition[2]);//(Angle.sensorvalue()-RobotPosition[2]);
Logger::Instance().WriteMsg("ObstacleAvoidance", "x: " + _toString(temp[0]) + "   y: " + _toString(temp[1]) + "   theta: " + _toString(temp[2]), Logger::ExtraExtraInfo);
	//opou distance bale 15
	if(fabs(temp[0] )> distance || fabs(temp[1]) > distance){
		//Logger::Instance().WriteMsg("ObstacleAvoidance", "temp2: " + _toString(temp[2]) , Logger::ExtraExtraInfo);
		straightMoveGrid(temp[0]*FrontDeviation, temp[1]*SideDeviation, 0);//, temp[2]);
	}
}

void ObstacleAvoidance::rotateGrid(double angle){
	double temp[M][N];
	int rotateCells=int(angle/RotationAngle);
	int change[N];
	Logger::Instance().WriteMsg("ObstacleAvoidance", "rotateangle " + _toString(rotateCells), Logger::ExtraExtraInfo);
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
	Logger::Instance().WriteMsg("ObstacleAvoidance", "---------------------------------------------------------", Logger::Info);
}

void ObstacleAvoidance::findCoordinates(){
	int tempCnt=0, dist=distance;
	int px[3], py[3];
	for (int i=0;i<=M+2+robotCells;i++){
		dist = (i)*distance;
		for (int j=N;j>0;j--){
			//x[tempCnt] = int((dist * (cos(RotationAngleRad*j)))+shiftConsole);
			//y[tempCnt] = int((dist * (sin(RotationAngleRad*j)))+shiftConsole);
			if(i==0){
				x[tempCnt] = shiftConsole;
				y[tempCnt] = shiftConsole;
			}else{
				x[tempCnt] = round((dist * (cos(((RotationAngle)*j)*ToRad))))+shiftConsole;
				y[tempCnt] = round((dist * (sin(((RotationAngle)*j)*ToRad))))+shiftConsole;
				//cout << "coord " << (x[tempCnt]) <<  " coordy " << (y[tempCnt]) << " i " << i << " j " << j << " " << cos(((RotationAngle)*j)*ToRad) << endl;
			}
			//Logger::Instance().WriteMsg("ObstacleAvoidance", "coord " + _toString(x[tempCnt]) +  " coordy " + _toString(y[tempCnt]) , Logger::ExtraExtraInfo);
			tempCnt++;
		}
	}
	
	for(int i=0;i<=M+1+robotCells;i++)
		for(int j=0;j<N;j++){
			px[0] = x[N*i+j];			py[0] = y[N*i+j];
			px[1] = x[N*(i+1)+j+1]; 	py[1] = y[N*(i+1)+j+1];
			px[2] = (int)(px[0] + px[1] )/2;	py[2] = (int)(py[0] + py[1] )/2;
			cellCenterX[i*N+j] = px[2];
			cellCenterY[i*N+j] = py[2];
			
		}
}

void ObstacleAvoidance::reset(){
	initPolarGrid();
			
	firstTimeInitializeOdometry = 1;
}

void ObstacleAvoidance::initChanged(){
	for(int i=0;i<M;i++)
		for(int j=0;j<N;j++)
			changed[i*N+j]=0;
}

double ObstacleAvoidance::angleDiff(double a1, double a2){
	return wrapToPi(wrapToPi(a1+PI-a2) - PI);
}

double ObstacleAvoidance::wrapToPi(double angle){
	while(angle < -PI) angle +=2.0*PI;
	while(angle > PI) angle -=2.0*PI;
	return (angle);
}

double ObstacleAvoidance::angleDiff2(double a1, double a2){
	double diff = a1*ToDegrees-a2*ToDegrees;
	while(diff < -angleDegrees/2) diff +=angleDegrees;
	while(diff > angleDegrees/2) diff -=angleDegrees;
	return diff/ToDegrees;
}

void ObstacleAvoidance::straightMoveGrid(double deltaFront,double deltaSide, double deltatheta){
	double px, py,r, theta, temp, xnew, ynew, rnew, thetanew=0;
	int index=0, indexm=0, indexn=0;//, indey=0, cellnum=0; int smooth =0;
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
			else if(i < M+robotCells)
				PolarGrid[current][i][j] = PolarGrid[1-current][i][j];//NoKnowledge;//
			else
				PolarGrid[current][i][j] = NoKnowledge;
		}
	
	for(int i=0;i<=M+robotCells;i++)
		for(int j=0;j<N;j++){
				//arxikopoioume to keli tou neou polar grid me 0.5 ektos an exei allaxei h timh toy apo allo move
				//if(!changed[i*N+j])
				//PolarGrid[current][i][j] = NoKnowledge;
				temp = PolarGrid[1-current][i][j];
				//if (fabs(PolarGrid[1-current][i][j]-NoKnowledge) >= TOLERANCE){// && changed[i*N+j]==0){ //check only the cells with obstacles.
					
				px = cellCenterX[i*N+j];
				py = cellCenterY[i*N+j];
				
				px = px - shiftConsole;
				py = shiftConsole - py;
				
				r = sqrt(pow(px, 2.0)+ pow(py, 2.0)) ;//M
				theta = angleDiff(atan2(py, px), double(RobotPosition[2]));//N

				
				xnew= r*cos((theta+ 2*RobotPosition[2])) + deltaSide;
				ynew= r*sin((theta+ 2*RobotPosition[2])) - deltaFront;
				
				rnew = sqrt(pow(xnew, 2.0) + pow(ynew, 2.0));
				thetanew = angleDiff(atan2(ynew, xnew), double(Angle.sensorvalue()))*ToDegrees;
				thetanew = thetanew < 0 ? angleDegrees + thetanew : thetanew;
				
				indexn = int(thetanew/RotationAngle);
				indexm = int(rnew/distance);//-1; // -1 gia swsto deikth
				
				
				if(indexn>=0 && indexm >=robotCells  && indexn < N && indexm < M+robotCells){
					PolarGrid[current][indexm][indexn] = temp;
					changed[index] = 1;
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
	  Logger::Instance().WriteMsg("ObstacleAvoidance", "initm " + _toString(initm) +  " initn " + _toString(initn) , Logger::ExtraExtraInfo);
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
					Logger::Instance().WriteMsg("ObstacleAvoidance", "returning..." + _toString(it), Logger::ExtraExtraInfo);
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
			curve1[0].x = x[k*N+l];      curve1[0].y = y[k*N+l];
			curve1[1].x = x[(k+1)*N+l];  curve1[1].y = y[(k+1)*N+l];
			
			if(l < N-1){
				curve1[2].x = x[(k+1)*N+1+l];	curve1[2].y = y[(k+1)*N+1+l];
				curve1[3].x = x[k*N+1+l];    	curve1[3].y = y[k*N+1+l];
			}
			else{
				curve1[2].x =x[(k+1)*N];     curve1[2].y = y[(k+1)*N];
				curve1[3].x =x[k*N];         curve1[3].y = y[k*N];
			}
			colorValue = int(colorMax-PolarGrid[current][k][l]*colorMax);
			paintColor = cvScalar(colorValue, colorValue, colorValue);
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
	cvWaitKey(10);
	return;

}


void ObstacleAvoidance::drawIterationGrid(){
	CvPoint* curveArr={curve1};
	for (int k=0;k<M;k++){
		for(int l=0;l<N;l++){
			curve1[0].x = x[k*N+l];      curve1[0].y = y[k*N+l];
			curve1[1].x = x[(k+1)*N+l];  curve1[1].y = y[(k+1)*N+l];
			
			if(l < N-1){
				curve1[2].x = x[(k+1)*N+1+l];	curve1[2].y = y[(k+1)*N+1+l];
				curve1[3].x = x[k*N+1+l];    	curve1[3].y = y[k*N+1+l];
			}
			else{
				curve1[2].x =x[(k+1)*N];     curve1[2].y = y[(k+1)*N];
				curve1[3].x =x[k*N];         curve1[3].y = y[k*N];
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
void ObstacleAvoidance::EuclideanLookup(){
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

void ObstacleAvoidance::reconstructPathNew(int currentcell, int orientt){
	if(debugModeCout)
	Logger::Instance().WriteMsg("ObstacleAvoidance", " cell m " + _toString(int(currentcell/N)) + " n " + _toString(int(currentcell%N)) + " " + _toString(orientt) + " parent  " + _toString(parent[currentcell][orientt]), Logger::ExtraExtraInfo);
	if(parent[currentcell][orientt] > -1){
		reconstructPathNew(parent[currentcell][orientt], orient[currentcell][orientt]);
		PolarGrid[current][int(currentcell/N)][int(currentcell%N)] = 2.0;
	}
	if(debugModeCout)
		Logger::Instance().WriteMsg("ObstacleAvoidance", " cell m " + _toString(int(currentcell/N)) + " n " + _toString(int(currentcell%N)) + " " + _toString(orientt) + " parent  " + _toString(parent[currentcell][orientt]), Logger::ExtraExtraInfo);
	PolarGrid[current][int(currentcell/N)][int(currentcell%N)] = 2.0;
	callVelocityWalk(currentcell/N, currentcell%N, orientt);
	nextM[counterPath] = int(currentcell/N);
	nextN[counterPath] = int(currentcell%N);
	nextOr[counterPath] = orientt;
	counterPath++;
}

void ObstacleAvoidance::reconstructPath(int currentcell){
	if(debugModeCout)
		Logger::Instance().WriteMsg("ObstacleAvoidance", " cell m " + _toString(currentcell/N) + " n " + _toString(currentcell%N), Logger::ExtraExtraInfo);
	if(parent[currentcell][0] > -1){
		reconstructPath(parent[currentcell][0]);
		PolarGrid[current][int(currentcell/N)][int(currentcell%N)] = 2.0;
	}
	if(debugModeCout)
		Logger::Instance().WriteMsg("ObstacleAvoidance", " cell m " + _toString(currentcell/N) + " n " + _toString(currentcell%N), Logger::ExtraExtraInfo);
	PolarGrid[current][int(currentcell/N)][int(currentcell%N)] = 2.0;
	callVelocityWalk(currentcell/N, currentcell%N, 0);
	nextM[counterPath] = int(currentcell/N);
	nextN[counterPath] = int(currentcell%N);
	counterPath++;
}

void ObstacleAvoidance::insertInOpenList(node anode){
	list<node>::iterator i;
	for(i=openList.begin(); i!=openList.end();i++)
		if(anode.fn < (*i).fn)
			break;
	
	openList.insert(i, anode);	
}

void ObstacleAvoidance::astar3Neighbours(int goalx, int goaly, int goalorient){
	if(debugModeCout)
		Logger::Instance().WriteMsg("ObstacleAvoidance", "ASTAR "  , Logger::ExtraExtraInfo);
	
	int startAngle = 0, nodes=0, indexNeighbour=0, currentm, currentn, goalIndex=goalx*N+goaly;
	int initinden = FRONT, initindem = 1, pathFound = 0;
	double  newg;
	node temp, currentNode;
	
	for (int i=0;i<(M+3)*N;i++)
		for(int j=0; j< WATCHDIRECTION; j++){
			whatList[i][j] = 0;
			parent[i][j] = -1;
			orient[i][j] = 0;
		}
		
	
	EuclideanLookup();
	EuclideanOrientationLookup();
	temp.cell = initinden + initindem*N;
	temp.gn = 0;							//cost to move from start point to the given cell following the generated path
	temp.hn =euclidean[temp.cell][goalIndex];  ////estimated cost from given cell to goal
	temp.fn = temp.gn + temp.hn;
	temp.parentcell =-1;
	temp.orientation =0;
	for (int orr=0;orr<NEIGHBOURS;orr++){
		orient[temp.cell][orr] = 0; 
		parent[temp.cell][orr] = -1;
	}
	openList.push_front(temp);

	whatList[temp.cell][0] = 1;
	
	while(!openList.empty()){
		currentNode = openList.front();
		currentm=int(currentNode.cell / N);
		currentn=int(currentNode.cell% N);
		//if(debugFullPathPlanning)
			//Logger::Instance().WriteMsg("ObstacleAvoidance", "CURRENT m: " + _toString((currentNode.cell)/N) + " n " + _toString((currentNode.cell)%N) + " me cost g" + _toString(currentNode.gn) + " f " + _toString(currentNode.fn) + " kai orient " _toString(currentNode.orientation), Logger::ExtraExtraInfo);
		
		if(debugFullPathPlanning)
			nodes++;
		if((currentm == goalx) && (currentn == goaly) && (currentNode.orientation == goalorient)){ //    ){  //
			if(debugModeCout)
				Logger::Instance().WriteMsg("ObstacleAvoidance", "EDWWWWWWWWWWWww "  , Logger::ExtraExtraInfo);
			pathFound = 1;
			reconstructPathNew(currentNode.cell, currentNode.orientation);
			counterPath = 0;
			if (cvHighgui)
				cvDrawGrid();
			return;
		}
		
		//remove from openlist
		openList.pop_front();
		
		//insert to closed list
		closedList.push_back(currentNode);
		whatList[currentNode.cell][currentNode.orientation] = -1;
		
				
		//expand neighbours
		initIndexes(currentm, currentn);
		startAngle= currentNode.orientation;
		for (int  j=0;j<NEIGHBOURS;j++){
			if((j!=startAngle) && (j!=(startAngle+1)%NEIGHBOURS) && (j!=(startAngle-1+NEIGHBOURS)%NEIGHBOURS) )continue;
			if (indexx[j] <= 1) continue;
			if (indexx[j] > M) continue;
			
			indexNeighbour = indexx[j]*N+indexy[j];
			//in closed
			if(whatList[indexNeighbour][j] == -1 )
				continue;
			newg = currentNode.gn + euclidean[currentNode.cell][indexNeighbour]; //
			if(PolarGrid[current][indexx[j]][indexy[j]] >= 0.8)
				newg += obstacleCost;
			
			if(debugFullPathPlanning)
				Logger::Instance().WriteMsg("ObstacleAvoidance", " j " + _toString(j) + "  " + _toString(indexNeighbour) + " " + _toString(whatList[indexNeighbour][j]) , Logger::ExtraExtraInfo);
			
			//if not in open then add in open
			if(whatList[indexNeighbour][j] != 1){
				temp.cell = indexNeighbour;
				temp.gn = newg;
				temp.hn =  euclidean[temp.cell][goalIndex]; //	//estimated cost from given cell to goal
				temp.fn = temp.gn + temp.hn; 
				temp.parentcell = currentNode.cell;
				parent[temp.cell][j] =currentNode.cell;
				temp.orientation = j;
				orient[temp.cell][j]= currentNode.orientation;
				insertInOpenList(temp);
				whatList[temp.cell][j] = 1;
				if(debugFullPathPlanning)
					cout << "add in open m: " << (temp.cell)/N << " n " << (temp.cell)%N << " cost " << temp.gn << " orient " << temp.orientation << endl;
			}
			else{
				list<node>::iterator it1;
				for (it1=openList.begin(); it1!=openList.end(); ++it1)
					if(((*it1).cell == indexNeighbour) && ((*it1).orientation == j) )
						break;
				if(debugFullPathPlanning)
					cout << " was in open " << " M " << ((*it1).cell/N)  << " N " << ((*it1).cell%N) << " ori " << (*it1).orientation << endl;
				if((*it1).gn > newg){
					openList.erase(it1);
					temp.cell = indexNeighbour;
					temp.gn = newg;
					temp.hn =  euclidean[temp.cell][goalIndex]; //	//estimated cost from given cell to goal
					temp.fn = temp.gn + temp.hn; 
					temp.parentcell = currentNode.cell;
					parent[temp.cell][j] =currentNode.cell;
					temp.orientation = j;
					orient[temp.cell][j] = currentNode.orientation;
					insertInOpenList(temp);
					whatList[temp.cell][j] = 1;
				}
			}	
		}
	}
	if(pathFound == 0)
		astarPlain(goalx, goaly);
}

void ObstacleAvoidance::astarPlain(int goalx, int goaly){
	int startAngle = 0, indexNeighbour=0, currentm, currentn, goalIndex=goalx*N+goaly;
	int initinden = FRONT, initindem = 1;
	double  newg;
	node temp, currentNode, temp1;

	if(debugModeCout)
		Logger::Instance().WriteMsg("ObstacleAvoidance", "ASTAR PLAIN "  , Logger::ExtraExtraInfo);
	for (int i=0;i<(M+3)*N;i++)
		for(int j=0; j< WATCHDIRECTION; j++){
			whatList[i][j] = 0;
			parent[i][j] = -1;
			orient[i][j] = 0;
		}
	
	temp.cell = initinden + initindem*N;
	temp.gn = 0;							//cost to move from start point to the given cell following the generated path
	temp.hn =euclidean[temp.cell][goalIndex];	//estimated cost from given cell to goal
	temp.fn = temp.gn + temp.hn;
	temp.parentcell =0;
	parent[temp.cell][0] =-1;
	temp.orientation =0;
	openList.push_front(temp);
	whatList[temp.cell][0] = 1;
	
	while(!openList.empty()){
		currentNode = openList.front(); 
		currentm=int(currentNode.cell / N);
		currentn=int((currentNode.cell% N)); 
		if(currentm == goalx && currentn == goaly){
			reconstructPath(currentNode.cell);
			counterPath = 0;
			if (cvHighgui)
				cvDrawGrid();
			break;
		}
		//remove from openlist
		openList.pop_front();
		
		//insert to closed list
		closedList.push_back(currentNode);
		whatList[currentNode.cell][0] = -1;
		
		//expand neighbours
		initIndexes(currentm, currentn);
		startAngle= currentNode.orientation;
		if(debugFullPathPlanning)
			Logger::Instance().WriteMsg("ObstacleAvoidance", "eimai m:  " + _toString((currentNode.cell)/N ) + " n " + _toString((currentNode.cell)%N) + " me cost " + _toString( currentNode.gn), Logger::ExtraExtraInfo);
			
		for (int  j=0;j<NEIGHBOURS;j++){
			
			if( j!= (( startAngle+1 < NEIGHBOURS )?startAngle+1:NEIGHBOURS-startAngle-1) &&
				j!= (( startAngle-1 >= 0 )?startAngle-1:NEIGHBOURS+startAngle-1) &&
				j!=  startAngle )  continue;
			
			indexNeighbour = indexx[j]*N+indexy[j];
			//in closed or not walkable
			if(whatList[indexNeighbour][0] == -1 )//|| PolarGrid[current][indexx[j]][indexy[j]] >= 0.8)
				continue;
			newg = currentNode.gn +euclidean[currentNode.cell][indexNeighbour]; 
			if(PolarGrid[current][indexx[j]][indexy[j]] >= 0.8)
				newg += obstacleCost;
			//if not in open then add in open
			if(whatList[indexNeighbour][0] != 1){
				temp.cell = indexNeighbour;
				temp.gn = euclidean[currentNode.cell][indexNeighbour]; 
				if(PolarGrid[current][indexx[j]][indexy[j]] >= 0.8)
					temp.gn += obstacleCost;
				temp.hn = euclidean[indexNeighbour][goalIndex]; 	//estimated cost from given cell to goal
				temp.fn = temp.gn + temp.hn; 
				temp.parentcell = currentNode.cell;
				temp.orientation = j;
				parent[temp.cell][0] =currentNode.cell;
				insertInOpenList(temp);
				whatList[temp.cell][0] = 1;
				if(debugFullPathPlanning)
					Logger::Instance().WriteMsg("ObstacleAvoidance", "add in open m: " + _toString((temp.cell)/N ) + " n " + _toString((temp.cell)%N) + " me cost " + _toString(temp.gn), Logger::ExtraExtraInfo);
			}
			else{
				if(debugFullPathPlanning)
					Logger::Instance().WriteMsg("ObstacleAvoidance"," was in open ", Logger::ExtraExtraInfo);
				list<node>::iterator it1;
				for (it1=openList.begin(); it1!=openList.end(); ++it1)
					if((*it1).cell == indexNeighbour)
						break;
				temp1 = (*it1);
				if(temp1.gn > newg){
					if(debugFullPathPlanning)
						Logger::Instance().WriteMsg("ObstacleAvoidance", " kai paw : " + _toString( temp1.cell ) + " me cost prin " + _toString(temp1.gn ) + " logo m: " + _toString(parent[temp1.cell][0]/N) +  " n " + _toString(parent[temp1.cell][0]%N), Logger::ExtraExtraInfo);
					temp1.parentcell = currentNode.cell;
					parent[temp1.cell][0] =currentNode.cell;
					temp1.gn = newg;
					temp1.fn = temp1.hn + temp1.gn;
					if(debugFullPathPlanning)
						Logger::Instance().WriteMsg("ObstacleAvoidance", " kai cost meta : " + _toString( temp1.gn) + " kai cell m: " + _toString((temp1.cell)/N) + " n: " + _toString((temp1.cell)%N), Logger::ExtraExtraInfo);
					openList.erase(it1);
					insertInOpenList(temp1);
					whatList[temp1.cell][0] = 1;
				}
			}
			
		}
	}
}


void ObstacleAvoidance::velocityWalk(double ix, double iy, double it, double f)
{
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
	_blk->publishSignal(*wmot, "motion");
	lastwalk = microsec_clock::universal_time();
}

void ObstacleAvoidance::callVelocityWalk(double bx, double by, double bb){
	static double X=0.0, Y=0.0, t=0.0, f=1.0, gain = 1.0;
	double maxd = fmaxf(fabs(bx),fabs(by));
	f    = fminf(1.0, 0.4+(maxd/0.5));
	gain = fminf(1.0, 0.0+(maxd/0.5));
	X = gain*(bx)/maxd;
	Y = gain*(by)/maxd;
	t = gain*(bb/M_PI);
	//velocityWalk(X,Y,t,f);
}



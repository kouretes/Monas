#include "ObstacleAvoidance.h"
#include "tools/toString.h"
#include <iostream>

#include "tools/logger.h"

namespace {
	ActivityRegistrar<ObstacleAvoidance>::Type temp("ObstacleAvoidance");
}

ObstacleAvoidance::ObstacleAvoidance():
	cvHighgui(true)  {
}

void ObstacleAvoidance::UserInit() {

	_blk->subscribeTo("sensors", 0);
	_blk->subscribeTo("behavior", 0);


	//obavm = new ObstacleMessage();

	//ypoloipa metakinhsewn
	resX=0.0;
	resY=0.0;
	resAngle=0.0;

	//mnmata pou diabazei
	//ussm =0;
	//rpsm=0;

	shiftConsole = 100;  //shift==metatopish gia probolh grid sthn ekastote konsola
	countAge=0;
	countLeft=0;
	countRight=0;
	//countPos=0;
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

	firstTimeInitializeOdometry=0;		//xrhsh gia prwth anafora se odometry metrhseis

	//syntetagmenes stoxou
	goalX=0;
	goalY=0;
	initGrid();		//arxikopoihsh twn pinakwn
	//boh8htikos pinakas gia update
	for(int i=0;i<SOnARsNum;i++)
		empty[i]=2.0;

	obavm.add_direction(0);
	obavm.add_direction(0);
	obavm.add_direction(0);

	obavm.add_distance(0);
	obavm.add_distance(0);
	obavm.add_distance(0);

	obavm.add_certainty(0);
	obavm.add_certainty(0);
	obavm.add_certainty(0);

	white = cvScalar(255,255,255);
	lightgrey = cvScalar(190,190,190);
	grey = cvScalar(125,125,125);
	darkgrey = cvScalar(65,65,65);
	black = cvScalar(0,0,0);
	blue = cvScalar(255, 0, 0);
	paintColor = grey;

	nCurvePts=4;
	nCurves=1;
	isCurveClosed=1;
	lineWidth=8;

	if (cvHighgui)
	{
		Logger::Instance().WriteMsg("ObstacleAvoidance", "Enable highgui", Logger::Info);
		img = cvCreateImage(cvSize(200, 200), IPL_DEPTH_8U, 3);
		cvNamedWindow("img", CV_WINDOW_AUTOSIZE);
	}
	Logger::Instance().WriteMsg("ObstacleAvoidance", " SonarCellRange " + _toString(SonarCellRange), Logger::Info);
	Logger::Instance().WriteMsg("ObstacleAvoidance", " front " + _toString(FRONT), Logger::Info);
	Logger::Instance().WriteMsg("ObstacleAvoidance", " RotationAngle " + _toString(RotationAngle), Logger::Info);

}

int ObstacleAvoidance::Execute() {
	read_messages();
	countAge++;
	//xrhsh gia elegxo ean to array me tis times twn sonars einai 0 = sfalma
	countLeft=0;
	countRight=0;



	double Right[SOnARsNum], Left[SOnARsNum];

	//(RobotPositionSensorMessage) find current robot position
	if(rpsm !=0){
		PosX = rpsm->sensordata(0);
		PosY = rpsm->sensordata(1);
		Angle = rpsm->sensordata(2);
		if (firstTimeInitializeOdometry ==0){
			RobotPosition[0]=PosX.sensorvalue();
			RobotPosition[1]=PosY.sensorvalue();
			RobotPosition[2]=Angle.sensorvalue();
			firstTimeInitializeOdometry=1;
		}
		else{
			findNewPosition();
			//for path planning
			//goalX = int(targetX/distance);
			//goalY = int(targetY/N);
			//pathPlanningInit(goalX, goalY);
			//paradeigma-test
			//pathPlanningInit(6, 21);
		}
		//delete rpsm;
	}


	//update the grid with the new sonar values
	if (ussm != 0) {
		for (int j=SOnARsNum-1;j>=0;j--){
			LeftValue[j] = ussm->sensordata(j);
			Left[j] = LeftValue[j].sensorvalue();
			//Left[j] = (ussm->sensordata(j)).sensorvalue();
			RightValue[j] = ussm->sensordata(j+SOnARsNum);
			Right[j] = RightValue[j].sensorvalue();
			//Right[j] = (ussm->sensordata(j+SOnARsNum)).sensorvalue();
			countLeft = countLeft+LeftValue[j].sensorvalue();
			countRight = countRight + RightValue[j].sensorvalue() ;
		}
		printSonarValues();
		if(countLeft == 0 && countRight == 0){;}
		else	updateGrid(countRight!=0?Right:empty, countLeft!=0?Left:empty);
		//delete ussm;
	}

	//aging grid every 3 seconds
	//if(countAge == 30){
	//	countAge =0;
	//	ageGrid();
	//}

	if (cvHighgui)
		cvDrawGrid();

	publishObstacleMessage();
	return 0;
}

void ObstacleAvoidance::printSonarValues(){

	Logger::Instance().WriteMsg("ObstacleAvoidance", "rightSensor: " + _toString(RightValue[0].sensorvalue()) + "   leftSensor: " + _toString(LeftValue[0].sensorvalue()), Logger::Info);
	Logger::Instance().WriteMsg("ObstacleAvoidance", "rightSensor: " + _toString(RightValue[1].sensorvalue()) + "   leftSensor: " + _toString(LeftValue[1].sensorvalue()), Logger::Info);
	Logger::Instance().WriteMsg("ObstacleAvoidance", "rightSensor: " + _toString(RightValue[2].sensorvalue()) + "   leftSensor: " + _toString(LeftValue[2].sensorvalue()), Logger::Info);
	Logger::Instance().WriteMsg("ObstacleAvoidance", "rightSensor: " + _toString(RightValue[3].sensorvalue()) + "   leftSensor: " + _toString(LeftValue[3].sensorvalue()), Logger::Info);
	Logger::Instance().WriteMsg("ObstacleAvoidance", "rightSensor: " + _toString(RightValue[4].sensorvalue()) + "   leftSensor: " + _toString(LeftValue[4].sensorvalue()), Logger::Info);
	Logger::Instance().WriteMsg("ObstacleAvoidance", "rightSensor: " + _toString(RightValue[5].sensorvalue()) + "   leftSensor: " + _toString(LeftValue[5].sensorvalue()), Logger::Info);
	Logger::Instance().WriteMsg("ObstacleAvoidance", "rightSensor: " + _toString(RightValue[6].sensorvalue()) + "   leftSensor: " + _toString(LeftValue[6].sensorvalue()), Logger::Info);
	Logger::Instance().WriteMsg("ObstacleAvoidance", "rightSensor: " + _toString(RightValue[7].sensorvalue()) + "   leftSensor: " + _toString(LeftValue[7].sensorvalue()), Logger::Info);
	Logger::Instance().WriteMsg("ObstacleAvoidance", "rightSensor: " + _toString(RightValue[8].sensorvalue()) + "   leftSensor: " + _toString(LeftValue[8].sensorvalue()), Logger::Info);
	Logger::Instance().WriteMsg("ObstacleAvoidance", "rightSensor: " + _toString(RightValue[9].sensorvalue()) + "   leftSensor: " + _toString(LeftValue[9].sensorvalue()), Logger::Info);

}

void ObstacleAvoidance::read_messages() {


	ussm =  _blk->read_data<UltaSoundSensorsMessage>("UltaSoundSensorsMessage");
	rpsm =  _blk->read_data<RobotPositionSensorMessage>("RobotPositionSensorMessage");
	//targetX = _blk->in_msg_nb<RobotPositionSensorMessage>("targetX", "Behavior");
	//targetY = _blk->in_msg_nb<RobotPositionSensorMessage>("targetY", "Behavior");
	Logger::Instance().WriteMsg("ObstacleAvoidance", "read messages " , Logger::ExtraExtraInfo);
}

void ObstacleAvoidance::publishObstacleMessage(){
	Logger::Instance().WriteMsg("ObstacleAvoidance", "aristera: " + _toString(aristera)+ " dist: " + _toString(aristeraDist) + " cert: "+_toString(aristeraCert) , Logger::ExtraExtraInfo);
	Logger::Instance().WriteMsg("ObstacleAvoidance", "mprosta: " + _toString(mprosta) + " dist: " + _toString(mprostaDist) + " cert: "+_toString(mprostaCert) , Logger::ExtraExtraInfo);
	Logger::Instance().WriteMsg("ObstacleAvoidance", "dexia: " + _toString(dexia) + " dist: " + _toString(dexiaDist) + " cert: "+_toString(dexiaCert) , Logger::ExtraExtraInfo);

	obavm.set_direction(0, aristera?1:0);
	obavm.set_direction(1, mprosta?1:0);
	obavm.set_direction(2, dexia?1:0);

	obavm.set_distance(0, aristera?aristeraDist:0);
	obavm.set_distance(1, mprosta?mprostaDist:0);
	obavm.set_distance(2, dexia?dexiaDist:0);

	obavm.set_certainty(0, aristera?aristeraCert:0);
	obavm.set_certainty(1, mprosta?mprostaCert:0);
	obavm.set_certainty(2, dexia?dexiaCert:0);

	_blk->publish_signal(obavm, "obstacle");
}

void ObstacleAvoidance::initPolarGrid(){
	//initialize Polar grid
	for(int i=0;i<M;i++)
		for(int j=0;j<N;j++)
			PolarGrid[i][j] = NoKnowledge;

}

void ObstacleAvoidance::initGrid(){
	int xx=0, yy=0;
	double r, theta;
	int coord=0, counter=0;

	//oi syntetagmenes pou antistoixoun sta 4 shmeia pou orizoun to keli
	findCoordinates();

	//initialize the variables of the robot position
	for (int i=0;i<RobotDirections;i++)
		RobotPosition[i]=0.0;

	//initialize arrays with sonar values
	for (int i=0;i<SOnARsNum;i++){
		rightArray[i]=0.0;
		leftArray[i]=0.0;
	}

	//initialize Polar grid
	initPolarGrid();


	for (int i=0;i<M+1;i++){
		for(int j=N;j>0;j--){
			xx = x[counter]+10;
			yy = y[counter];
			r = sqrt(pow((float)xx-shiftConsole, 2) + pow((float)shiftConsole-yy, 2) ) - (BodyCm/2);
			theta = atan2(shiftConsole-yy, xx-shiftConsole)*ToDegrees;
			theta = theta>=0?theta:360 + theta;
			coord = xx*1000+yy;
			//out << " coord: " << coord << " cell: " <<  (r >= 0?(int(r/distance))*N+(int(theta/(RotationAngle))):-1) <<" xx: " <<xx << " yy: " << yy << "   r: " << r << " theta: " << theta  << endl;
			MoveGrid[coord] =  r >= 0?(int(r/distance))*N+(int(theta/(RotationAngle))):-1;

			xx = x[counter];
			yy = y[counter]+10;
			r = sqrt(pow((float)xx-shiftConsole, 2) + pow((float)shiftConsole-yy, 2) ) - (BodyCm/2);
			theta = atan2(shiftConsole-yy, xx-shiftConsole)*ToDegrees;
			theta = theta>=0?theta:360 + theta;
			coord = xx*1000+yy;
			//out << " coord: " << coord << " cell: " <<  (r >= 0?(int(r/distance))*N+(int(theta/(RotationAngle))):-1) <<" xx: " <<xx << " yy: " << yy << "   r: " << r << " theta: " << theta  << endl;
			MoveGrid[coord] =  r >= 0?(int(r/distance))*N+(int(theta/(RotationAngle))):-1;

			xx = x[counter]+10;
			yy = y[counter]+10;
			r = sqrt(pow((float)xx-shiftConsole, 2) + pow((float)shiftConsole-yy, 2) ) - (BodyCm/2);
			theta = atan2(shiftConsole-yy, xx-shiftConsole)*ToDegrees;
			theta = theta>=0?theta:360 + theta;
			coord = xx*1000+yy;
			//out << " coord: " << coord << " cell: " <<  (r >= 0?(int(r/distance))*N+(int(theta/(RotationAngle))):-1) <<" xx: " <<xx << " yy: " << yy << "   r: " << r << " theta: " << theta  << endl;
			MoveGrid[coord] =  r >= 0?(int(r/distance))*N+(int(theta/(RotationAngle))):-1;

			xx = x[counter]-10;
			yy = y[counter];
			r = sqrt(pow((float)xx-shiftConsole, 2) + pow((float)shiftConsole-yy, 2) ) - (BodyCm/2);
			theta = atan2(shiftConsole-yy, xx-shiftConsole)*ToDegrees;
			theta = theta>=0?theta:360 + theta;
			coord = xx*1000+yy;
			//out << " coord: " << coord << " cell: " <<  (r >= 0?(int(r/distance))*N+(int(theta/(RotationAngle))):-1) <<" xx: " <<xx << " yy: " << yy << "   r: " << r << " theta: " << theta  << endl;
			MoveGrid[coord] =  r >= 0?(int(r/distance))*N+(int(theta/(RotationAngle))):-1;

			xx = x[counter];
			yy = y[counter]-10;
			r = sqrt(pow((float)xx-shiftConsole, 2) + pow((float)shiftConsole-yy, 2) ) - (BodyCm/2);
			theta = atan2(shiftConsole-yy, xx-shiftConsole)*ToDegrees;
			theta = theta>=0?theta:360 + theta;
			coord = xx*1000+yy;
			//out << " coord: " << coord << " cell: " <<  (r >= 0?(int(r/distance))*N+(int(theta/(RotationAngle))):-1) <<" xx: " <<xx << " yy: " << yy << "   r: " << r << " theta: " << theta  << endl;
			MoveGrid[coord] =  r >= 0?(int(r/distance))*N+(int(theta/(RotationAngle))):-1;

			xx = x[counter]-10;
			yy = y[counter]-10;
			r = sqrt(pow((float)xx-shiftConsole, 2) + pow((float)shiftConsole-yy, 2) ) - (BodyCm/2);
			theta = atan2(shiftConsole-yy, xx-shiftConsole)*ToDegrees;
			theta = theta>=0?theta:360 + theta;
			coord = xx*1000+yy;
			//out << " coord: " << coord << " cell: " <<  (r >= 0?(int(r/distance))*N+(int(theta/(RotationAngle))):-1) <<" xx: " <<xx << " yy: " << yy << "   r: " << r << " theta: " << theta  << endl;
			MoveGrid[coord] =  r >= 0?(int(r/distance))*N+(int(theta/(RotationAngle))):-1;

			counter++;
		}
	}
}

//age all other cells besides those that were resently updated
void ObstacleAvoidance::ageRestGrid(){
	for(int i=0;i<M;i++)
		for (int j=0;j<N;j++){
			if(j >= 13 && j <=21) continue;
			PolarGrid[i][j] = (PolarGrid[i][j]-NoKnowledge)*0.9 + NoKnowledge;
		}
}

//age all other cells besides those that were resently updated
void ObstacleAvoidance::ageSpecGrid(int x, int y){
	PolarGrid[x][y] = (PolarGrid[x][y]-NoKnowledge)*0.9 + NoKnowledge;
}

void ObstacleAvoidance::smoothGrid(int smooth){
	//psaxnei gia kelia pou peritrigyrizontai apo sigoura empodia
	//h sigourh apousia empodiwn kai ta enhmerwnei antistoixa
	if(smooth !=0){
	//no obstacle right behind the robot while it moves..
	//front
		if (smooth==1)
			for(int l=0;l<N/8;l++) {
				PolarGrid[0][l]=0.0;
				PolarGrid[0][l+int(7*N/8)]=0.0;
			}
		//back
		else if(smooth == -1)
			for(int l=N/8;l<5*N/8;l++)
				PolarGrid[0][l]=0.0;
		//right
		else if(smooth==3)
			for(int l=5*N/8;l<7*N/8;l++)
				PolarGrid[0][l]=0.0;
		//left
		else if(smooth==-3)
			for(int l=N/8;l<3*N/8;l++)
			PolarGrid[0][l]=0.0;
		//frontright
		else if(smooth == 2)
			for(int l=3*N/4;l<N;l++)
				PolarGrid[0][l]=0.0;
		//backleft
		else if(smooth == -2)
			for(int l=N/4;l<N/2;l++)
				PolarGrid[0][l]=0.0;
		//frontleft
		else if(smooth == 4)
			for(int l=0;l<N/4;l++)
				PolarGrid[0][l]=0.0;
		//backright
		else if(smooth == -4)
			for(int l=N/2;l<3*N/4;l++)
				PolarGrid[0][l]=0.0;

		for (int k=0;k<M;k++)
			for(int l=0;l<N;l++){
			//horizontal : koitame keli para keli kai an exoun kolntinh pi8anothta 8etoume kai sto endiameso aythn thn pi8anothta
			if (PolarGrid[k][l] != NoKnowledge && PolarGrid[k][l+2<N?l+2:(l+2)%N]==PolarGrid[k][l])
				PolarGrid[k][l+1<N?l+1:(l+2)%N] =  PolarGrid[k][l];
			//vertical
			if (PolarGrid[k][l] != NoKnowledge && PolarGrid[l+2<N?l+2:(l+2)%N][l]==PolarGrid[k][l])
				PolarGrid[l+2<N?l+2:(l+2)%N][l] =  PolarGrid[k][l];
			}
		smooth=0;
	}
}

void ObstacleAvoidance::updateFront(){
	for(int i=0;i<3;i++)
		if(abs(PolarGrid[i][15] - PolarGrid[i][21]) <= 0.1)
			for(int k =16;k<19;k++)
				PolarGrid[i][k] = (PolarGrid[i][15] + PolarGrid[i][21])/2;
	for(int i=3;i<M;i++)
		for(int j= 16;j< 19;j++)
			PolarGrid[i][j] = (PolarGrid[i][j]-NoKnowledge)*0.9 + NoKnowledge;
}

void ObstacleAvoidance::updateGrid(double (&left)[SOnARsNum], double (&right)[SOnARsNum]){//left, right
	int index, range=2, v=0;
	double temp[2], possibilityUp, possibilityDown, gridValue;
	mprosta=false;
	dexia= false;
	aristera = false;

	//Gia periptwsh eleipsis empodiou -> meiwsh pi8anothtas yparxis empodiou
	Logger::Instance().WriteMsg("ObstacleAvoidance", "countLeft " + _toString(countLeft) +  " countRight " + _toString(countRight) , Logger::ExtraExtraInfo);

	//in case of no obstacle reduce possibility in the front
	if (countRight >= 19 || countRight ==0  ){
		for(int i=0;i<M;i++){
			for(int j=FRONT - SonarCellRange -1 ;j<FRONT  ;j++){ //13:16
				PolarGrid[i][j]=(PolarGrid[i][j]*PossibilityDiscountNoObst<=0.05)?0.05:PolarGrid[i][j]*PossibilityDiscountNoObst;
			}PolarGrid[i][FRONT]=(PolarGrid[i][FRONT]*PossibilityDiscountNoObst<=0.05)?0.05:PolarGrid[i][FRONT]*PossibilityDiscountNoObst; //17
		}
	}
	if (countLeft >= 19 || countLeft ==0 ){
		for(int i=0;i<M;i++){
			for(int j=FRONT +1 ;j<FRONT + SonarCellRange +range ;j++){ //18:21
				PolarGrid[i][j]=PolarGrid[i][j]*PossibilityDiscountNoObst<=0.05?0.05:(PolarGrid[i][j]*PossibilityDiscountNoObst);
			}PolarGrid[i][FRONT]=PolarGrid[i][FRONT]*PossibilityDiscountNoObst<=0.05?0.05:(PolarGrid[i][FRONT]*PossibilityDiscountNoObst); //17
		}
	}

	for (int i=SOnARsNum-1;i>=0;i--){

		left[i] = left[i] == 0.29?0.3:left[i] ;
		right[i] = right[i] == 0.29?0.3:right[i] ;
		left[i] = (left[i] < TooFar && left[i] > TooClose)?left[i]:0.0; //left
		right[i] = (right[i] < TooFar && right[i] > TooClose)?right[i]:0.0; //right
		if(left[i] == 0.0 && right[i] == 0.0) continue;
		temp[0] = left[i];
		temp[1] = right[i];


		for(int k=0;k<2;k++){
			index=(int)(temp[k]/0.1 - 3);//-2 giati den mas endiaferei mexri ta 20 cm
			if(index < 0) continue;
			for(int j=((k==0)?FRONT-SonarCellRange-1:FRONT+SonarCellRange-1);j<=((k==0)?(FRONT-SonarCellRange-1+range):(FRONT+SonarCellRange-1+range));j++){//13 k 19
				gridValue = PolarGrid[index][j];
				PolarGrid[index][j]=PolarGrid[index][j]*1.6>=1.0?1.0:PolarGrid[index][j]*1.6;
				v=1;
				possibilityUp = UsePossibilityUp;
				while (index-v >= 0 || index+v < M ){
					if (index-v>=0)
						PolarGrid[index-v][j]=(PolarGrid[index-v][j]*UsePossibilityDown<=0.05)?0.05:PolarGrid[index-v][j]*UsePossibilityDown;
					if (index+v<M)
						if (v >= 2)
							ageSpecGrid(index+v, j);
						else
							PolarGrid[index+v][j]=(gridValue*possibilityUp>=1.0)?1.0:(gridValue*possibilityUp  <= PolarGrid[index+v][j]?PolarGrid[index+v][j]:gridValue*possibilityUp);
					v++;
					possibilityUp *= PossibilityDiscount;
				}
			}
		}
	}

	updateFront();
	ageRestGrid();

	for(int i=0;i<M;i++){
		mprosta = PolarGrid[i][FRONT] >= 0.8?true:false;
		mprostaDist = mprosta == true?(i+1):0;
		mprostaCert = mprosta == true?PolarGrid[i][FRONT]:0;
		break;
	}
	for(int i=0;i<M;i++){
		dexia = PolarGrid[i][FRONT] >= 0.8?true:false;
		dexiaDist = dexia == true?(i+1):0;
		dexiaCert = dexia == true?PolarGrid[i][FRONT]:0;
		break;
	}
	for(int i=0;i<M;i++){
		aristera = PolarGrid[i][FRONT] >= 0.8?true:false;
		aristeraDist = aristera == true?(i+1):0;
		aristeraCert = aristera == true?PolarGrid[i][FRONT]:0;
		break;
	}

	//printSonarValues();
	//drawGrid();
}


void ObstacleAvoidance::ageGrid(){
	for (int i=0;i<M;i++)
		for(int j=0;j<N;j++)
			PolarGrid[i][j] = (PolarGrid[i][j]-NoKnowledge)*0.9 + NoKnowledge;
}

void ObstacleAvoidance::findNewPosition(){
	double temp[2]={0.0, 0.0};
	double anglee;
	double value1=0.0, value2 = 0.0;
	temp[0] =(fabs(PosX.sensorvalue())-fabs(RobotPosition[0]))*100;
	temp[1] =(fabs(PosY.sensorvalue())-fabs(RobotPosition[1]))*100;
	//Logger::Instance().WriteMsg("ObstacleAvoidance", "temp[0] " + _toString(temp[0]) +  " temp[1] " + _toString(temp[1]) , Logger::Info);
	if (fabs(Angle.sensorvalue()-(RobotPosition[2]+resAngle))*ToDegrees > RotationAngle ){ //rotate  PI/18
		anglee =(RobotPosition[2]-Angle.sensorvalue())*ToDegrees;
		rotateGrid(fabs(anglee));
		resAngle = fmod(RobotPosition[2]-Angle.sensorvalue(), (double)RotationAngleRad);
		RobotPosition[2]=Angle.sensorvalue();
	}

	value1 = int(temp[0] + resX);
	value2 = int(temp[1] + resY);
	Logger::Instance().WriteMsg("ObstacleAvoidance", "value1 " + _toString(value1) +  " value2 " + _toString(value2) , Logger::Info);
	if(value1 > 20 || value2 > 20){
		RobotPosition[0]=PosX.sensorvalue();
		RobotPosition[1]=PosY.sensorvalue();
		resX =0;
		resY =0;
		value1=0;
		value2=0;
		initPolarGrid();
	}

	//Logger::Instance().WriteMsg("ObstacleAvoidance", "value1 " + _toString(value1) +  " value2 " + _toString(value2) , Logger::Info);
	//cout << "value1: " << value1 << " value2: " << value2 << endl;
	if(abs(value1) > distance && abs(value2) > distance){
		straightMove(10, 10);
		resX = resX+value1-10;
		RobotPosition[0]=PosX.sensorvalue();
		resY = resY+value2-10;
		RobotPosition[1]=PosY.sensorvalue();
	}
	else{
		if(abs(value1) > distance){
			straightMove(10, 0);
			resX = resX+value1-10;
			RobotPosition[0]=PosX.sensorvalue();
		}
		if(abs(value2) > distance){
			straightMove(0, 10);
			resY = resY+value2-10;
			RobotPosition[1]=PosY.sensorvalue();
		}
	}
}



void ObstacleAvoidance::straightMove(int distanceFront, int distanceSide){
	double px[6], py[6], temp;
	int index=0, indey=0, cellnum=0, smooth=0;
	//Logger::Instance().WriteMsg("ObstacleAvoidance", "move front " + _toString(distanceFront) + "move side " + _toString(distanceSide), Logger::ExtraExtraInfo);
	//initialize changed
	//cout << "move" << endl;
	//initChanged();
	for(int i=0;i<M;i++)
	  for(int j=0;j<N;j++)
		changed[i*N+j]=0;
	for(int i=0;i<M;i++)
		for(int j=0;j<N;j++){
			temp = PolarGrid[i][j];
			if (fabs(PolarGrid[i][j]-NoKnowledge) >= 0.1 && changed[i*N+j]==0){ //check only the cells with obstacles.
				PolarGrid[i][j] = NoKnowledge;
				//syntetagmenes pou orizoun to keli
				px[0] = x[N*i+j]+distanceFront;			py[0] = y[N*i+j]+distanceSide;
				px[1] = x[N*(i+1)+j]+distanceFront;		py[1] = y[N*(i+1)+j]+distanceSide;
				px[2] = x[N*(i+1)+j+1]+distanceFront;	py[2] = y[N*(i+1)+j+1]+distanceSide;
				px[3] = x[N*i+j+1]+distanceFront;		py[3] = y[N*i+j+1]+distanceSide;

				/*for (int k=0;k<=5;k++){

					index = int((px[k] -1)/2);
					indey = int((py[k] -1)/2);

					cellnum = MoveGrid[index*100+indey];
					//cout << "index: " << index*100+indey << "cellnum: " <<  cellnum << endl;

					if (cellnum != -1 && cellnum ){
						PolarGrid[int(cellnum/N)][int(cellnum%N)] = temp*0.95;
						changed[cellnum] = 1;
					}
				}*/

				for (int k=0;k<=3;k++){
					cellnum = MoveGrid[(px[k])*1000+py[k]];
					if ((cellnum/N)<M && (cellnum/N) >=0){
						PolarGrid[int(cellnum/N)][int(cellnum%N)] = (temp*0.95) <= 0.05 ? 0.05 : temp*0.95;
						changed[cellnum] = 1;
					}
				}
			}
		}
	cvDrawGrid();
	if(distanceFront > 0) {
		if (distanceSide > 0)
			smooth=4;
		else if (distanceSide < 0)
			smooth=2;
		else smooth=1;
	}
	else if(distanceFront < 0){
		if (distanceSide > 0)
			smooth=-4;
		else if (distanceSide < 0)
			smooth=-2;
		else smooth=-1;
	}
	else if(distanceSide > 0) smooth=3;
	else smooth = -3;

	smoothGrid(smooth);
}



void ObstacleAvoidance::rotateGrid(double angle){
	double temp[M][N];
	int ll=int(angle/RotationAngle);
	int change[N];
	//Logger::Instance().WriteMsg("ObstacleAvoidance", "rotateangle " + _toString(ll), Logger::ExtraExtraInfo);
	for (int i=0;i<M;i++){
		for(int j=0;j<N;j++) change[j] = 0;//init change for every ring
		for(int j=0;j<N;j++){
		  temp[i][(j+ll)%N] = PolarGrid[i][(j+ll)%N];
		  if(j+ll < 0) ll=N+ll;
		  if(change[j]==0){
			PolarGrid[i][(j+ll)%N] = PolarGrid[i][j];
			change[(j+ll)%N] = 1;
		  }
		  else{
			PolarGrid[i][(j+ll)%N] = temp[i][j];
			change[(j+ll)%N] = 1;
		  }
		}
	}
	Logger::Instance().WriteMsg("ObstacleAvoidance", "---------------------------------------------------------", Logger::Info);
	//drawGrid();
}

void ObstacleAvoidance::findCoordinates(){
	int tempCnt=0, dist;
	for (int i=0;i<M+1;i++){
		dist = (i+1)*distance;
		for (int j=N;j>0;j--){
			x[tempCnt] = int((dist * (cos(RotationAngleRad*j)))+shiftConsole);
			y[tempCnt] = int((dist * (sin(RotationAngleRad*j)))+shiftConsole);
			//Logger::Instance().WriteMsg("ObstacleAvoidance", "coord " + _toString(x[tempCnt]) +  " coordy " + _toString(y[tempCnt]) , Logger::ExtraExtraInfo);
			tempCnt++;
		}
	}
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
	indexx[0] = mm!=M-1?mm+1:mm;
	indexx[1] = mm!=M-1?mm+1:mm;
	indexx[2] = mm;
	indexx[3] = mm!=0?mm-1:mm;
	indexx[4] = mm!=0?mm-1:mm;
	indexx[5] = mm!=0?mm-1:mm;
	indexx[6] = mm;
	indexx[7] = mm!=M-1?mm+1:mm;
	indexx[8] = mm;

	indexy[0] = nn;
	indexy[1] = nn!=N-1?nn+1:0;
	indexy[2] = nn!=N-1?nn+1:0;
	indexy[3] = nn!=N-1?nn+1:0;
	indexy[4] = nn ;
	indexy[5] = nn!=0?nn-1:N-1;
	indexy[6] = nn!=0?nn-1:N-1;
	indexy[7] = nn!=0?nn-1:N-1;
	indexy[8] = nn;
}

void ObstacleAvoidance::iterationGridDir(int mm, int nn){
	initIndexes(mm, nn);
	for(int i=0;i<NEIGHBOURS+1;i++)
		value[i] = IterationGrid[indexx[i]][indexy[i]];
}

void ObstacleAvoidance::bestPath(int goalx, int goaly){
	int initm=0, initn=FRONT, i=1,g=0;
	double bigger=-10, temp;
	int pathx[10], pathy[10];
	//briskw prwto eley8ero keli gia kinhsh an oxi (0,17)
	while(PolarGrid[initm][initn] > 0.75){
		if (PolarGrid[initm][initn+1] < 0.6){
		  initn=initn+1;
		  break;
		}
		if (PolarGrid[initm][initn-i] < 0.6){
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
	  if(initm == pathx[g-1] && initn == pathy[g-1]) break;
	}while(initm != goalx || initn!=goaly);

	for (i=0;i<g;i++)
		IterationGrid[pathx[i]][pathy[i]] = 2.0;
	printIterationGrid();
}

void ObstacleAvoidance::fillIterationGrid(){

  //int  disc, k, a, i=0;
	double sum=0.0, sumold = 0.0, max=0.0;
	initPossibilities();
	for(int it=0;it<20;it++){
	//do{
		for(int mm=0;mm<M;mm++){
		  for(int nn=0;nn<N;nn++){
		  //nn=21;mm=5;
			sum=0.0;
			sumold=0.0;
			max=0.0;
			iterationGridDir(mm, nn); //bazw sta value tis times twn gyrw keliwn tou (mm, nn)
			initIndexes(mm, nn);
			if (PolarGrid[mm][nn] > HighObstaclePossibility)
			  IterationGrid[mm][nn] = obstacle;
			else if(IterationGrid[mm][nn] == goal)
			  IterationGrid[mm][nn] = goal;
			else{
			  for(int a=0;a<NEIGHBOURS;a++){
				sum=0.0;
				for (int k=0;k<NEIGHBOURS;k++){
					if(mm==0){
						if(k==3 || k==4 || k==5)
							sum = sum + IterationGrid[indexx[8]][indexy[8]]*possibilities[(k-a>0?k-a:NEIGHBOURS+k-a)%NEIGHBOURS];
						else
							sum = sum + IterationGrid[indexx[k]][indexy[k]]*possibilities[(k-a>0?k-a:NEIGHBOURS+k-a)%NEIGHBOURS];
					}
					else if(mm==M-1){
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
			  if (fabs(IterationGrid[mm][mm] - reward+max) < 0.1){
				Logger::Instance().WriteMsg("ObstacleAvoidance", "returning..." + _toString(it), Logger::ExtraExtraInfo);
				return;
			}
			  IterationGrid[mm][nn] = reward + max;
			}
		  }
		}
	}//while(1);
	printIterationGrid();
}

void ObstacleAvoidance::printIterationGrid(){
for (int i=5;i<25;i++){
		Logger::Instance().WriteMsg("ObstacleAvoidance", " " + _toString(i) + " " + _toString(IterationGrid[0][i]) +" | " + _toString(IterationGrid[1][i]) + " | " + _toString(IterationGrid[2][i])
				+ " | " + _toString(IterationGrid[3][i])  + " | " + _toString(IterationGrid[4][i])
				+ " | " + _toString(IterationGrid[5][i])  + " | " + _toString(IterationGrid[6][i]) , Logger::ExtraExtraInfo);
		Logger::Instance().WriteMsg("ObstacleAvoidance", "    ------------------------------------------ ", Logger::ExtraExtraInfo);
	}
}


void ObstacleAvoidance::pathPlanningInit(int goalx, int goaly){

	/*int index, indey;
	index = int(goalx/distance);
	indey = int(goaly/N);
	IterationGrid[index][indey] = goal;
	*/

	//uncomment for path planning example
	PolarGrid[5][16] = 1.0;
	PolarGrid[5][15] = 1.0;
	PolarGrid[5][14] = 1.0;
	PolarGrid[2][20] = 1.0;
	PolarGrid[2][21] = 1.0;
	PolarGrid[2][22] = 1.0;
	PolarGrid[2][23] = 1.0;

	//IterationGrid[6][21] = 0.0;

	//initialize iterationGrid to be same as PolarGrid
	for (int i=0;i<M;i++)
		for (int j=0;j<N;j++)
			IterationGrid[i][j] = PolarGrid[i][j] ;
	IterationGrid[goalx][goaly] = goal;
	fillIterationGrid();
	//printIterationGrid();
	bestPath(goalx, goaly);
}

void ObstacleAvoidance::drawGrid(){
	//int val[M];

	for (int i=11;i<21;i++){
		Logger::Instance().WriteMsg("ObstacleAvoidance", " " + _toString(i) + " " + _toString(PolarGrid[0][i]) +" | " + _toString(PolarGrid[1][i]) + " | " + _toString(PolarGrid[2][i])
				+ " | " + _toString(PolarGrid[3][i])  + " | " + _toString(PolarGrid[4][i])
				+ " | " + _toString(PolarGrid[5][i])  + " | " + _toString(PolarGrid[6][i]) , Logger::ExtraExtraInfo);
		Logger::Instance().WriteMsg("ObstacleAvoidance", "    ------------------------------------------ ", Logger::ExtraExtraInfo);
	}
	/*for (int i=0;i<35;i++){
		for (int j=0;j<M;j++)
			val[j]=PolarGrid[j][i] > 0.6 ?1:0;
		Logger::Instance().WriteMsg("ObstacleAvoidance", " " + _toString(i) + " " + _toString(val[0]) +" | " + _toString(val[1]) + " | " + _toString(val[2])
				 + " | " + _toString(val[3])  + " | " + _toString(val[4])
				 + " | " + _toString(val[5])  + " | " + _toString(val[6]) , Logger::ExtraExtraInfo);
		Logger::Instance().WriteMsg("ObstacleAvoidance", "    -------------------------------------------- ", Logger::ExtraExtraInfo);
	}*/
	Logger::Instance().WriteMsg("ObstacleAvoidance", "    -------------------------------------------- ", Logger::ExtraExtraInfo);
}

void ObstacleAvoidance::cvDrawGrid(){


	int k=0, l=0;
	CvPoint* curveArr={curve1};



	for (k=0;k<M;k++){
		for(l=0;l<N-1;l++){
			curve1[0].x = x[k*N+l];      curve1[0].y = y[k*N+l];
			curve1[1].x = x[(k+1)*N+l];  curve1[1].y = y[(k+1)*N+l];
			curve1[2].x = x[(k+1)*N+1+l];curve1[2].y = y[(k+1)*N+1+l];
			curve1[3].x = x[k*N+1+l];    curve1[3].y = y[k*N+1+l];
			if(PolarGrid[k][l] < 0.2)	paintColor = white;
			else if(PolarGrid[k][l]  < 0.4)	paintColor = lightgrey;
			else if(PolarGrid[k][l]  < 0.6)	paintColor = grey;
			else if(PolarGrid[k][l]  < 0.8)	paintColor = darkgrey;
			else if(PolarGrid[k][l]  <=1)	paintColor = black;
			else paintColor = grey;
			cvFillPoly( img, &curveArr,&nCurvePts, nCurves, paintColor);
			cvPolyLine( img,&curveArr,&nCurvePts, nCurves, isCurveClosed, white, 1, lineWidth, 0 );
		}
		curve1[0].x =x[k*N+N-1];     curve1[0].y = y[k*N+N-1];
		curve1[1].x =x[(k+1)*N+N-1]; curve1[1].y = y[(k+1)*N+N-1];
		curve1[2].x =x[(k+1)*N];     curve1[2].y = y[(k+1)*N];
		curve1[3].x =x[k*N];         curve1[3].y = y[k*N];

		if(PolarGrid[k][l] < 0.2)	paintColor = white;
		else if(PolarGrid[k][l]  < 0.4)	paintColor = lightgrey;
		else if(PolarGrid[k][l]  < 0.6)	paintColor = grey;
		else if(PolarGrid[k][l]  < 0.8)	paintColor = darkgrey;
		else if(PolarGrid[k][l]  <=1)	paintColor = black;
		else paintColor = grey;
	    cvFillPoly( img,&curveArr,&nCurvePts, nCurves, paintColor);
	    cvPolyLine( img,&curveArr,&nCurvePts, nCurves, isCurveClosed, white, 1, lineWidth, 0 );
	}

	cvShowImage("img", img);
	cvWaitKey(10);


	return;

}

void ObstacleAvoidance::drawIterationGrid(){

	CvPoint curve1[4];

	int k=0, l=0;
	CvPoint* curveArr={curve1};
	int      nCurvePts=4;
	int      nCurves=1;

	int      isCurveClosed=1;
	int      lineWidth=8;
	CvScalar white = cvScalar(255,255,255);
	CvScalar lightgrey = cvScalar(190,190,190);
	CvScalar grey = cvScalar(125,125,125);
	CvScalar darkgrey = cvScalar(65,65,65);
	CvScalar black = cvScalar(0,0,0);
	CvScalar blue = cvScalar(255, 0, 0);
	CvScalar paintColor = grey;

	for (k=0;k<M;k++){
		for(l=0;l<N-1;l++){
			curve1[0].x = x[k*N+l];      curve1[0].y = y[k*N+l];
			curve1[1].x = x[(k+1)*N+l];  curve1[1].y = y[(k+1)*N+l];
			curve1[2].x = x[(k+1)*N+1+l];curve1[2].y = y[(k+1)*N+1+l];
			curve1[3].x = x[k*N+1+l];    curve1[3].y = y[k*N+1+l];
			if(IterationGrid[k][l] < 0.2)	paintColor = white;
			else if(IterationGrid[k][l]  < 0.4)	paintColor = lightgrey;
			else if(IterationGrid[k][l]  < 0.6)	paintColor = grey;
			else if(IterationGrid[k][l]  < 0.8)	paintColor = darkgrey;
			else if(IterationGrid[k][l]  <=1.0)	paintColor = black;
			else if(IterationGrid[k][l]  >1.0)	paintColor = blue;
			else paintColor = grey;
			cvFillPoly( img, &curveArr,&nCurvePts, nCurves, paintColor);
			cvPolyLine( img,&curveArr,&nCurvePts, nCurves, 1, white, isCurveClosed, lineWidth, 0 );
		}
		curve1[0].x =x[k*N+N-1];     curve1[0].y = y[k*N+N-1];
		curve1[1].x =x[(k+1)*N+N-1]; curve1[1].y = y[(k+1)*N+N-1];
		curve1[2].x =x[(k+1)*N];     curve1[2].y = y[(k+1)*N];
		curve1[3].x =x[k*N];         curve1[3].y = y[k*N];

		if(IterationGrid[k][N-1] < 0.2)	paintColor = white;
		else if(IterationGrid[k][N-1]  < 0.4)	paintColor = lightgrey;
		else if(IterationGrid[k][N-1]  < 0.6)	paintColor = grey;
		else if(IterationGrid[k][N-1]  < 0.8)	paintColor = darkgrey;
		else if(IterationGrid[k][N-1]  <=1.0)	paintColor = black;
		else if(IterationGrid[k][N-1]  ==2.0)	paintColor = blue;
		else paintColor = grey;
	    cvFillPoly( img,&curveArr,&nCurvePts, nCurves, paintColor);
	    cvPolyLine( img,&curveArr,&nCurvePts, nCurves, 1, white, isCurveClosed, lineWidth, 0 );
	}

	cvNamedWindow("img", CV_WINDOW_AUTOSIZE);
	cvShowImage("img", img);
	cvWaitKey(0);
	//printIterationGrid();

}


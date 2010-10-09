#include "ObstacleAvoidance.h"
#include "tools/toString.h"
#include <iostream>

#include "tools/logger.h"

namespace {
	ActivityRegistrar<ObstacleAvoidance>::Type temp("ObstacleAvoidance");
}

ObstacleAvoidance::ObstacleAvoidance() :
	Publisher("ObstacleAvoidance") {
		;
}

void ObstacleAvoidance::UserInit() {
	
	_com->get_message_queue()->add_subscriber(_blk);
	_com->get_message_queue()->subscribe("sensors", _blk, 0);
	_com->get_message_queue()->subscribe("behavior", _blk, 0);
	_com->get_message_queue()->add_publisher(this);
	
	//obavm = new ObstacleMessage();

	//ypoloipa metakinhsewn
	resX=0.0;
	resY=0.0;
	resAngle=0.0;
	
	//mnmata pou diabazei
	ussm =0;
	rpsm=0;
	
	shift = 0;  //shift==metatopish gia probolh grid sthn ekastote konsola
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
		delete rpsm;
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
		//printSonarValues();
		updateGrid(countRight!=0?Right:empty, countLeft!=0?Left:empty);
		delete ussm;
	}
	
	//aging grid every 3 seconds
	if(countAge == 30){
		countAge =0;
		ageGrid();
	}
	
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
	_blk->process_messages();
	ussm = _blk->in_msg_nb<UltaSoundSensorsMessage>("UltaSoundSensorsMessage");
	rpsm = _blk->in_msg_nb<RobotPositionSensorMessage>("RobotPositionSensorMessage");
	//targetX = _blk->in_msg_nb<RobotPositionSensorMessage>("targetX", "Behavior");
	//targetY = _blk->in_msg_nb<RobotPositionSensorMessage>("targetY", "Behavior");
	Logger::Instance().WriteMsg("ObstacleAvoidance", "read messages " , Logger::ExtraExtraInfo);
}

void ObstacleAvoidance::publishObstacleMessage(){
	//Logger::Instance().WriteMsg("ObstacleAvoidance", "mprosta dist " + _toString(mprostaDist) + "cert "+_toString(mprostaCert) , Logger::ExtraExtraInfo);
	//Logger::Instance().WriteMsg("ObstacleAvoidance", "dexia dist " + _toString(dexiaDist) + "cert "+_toString(dexiaCert) , Logger::ExtraExtraInfo);
	//Logger::Instance().WriteMsg("ObstacleAvoidance", "aristera dist " + _toString(aristeraDist) + "cert "+_toString(aristeraCert) , Logger::ExtraExtraInfo);


	obavm.set_direction(0, aristera?1:0);
	obavm.set_direction(1, mprosta?1:0);
	obavm.set_direction(2, dexia?1:0);
	
	obavm.set_distance(0, aristera?aristeraDist:0);
	obavm.set_distance(1, mprosta?mprostaDist:0);
	obavm.set_distance(2, dexia?dexiaDist:0);
	
	obavm.set_certainty(0, aristera?aristeraCert:0);
	obavm.set_certainty(1, mprosta?mprostaCert:0);
	obavm.set_certainty(2, dexia?dexiaCert:0);
	
	Publisher::publish(&obavm,"obstacle");
}

void ObstacleAvoidance::initGrid(){
	//int x, y;
	//double r, theta;
	
	//initialize the variables of the robot position
	for (int i=0;i<RobotDirections;i++)
		RobotPosition[i]=0.0;
		
	//initialize arrays with sonar values
	for (int i=0;i<SOnARsNum;i++){
		rightArray[i]=0.0;
		leftArray[i]=0.0;
	}
	
	//initialize Polar grid
	for(int i=0;i<M;i++)
		for(int j=0;j<N;j++)
			PolarGrid[i][j] = NoKnowledge;
	
	
	//PolarGrid[3][16]=1.0;
	
	/*PolarGrid[2][14]=1.0;
	PolarGrid[2][15]=1.0;
	PolarGrid[3][11]=0.789;
	*/

	/*for (int i=0;i<100;i++)
		for (int j=0;j<100;j++){
			x=i-50;
			y=j-50;
			r = sqrt(pow(x, 2) + pow(y, 2));
			theta = -atan2(y, x)*ToDegrees;
			if (theta<0) theta=360+theta;
			MoveGrid[i][j] = int ((r/distance)*N+(theta/RotationAngle)); //no sto PolarGrid 
			//Logger::Instance().WriteMsg("ObstacleAvoidance", "theta " + _toString(r) , Logger::Info);
		}
	*/
	
	//oi syntetagmenes pou antistoixoun sta 4 shmeia pou orizoun to keli
	findCoordinates();

}

void ObstacleAvoidance::smoothGrid(int smooth){
	//psaxnei gia kelia pou peritrigyrizontai apo sigoura empodia
	//h sigourh apousia empodiwn kai ta enhmerwnei antistoixa
	if(smooth==1){
	//no obstacle right behind the robot while it moves..
		for(int l=0;l<N/4;l++) {
			PolarGrid[0][l]=0.0;
			PolarGrid[0][l+3*N/4]=0.0;
		}
		for (int k=0;k<M;k++)
			for(int l=0;l<N;l++){
			//horizontal : koitame keli para keli kai an exoun kolntinh pi8anothta 8etoume kai sto endiameso aythn thn pi8anothta
			if (PolarGrid[k][l] != NoKnowledge && PolarGrid[k][l+2<M?l+2:(l+2)%N]==PolarGrid[k][l])
				PolarGrid[k][l+1<N?l+1:(l+2)%N] =  PolarGrid[k][l];
			//vertical
			if (PolarGrid[k][l] != NoKnowledge && PolarGrid[l+2<M?l+2:(l+2)%M][l]==PolarGrid[k][l])
				PolarGrid[l+2<N?l+2:(l+2)%M][l] =  PolarGrid[k][l];      
			}
		smooth=0;
	}
}

void ObstacleAvoidance::updateGrid(double (&newValues1)[SOnARsNum], double (&newValues2)[SOnARsNum]){//right, left
	int index, range=2, holdj=0, v=0;
	double temp[2];
	
	for(int i=SOnARsNum-1;i>=0;i--){
		//Logger::Instance().WriteMsg("ObstacleAvoidance", "in_update new1 " + _toString(newValues1[i]) +  " new2 " + _toString(newValues2[i]) , Logger::ExtraExtraInfo);
		if (newValues1[i] > TooFar && newValues2[i] > TooFar )
			continue;
		if(newValues1[i] == 0.0) continue;
		temp[0]=(newValues1[i] < TooFar && newValues1[i] > TooClose)?newValues1[i]:0.0; //left
		for (int l=0;l<10;l++){ //gia na brw an yparxei sto temp[1] timh konta sto temp[0]
			temp[1]=(newValues2[l] < TooFar && newValues2[l] > TooClose)?newValues2[l]:0.0; //right
			if (int(temp[0]/0.1) == int(temp[1]/0.1)) { //apenanti
				
				v=0;
				for(int k=0;k<2;k++){
					if(temp[k] > 0.0) {  //alliws den mas endiaferei giati einai eite poly makrya eite poly konta
						index=(temp[k]>=0.3)?(int)(temp[k]/0.1 - 3):0;//(int)(temp[k]/0.1 - 3); //-2 giati den mas endiaferei mexri ta 20 cm
						for(int j=FRONT - SonarCellRange +1 ;j<FRONT + SonarCellRange -1  ;j++){ //15:19
							PolarGrid[index][j]=PolarGrid[index][j]*1.1>=1.0?1.0:PolarGrid[index][j]*1.1;
							v=0;
							do{
								if (index-v>=0)
									PolarGrid[index-v][j]=PolarGrid[index-v][j]*UsePossibilityDown<=0.0?0.0:PolarGrid[index-v][j]*UsePossibilityDown;
								if (index+v<M)
									PolarGrid[index+v][j]=PolarGrid[index+v][j]*UsePossibilityUp>=1.0?1.0:PolarGrid[index+v][j]*UsePossibilityUp;
								v++;
							}while(index-v >= 0 || index+v < M);
						}
					}
				}
				mprostaCert = PolarGrid[(temp[1]>=0.3)?(int)(temp[1]/0.1 - 3):0][FRONT];
				mprosta = true;
				mprostaDist = temp[0]; 
				if (temp[0]/0.1 <= 3 && PolarGrid[0][FRONT]>HighObstaclePossibility){
					Logger::Instance().WriteMsg("ObstacleAvoidance", "STOOOOOOOOOOOOOOOP" , Logger::ExtraExtraInfo);
					//obavm->set_direction(0);
					//obavm->set_distance(mprostaDist);
					//obavm->set_certainty(mprostaCert);
				}
				newValues1[i] = 0.0;
				newValues2[l] = 0.0;
				
			}
		}
	}
	for(int i=0;i<SOnARsNum;i++){
		
		temp[0]=(newValues1[i] < TooFar && newValues1[i] > TooClose)?newValues1[i]:0.0; //left
		//gia tis ypoloipes times pou deixnoun parousia empodiou sta plagia
		temp[1]=(newValues2[i] < TooFar && newValues2[i] > TooClose)?newValues2[i]:0.0; //right
		//Logger::Instance().WriteMsg("ObstacleAvoidance", "in_update rest1 " + _toString(temp[0]) +  " rest2 " + _toString(temp[1]) , Logger::ExtraExtraInfo);
		for(int k=0;k<2;k++){
			if(temp[k] > 0.0) {  //alliws den mas endiaferei giati einai eite poly makrya eite poly konta
				//Logger::Instance().WriteMsg("ObstacleAvoidance", "oxi mprosta" , Logger::ExtraExtraInfo);
				///////////////////send message for left or right obstacle angle 30 
				
				index=(temp[k]>=0.3)?(int)(temp[k]/0.1 - 3):0;//(int)(temp[k]/0.1 - 3); //-2 giati den mas endiaferei mexri ta 20 cm
				for(int j=((k==0)?FRONT-SonarCellRange-1:FRONT+SonarCellRange-1);j<((k==0)?(FRONT-SonarCellRange-1+range):(FRONT+SonarCellRange-1+range));j++){			//13 k 19
					PolarGrid[index][j]=PolarGrid[index][j]*1.1>=1.0?1.0:PolarGrid[index][j]*1.1;
					v=0;
					do{
						if (index-v>=0)
							PolarGrid[index-v][j]=PolarGrid[index][j]*UsePossibilityDown<=0.0?0.0:PolarGrid[index-v][j]*UsePossibilityDown;
						if (index+v<M)
							PolarGrid[index+v][j]=PolarGrid[index][j]*UsePossibilityUp>=1.0?1.0:PolarGrid[index+v][j]*UsePossibilityUp;
						v++;
					}while(index-v >= 0 || index+v < M);
				}
				if (k == 0){
					dexia = true;
					dexiaDist = temp[k];
					dexiaCert = PolarGrid[(temp[k]>=0.3)?(int)(temp[k]/0.1 - 3):0][FRONT-SonarCellRange-1];
					//Logger::Instance().WriteMsg("ObstacleAvoidance", "dexia dist " + _toString(dexiaDist) + "cert "+_toString(dexiaCert) , Logger::ExtraExtraInfo);
				}else{
					aristera = true;
					aristeraDist = temp[k];
					aristeraCert = PolarGrid[(temp[k]>=0.3)?(int)(temp[k]/0.1 - 3):0][FRONT+SonarCellRange];
					//Logger::Instance().WriteMsg("ObstacleAvoidance", "aristera dist " + _toString(aristeraDist) + "cert "+_toString(aristeraCert) , Logger::ExtraExtraInfo);
				}
			}
		}
	}
	
	/*for (int i=0;i<SOnARsNum; i++){
		if(newValues1[i] < TooClose || newValues1[i] > TooFar ) break;
		newValues1[i]=(newValues1[i] < TooFar && newValues1[i] > TooClose)?newValues1[i]:0.0; //left
		//newValues2[i]=(newValues2[i] < TooFar && newValues2[i] > TooClose)?newValues2[i]:0.0; //right
		//Logger::Instance().WriteMsg("ObstacleAvoidance", "in_update new1 " + _toString(newValues1[i]) +  " new2 " + _toString(newValues2[i]) , Logger::ExtraExtraInfo);
		if (newValues1[i] ==0 && newValues2[i] > TooFar )
			break;
		//newValues1[i]=(newValues1[i] < TooFar && newValues1[i] > TooClose)?newValues1[i]:0.0; //left
		for (int j=0;j<SOnARsNum; j++){
			newValues2[j]=(newValues2[j] < TooFar && newValues2[j] > TooClose)?newValues2[j]:0.0; //right
			if(newValues1[i] == 0.0 ) break;
			if(newValues2[j] == 0.0 ) continue;
			
			temp[0] = newValues1[i];
			temp[1] = newValues2[j];
			Logger::Instance().WriteMsg("ObstacleAvoidance", "in_update new1 " + _toString(temp[0]) +  " new2 " + _toString(temp[1]) , Logger::ExtraExtraInfo);
			for(int k=0;k<2;k++){
				index=(newValues2[j]>=0.3)?(int)(newValues2[j]/0.1 - 3):0;//(int)(temp[k]/0.1 - 3); //-2 giati den mas endiaferei mexri ta 20 cm
				for(int j=15 ;j< 19 ;j++){  //15:19
					PolarGrid[index][j]=PolarGrid[index][j]*1.1>=1.0?1.0:PolarGrid[index][j]*1.1;
					if (index>1)
						PolarGrid[index-1][j]=PolarGrid[index-1][j]*UsePossibilityDown<=0.0?0.0:PolarGrid[index-1][j]*UsePossibilityDown;
					if (index<5)
						PolarGrid[index+1][j]=PolarGrid[index+1][j]*UsePossibilityUp>=1.0?1.0:PolarGrid[index+1][j]*UsePossibilityUp;
				}
			}
			
			//Logger::Instance().WriteMsg("ObstacleAvoidance", "in_update new1 " + _toString(newValues1[i]) +  " new2 " + _toString(newValues2[i]) , Logger::ExtraExtraInfo);
			if (int(newValues1[i]/0.1) == int(newValues2[j]/0.1)) { //apenanti
				mprosta = true;
				mprostaDist = newValues1[i];
				mprostaCert = PolarGrid[(newValues2[j]>=0.3)?(int)(newValues2[j]/0.1 - 3):0][FRONT];
				Logger::Instance().WriteMsg("ObstacleAvoidance", "apenanti index " + _toString((newValues2[j]>=0.3)?(int)(newValues2[j]/0.1 - 3):0) , Logger::ExtraExtraInfo);
			}
			
			
			Logger::Instance().WriteMsg("ObstacleAvoidance", "oxi apenanti" , Logger::ExtraExtraInfo);
			newValues1[i] = 0.0;
			newValues2[j] = 0.0;
			
		}
	}*/
	
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
	double value=0.0;
	temp[0] =(fabs(PosX.sensorvalue())-fabs(RobotPosition[0]))*100;
	temp[1] =(fabs(PosY.sensorvalue())-fabs(RobotPosition[1]))*100;
	//Logger::Instance().WriteMsg("ObstacleAvoidance", "temp[0] " + _toString(temp[0]) +  " temp[1] " + _toString(temp[1]) , Logger::Info);
	if (fabs(Angle.sensorvalue()-(RobotPosition[2]+resAngle))*ToDegrees > RotationAngle ){ //rotate  PI/18
		anglee =(RobotPosition[2]-Angle.sensorvalue())*ToDegrees;
		rotateGrid(fabs(anglee));
		resAngle = fmod(RobotPosition[2]-Angle.sensorvalue(), RotationAngleRad);
		RobotPosition[2]=Angle.sensorvalue();
	}
	/*value = int(temp[0] + resX);
	if(abs(value) > distance){
		straightMove(value, 0);
		resX = fmod(temp[0] + resX,distance);
		RobotPosition[0]=PosX.sensorvalue();
	}
	value = int(temp[1] + resY);
	if(abs(value) > distance){
		straightMove(0, value);
		resY = fmod( value, distance);
		RobotPosition[1]=PosY.sensorvalue();
	}*/
}

void ObstacleAvoidance::straightMove(int distanceFront, int distanceSide){
	double px[6], py[6],r[6], theta[6], temp;
	int index=0;//, indey=0;
	//Logger::Instance().WriteMsg("ObstacleAvoidance", "move front " + _toString(distanceFront) + "move side " + _toString(distanceSide), Logger::ExtraExtraInfo);
	//initialize changed
	for(int i=0;i<M;i++)
	  for(int j=0;j<N;j++)
		changed[i*N+j]=0;
		
	for(int i=0;i<M;i++)
		for(int j=0;j<N;j++){
			temp = PolarGrid[i][j];
			if (PolarGrid[i][j] > NoKnowledge && changed[i*N+j]==0){ //check only the cells with obstacles.
				PolarGrid[i][j] = NoKnowledge;
				//syntetagmenes pou orizoun to keli
				
				px[0] = x[N*i+j]+distanceFront;			py[0] = y[N*i+j]+distanceSide;
				px[1] = x[N*i+j+N]+distanceFront;		py[1] = y[N*i+j+N]+distanceSide;
				px[2] = x[N*i+j+1+N]+distanceFront;		py[2] = y[N*i+j+1+N]+distanceSide;
				px[3] = x[N*i+j+1]+distanceFront;		py[3] = y[N*i+j+1]+distanceSide;
				/*
				px[0] = x[N*i+j-N<0?0:N*i+j-N]+distanceFront;		py[0] = y[N*i+j-N<0?0:N*i+j-N]+distanceSide;
				px[1] = x[N*i+j]+distanceFront;						py[1] = y[N*i+j]+distanceSide;
				px[2] = x[N*i+j+1]+distanceFront;					py[2] = y[N*i+j+1]+distanceSide;
				px[3] = x[N*i+j+1-N<0?0:N*i+j+1-N]+distanceFront;	py[3] = y[N*i+j+1-N<0?0:N*i+j+1-N]+distanceSide;
				*/
				/*if(j % 9 == 0){
					px[2] -= 2;py[3] -= 2;
					px[4] += 1;py[1] += 1;
				} 
				if(j % 9  == 01){
					px[3] += 2;py[2] -= 2;
					px[1] -= 1;py[4] += 1;
				} 
				if(j % 9 == 2){
					px[2] -= 2;py[3] -= 2;
					px[4] -= 1;py[1] -= 1;
				} 
				if(j % 9  == 3){
					px[3] -= 2;py[2] += 2;
					px[1] += 1;py[4] -= 1;
				} */
				px[4] = (px[0]+px[2])/2;            py[4] = (py[0]+py[2])/2;
				px[5] = (px[1]+px[3])/2;            py[5] = (py[1]+py[3])/2;
				px[0] = px[0];                      py[0] = py[0];
				px[1] = px[2];                      py[1] = py[2];
				px[2] = px[5];                      py[2] = py[5];
				
				
				//Logger::Instance().WriteMsg("ObstacleAvoidance", "straight px" + _toString(px[0]) + "py " + _toString(py[0]), Logger::ExtraExtraInfo);
				for (int k=0;k<3;k++){
					//Logger::Instance().WriteMsg("ObstacleAvoidance", "px " + _toString(px[k]) + "py " + _toString(py[k]), Logger::ExtraExtraInfo);
					if(px[k]+50>=100 || py[k]+50>=100 || px[k]+50 <0 || py[k]+50 <0) continue; //check if out of MoveGrid(100x100) boundaries
					//printf("PX %f  PY %f\n", px[k]-50, py[k]-50);
					//MoveGrid[int(px[k]+50)][int(py[k]+50)] == to keli tou MoveGrid sto opoio metafer8hkane oi syntetagmenes twn keliwn-empodiwn
					//index=int(MoveGrid[int(px[k]+50)][int(py[k]+50)]/N); //r
					//indey=int(fmod(MoveGrid[int(px[k]+50)][int(py[k]+50)], N)); //theta
					r[k] = sqrt(pow(px[k], 2) + pow(py[k], 2));
					theta[k] = atan2(py[k], px[k])*ToDegrees;
					/*PolarGrid[(int)r[k]/distance][(int)(theta[k]/RotationAngle)] = temp;
					Logger::Instance().WriteMsg("ObstacleAvoidance", "index" + _toString(index) + "indey " + _toString(indey), Logger::ExtraExtraInfo);
					changed[index*N+indey] = 1;*/
					if (theta[k]>=0 && r[k] < 90) {
						PolarGrid[int(r[k]/distance)][int(theta[k]/RotationAngle) ] = temp*0.95;
						index = (int(r[k]/distance))*N+(int(theta[k]/RotationAngle));
						//Logger::Instance().WriteMsg("ObstacleAvoidance", "r" + _toString(r[k]) + "theta " + _toString(index), Logger::ExtraExtraInfo);
						//printf("INDEX %d\n", index);
						changed[index] = 1;
					}
					else if (theta[k]<0 && r[k] < 90){
						theta[k] = 360 + theta[k];
						PolarGrid[int(r[k]/distance)][(int(theta[k]/RotationAngle ))] = temp*0.95;
						//Logger::Instance().WriteMsg("ObstacleAvoidance", "thetar" + _toString(r[k]) + "theta " + _toString(theta[k]), Logger::ExtraExtraInfo);
						index = (int(r[k]/distance))*N+(int(theta[k]/RotationAngle));
						changed[index] = 1;
					}
				}
			}
		}
	smoothGrid(1);
	//Logger::Instance().WriteMsg("ObstacleAvoidance", "---------------------------------------------------------", Logger::Info);
	//drawGrid();
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
		for (int j=0;j<N;j++){
			x[tempCnt] = int((dist * (cos(RotationAngleRad*j)))+shift);
			y[tempCnt] = int((dist * (sin(RotationAngleRad*j)))+shift);
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
	
	for (int i=0;i<35;i++){
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


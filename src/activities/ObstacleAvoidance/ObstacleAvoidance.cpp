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
	_com->get_message_queue()->subscribe("obstacle", _blk, 0);
	_com->get_message_queue()->add_publisher(this);
	obavm = new ObstacleMessage();
	ppresm = new PathPlanningResultMessage();
	
	resX=0.0;
	resY=0.0;
	resAngle=0.0;
	ussm =0;
	rpsm=0;
	DataFromVision =0;
	ppreqm=0;
	shift = 0;  //shift==metatopish gia probolh grid sthn ekastote konsola
	countAge=0;
	countLeft=0;
	countRight=0;
	countPos=0;
	mprosta=false;
	dexia= false;
	aristera = false;
	mprostaDist = 0.0;
	dexiaDist = 0.0;
	aristeraDist = 0.0;
	mprostaCert = 0.0;
	dexiaCert = 0.0;
	aristeraCert = 0.0;
	flag=0;
	goalX=6;
	goalY=17;
	initGrid();
	Logger::Instance().WriteMsg("ObstacleAvoidance", "Controller Initialized", Logger::Info);
}

int ObstacleAvoidance::Execute() {
	read_messages();
	countAge++;
	countPos++;
	countLeft=0;
	countRight=0;
	
	
	for(int i=0;i<10;i++)
		empty[i]=2.0;
	if(rpsm !=0){
		//Logger::Instance().WriteMsg("ObstacleAvoidance", "robot position", Logger::Info);
		PosX = rpsm->sensordata(0);
		PosY = rpsm->sensordata(1);
		Angle = rpsm->sensordata(2);
		if (flag ==0){
			//Logger::Instance().WriteMsg("ObstacleAvoidance", " init robotposition" , Logger::Info);
			RobotPosition[0]=PosX.sensorvalue();
			RobotPosition[1]=PosY.sensorvalue();
			RobotPosition[2]=Angle.sensorvalue();
			flag=1;
			//Logger::Instance().WriteMsg("ObstacleAvoidance", " Posx " + _toString(PosX.sensorvalue()) + " PosY " + _toString(PosY.sensorvalue()) + " ANGLE " + _toString(Angle.sensorvalue()) , Logger::Info);
		} 
		else{
			//Logger::Instance().WriteMsg("ObstacleAvoidance", " Posx " + _toString(PosX.sensorvalue()) + " PosY " + _toString(PosY.sensorvalue()) + " ANGLE " + _toString(Angle.sensorvalue()) , Logger::Info);
			findNewPosition();
			//goalX = int(targetX/10);
			//goalY = int(targetY/10);
			
			
		}
		delete rpsm;
	}
	if (ussm != 0) {
		for (int j=9;j>=0;j--){
			LeftValue[j] = ussm->sensordata(j);
			Left[j] = LeftValue[j].sensorvalue();
			RightValue[j] = ussm->sensordata(j+10);
			Right[j] = RightValue[j].sensorvalue();
			countLeft = countLeft+LeftValue[j].sensorvalue();
			countRight = countRight + RightValue[j].sensorvalue() ;
		}
		/*Logger::Instance().WriteMsg("ObstacleAvoidance", "rightSensor: " + _toString(RightValue[0].sensorvalue()) + "   leftSensor: " + _toString(LeftValue[0].sensorvalue()), Logger::Info);
		Logger::Instance().WriteMsg("ObstacleAvoidance", "rightSensor: " + _toString(RightValue[1].sensorvalue()) + "   leftSensor: " + _toString(LeftValue[1].sensorvalue()), Logger::Info);
		Logger::Instance().WriteMsg("ObstacleAvoidance", "rightSensor: " + _toString(RightValue[2].sensorvalue()) + "   leftSensor: " + _toString(LeftValue[2].sensorvalue()), Logger::Info);
		Logger::Instance().WriteMsg("ObstacleAvoidance", "rightSensor: " + _toString(RightValue[3].sensorvalue()) + "   leftSensor: " + _toString(LeftValue[3].sensorvalue()), Logger::Info);
		Logger::Instance().WriteMsg("ObstacleAvoidance", "rightSensor: " + _toString(RightValue[4].sensorvalue()) + "   leftSensor: " + _toString(LeftValue[4].sensorvalue()), Logger::Info);
		Logger::Instance().WriteMsg("ObstacleAvoidance", "rightSensor: " + _toString(RightValue[5].sensorvalue()) + "   leftSensor: " + _toString(LeftValue[5].sensorvalue()), Logger::Info);
		Logger::Instance().WriteMsg("ObstacleAvoidance", "rightSensor: " + _toString(RightValue[6].sensorvalue()) + "   leftSensor: " + _toString(LeftValue[6].sensorvalue()), Logger::Info);
		Logger::Instance().WriteMsg("ObstacleAvoidance", "rightSensor: " + _toString(RightValue[7].sensorvalue()) + "   leftSensor: " + _toString(LeftValue[7].sensorvalue()), Logger::Info);
		Logger::Instance().WriteMsg("ObstacleAvoidance", "rightSensor: " + _toString(RightValue[8].sensorvalue()) + "   leftSensor: " + _toString(LeftValue[8].sensorvalue()), Logger::Info);
		Logger::Instance().WriteMsg("ObstacleAvoidance", "rightSensor: " + _toString(RightValue[9].sensorvalue()) + "   leftSensor: " + _toString(LeftValue[9].sensorvalue()), Logger::Info);
		*/
		updateGrid(countRight!=0?Right:empty, countLeft!=0?Left:empty);
		//drawGrid();
		delete ussm;
	}
	if(ppreqm !=0){
		//pathPlanningInit(ppreqm->gotox(), ppreqm->gotoy()); 
		delete ppreqm;
	}
	/*if(DataFromVision !=0){
		//update me dedomena vision
		delete DataFromVision;
	}*/
	if(countAge == 30){
		countAge =0;
		ageGrid();
		//pathPlanningInit(goalX, goalY); //pathPlanningInit(goalX, goalY); 
	}
	
	publishObstacleMessage();
	return 0;
}

void ObstacleAvoidance::read_messages() {
	_blk->process_messages();
	ussm = _blk->in_msg_nb<UltaSoundSensorsMessage>("UltaSoundSensorsMessage");
	rpsm = _blk->in_msg_nb<RobotPositionSensorMessage>("RobotPositionSensorMessage"); 
	ppreqm = _blk->in_msg_nb<PathPlanningRequestMessage>("PathPlanningRequestMessage");
	//targetX = _blk->in_nb<RobotPositionSensorMessage>("targetX", "Behavior");
	//targetY = _blk->in_nb<RobotPositionSensorMessage>("targetY", "Behavior");
	Logger::Instance().WriteMsg("ObstacleAvoidance", "read_messages ", Logger::ExtraExtraInfo);
	
}

void ObstacleAvoidance::publishObstacleMessage(){
	
	for(int i=0;i<=6;i++){
		if(PolarGrid[i][17] > 0.75){
			obavm->set_direction(0);
			obavm->set_distance(0.3+i*0.1);
			obavm->set_certainty(PolarGrid[i][17]);
			Logger::Instance().WriteMsg("ObstacleAvoidance", "mprosta dist " + _toString(mprostaDist) + "cert "+_toString(mprostaCert) , Logger::ExtraExtraInfo);
			Publisher::publish(obavm,"obstacle");
			return;
		}
	}
}

void ObstacleAvoidance::initGrid(){
	//int x, y;
	//double r, theta;
	
	//initialize the variables of the robot position
	for (int i=0;i<3;i++)
		RobotPosition[i]=0.0;
		
	for (int i=0;i<10;i++){
		rightArray[i]=0.0;
		leftArray[i]=0.0;
	}
	for(int i=0;i<M;i++)
		for(int j=0;j<N;j++)
			PolarGrid[i][j] = 0.5;
	//PolarGrid[3][16]=1.0;
	//PolarGrid[3][17]=1.0;
	//PolarGrid[3][18]=1.0;
	
	/*for (int i=0;i<100;i++)
		for (int j=0;j<100;j++){
			x=i-50;
			y=j-50;
			r = sqrt(pow(x, 2) + pow(y, 2));
			theta = -atan2(y, x)*180/PI;
			if (theta<0) theta=360+theta;
			MoveGrid[i][j] = int ((r/10)*N+(theta/10)); //no sto PolarGrid 
			//Logger::Instance().WriteMsg("ObstacleAvoidance", "theta " + _toString(r) , Logger::Info);
		}
	*/
	for(int i=0;i<3;i++)
		for(int j=0;j<3;j++)
			grid[i][j] = 1;
			
	findCoordinates();

}

void ObstacleAvoidance::smoothGrid(int smooth){
  if(smooth==1){
	//no obstacle right behind the robot while it moves..
    for(int l=0;l<N/4;l++) {
      PolarGrid[0][l]=0.0;
      PolarGrid[0][l+3*N/4]=0.0;
    }
    for (int k=0;k<M;k++)
      for(int l=0;l<N;l++){
        //horizontal
        if (PolarGrid[k][l] != 0.5 && PolarGrid[k][l+2<N?l+2:(l+2)%N]==PolarGrid[k][l])
          PolarGrid[k][l+1<36?l+1:(l+2)%N] =  PolarGrid[k][l];
        //vertical
        if (PolarGrid[k][l] != 0.5 && PolarGrid[l+2<N?l+2:(l+2)%M][l]==PolarGrid[k][l])
          PolarGrid[l+2<36?l+2:(l+2)%M][l] =  PolarGrid[k][l];      
      }
    smooth=0;
  }  
}

void ObstacleAvoidance::updateGrid(double (&newValues1)[10], double (&newValues2)[10]){//right, left
	int index, range=2;
	double temp[2];
	//double counter = 0.9;
	for(int i=9;i>=0;i--){
		Logger::Instance().WriteMsg("ObstacleAvoidance", "in_update new1 " + _toString(newValues1[i]) +  " new2 " + _toString(newValues2[i]) , Logger::ExtraExtraInfo);
		if (newValues1[i] > 1.05 && newValues2[i] > 1.05 )
			continue;
		if(newValues1[i] == 0.0) continue;
		temp[0]=(newValues1[i] < 0.89 && newValues1[i] > 0.2)?newValues1[i]:0.0; //left
		for (int l=0;l<10;l++){ //gia na brw an yparxei sto temp[1] timh konta sto temp[0]
			temp[1]=(newValues2[l] < 0.89 && newValues2[l] > 0.2)?newValues2[l]:0.0; //right
			if (int(temp[0]/0.1) == int(temp[1]/0.1)) { //apenanti
				mprosta = true;
				mprostaDist = temp[0]; 
				if (temp[0]/0.1 <= 3 && PolarGrid[0][17]>0.8){
					Logger::Instance().WriteMsg("ObstacleAvoidance2", "STOOOOOOOOOOOOOOOP" , Logger::ExtraExtraInfo);
					obavm->set_direction(0);
					obavm->set_distance(mprostaDist);
					obavm->set_certainty(mprostaCert);
				}
				for(int k=0;k<2;k++){
					if(temp[k] > 0.0) {  //alliws den mas endiaferei giati einai eite poly makrya eite poly konta
						index=(int)(temp[k]/0.1 - 3); //-2 giati den mas endiaferei mexri ta 20 cm
						for(int j=15;j<19;j++){
							PolarGrid[index][j]=PolarGrid[index][j]*1.1>=1.0?1.0:PolarGrid[index][j]*1.1;
							if (index>0)
								PolarGrid[index-1][j]=PolarGrid[index-1][j]*0.98<=0.0?0.0:PolarGrid[index-1][j]*0.98;
							if (index<5)
								PolarGrid[index+1][j]=PolarGrid[index+1][j]*1.02>=1.0?1.0:PolarGrid[index+1][j]*1.02;
						}
					}
				}
				mprostaCert = PolarGrid[(int)(temp[1]/0.1 - 3)][17];
				
				temp[1]=0.0;
				temp[0]=0.0;
			}
		}
	}
	for(int i=0;i<10;i++){
		temp[0]=(newValues1[i] < 0.89 && newValues1[i] > 0.2)?newValues1[i]:0.0; //left
		//gia tis ypoloipes times pou deixnoun parousia empodiou sta plagia
		temp[1]=(newValues2[i] < 0.89 && newValues2[i] > 0.2)?newValues2[i]:0.0; //right
		for(int k=0;k<2;k++){
			if(temp[k] > 0.0) {  //alliws den mas endiaferei giati einai eite poly makrya eite poly konta
				//Logger::Instance().WriteMsg("ObstacleAvoidance", "oxi mprosta" , Logger::ExtraExtraInfo);
				///////////////////send message for left or right obstacle angle 30 
				
				index=(int)(temp[k]/0.1 - 3); //-2 giati den mas endiaferei mexri ta 20 cm
				for(int j=((k==0)?13:19);j<((k==0)?(13+range):(19+range));j++){
					PolarGrid[index][j]=PolarGrid[index][j]*1.1>=1.0?1.0:PolarGrid[index][j]*1.1;
					if (index>0)
						PolarGrid[index-1][j]=PolarGrid[index][j]*0.98<=0.0?0.0:PolarGrid[index-1][j]*0.98;
					if (index<5)
						PolarGrid[index+1][j]=PolarGrid[index][j]*1.02>=1.0?1.0:PolarGrid[index+1][j]*1.02;
				}
				if (k == 0){
					dexia = true;
					dexiaDist = temp[k];
					dexiaCert = PolarGrid[(int)(temp[k]/0.1 - 3)][14];
				}else{
					aristera = true;
					aristeraDist = temp[k];
					aristeraCert = PolarGrid[(int)(temp[k]/0.1 - 3)][20];
				}
			}
		}
	}
	
	/*if(temp[0]/0.1 < 3 && abs(temp[0] - temp[1])<0.1 && temp[0]>0) 
		Logger::Instance().WriteMsg("ObstacleAvoidance", " mprosta " + _toString(temp[0]) , Logger::ExtraExtraInfo);
	else {
		if(temp[0]/0.1 < 3 && temp[0]>0) Logger::Instance().WriteMsg("ObstacleAvoidance", " left " , Logger::ExtraExtraInfo);
	else if(newValues2/0.1< 3 && temp[0]>0) Logger::Instance().WriteMsg("ObstacleAvoidance", " right " , Logger::ExtraExtraInfo);
	else Logger::Instance().WriteMsg("ObstacleAvoidance", " no obstacle amesa " , Logger::ExtraExtraInfo);
	}*/
	drawGrid();

}

void ObstacleAvoidance::ageGrid(){
	for (int i=0;i<M;i++)
		for(int j=0;j<N;j++)
			PolarGrid[i][j] = (PolarGrid[i][j]-0.5)*0.9 + 0.5;
}

void ObstacleAvoidance::findNewPosition(){
	double temp[2]={0.0, 0.0};
	double anglee;
	double value=0.0;
	temp[0] =(fabs(PosX.sensorvalue())-fabs(RobotPosition[0]))*100;
	temp[1] =(fabs(PosY.sensorvalue())-fabs(RobotPosition[1]))*100;
	//Logger::Instance().WriteMsg("ObstacleAvoidance", "temp[0] " + _toString(temp[0]) +  " temp[1] " + _toString(temp[1]) , Logger::Info);
	if (fabs(Angle.sensorvalue()-(RobotPosition[2]+resAngle)) > (PI/18) ){ //rotate
		anglee =(RobotPosition[2]-Angle.sensorvalue())*180/PI;
		rotateGrid(fabs(anglee));
		resAngle = fmod(RobotPosition[2]-Angle.sensorvalue(), RotAngleRad);
		RobotPosition[2]=Angle.sensorvalue();
	}
	value = int(temp[0] + resX);
	/*if(abs(value) > 10){
		straightMove(value, 0);
		resX = fmod(temp[0] + resX,10);
		RobotPosition[0]=PosX.sensorvalue();
	}
	value = int(temp[1] + resY);
	if(abs(value) > 10){
		straightMove(0, value);
		resY = fmod( value, 10);
		RobotPosition[1]=PosY.sensorvalue();
	}*/
}

void ObstacleAvoidance::straightMove(int distanceFront, int distanceSide){
	double px[6], py[6],r[6], theta[6], temp;
	int index=0, indey=0;
	Logger::Instance().WriteMsg("ObstacleAvoidance", "move front " + _toString(distanceFront) + "move side " + _toString(distanceSide), Logger::ExtraExtraInfo);
	//initialize changed
	for(int i=0;i<M;i++)
	  for(int j=0;j<N;j++)
		changed[i*N+j]=0;
		
	for(int i=0;i<M;i++)
		for(int j=0;j<N;j++){
			temp = PolarGrid[i][j];
			if (PolarGrid[i][j] > 0.6 && changed[i*N+j]==0){ //check only the cells with obstacles.
				PolarGrid[i][j] = 0.5;
				//syntetagmenes pou orizoun to keli
				
				px[0] = x[N*i+j]+distanceFront;		py[0] = y[N*i+j]+distanceSide;
				px[1] = x[N*i+j+N]+distanceFront;						py[1] = y[N*i+j+N]+distanceSide;
				px[2] = x[N*i+j+1+N]+distanceFront;					py[2] = y[N*i+j+1+N]+distanceSide;
				px[3] = x[N*i+j+1]+distanceFront;	py[3] = y[N*i+j+1]+distanceSide;
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
				
				
				Logger::Instance().WriteMsg("ObstacleAvoidance", "straight px" + _toString(px[0]) + "py " + _toString(py[0]), Logger::ExtraExtraInfo);
				for (int k=0;k<3;k++){
					r[k] = floorf(sqrt(pow(px[k], 2) + pow(py[k], 2)))/10;
					///////////////////////check - sto atan 
					theta[k] = floorf(atan2(py[k], px[k])*180/PI)/10;
					theta[k] = (theta[k] >= 0)?theta[k]:(N)-theta[k];
					if (theta[k]>=0 && r[k]<7 && theta[k]<36) {
						index = int(r[k]);
						indey = int(theta[k]);
						//index = (int(r[k]/10))*N+(int(theta[k]/10));
						Logger::Instance().WriteMsg("ObstacleAvoidance", "r" + _toString(index) + "theta " + _toString(indey), Logger::ExtraExtraInfo);
						PolarGrid[index][indey] = temp*0.95;
						//printf("INDEX %d\n", index);
						changed[index*N+indey] = 1;
					}
					/*else if (theta[k]<0 && r[k] < 7){
						theta[k] = N + theta[k];
						index = int(r[k]);
						indey = int(theta[k]);
						Logger::Instance().WriteMsg("ObstacleAvoidance", "thetar" + _toString(r[k]) + "theta " + _toString(theta[k]), Logger::ExtraExtraInfo);
						PolarGrid[index][indey] = temp*0.95;
						changed[index*N+indey] = 1;
					}*/
				}
			}
		}
		//goalY = goalY;
	//smoothGrid(1);
	//Logger::Instance().WriteMsg("ObstacleAvoidance", "---------------------------------------------------------", Logger::Info);
	//drawGrid();
}

void ObstacleAvoidance::rotateGrid(double angle){
	double temp[M][N];
	int ll=int(angle/10);
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
	goalY = goalY+ll;
	Logger::Instance().WriteMsg("ObstacleAvoidance", "---------------------------------------------------------", Logger::Info);
	//drawGrid();
}

void ObstacleAvoidance::findCoordinates(){
	int tempCnt=0, dist;
	for (int i=0;i<M+1;i++){
		dist = (i+1)*distance;
		for (int j=0;j<N;j++){
			x[tempCnt] = int((dist * (cos(RotAngleRad*j)))+shift);
			y[tempCnt] = int((dist * (sin(RotAngleRad*j)))+shift);
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
	for(int i=0;i<9;i++)
		value[i] = IterationGrid[indexx[i]][indexy[i]];
}

void ObstacleAvoidance::bestPath(int goalx, int goaly){
	int initm=0, initn=17, i=1,g=0;//, smt=0;
	double nextX=0.0, nextY=0.0;
	double bigger=-10, temp;
	int pathx[10], pathy[10];
	Logger::Instance().WriteMsg("ObstacleAvoidance", "goalx " + _toString(goalx) +  " goaly " + _toString(goaly) , Logger::ExtraExtraInfo);
	//briskw prwto eley8ero keli gia kinhsh an oxi (0,17)
	while(PolarGrid[initm][initn] > 0.75){
		if (PolarGrid[initm][initn+1] < 0.65){
		  initn=initn+1;
		  break;
		}
		if (PolarGrid[initm][initn-i] < 0.65){
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
    
    
	  for (i=1;i<8;i++){
      
		  temp=IterationGrid[indexx[i]][indexy[i]];//value[i];
		  if (temp>bigger){
			  initm = indexx[i];
			  initn = indexy[i];
			  bigger=temp;
		  }
	  }
	 // Logger::Instance().WriteMsg("ObstacleAvoidance", "initm " + _toString(initm) +  " initn " + _toString(initn) , Logger::ExtraExtraInfo);
	  pathx[g] = initm;
	  pathy[g] = initn;
	  initIndexes(initm, initn);
	  if(initm == pathx[g-1] && initn == pathy[g-1]) break;
	}while(initm != goalx || initn!=goaly);
	
	for (i=0;i<g;i++)
		IterationGrid[pathx[i]][pathy[i]] = 2.0;
		
	
	nextX = (double(x[N*pathx[1]+pathy[1]+N+1]))/100;
	nextY = (double(y[N*pathx[1]+pathy[1]+N+1]))/100;
	Logger::Instance().WriteMsg("ObstacleAvoidance", "initm " + _toString(nextX) +  " initn " + _toString(nextY) , Logger::ExtraExtraInfo);
	if(nextX >= 0.1 && nextY >= 0.1){
		ppresm->set_movetox(nextX);
		ppresm->set_movetoy(nextY);
		ppresm->set_movetoangle(atan2(nextY, nextX)*180/PI);
		Publisher::publish(ppresm, "obstacle");
	}
	drawGrid();
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
			if (PolarGrid[mm][nn] > 0.8){
			  IterationGrid[mm][nn] = obstacle;
			  continue;
			}
			if(IterationGrid[mm][nn] == goal){
			  IterationGrid[mm][nn] = goal;
			  continue;
			}
			else{
			  for(int a=0;a<8;a++){ 
				sum=0.0;
				for (int k=0;k<8;k++){
					if(mm==0){
						if(k==3 || k==4 || k==5)
							sum = sum + IterationGrid[indexx[8]][indexy[8]]*possibilities[(k-a>0?k-a:8+k-a)%8];
						else
							sum = sum + IterationGrid[indexx[k]][indexy[k]]*possibilities[(k-a>0?k-a:8+k-a)%8];
					}
					else if(mm==M-1){
						if(k==0 || k==1 || k==7)
							sum = sum + IterationGrid[indexx[8]][indexy[8]]*possibilities[(k-a>0?k-a:8+k-a)%8];
						else
							sum = sum + IterationGrid[indexx[k]][indexy[k]]*possibilities[(k-a>0?k-a:8+k-a)%8];
					}
					else 
						sum = sum + IterationGrid[indexx[k]][indexy[k]] * possibilities[((k-a>=0)?(k-a):(8+k-a))%8];
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
		Logger::Instance().WriteMsg("ObstacleAvoidance-", "    ------------------------------------------ ", Logger::ExtraExtraInfo);
	}
}


void ObstacleAvoidance::pathPlanningInit(int goalx, int goaly){
	
	/*int index, indey;
	index = int(goalx/10);
	indey = int(goaly/36);
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
			
	/*IterationGrid[5][16] = 1.0;
	IterationGrid[5][15] = 1.0;
	IterationGrid[5][14] = 1.0;
	IterationGrid[2][20] = 1.0;
	IterationGrid[2][21] = 1.0;
	IterationGrid[2][22] = 1.0;
	IterationGrid[2][23] = 1.0;*/
	IterationGrid[goalx][goaly] = goal;
	fillIterationGrid();
	//printIterationGrid();
	bestPath(goalx, goaly);
}

void ObstacleAvoidance::drawGrid(){
	int val[7];
	
	for (int i=0;i<35;i++){
		Logger::Instance().WriteMsg("ObstacleAvoidance", " " + _toString(i) + " " + _toString(PolarGrid[0][i]) +" | " + _toString(PolarGrid[1][i]) + " | " + _toString(PolarGrid[2][i]) 
				+ " | " + _toString(PolarGrid[3][i])  + " | " + _toString(PolarGrid[4][i])
				+ " | " + _toString(PolarGrid[5][i])  + " | " + _toString(PolarGrid[6][i]) , Logger::ExtraExtraInfo);
		Logger::Instance().WriteMsg("ObstacleAvoidance", "    ------------------------------------------ ", Logger::ExtraExtraInfo);
	}
	/*for (int i=0;i<35;i++){
		for (int j=0;j<7;j++)
			val[j]=PolarGrid[j][i] > 0.6 ?1:0;
		Logger::Instance().WriteMsg("ObstacleAvoidance", " " + _toString(i) + " " + _toString(val[0]) +" | " + _toString(val[1]) + " | " + _toString(val[2]) 
				 + " | " + _toString(val[3])  + " | " + _toString(val[4])
				 + " | " + _toString(val[5])  + " | " + _toString(val[6]) , Logger::ExtraExtraInfo);
		Logger::Instance().WriteMsg("ObstacleAvoidance", "    -------------------------------------------- ", Logger::ExtraExtraInfo);
	}*/
	Logger::Instance().WriteMsg("ObstacleAvoidance", "    -------------------------------------------- ", Logger::ExtraExtraInfo);
}


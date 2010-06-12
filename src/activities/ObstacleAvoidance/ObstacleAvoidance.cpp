#include "ObstacleAvoidance.h"
#include "tools/toString.h"
#include <iostream>

#include "tools/logger.h"

namespace {
	ActivityRegistrar<ObstacleAvoidance>::Type temp("ObstacleAvoidance");
}

ObstacleAvoidance::ObstacleAvoidance() :
	Publisher("ObstacleAvoidance") {
}

void ObstacleAvoidance::UserInit() {
	_com->get_message_queue()->add_subscriber(_blk);
	_com->get_message_queue()->subscribe("sensors", _blk, 0);
	_com->get_message_queue()->subscribe("behavior", _blk, 0);

	resX=0;
	resY=0;
	resAngle=0;
	ussm =0;
	rpsm=0;
	shift = 100;  //shift==metatopish gia probolh grid sthn ekastote konsola
	countAge=0;
	countLeft=0;
	countRight=0;
	countPos=0;
	flag=0;
	initGrid();
	
	Logger::Instance().WriteMsg("ObstacleAvoidance", "Controller Initialized", Logger::Info);
}

int ObstacleAvoidance::Execute() {
	read_messages();
	countAge++;
	countPos++;
	countLeft=0;
	countRight=0;
	if(rpsm !=0){
		Logger::Instance().WriteMsg("ObstacleAvoidance", "robot position", Logger::Info);
		PosX = rpsm->sensordata(0);
		PosY = rpsm->sensordata(1);
		Angle = rpsm->sensordata(2);
		if (flag ==0){
			RobotPosition[0]=PosX.sensorvalue();
			RobotPosition[1]=PosY.sensorvalue();
			RobotPosition[2]=Angle.sensorvalue();
			flag=1;
		} 
		else{
			Logger::Instance().WriteMsg("ObstacleAvoidance", " Posx " + _toString(PosX.sensorvalue()) + " PosY " + _toString(PosY.sensorvalue()) + " ANGLE " + _toString(Angle.sensorvalue()) , Logger::Info);
			//findNewPosition();
		}
		delete rpsm;
	}
	if (ussm != 0) {
		for (int j=9;j>=0;j--){
			LeftValue[j] = ussm->sensordata(j);
			RightValue[j] = ussm->sensordata(j+10);
			countLeft = countLeft+LeftValue[j].sensorvalue();
			countRight = countRight + RightValue[j].sensorvalue() ;
		}
		//Logger::Instance().WriteMsg("ObstacleAvoidance", "rightSensor: " + _toString(RightValue[0].sensorvalue()) + "   leftSensor: " + _toString(LeftValue[0].sensorvalue()), Logger::Info);
		//Logger::Instance().WriteMsg("ObstacleAvoidance", "rightSensor: " + _toString(RightValue[1].sensorvalue()) + "   leftSensor: " + _toString(LeftValue[1].sensorvalue()), Logger::Info);
		//Logger::Instance().WriteMsg("ObstacleAvoidance", "rightSensor: " + _toString(RightValue[2].sensorvalue()) + "   leftSensor: " + _toString(LeftValue[2].sensorvalue()), Logger::Info);
		//Logger::Instance().WriteMsg("ObstacleAvoidance", "rightSensor: " + _toString(RightValue[3].sensorvalue()) + "   leftSensor: " + _toString(LeftValue[3].sensorvalue()), Logger::Info);
		//Logger::Instance().WriteMsg("ObstacleAvoidance", "rightSensor: " + _toString(RightValue[4].sensorvalue()) + "   leftSensor: " + _toString(LeftValue[4].sensorvalue()), Logger::Info);
		//Logger::Instance().WriteMsg("ObstacleAvoidance", "rightSensor: " + _toString(RightValue[5].sensorvalue()) + "   leftSensor: " + _toString(LeftValue[5].sensorvalue()), Logger::Info);
		//Logger::Instance().WriteMsg("ObstacleAvoidance", "rightSensor: " + _toString(RightValue[6].sensorvalue()) + "   leftSensor: " + _toString(LeftValue[6].sensorvalue()), Logger::Info);
		//Logger::Instance().WriteMsg("ObstacleAvoidance", "rightSensor: " + _toString(RightValue[7].sensorvalue()) + "   leftSensor: " + _toString(LeftValue[7].sensorvalue()), Logger::Info);
		//Logger::Instance().WriteMsg("ObstacleAvoidance", "rightSensor: " + _toString(RightValue[8].sensorvalue()) + "   leftSensor: " + _toString(LeftValue[8].sensorvalue()), Logger::Info);
		//Logger::Instance().WriteMsg("ObstacleAvoidance", "rightSensor: " + _toString(RightValue[9].sensorvalue()) + "   leftSensor: " + _toString(LeftValue[9].sensorvalue()), Logger::Info);
		//double newValue1, newValue2;
		//if (countRight != 0){ //epairna mhdenikes times-la8os
		//for (int i=9;i>=0;i--){
		//	updateGrid(newValue1=(countRight!=0?RightValue[i].sensorvalue():0), newValue2=(countLeft!=0?LeftValue[i].sensorvalue():0));
		//}
		//drawGrid();
		//}
		delete ussm;
	}
	if (countPos == 10){
		countPos=0;
		//findNewPosition();
	}
	
	if(countAge == 20){
		countAge =0;
		ageGrid();
	}
	
	Logger::Instance().WriteMsg("ObstacleAvoidance", "inside_execute", Logger::Info);
	
	return 0;
}

void ObstacleAvoidance::read_messages() {
	_blk->process_messages();
	ussm = dynamic_cast<UltaSoundSensorsMessage*> (_blk->in_nb<UltaSoundSensorsMessage>("UltaSoundSensorsMessage", "Sensors"));
	rpsm = dynamic_cast<RobotPositionSensorMessage*> (_blk->in_nb<RobotPositionSensorMessage>("RobotPositionSensorMessage", "Sensors"));
	Logger::Instance().WriteMsg("ObstacleAvoidance", "read_messages ", Logger::ExtraExtraInfo);
}



void ObstacleAvoidance::initGrid(){
	int x, y;
	double r, theta;
	
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
	PolarGrid[2][13]=1.0;
	PolarGrid[2][14]=1.0;
	PolarGrid[2][15]=1.0;
	PolarGrid[3][11]=0.789;
	

	for (int i=0;i<100;i++)
		for (int j=0;j<100;j++){
			x=i-50;
			y=j-50;
			r = sqrt(pow(x, 2) + pow(y, 2));
			theta = atan2(y, x)*180/PI;
			if (theta<0) theta=360+theta;
			MoveGrid[i][j] = (int) (r/10)*N+(theta/10);
		}
	
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
        if (PolarGrid[k][l] != 0.5 && PolarGrid[k][l+2<M?l+2:(l+2)%N]==PolarGrid[k][l])
          PolarGrid[k][l+1<36?l+1:(l+2)%N] =  PolarGrid[k][l];
        //vertical
        if (PolarGrid[k][l] != 0.5 && PolarGrid[l+2<M?l+2:(l+2)%M][l]==PolarGrid[k][l])
          PolarGrid[l+2<36?l+2:(l+2)%M][l] =  PolarGrid[k][l];      
      }
    smooth=0;
  }  
}

void ObstacleAvoidance::updateGrid(double newValues1, double newValues2){//right, left
	int index, range=4;
	double temp[2];
	//double counter = 0.9;
	Logger::Instance().WriteMsg("ObstacleAvoidance", "in_update new1 " + _toString(newValues2) +  " new2 " + _toString(newValues1) , Logger::ExtraExtraInfo);
	temp[0]=(newValues1 < 0.89 && newValues1 > 0.2)?newValues1:0.0; //left
	temp[1]=(newValues2 < 0.89 && newValues2 > 0.2)?newValues2:0.0; //right
	//Logger::Instance().WriteMsg("ObstacleAvoidance", "in_update new1 " + _toString(temp[0]) +  " new2 " + _toString(temp[1]) , Logger::ExtraExtraInfo);
	for(int k=0;k<2;k++){
		if(temp[k] > 0.0) {  //alliws den mas endiaferei giati einai eite poly makrya eite poly konta
			index=(int)(temp[k]/0.1 - 2); //-2 giati den mas endiaferei mexri ta 20 cm
			//Logger::Instance().WriteMsg("ObstacleAvoidance", " in if " + _toString(index), Logger::ExtraExtraInfo);
			for(int j=((k==0)?13:19);j<((k==0)?(13+range):(19+range));j++){
				PolarGrid[index][j]=PolarGrid[index][j]*1.1>=1.0?1.0:PolarGrid[index][j]*1.1;
				//for (int i=(index-1);i>=0;i--){
					//PolarGrid[i][j]=PolarGrid[i][j]*counter<=0.0?0.0:PolarGrid[i][j]*counter;
				//	counter = counter * 0.95;
				//}
				if (index>0)
					PolarGrid[index-1][j]=PolarGrid[index-1][j]*0.98<=0.0?0.0:PolarGrid[index-1][j]*0.98;
				//for (int i=(index);i<M;i++){
					//PolarGrid[i][j]=PolarGrid[i][j]*1.1>=1.0?1.0:PolarGrid[i][j]*1.1;
				//	counter = counter * 1.05;
				//}
				if (index<5)
					PolarGrid[index+1][j]=PolarGrid[index+1][j]*1.02>=1.0?1.0:PolarGrid[index+1][j]*1.02;
			}  
		}/*
		else {
			//Logger::Instance().WriteMsg("ObstacleAvoidance", " in else " , Logger::ExtraExtraInfo);
		for(int j=((k==0)?13:19);j<((k==0)?(13+range):(19+range));j++)
		  for (int i=0;i<M;i++)
			PolarGrid[i][j]=PolarGrid[i][j]<=0.0?0.0:PolarGrid[i][j]*0.98;
		}*/
	}
	/*if(temp[0]/0.1 < 3 && abs(temp[0] - temp[1])<0.1 && temp[0]>0) 
		Logger::Instance().WriteMsg("ObstacleAvoidance", " mprosta " + _toString(temp[0]) , Logger::ExtraExtraInfo);
	else {
		if(temp[0]/0.1 < 3 && temp[0]>0) Logger::Instance().WriteMsg("ObstacleAvoidance", " left " , Logger::ExtraExtraInfo);
	else if(newValues2/0.1< 3 && temp[0]>0) Logger::Instance().WriteMsg("ObstacleAvoidance", " right " , Logger::ExtraExtraInfo);
	else Logger::Instance().WriteMsg("ObstacleAvoidance", " no obstacle amesa " , Logger::ExtraExtraInfo);
	}*/
}

void ObstacleAvoidance::ageGrid(){
  for (int i=0;i<M;i++)
    for(int j=0;j<N;j++)
      PolarGrid[i][j] = (PolarGrid[i][j]-0.5)*0.9 + 0.5;
}

void ObstacleAvoidance::findNewPosition(){
	double temp[2];
	double anglee;
	temp[0] =(PosX.sensorvalue()-RobotPosition[0])*100;
	temp[1] =(PosY.sensorvalue()-RobotPosition[1])*100;
	Logger::Instance().WriteMsg("ObstacleAvoidance", "temp[0] " + _toString(PosX.sensorvalue()) +  " temp[1] " + _toString(PosY.sensorvalue()) , Logger::ExtraExtraInfo);
	if (fabs(Angle.sensorvalue()-(RobotPosition[2]+resAngle)) > (PI/18) ){ //rotate
		anglee =(RobotPosition[2]-Angle.sensorvalue())*180/PI;
		rotateGrid(fabs(anglee));
		Logger::Instance().WriteMsg("ObstacleAvoidance", "rotate " + _toString(anglee), Logger::ExtraExtraInfo);
		resAngle = fmod(RobotPosition[2]-Angle.sensorvalue(), RotAngleRad);
		RobotPosition[2]=Angle.sensorvalue();
	}
	if(temp[0]+resX>10){
		straightMove(int(temp[0]+resX), 0);
		RobotPosition[0]=PosX.sensorvalue();
		resX = fmod(temp[0], 10);
	}
	if(temp[1]+resY>10){
		straightMove(0, int(temp[1]+resY));
		RobotPosition[1]=PosY.sensorvalue();
		resY = fmod(temp[1], 10);
	}
}

/*
void ObstacleAvoidance::findNewPosition(){
	double  temp[2];//r, theta,
	double anglee=0;
	temp[0] =(PosX.sensorvalue()-RobotPosition[0])*100;
	temp[1] =(PosY.sensorvalue()-RobotPosition[1])*100;
	//printf("t0 %f   t1 %f valx %f   valy %f  angle %f\n", temp[0], temp[1], gpsValx*100, gpsValy*100, fabs(angle-robot_initial_angle));
	if(fabs(Angle.sensorvalue()-RobotPosition[2]) > (PI/180) && fabs(Angle.sensorvalue()-RobotPosition[2]) < PI){
		//printf("Rotate\n");
		anglee = (RobotPosition[2]-Angle.sensorvalue())*180/PI;
		rotateGrid(anglee);
		//printf("resanglejh %f\n", fmod(robot_initial_angle-angle, RotAngle));
		resAngle = fmod(anglee, RotAngle);
		RobotPosition[2]=Angle.sensorvalue();
		//angleR=0;
		//show_grid();
	}else if(fabs(fabs(Angle.sensorvalue())-PI)<PI/12){//an koitaei mprosta 
		//printf("1\n");
		if(fabs(temp[0])+resX>10){//ston x proxwraei mprosta
			straightMove(fabs(temp[0])+resX, 0);
			RobotPosition[0]=PosX.sensorvalue();
			resX = fmod(temp[0], 10);
		}if(fabs(temp[1])+resY>10){
			//printf("SS1\n");
			straightMove(0, fabs(temp[1])+resY);//ston y proxwraei plagia
			RobotPosition[1]=PosY.sensorvalue();
			resY = fmod(temp[1], 10);
		}
	}else if( (fabs(fabs(Angle.sensorvalue())-180)>30 && fabs(fabs(Angle.sensorvalue())-180)<70)){
		//printf("2\n");//sta plagia proxwraei kai mprosta kai plagia
		straightMove(fabs(temp[0]), fabs(temp[1]));
		RobotPosition[0]=PosX.sensorvalue();
		RobotPosition[1]=PosY.sensorvalue();
	}else if(fabs(fabs(Angle.sensorvalue())-180)>70 && fabs(fabs(Angle.sensorvalue())-180)<105){
		//printf("3 %f\n", fabs(fabs(angle)-180));//an koitaei mprosta alla meta apo strofh 90 moirwn
		if(fabs(temp[0])>10){//proxwraei mprosta ston y==plagia bhmata
			//printf("AA\n");
			straightMove(0, fabs(temp[0]));
			RobotPosition[0]=PosX.sensorvalue();
			resY = fmod(temp[0], 10);
		}
		if(fabs(temp[1])>10){//proxwraei plagia ston x==eu8eia
			//printf("CC%f, %f\n", fabs(temp[0]), fabs(temp[1]));
			straightMove(fabs(temp[1]),0);
			RobotPosition[1]=PosY.sensorvalue();
			resX = fmod(temp[1], 10);
		}
	}else {
		//printf("4\n");
	}
}
*/


void ObstacleAvoidance::straightMove(int distanceFront, int distanceSide){
	double px[6], py[6], temp;
	int index=0, indey=0;

	//initialize changed
	for(int i=0;i<M;i++)
	  for(int j=0;j<N;j++)
		changed[i*N+j]=0;
		
	for(int i=0;i<M;i++)
		for(int j=0;j<N;j++){
			temp = PolarGrid[i][j];
			if (PolarGrid[i][j] > 0.35 && changed[i*N+j]==0){ //check only the cells with obstacles.
				PolarGrid[i][j] = 0.0;
				//syntetagmenes pou orizoun to keli
				px[0] = x[N*i+j]+distanceFront;     py[0] = y[N*i+j]+distanceSide;
				px[1] = x[N*i+j+N]+distanceFront;   py[1] = y[N*i+j+N]+distanceSide;
				px[2] = x[N*i+j+1+N]+distanceFront; py[2] = y[N*i+j+1+N]+distanceSide;
				px[3] = x[N*i+j+1]+distanceFront;   py[3] = y[N*i+j+1]+distanceSide;
				px[4] = (px[0]+px[2])/2;            py[4] = (py[0]+py[2])/2;
				px[5] = (px[1]+px[3])/2;            py[5] = (py[1]+py[3])/2;
				
				for (int k=0;k<6;k++){
					if(px[k]-50>=100 || py[k]-50>=100 || px[k]-50 <0 || py[k]-50 <0) continue; //check if out of MoveGrid(100x100) boundaries
					//printf("PX %f  PY %f\n", px[k]-50, py[k]-50);
					index=(int) MoveGrid[(int)px[k]-50][(int)py[k]-50]/N;
					indey=(int) fmod(MoveGrid[(int)px[k]-50][(int)py[k]-50], N);
					PolarGrid[index][indey] = temp;
					changed[index*N+indey] = 1;
				}
				Logger::Instance().WriteMsg("ObstacleAvoidance", "index " + _toString(index) +  " indey " + _toString(indey) , Logger::ExtraExtraInfo);
			}
		}
}

void ObstacleAvoidance::rotateGrid(double angle){
	double temp[M][N];
	int ll=int(angle/10);
	int change[N];
	Logger::Instance().WriteMsg("ObstacleAvoidance", "rotateangle " + _toString(ll), Logger::ExtraExtraInfo);
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
}

void ObstacleAvoidance::findCoordinates(){
	int tempCnt=0, dist=distance;
	for (int i=0;i<M+1;i++){
		dist = (i+1)*distance;
		for (int j=0;j<N;j++){
			x[tempCnt] = (int) (dist * (cos(RotAngle*j*PI/180)))+shift;
			y[tempCnt] = (int) (dist * (sin(RotAngle*j*PI/180)))+shift;
			//printf("px%d: %d, %d\n",counter, x[counter], y[counter]); 
			tempCnt++;
		}
	}
}


///////////////////////////value iteration/////////////////////////////

//8 neighbour cells
void ObstacleAvoidance::initIndexes(int mm, int nn){
	index[0] = mm!=M-1?mm+1:mm;
	index[1] = mm!=M-1?mm+1:mm;
	index[2] = mm!=M-1?mm+1:mm;
	index[3] = mm;
	index[4] = mm;
	index[5] = mm!=0?mm-1:mm;
	index[6] = mm!=0?mm-1:mm;
	index[7] = mm!=0?mm-1:mm;

	indey[0] = nn;
	indey[1] = nn!=N-1?nn+1:0;
	indey[2] = nn!=0?nn-1:N-1;
	indey[3] = nn!=N-1?nn+1:0;
	indey[4] = nn!=0?nn-1:N-1;
	indey[5] = nn ;
	indey[6] = nn!=N-1?nn+1:0;
	indey[7] = nn!=0?nn-1:N-1;
}

double ObstacleAvoidance::iterationGridDir(int num, int mm, int nn){
	double value[NEIGHBOURS];
	value[0] = IterationGrid[mm!=M-1?mm+1:mm][nn];
	value[1] = IterationGrid[mm!=M-1?mm+1:mm][nn!=N-1?nn+1:0];
	value[2] = IterationGrid[mm!=M-1?mm+1:mm][nn!=0?nn-1:N-1];
	value[3] = IterationGrid[mm][nn!=N-1?nn+1:0];
	value[4] = IterationGrid[mm][nn!=0?nn-1:N-1];
	value[5] = IterationGrid[mm!=0?mm-1:mm][nn] ;
	value[6] = IterationGrid[mm!=0?mm-1:mm][nn!=N-1?nn+1:0];
	value[7] = IterationGrid[mm!=0?mm-1:mm][nn!=0?nn-1:N-1];
	return value[num];
}

void ObstacleAvoidance::bestPath(int goalx, int goaly){
	int initm=0, initn=17;      //intitialize as first cell in front of robot
	//int px[5], py[5], 
	int flag=0;
	double bigger=-10, temp;
	initIndexes(initm, initn);
	do{
		bigger = IterationGrid[initm][initn];
		flag=0;
		for(int i=0;i<NEIGHBOURS;i++){
			temp=iterationGridDir(i, initm, initn);
			if(temp > bigger) {
				initm = index[i];
				initn=indey[i];
				bigger=temp;
				flag=1;
			}
		}

		initIndexes(initm, initn);
		//uncomment for showGrid
		/*if(initn == N-1){
			px[0] =x[initm*N+N-1];    py[0] = y[initm*N+N-1];
			px[1] =x[(initm+1)*N+N-1];py[1] = y[(initm+1)*N+N-1];
			px[2] =x[(initm+1)*N];    py[2] = y[(initm+1)*N];
			px[3] =x[initm*N];        py[3] = y[initm*N];
			px[4] =x[initm*N+N-1];    py[4] = y[initm*N+N-1];

		}else{
			px[0] = x[initm*N+initn];      py[0] = y[initm*N+initn];
			px[1] = x[(initm+1)*N+initn];  py[1] = y[(initm+1)*N+initn];
			px[2] = x[(initm+1)*N+1+initn];py[2] = y[(initm+1)*N+1+initn];
			px[3] = x[initm*N+1+initn];    py[3] = y[initm*N+1+initn];
			px[4] = x[initm*N+initn];      py[4] = y[initm*N+initn];
		}*/
		//printf("INITM%d, INITN%d\n", initm, initn);
		PolarGrid[initm][initn] = 2.0;
	}while(initm!= goalx || initn!=goaly);
}

void ObstacleAvoidance::fillIterationGrid(){
	for(int it=0;it<ITERATIONS;it++){
		for(int mm=0;mm<M;mm++){
			for(int nn=0;nn<N;nn++){
				//to if na ginei ths morfhs : if(IterationGrid[mm][nn] > 0.6)
				if ((mm==6 && nn==21) || (mm==5 && (nn==16 || nn==15 || nn==14)) || 
					(mm==2 && (nn==20 || nn==21 || nn==22 || nn==23))) continue;
				IterationGrid[mm][nn] = reward + 0.125*discount*(IterationGrid[mm!=M-1?mm+1:mm][nn]+
				IterationGrid[mm!=M-1?mm+1:mm][nn!=N-1?nn+1:0]+IterationGrid[mm!=M-1?mm+1:mm][nn!=0?nn-1:N-1]+
				IterationGrid[mm][nn!=N-1?nn+1:0]+ IterationGrid[mm][nn!=0?nn-1:N-1]+
				IterationGrid[mm!=0?mm-1:mm][nn] + IterationGrid[mm!=0?mm-1:mm][nn!=N-1?nn+1:0]+
				IterationGrid[mm!=0?mm-1:mm][nn!=0?nn-1:N-1]);
			}
		}
	}
}

void ObstacleAvoidance::pathPlanningInit(int goalx, int goaly){
	
	//uncomment for path planning example
	/*PolarGrid[5][16] = 1.0;
	PolarGrid[5][15] = 1.0;
	PolarGrid[5][14] = 1.0;
	PolarGrid[2][20] = 1.0;
	PolarGrid[2][21] = 1.0;
	PolarGrid[2][22] = 1.0;
	PolarGrid[2][23] = 1.0;
	PolarGrid[6][21] = 0.0;*/
	
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
	for (int i=10;i<21;i++){
		Logger::Instance().WriteMsg("ObstacleAvoidance", " " + _toString(i) + " " + _toString(PolarGrid[0][i]) +" | " + _toString(PolarGrid[1][i]) + " | " + _toString(PolarGrid[2][i]) 
				 + " | " + _toString(PolarGrid[3][i])  + " | " + _toString(PolarGrid[4][i])
				 + " | " + _toString(PolarGrid[5][i])  + " | " + _toString(PolarGrid[6][i]) , Logger::ExtraExtraInfo);
		Logger::Instance().WriteMsg("ObstacleAvoidance", "    -----|-----|-----|-----|----- ", Logger::ExtraExtraInfo);
		}
}


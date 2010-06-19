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

	resX=0.0;
	resY=0.0;
	resAngle=0.0;
	ussm =0;
	rpsm=0;
	shift = 0;  //shift==metatopish gia probolh grid sthn ekastote konsola
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
	double Right[10], Left[10], empty[10];
	
	for(int i=0;i<10;i++)
		empty[i]=2.0;
	if(rpsm !=0){
		//Logger::Instance().WriteMsg("ObstacleAvoidance", "robot position", Logger::Info);
		PosX = rpsm->sensordata(0);
		PosY = rpsm->sensordata(1);
		Angle = rpsm->sensordata(2);
		if (flag ==0){
			Logger::Instance().WriteMsg("ObstacleAvoidance", " init robotposition" , Logger::Info);
			RobotPosition[0]=PosX.sensorvalue();
			RobotPosition[1]=PosY.sensorvalue();
			RobotPosition[2]=Angle.sensorvalue();
			flag=1;
			Logger::Instance().WriteMsg("ObstacleAvoidance", " Posx " + _toString(PosX.sensorvalue()) + " PosY " + _toString(PosY.sensorvalue()) + " ANGLE " + _toString(Angle.sensorvalue()) , Logger::Info);
		} 
		else{
			//Logger::Instance().WriteMsg("ObstacleAvoidance", " Posx " + _toString(PosX.sensorvalue()) + " PosY " + _toString(PosY.sensorvalue()) + " ANGLE " + _toString(Angle.sensorvalue()) , Logger::Info);
			//findNewPosition();
			
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
		
		//}
		//updateGrid(countRight!=0?Right:empty, countLeft!=0?Left:empty);
		//drawGrid();
		delete ussm;
	}
	/*if (countPos == 10){
		countPos=0;
		//findNewPosition();
	}
	*/
	if(countAge == 30){
		countAge =0;
		ageGrid();
		straightMove(10, 0);
	}
	
	//Logger::Instance().WriteMsg("ObstacleAvoidance", "inside_execute", Logger::Info);
	
	return 0;
}

void ObstacleAvoidance::read_messages() {
	_blk->process_messages();
	ussm = dynamic_cast<UltaSoundSensorsMessage*> (_blk->in_nb<UltaSoundSensorsMessage>("UltaSoundSensorsMessage", "Sensors"));
	rpsm = dynamic_cast<RobotPositionSensorMessage*> (_blk->in_nb<RobotPositionSensorMessage>("RobotPositionSensorMessage", "Sensors"));
	//Logger::Instance().WriteMsg("ObstacleAvoidance", "read_messages ", Logger::ExtraExtraInfo);
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
	PolarGrid[2][12]=1.0;
	
	/*PolarGrid[2][14]=1.0;
	PolarGrid[2][15]=1.0;
	PolarGrid[3][11]=0.789;
	*/

	for (int i=0;i<100;i++)
		for (int j=0;j<100;j++){
			x=i-50;
			y=j-50;
			r = sqrt(pow(x, 2) + pow(y, 2));
			theta = -atan2(y, x)*180/PI;
			if (theta<0) theta=360+theta;
			MoveGrid[i][j] = int ((r/10)*N+(theta/10)); //no sto PolarGrid 
			//Logger::Instance().WriteMsg("ObstacleAvoidance", "theta " + _toString(r) , Logger::Info);
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

void ObstacleAvoidance::updateGrid(double (&newValues1)[10], double (&newValues2)[10]){//right, left
	int index, range=2;
	double temp[2];
	//double counter = 0.9;
	for(int i=0;i<10;i++){
		Logger::Instance().WriteMsg("ObstacleAvoidance", "in_update new1 " + _toString(newValues1[i]) +  " new2 " + _toString(newValues2[i]) , Logger::ExtraExtraInfo);
		if (newValues1[i] > 1.05 && newValues2[i] > 1.05 )
			break;
		temp[0]=(newValues1[i] < 0.89 && newValues1[i] > 0.2)?newValues1[i]:0.0; //left
		for (int l=0;l<10;l++){ //gia a brw an yparxei sto temp[1] timh konta sto temp[0]
			temp[1]=(newValues2[l] < 0.89 && newValues2[l] > 0.2)?newValues2[l]:0.0; //right
			if (int(temp[0]/0.1) == int(temp[1]/0.1)) { //apenanti
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
				Logger::Instance().WriteMsg("ObstacleAvoidance", "mprosta empodio" , Logger::ExtraExtraInfo);
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
				Logger::Instance().WriteMsg("ObstacleAvoidance", "oxi mprosta" , Logger::ExtraExtraInfo);
				index=(int)(temp[k]/0.1 - 3); //-2 giati den mas endiaferei mexri ta 20 cm
				for(int j=((k==0)?13:19);j<((k==0)?(13+range):(19+range));j++){
					PolarGrid[index][j]=PolarGrid[index][j]*1.1>=1.0?1.0:PolarGrid[index][j]*1.1;
					if (index>0)
						PolarGrid[index-1][j]=PolarGrid[index-1][j]*0.98<=0.0?0.0:PolarGrid[index-1][j]*0.98;
					if (index<5)
						PolarGrid[index+1][j]=PolarGrid[index+1][j]*1.02>=1.0?1.0:PolarGrid[index+1][j]*1.02;
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
	Logger::Instance().WriteMsg("ObstacleAvoidance", "temp[0] " + _toString(temp[0]) +  " temp[1] " + _toString(temp[1]) , Logger::Info);
	if (fabs(Angle.sensorvalue()-(RobotPosition[2]+resAngle)) > (PI/18) ){ //rotate
		anglee =(RobotPosition[2]-Angle.sensorvalue())*180/PI;
		rotateGrid(fabs(anglee));
		resAngle = fmod(RobotPosition[2]-Angle.sensorvalue(), RotAngleRad);
		RobotPosition[2]=Angle.sensorvalue();
	}
	value = int(temp[0] + resX);
	if(abs(value) > 10){
		straightMove(value, 0);
		resX = fmod(temp[0] + resX,10);
		RobotPosition[0]=PosX.sensorvalue();
	}
	/*value = int(temp[1] + resY);
	if(abs(value) > 10){
		straightMove(0, value);
		resY = fmod( value, 10);
		RobotPosition[1]=PosY.sensorvalue();
	}*/
}

void ObstacleAvoidance::straightMove(int distanceFront, int distanceSide){
	double px[6], py[6],r[6], theta[6], temp;
	int index=0;//, indey=0;
	Logger::Instance().WriteMsg("ObstacleAvoidance", "move front " + _toString(distanceFront) + "move side " + _toString(distanceSide), Logger::ExtraExtraInfo);
	//initialize changed
	for(int i=0;i<M;i++)
	  for(int j=0;j<N;j++)
		changed[i*N+j]=0;
		
	for(int i=0;i<M;i++)
		for(int j=0;j<N;j++){
			temp = PolarGrid[i][j];
			if (PolarGrid[i][j] > 0.5 && changed[i*N+j]==0){ //check only the cells with obstacles.
				PolarGrid[i][j] = 0.5;
				//syntetagmenes pou orizoun to keli
				px[0] = x[N*i+j-N<0?0:N*i+j-N]+distanceFront;     py[0] = y[N*i+j-N<0?0:N*i+j-N]+distanceSide;
				px[1] = x[N*i+j]+distanceFront;   py[1] = y[N*i+j]+distanceSide;
				px[2] = x[N*i+j+1]+distanceFront; py[2] = y[N*i+j+1]+distanceSide;
				px[3] = x[N*i+j+1-N<0?0:N*i+j+1-N]+distanceFront;   py[3] = y[N*i+j+1-N<0?0:N*i+j+1-N]+distanceSide;
				//px[4] = (px[0]+px[2])/2;            py[4] = (py[0]+py[2])/2;
				//px[5] = (px[1]+px[3])/2;            py[5] = (py[1]+py[3])/2;
				//Logger::Instance().WriteMsg("ObstacleAvoidance", "straight px" + _toString(px[0]) + "py " + _toString(py[0]), Logger::ExtraExtraInfo);
				for (int k=0;k<4;k++){
					Logger::Instance().WriteMsg("ObstacleAvoidance", "px " + _toString(px[k]) + "py " + _toString(py[k]), Logger::ExtraExtraInfo);
					if(px[k]+50>=100 || py[k]+50>=100 || px[k]+50 <0 || py[k]+50 <0) continue; //check if out of MoveGrid(100x100) boundaries
					//printf("PX %f  PY %f\n", px[k]-50, py[k]-50);
					//MoveGrid[int(px[k]+50)][int(py[k]+50)] == to keli tou MoveGrid sto opoio metafer8hkane oi syntetagmenes twn keliwn-empodiwn
					//index=int(MoveGrid[int(px[k]+50)][int(py[k]+50)]/N); //r
					//indey=int(fmod(MoveGrid[int(px[k]+50)][int(py[k]+50)], N)); //theta
					r[k] = sqrt(pow(px[k], 2) + pow(py[k], 2));
					theta[k] = atan2(py[k], px[k])*180/PI;
					Logger::Instance().WriteMsg("ObstacleAvoidance", "r" + _toString(r[k]) + "theta " + _toString(theta[k]), Logger::ExtraExtraInfo);
					/*PolarGrid[(int)r[k]/10][(int)(theta[k]/10)] = temp;
					Logger::Instance().WriteMsg("ObstacleAvoidance", "index" + _toString(index) + "indey " + _toString(indey), Logger::ExtraExtraInfo);
					changed[index*N+indey] = 1;*/
					if (theta[k]>=0 && r[k] < 70) {
                    PolarGrid[int(r[k]/10)][int(theta[k]/10) ] = temp;
                    index = (int(r[k]/10))*M+(int(theta[k]/10));
                    //printf("INDEX %d\n", index);
                    changed[index] = 1;
					}
					else if (theta[k]<0 && r[k] < 70){
						PolarGrid[int(r[k]/10)][N+(int(theta[k]/10 ))] = temp;
						index = (int(r[k]/10))*M+N+(int(theta[k]/10));
						changed[index] = 1;
					}
				}
			}
		}
	//smoothGrid(1);
	//Logger::Instance().WriteMsg("ObstacleAvoidance", "---------------------------------------------------------", Logger::Info);
	//drawGrid();
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
	Logger::Instance().WriteMsg("ObstacleAvoidance", "---------------------------------------------------------", Logger::Info);
	drawGrid();
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
	indexx[0] = mm!=M-1?mm+1:0; 
	indexx[1] = mm!=M-1?mm+1:0; 
	indexx[2] = mm;
	indexx[3] = mm!=0?mm-1:M-1;
	indexx[4] = mm!=0?mm-1:M-1;
	indexx[5] = mm!=0?mm-1:M-1;
	indexx[6] = mm;
	indexx[7] = mm!=M-1?mm+1:0;
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
		value[i] = IterationGrid[index[i]][indey[i]];
}

void ObstacleAvoidance::bestPath(int goalx, int goaly){
	int initm=0, initn=17, i=0,g=0;
	double bigger=-10, temp;
	int pathx[10], pathy[10];
	//briskw prwto eley8ero keli gia kinhsh an oxi (0,17)
	while(PolarGrid[initm][initn] > 0.7){
		if (PolarGrid[initm][initn+1] < 0.6){
		  initn++;
		  break;
		}
		if (PolarGrid[initm][initn-i] < 0.6){
		  initn=initn-i;
		  break;
		}
		initn++;
		i++;
	}
  
  do{
	pathx[g] = initm;
	pathy[g] = initn;
	
	initIndexes(initm, initn);
	//iterationGridDir(initm, initn);
	 
	bigger = value[0];//iterationGridDir(0, initm, initn); //bigger prwto mprosta
	for (i=1;i<8;i++){
	  temp=IterationGrid[index[i]][indey[i]];//value[i];
	  if (temp>bigger){
		  initm = index[i];
		  initn = indey[i];
		  bigger=temp;
	  }
	}
	g++;  
	}while(initm != goalx || initn!=goaly);
	
	for (i=0;i<g;i++)
		PolarGrid[pathx[i]][pathx[i]] = 2.0;
}

void ObstacleAvoidance::fillIterationGrid(){
	initPossibilities();
  //int  disc, k, a, i=0;
	double sum=0.0, sumold = 0.0, max=0.0;
	for(int it=0;it<20;it++){
		for(int mm=0;mm<M;mm++){
		  for(int nn=0;nn<N;nn++){
		  //nn=21;mm=5;
			sum=0.0;
			sumold=0.0;
			max=0.0;
			iterationGridDir(mm, nn); //bazw sta value tis times twn gyrw keliwn tou (mm, nn)
			if (PolarGrid[mm][nn] > 0.8)
			  IterationGrid[mm][nn] = obstacle;
			else if(PolarGrid[mm][nn] == 0.0)
			  IterationGrid[mm][nn] = goal;
			else{
			  for(int a=0;a<8;a++){ 
				sum=0.0;
				for (int k=0;k<8;k++){
					if(mm==0){
						if(k==3 || k==4 || k==5)
							sum = sum + value[8]*possibilities[(k-a>0?k-a:8+k-a)%8];
						else
							sum = sum + value[k]*possibilities[(k-a>0?k-a:8+k-a)%8];
					}
					else if(mm==M-1){
						if(k==0 || k==1 || k==7)
							sum = sum + value[8]*possibilities[(k-a>0?k-a:8+k-a)%8];
						else
							sum = sum + value[k]*possibilities[(k-a>0?k-a:8+k-a)%8];
					}
					else 
						sum = sum + value[k] * possibilities[((k-a>=0)?(k-a):(8+k-a))%8];
				}
				max = (sum>sumold)?sum:sumold;
				sumold = max;
				//printf("a%d max[%d][%d] %f\n",a ,mm, nn,max);
			  }
			  IterationGrid[mm][nn] = reward + max;
			}
		  }
		}//printIterationGrid();

	}//printIterationGrid();
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
	int val[7];
	
	/*for (int i=5;i<25;i++){
		Logger::Instance().WriteMsg("ObstacleAvoidance", " " + _toString(i) + " " + _toString(PolarGrid[0][i]) +" | " + _toString(PolarGrid[1][i]) + " | " + _toString(PolarGrid[2][i]) 
				+ " | " + _toString(PolarGrid[3][i])  + " | " + _toString(PolarGrid[4][i])
				+ " | " + _toString(PolarGrid[5][i])  + " | " + _toString(PolarGrid[6][i]) , Logger::ExtraExtraInfo);
		Logger::Instance().WriteMsg("ObstacleAvoidance", "    ------------------------------------------ ", Logger::ExtraExtraInfo);
	}*/
	for (int i=8;i<25;i++){
		for (int j=0;j<7;j++)
			val[j]=PolarGrid[j][i] > 0.6 ?1:0;
		Logger::Instance().WriteMsg("ObstacleAvoidance", " " + _toString(i) + " " + _toString(val[0]) +" | " + _toString(val[1]) + " | " + _toString(val[2]) 
				 + " | " + _toString(val[3])  + " | " + _toString(val[4])
				 + " | " + _toString(val[5])  + " | " + _toString(val[6]) , Logger::ExtraExtraInfo);
		Logger::Instance().WriteMsg("ObstacleAvoidance", "    -------------------------------------------- ", Logger::ExtraExtraInfo);
	}
}


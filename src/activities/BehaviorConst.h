#ifndef BehaviorConst_H
#define BehaviorConst_H


//********headbehavior enums***********
#define DONOTHING 0
#define CALIBRATE 1
#define SCANFORBALL 2
#define SCANFORPOST 3
#define BALLTRACK 4
#define HIGHSCANFORBALL 5
#define SCANFIELD 6

//#define PENALTY_ON

//#define RETURN_TO_POSITION
//*********************Positions and distances in the field!***********************

static const int limitY = 2, 	//2m half of y axis
				 limitX = 3;	//3m half of x axis
				 
static const float  lPost = 0.7,
					rPost = -0.7,
					robotWidth = 0.1,			//half of robot's width
					dDistBallX = 0.117,			//desired distance from the ball in x axis, aka posx
					dDistBallY = 0.03,			//desired distance from the ball in y axis, aka posy
					dDistBallOffset = 0.012,	//desired distance from the ball offset 
					minDistGB = 0.2,			//minimum goalkeeper 's distance from ball while ProtectPost
					locDeviation = 0.1,			//deviation of localization's measurments 
					yellowPostCenterX = 3.0,	//Yellow Post's center in x axis in m
					yellowPostCenterY = 0.0,	//Yellow Post's center in y axis in m
					ballDistLimit = 0.26,
					goalieX = 2.5,
					goalieY = 0,
					away = 0.8,				//Ball away distance for the goalkkeper usual value 1.0 meter
					goodConfidence = 10.0,
					badConfidence = 5.0;					
												//Need change for penalties!!!!!!!!!!!!!!!!!!!!!!!!!! Suggested 0.40

#ifndef TO_RAD
#define TO_RAD 0.01745329f
#endif	




#endif
#ifndef BehaviorConst_H
#define BehaviorConst_H

#include "tools/mathcommon.h"
#include "messages/AllMessagesHeader.h"
//********headbehavior enums***********
#define DONOTHING 0
#define CALIBRATE 1
#define SCANFORBALL 2
#define SCANFORPOST 3
#define BALLTRACK 4
#define HIGHSCANFORBALL 5
#define SCANFIELD 6
#define SCANAFTERPENALISED 7
#define BALLTRACKKALMAN 8

//#define PENALTY_ON

//#define RETURN_TO_POSITION
//*********************Positions and distances in the field!***********************

static const int limitY = 2, 	//2m half of y axis
                 limitX = 3;	//3m half of x axis

static const float  lPost = 0.7,
                    rPost = -0.7,
                    robotWidth = 0.1,			//half of robot's width
                    dDistBallX = 0.122,			//desired distance from the ball in x axis, aka posx
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


bool readyToKick(boost::shared_ptr<const ObservationMessage>  msg1)
{
	int side;

	if ( msg1.get() != 0 )
	{
		side = (msg1->ball().bearing() > 0) ? 1 : -1;

		if ((fabs( msg1->ball().dist() * cos(msg1->ball().bearing()) - dDistBallX ) <= dDistBallOffset && fabs( msg1->ball().dist() * sin(msg1->ball().bearing()) - (side * dDistBallY) ) <= dDistBallY) )
		{
			//	Logger::Instance().WriteMsg("readyToKick", "TRUE", Logger::Info);
			return true;
		}
	}

	return false;
}

bool readyToKick( boost::shared_ptr<const WorldInfo> msg2)
{
	int side;

	if ( msg2.get() != 0 && msg2->balls_size() != 0 )
	{
		if ( fabs( msg2->balls(0).relativex() - dDistBallX ) <= dDistBallOffset && fabs(fabs( msg2->balls(0).relativey()) - dDistBallY ) <= dDistBallY )
		{
			//Logger::Instance().WriteMsg("readyToKick", "TRUE", Logger::Info);
			return true;
		}
	}

	return false;
}

bool ballAway( boost::shared_ptr<const WorldInfo> msg2 )
{
	int side;

	if ( msg2.get() != 0 && msg2->balls_size() != 0 )
	{
		if ( fabs( msg2->balls(0).relativex() - dDistBallX ) > away || fabs(fabs( msg2->balls(0).relativey()) - dDistBallY ) > away )
		{
			//Logger::Instance().WriteMsg("ballAway", "TRUE", Logger::Info);
			return true;
		}
	}

	return false;
}
bool ballAway(boost::shared_ptr<const ObservationMessage> msg1 )
{
	int side;

	if ( msg1.get() != 0 )
	{
		side = (msg1->ball().bearing() > 0) ? 1 : -1;

		if ((fabs( msg1->ball().dist() * cos(msg1->ball().bearing()) - dDistBallX ) > away || fabs( msg1->ball().dist() * sin(msg1->ball().bearing()) - (side * dDistBallY) ) > away) )
		{
			//Logger::Instance().WriteMsg("ballAway", "TRUE", Logger::Info);
			return true;
		}
	}

	return false;
}

float rotation(float a, float b, float theta)
{
	return a * cos(theta) + b * sin(theta);
}

bool robotInPosition(boost::shared_ptr <const WorldInfo> w, boost::shared_ptr <const PositionMessage> pm )
{
	//Logger::Instance().WriteMsg("robotIposition",  " entered", Logger::Info);
	float currentX = 0.0, currentY = 0.0, currentTheta = 0.0, targetX = 0.0, targetY = 0.0, targetTheta = 0.0;

	if(pm.get() != 0) 		///get my target
	{
		targetX = pm->posx();
		targetY = pm->posy();
		targetTheta = pm->theta();
	}
	else
		return false;

	if(w.get() != 0) 		///get my position
	{
		currentX = w->myposition().x();
		currentY = w->myposition().y();
		currentTheta = w->myposition().phi();
	}
	else
		return false;

	if( targetX - locDeviation > currentX || currentX > targetX + locDeviation )
		return false;

	if( targetY - locDeviation > currentY || currentY > targetY + locDeviation  )
		return false;

	if( KMath::anglediff2(targetTheta , currentTheta) > M_PI / 8  )
		return false;

	return true;
}



bool robotInPosition(float currentX, float targetX, float currentY, float targetY, float currentTheta, float targetTheta)
{
	//Logger::Instance().WriteMsg("robotIposition", " entered", Logger::Info);
	if( targetX - locDeviation > currentX || currentX > targetX + locDeviation )
		return false;

	if( targetY - locDeviation > currentY || currentY > targetY + locDeviation )
		return false;

	if( KMath::anglediff2(targetTheta , currentTheta) > M_PI / 8 )
		return false;

	return true;
}

#endif

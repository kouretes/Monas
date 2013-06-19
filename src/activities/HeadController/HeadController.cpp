#include "HeadController.h"


using namespace KMath;
using namespace boost::posix_time;

ACTIVITY_REGISTER(HeadController);
using namespace std;

const float HeadController::headSpeed[3] = {1.4f, 1.6f, 1.8f}; /*{SOMEDAY, SLOW, BEAM_ME_UP}*/
/* HeadController Initialization */

void HeadController::UserInit()
{
	_blk.updateSubscription("behavior", msgentry::SUBSCRIBE_ON_TOPIC);
	_blk.updateSubscription("vision", msgentry::SUBSCRIBE_ON_TOPIC);
	_blk.updateSubscription("sensors", msgentry::SUBSCRIBE_ON_TOPIC);
	_blk.updateSubscription("worldstate", msgentry::SUBSCRIBE_ON_TOPIC);

	hmot.set_command("setHead");
	hmot.add_parameter(0.0f);
	hmot.add_parameter(-0.66322512);
	hmot.add_parameter(1.0f); //Head speed

	targetSpeed = headSpeed[NORMAL];

	scanforball = false;
	startscan = true;
	useExternalSpeed = false;
	ballFound = false;

	state = BALL1;

	bd = 0.0;
	bb = 0.0;
	bx = 0.0;
	by = 0.0;
	robot_x = 0.0;
	robot_y = 0.0;
	robot_phi = 0.0;
	robot_confidence = 1.0;

	Reset();

	lastball = microsec_clock::universal_time();

	LogEntry(LogLevel::Info,GetName())<< "Initialized" ;
}


void HeadController::Reset(){
	readGoalConfiguration();
}

/* HeadController Main Execution Function */

int HeadController::Execute()
{
	ReadMessages();
	GetPosition();
	CheckForBall();

	if(allsm)
	{
		currentHeadYaw = allsm->jointdata(KDeviceLists::HEAD + KDeviceLists::YAW).sensorvalue();
		currentHeadPitch = allsm->jointdata(KDeviceLists::HEAD + KDeviceLists::PITCH).sensorvalue();
	}

	unsigned int whattodo;
	if(control.get()==0){
		whattodo=HeadControlMessage::FROWN;
		useExternalSpeed = false;
	}else{
		whattodo=control->task().action();
		if(control->task().speed() != -1){
			useExternalSpeed = true;
			externalSpeed = control->task().speed();
		}else{
			useExternalSpeed = false;
		}
	}


	switch(whattodo)
	{
		case HeadControlMessage::NOTHING:
			scanforball=false;
			break;
		case HeadControlMessage::FROWN:
			targetYaw = 0;
			targetPitch =  0.05;
			scanforball=false;
			MakeHeadAction();
			break;
		case HeadControlMessage::LOCALIZE:
			scanforball=false;
			HeadScanStepHigh(1.4);
			break;
		case HeadControlMessage::LOCALIZE_FAR:
			scanforball=false;
			HeadScanStepHigh(1.57);
			break;
		case HeadControlMessage::SCAN_AND_TRACK_FOR_BALL:
			CheckForBall();
			if(ballFound) //Do we see the ball? then
			{
                targetYaw = lookAtPointRelativeYaw(bx, by);
                targetPitch = lookAtPointRelativePitch(bx, by);
                MakeHeadAction();
                scanforball=false;
                bfm.set_ballfound(true);
                _blk.publishState(bfm, "behavior");
			}
			else
			{
				if(scanforball==false)
				{
					startscan=true;
					scanforball=true;
					state = BALL1;
				}
				HeadScanStepSmart();
				bfm.set_ballfound(false);
                _blk.publishState(bfm, "behavior");
			}
			break;
		case HeadControlMessage::SMART_SELECT:
			CheckForBall();
			if(ballFound) //Do we see the ball? then
			{
				HeadTrackIntelligent();
				scanforball=false;
                bfm.set_ballfound(true);
                _blk.publishState(bfm, "behavior");
			}
			else
			{
				if(scanforball==false)
				{
					startscan=true;
					scanforball=true;
					state = BALL1;
				}
				HeadScanStepSmart();
				bfm.set_ballfound(false);
                _blk.publishState(bfm, "behavior");
			}
			break;
	}
	waiting++;
	return 0;
}

/* Read Incoming Messages */

void HeadController::ReadMessages()
{
	allsm = _blk.readData<AllSensorValuesMessage> ("sensors");
	wim  = _blk.readData<WorldInfo> ("worldstate");
	control = _blk.readState<HeadControlMessage> ("behavior");
}


/* Information Gathering Functions */

void HeadController::GetPosition()
{
	if(wim != 0)
	{
		if(wim.get() != 0)
		{
			robot_x = wim->myposition().x();
			robot_y = wim->myposition().y();
			robot_phi = wrapToPi( wim->myposition().phi() );
		}
	}
}

void HeadController::CheckForBall()
{
	ballFound = false;
	if(wim != 0)
	{
	    if(wim.get() != 0)
		{
            if (wim->balls_size() > 0)
            {	
                bx = wim->balls(0).relativex();// + wim->balls(0).relativexspeed() * 0.200;
                by = wim->balls(0).relativey();// + wim->balls(0).relativeyspeed() * 0.200;
                bd = sqrt(pow(bx, 2) + pow(by, 2));
                bb = atan2(by, bx);
				ballFound = true;
            }
		}
	}

	return;
}

int HeadController::MakeHeadAction()
{
	hmot.set_command("setHead");
	hmot.set_parameter(0, targetYaw);
	hmot.set_parameter(1, targetPitch);
	if(useExternalSpeed == true){
		hmot.set_parameter(2, externalSpeed);
	}else{
		hmot.set_parameter(2, targetSpeed);
	}
	_blk.publishSignal(hmot, "motion");
	waiting=0;
	return 1;
}

void HeadController::HeadScanStepHigh(float yaw_limit)
{
	static bool middle=true;
	static int sign=1;
	targetSpeed = headSpeed[SLOW];
	if(startscan==true)
	{
		startscan=false;
		middle=true;
		targetPitch=-0.55;
		targetYaw=0;
		sign = (currentHeadYaw>0?1:-1);
		MakeHeadAction();

	}
	if(reachedTargetHead())
	{
		
		if(middle)
		{
			middle=false;
			targetPitch=-0.35;
			targetYaw=yaw_limit*sign;
			sign=-sign;

		}
		else
		{
			middle=true;
			targetPitch=-0.55;
			targetYaw=0;
		}
		MakeHeadAction();
	}
}

void HeadController::HeadScanStepSmart()
{
	float  blue1y, blue1p, blue2y, blue2p;
	blue1y = +0.75;
	blue1p = +0.38;
	blue2y = +0.00;
	blue2p = -0.55;
	float green1y, green1p, green2y, green2p;
	green1y = +1.45;
	green1p = -0.42;
	green2y = +0.00;
	green2p = +0.35;
	float red1y, red1p, red2y, red2p;
	red1y = +1.80;
	red1p = -0.39;
	red2y = +0.00;
	red2p = -0.60;
	static enum {BLUE, RED, GREEN} state = BLUE;
	static enum {START, MIDDLE, END} phase = START;

	targetSpeed = headSpeed[SLOW];

	if (startscan)
	{
		ysign = currentHeadYaw > 0 ? +1 : -1; //Side
		targetYaw = blue1y * ysign;
		targetPitch = blue1p;
		state = BLUE;
		phase = START;
		MakeHeadAction();
		startscan = false;
		return;
	}

	if ( ( reachedTargetHead() ) || (waiting >= WAITFOR) )
	{
		if (phase == START)
		{
			phase = MIDDLE;

			switch (state)
			{
			case BLUE:
				targetYaw = blue2y;
				targetPitch = blue2p;
				break;

			case GREEN:
				targetYaw = green2y;
				targetPitch = green2p;
				break;

			case RED:
				targetYaw = red2y;
				targetPitch = red2p;
				break;
			}
		}
		else if (phase == MIDDLE)
		{
			ysign = -ysign;
			phase = END;

			switch (state)
			{
			case BLUE:
				targetYaw = blue1y * ysign;
				targetPitch = blue1p;
				break;

			case GREEN:
				targetYaw = green1y * ysign;
				targetPitch = green1p;
				break;

			case RED:
				targetYaw = red1y * ysign;
				targetPitch = red1p;
				break;
			}
		}
		else
		{
			phase = START;

			switch (state)
			{
			case BLUE:
				state = GREEN;
				targetYaw = green1y * ysign;
				targetPitch = green1p;
				break;

			case GREEN:
				state = RED;
				targetYaw = red1y * ysign;
				targetPitch = red1p;
				break;

			case RED:
				state = BLUE;
				targetYaw = blue1y * ysign;
				targetPitch = blue1p;
				break;
			}
		}

		MakeHeadAction();
	}
}

void HeadController::HeadTrackIntelligent()
{
	if (reachedTargetHead() || (waiting >= WAITFOR) )
	{
		if(bd < closeToBall){
			state = BALL1; //Only track ball when we are close to the ball
		}
		switch (state)
		{
			case BALL1:
				if(ballFound) //Do we see the ball? then
				{
    	            targetYaw = lookAtPointRelativeYaw(bx, by);
    	            targetPitch = lookAtPointRelativePitch(bx, by);
					state = OPPG;
				}
				targetSpeed = headSpeed[SLOW];
				break;

			case OPPG:
				targetYaw = KMath::anglediff2(lookAtPointRelativeYaw(oppGoalX - robot_x, oppGoalY - robot_y), robot_phi);
				targetPitch = lookAtPointRelativePitch(oppGoalX - robot_x, oppGoalY - robot_y);
				targetSpeed = headSpeed[NORMAL];
				state = BALL2;
				break;

			case BALL2:
				if(ballFound) //Do we see the ball? then
				{
    	            targetYaw = lookAtPointRelativeYaw(bx, by);
    	            targetPitch = lookAtPointRelativePitch(bx, by);
					state = OWNG;
				}
				targetSpeed = headSpeed[SLOW];

				break;

			case OWNG:
				targetYaw = KMath::anglediff2(lookAtPointRelativeYaw(ownGoalX - robot_x, ownGoalY - robot_y), robot_phi);
				targetPitch = lookAtPointRelativePitch(ownGoalX - robot_x, ownGoalY - robot_y);
				targetSpeed = headSpeed[NORMAL];
				state = BALL1;
				break;
			}
		MakeHeadAction();
	}
}

float HeadController::lookAtPointYaw(float x, float y)
{
	return anglediff2(atan2(y - robot_y, x - robot_x), robot_phi);
}

float HeadController::lookAtPointPitch(float x, float y)
{
	return TO_RAD(50.0) - atan2f( sqrt((x - robot_x) * (x - robot_x) + (y - robot_y) * (y - robot_y)), 0.45 );
}

float HeadController::lookAtPointRelativeYaw(float x, float y)
{
	return atan2(y, x);
}

float HeadController::lookAtPointRelativePitch(float x, float y)
{
	return  TO_RAD(50.0)  - atan2f( sqrt((x) * (x) + (y) * (y)), 0.45 );
}

bool HeadController::reachedTargetHead()
{
	return  waiting > WAITFOR || ((fabs(targetPitch - currentHeadPitch) <= OVERSH) && ((fabs(targetYaw - currentHeadYaw) <= OVERSH))) ||
			fabs(currentHeadYaw)>YAWMAX ||
			((currentHeadPitch <= PITCHMIN || currentHeadPitch >= PITCHMAX) && ((fabs(targetYaw - currentHeadYaw) <= OVERSH))); // fabs(currentHeadPitch)>PITCHMAX);
}

/* Read Configuration Functions */

void HeadController::readGoalConfiguration()
{
	// === read goal configuration xml data from Fearures.xml ===
	std::string ID;
	for(int v = 0 ; v < Configurator::Instance().numberOfNodesForKey("features.ftr") ; v++)
	{
		ID = Configurator::Instance().findValueForKey("features.ftr~"+_toString(v)+".ID").c_str();
		if(ID == "YellowGoal")
		{
			oppGoalX = atof(Configurator::Instance().findValueForKey("features.ftr~"+_toString(v)+".x").c_str());
			oppGoalY = atof(Configurator::Instance().findValueForKey("features.ftr~"+_toString(v)+".y").c_str());
			ownGoalX = -oppGoalX;
			ownGoalY = -oppGoalY;
		}
		else if(ID == "YellowLeft")
		{
			oppGoalLeftX = atof(Configurator::Instance().findValueForKey("features.ftr~"+_toString(v)+".x").c_str());
			oppGoalLeftY = atof(Configurator::Instance().findValueForKey("features.ftr~"+_toString(v)+".y").c_str());
			ownGoalLeftX = -oppGoalLeftX;
			ownGoalLeftY = -oppGoalLeftY;
		}
		else if(ID == "YellowRight")
		{
			oppGoalRightX = atof(Configurator::Instance().findValueForKey("features.ftr~"+_toString(v)+".x").c_str());
			oppGoalRightY = atof(Configurator::Instance().findValueForKey("features.ftr~"+_toString(v)+".y").c_str());
			ownGoalRightX = -oppGoalRightX;
			ownGoalRightY = -oppGoalRightY;
		}
	}
}


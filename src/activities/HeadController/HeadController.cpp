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
	hmot.add_parameter(0.0f);
	hmot.add_parameter(0.0f); //Head speed

	targetSpeed = headSpeed[NORMAL];

	useExternalSpeed = false;

	robotX = 0.0;
	robotY = 0.0;
	robotPhi = 0.0;

	Reset();

	actionStarted = microsec_clock::universal_time();
	
	currentCommand = HeadControlMessage::NOTHING;
	previousCommand = HeadControlMessage::NOTHING;
	
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
	
	if(control.get()==0){
		currentCommand=HeadControlMessage::FROWN;
		useExternalSpeed = false;
	}else{
		currentCommand=control->task().action();
		if(control->task().speed() != -1){
			useExternalSpeed = true;
			externalSpeed = control->task().speed();
		}else{
			useExternalSpeed = false;
		}
	}


	switch(currentCommand)
	{
		case HeadControlMessage::NOTHING:
			targetYaw = currentHeadYaw;
			targetPitch = currentHeadPitch;
			break;
		case HeadControlMessage::FROWN:
			targetYaw = 0;
			targetPitch = 0.05;
			MakeHeadAction();
			break;
		case HeadControlMessage::LOCALIZE:
			HeadScanStepHigh(1.4, -0.55);
			break;
		case HeadControlMessage::LOCALIZE_FAR:
			HeadScanStepHigh(1.57, -0.55);
			break;
		case HeadControlMessage::SCAN_AND_TRACK_FOR_BALL:
			if(currentCommand != previousCommand){
				smartPhase = BLUE;
				smartState = START;
				ysign = currentHeadYaw > 0 ? +1 : -1;
			}
			if(CheckForBall()) //Do we see the ball? then
			{
				smartPhase = BLUE;
				smartState = START;
                targetYaw = lookAtPointRelativeYaw(bx, by);
                targetPitch = lookAtPointRelativePitch(bx, by);
				targetSpeed = headSpeed[FAST];
                MakeHeadAction();
			}
			else
			{
				HeadScanStepSmart(NORMAL);
			}
			break;
		case HeadControlMessage::SMART_SELECT:
			if(currentCommand != previousCommand){
				smartPhase = BLUE;
				smartState = START;
                intelState = BALL1;
				ysign = currentHeadYaw > 0 ? +1 : -1; //Side
			}
			if(CheckForBall()) //Do we see the ball? then
			{
				smartPhase = BLUE;
				smartState = START;
				HeadTrackIntelligent();
				ysign = currentHeadYaw > 0 ? +1 : -1;
			}
			else
			{
                intelState = BALL1;
				HeadScanStepSmart(NORMAL);
			}
			break;
		case HeadControlMessage::SCAN:
			if(currentCommand != previousCommand){
				//First look down
				smartPhase = BLUE;
				smartState = MIDDLE;
				ysign = currentHeadYaw > 0 ? +1 : -1; //Side
			}
			HeadScanStepSmart(NORMAL);
            break;
		
	}
	previousCommand = currentCommand;
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
	if(wim != 0 && wim.get() != 0)
	{
		robotX = wim->myposition().x();
		robotY = wim->myposition().y();
		robotPhi = wrapToPi( wim->myposition().phi() );
	}
}

bool HeadController::CheckForBall()
{
	if(wim != 0 && wim.get() != 0 && wim->balls_size() > 0)
	{
        bx = wim->balls(0).relativex();// + wim->balls(0).relativexspeed() * 0.200;
        by = wim->balls(0).relativey();// + wim->balls(0).relativeyspeed() * 0.200;
        bd = sqrt(pow(bx, 2) + pow(by, 2));
        bb = atan2(by, bx);
		return true;
	}

	return false;
}

void HeadController::MakeHeadAction()
{
	hmot.set_command("setHead");
	hmot.set_parameter(0, targetYaw);
	hmot.set_parameter(1, targetPitch);
	if(useExternalSpeed == true){
		hmot.set_parameter(2, externalSpeed);
	}else{
		hmot.set_parameter(2, targetSpeed);
	}
	actionStarted = boost::posix_time::microsec_clock::universal_time();
	_blk.publishSignal(hmot, "motion");
}

void HeadController::HeadScanStepHigh(float yawLimit, float pitch)
{
	//Go to middle
	if(currentCommand!=previousCommand)
	{
		goalScanState = GOALMIDDLE1;
	}
	if(reachedTargetHead())
	{
		if(goalScanState == GOALMIDDLE1)
		{	
			//GO LEFT
			targetPitch = pitch;
			targetYaw = 0.0f;
			goalScanState = GOALLEFT;
		}
		else if(goalScanState == GOALLEFT)
		{
			targetPitch = pitch;
			targetYaw = yawLimit;
			goalScanState = GOALMIDDLE2;
		}else if(goalScanState == GOALMIDDLE2){
			
			targetPitch = pitch;
			targetYaw = 0.0f;
			goalScanState = GOALRIGHT;
		}else{
			//GO LEFT
			targetPitch = pitch;
			targetYaw = -yawLimit;
			goalScanState = GOALMIDDLE1;
		}
		MakeHeadAction();
	}
}

void HeadController::HeadScanStepSmart(int speed)
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

	targetSpeed = headSpeed[speed];

	if (reachedTargetHead())
	{
		if (smartState == START)
		{
			smartState = MIDDLE;

			switch (smartPhase)
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
		else if (smartState == MIDDLE)
		{
			ysign = -ysign;
			smartState = END;

			switch (smartPhase)
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
			smartState = START;

			switch (smartPhase)
			{
			case BLUE:
				smartPhase = GREEN;
				targetYaw = green1y * ysign;
				targetPitch = green1p;
				break;

			case GREEN:
				smartPhase = RED;
				targetYaw = red1y * ysign;
				targetPitch = red1p;
				break;

			case RED:
				smartPhase = BLUE;
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
	if (reachedTargetHead())
	{
		if(bd < closeToBall){
			intelState = BALL1; //Only track ball when we are close to the ball
		}
		switch (smartState)
		{
			case BALL1:
	            targetYaw = lookAtPointRelativeYaw(bx, by);
	            targetPitch = lookAtPointRelativePitch(bx, by);
				intelState = OPPG;
				targetSpeed = headSpeed[NORMAL];
				break;

			case OPPG:
				targetYaw = KMath::anglediff2(lookAtPointRelativeYaw(oppGoalX - robotX, oppGoalY - robotY), robotPhi);
				targetPitch = lookAtPointRelativePitch(oppGoalX - robotX, oppGoalY - robotY);
				targetSpeed = headSpeed[NORMAL];
				intelState = BALL2;
				break;

			case BALL2:
    	        targetYaw = lookAtPointRelativeYaw(bx, by);
    	        targetPitch = lookAtPointRelativePitch(bx, by);
				intelState = OWNG;
				targetSpeed = headSpeed[NORMAL];

				break;

			case OWNG:
				targetYaw = KMath::anglediff2(lookAtPointRelativeYaw(ownGoalX - robotX, ownGoalY - robotY), robotPhi);
				targetPitch = lookAtPointRelativePitch(ownGoalX - robotX, ownGoalY - robotY);
				targetSpeed = headSpeed[NORMAL];
				intelState = BALL1;
				break;
		}
		MakeHeadAction();
	}
}

float HeadController::lookAtPointYaw(float x, float y)
{
	return anglediff2(atan2(y - robotY, x - robotX), robotPhi);
}

float HeadController::lookAtPointPitch(float x, float y)
{
	return TO_RAD(50.0) - atan2f( sqrt((x - robotX) * (x - robotX) + (y - robotY) * (y - robotY)), 0.45 );
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
	return  boost::posix_time::microsec_clock::universal_time() - actionStarted > boost::posix_time::milliseconds(millisecondsToWait) ||
			previousCommand != currentCommand ||
			((fabs(targetPitch - currentHeadPitch) <= OVERSH) && ((fabs(targetYaw - currentHeadYaw) <= OVERSH))) ||
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

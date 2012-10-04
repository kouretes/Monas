#include "HeadController.h"

#include <math.h>
#include "tools/logger.h"
#include "tools/toString.h"
#include "messages/RoboCupGameControlData.h"
#include "hal/robot/generic_nao/robot_consts.h"
#include "tools/mathcommon.h"

using namespace KMath;
using namespace boost::posix_time;

ACTIVITY_REGISTER(HeadController);
using namespace std;

/* HeadController Initialization */

void HeadController::UserInit()
{
	readConfiguration(ArchConfig::Instance().GetConfigPrefix() + "/team_config.xml");
	_blk.updateSubscription("behavior", msgentry::SUBSCRIBE_ON_TOPIC);
	_blk.updateSubscription("vision", msgentry::SUBSCRIBE_ON_TOPIC);
	_blk.updateSubscription("sensors", msgentry::SUBSCRIBE_ON_TOPIC);
	_blk.updateSubscription("worldstate", msgentry::SUBSCRIBE_ON_TOPIC);
	_blk.updateSubscription("obstacle", msgentry::SUBSCRIBE_ON_TOPIC);
	hmot.set_command("setHead");
	hmot.add_parameter(0.0f);
	hmot.add_parameter(-0.66322512);

	readRobotConf = false;
	leftright = 1;
	headpos = 0;
	seeballtrust = 0;
	seeballmessage =0;
	scanforball = false;
	startscan = true;

	calibrated = 0;

	bd = 0.0;
	bb = 0.0;
	bx = 0.0;
	by = 0.0;
	robot_x = 0.0;
	robot_y = 0.0;
	robot_phi = 0.0;
	robot_confidence = 1.0;

	gameState = PLAYER_INITIAL;
	teamColor = TEAM_BLUE;
	playerNumber = 1;
	readConfiguration(ArchConfig::Instance().GetConfigPrefix() + "/team_config.xml");		// reads playerNumber, teamColor
	readRobotConfiguration(ArchConfig::Instance().GetConfigPrefix() + "/robotConfig.xml");	// reads initX, initY, initPhi
	readGoalConfiguration(ArchConfig::Instance().GetConfigPrefix() + "/Features.xml");		// reads blueGoal*, yellowGoal*
	srand(time(0));
	lastmove = microsec_clock::universal_time();
	lastball = microsec_clock::universal_time();
	lastwalk = microsec_clock::universal_time();
	lastplay = microsec_clock::universal_time();
	lastpenalized = microsec_clock::universal_time();
	//    generateFakeObstacles();
	Logger::Instance().WriteMsg("HeadController", "Initialized: My number is " + _toString(playerNumber) + " and my color is " + _toString(teamColor), Logger::Info);
}


void HeadController::Reset(){

}

/* HeadController Main Execution Function */

int HeadController::Execute()
{

	read_messages();
	GetGameState();
	GetPosition();
	if(allsm)
	{
			HeadYaw = allsm->jointdata(KDeviceLists::HEAD + KDeviceLists::YAW);
			HeadPitch = allsm->jointdata(KDeviceLists::HEAD + KDeviceLists::PITCH);
	}


	if (calibrated == 0)
	{
		std::cout<<"-----------------------cal"<<std::endl;
		calibrate();
		return 0;
	}

	if (calibrated == 1)
	{
		std::cout<<"-----------------------noth"<<std::endl;
		return 0;

	}

	if(calibrated!=2)
		return 0;

	unsigned int whattodo;
	if(control.get()==0)
		whattodo=HeadControlMessage::SCAN_AND_TRACK_FOR_BALL;
	else
		whattodo=control->task().action();
	std::cout<<whattodo<<std::endl;


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
			if(scanforball==false)
			{
				startscan=true;
				scanforball=true;

			}
			std::cout<<"Localize"<<scanforball<<startscan<<std::endl;
			HeadScanStepHigh(1.4);
			break;
		case HeadControlMessage::LOCALIZE_FAR:
			scanforball=false;
			HeadScanStepHigh(1.57);
			break;
		case HeadControlMessage::SCAN_AND_TRACK_FOR_BALL:
			CheckForBall();
			std::cout<<"trust:"<<seeballtrust<<std::endl;
			if(seeballmessage) //Do we see the ball? then
			{

					targetYaw = bmsg->referenceyaw();
					targetPitch =  bmsg->referencepitch();
					MakeHeadAction();
					scanforball=false;
					bfm.set_ballfound(true);
					_blk.publishState(bfm, "behavior");
			}
			else if(seeballtrust) // Try and look at where we expect the ball to be
			{		targetYaw = lookAtPointRelativeYaw(bx, by);
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
				}
				HeadScanStepSmart();
				bfm.set_ballfound(false);
                _blk.publishState(bfm, "behavior");
			}

			break;
		case HeadControlMessage::SMART_SELECT:

			//TODO :)
			break;

	}

	if(whattodo!=HeadControlMessage::NOTHING)
	{
		if(!reachedTargetHead())
		{
			waiting++;
			//std::cout<<"waiting"<<std::endl;
		}


		if(waiting>=WAITFOR)
		{
			//std::cout<<"REPEAT"<<std::endl;
			MakeHeadAction();
		}

	}

	return 0;
}


/* Read Incoming Messages */

void HeadController::read_messages()
{
	gsm  = _blk.readState<GameStateMessage> ("worldstate");
	bmsg = _blk.readSignal<BallTrackMessage> ("vision");
	allsm = _blk.readData<AllSensorValuesMessage> ("sensors");
	wim  = _blk.readData<WorldInfo> ("worldstate");
	swim = _blk.readData<SharedWorldInfo> ("worldstate");
	control=_blk.readState<HeadControlMessage> ("behavior");
	//Logger::Instance().WriteMsg("HeadController", "read_messages ", Logger::ExtraExtraInfo);
	boost::shared_ptr<const KCalibrateCam> c = _blk.readState<KCalibrateCam> ("vision");

	if (c != NULL)
	{
		if (c->status() == 1)
			calibrated = 2;
	}
}


/* Information Gathering Functions */

void HeadController::GetGameState()
{
	if (gsm != 0)
	{
		//Logger::Instance().WriteMsg("HeadController", " Player_state " + _toString(gsm->player_state()), Logger::ExtraExtraInfo);
		int prevGameState = gameState;
		gameState = gsm->player_state();
		teamColor = gsm->team_color();
		playerNumber = gsm->player_number();

		if (gameState == PLAYER_PLAYING)
		{
			if (prevGameState == PLAYER_PENALISED)
			{
				//			calibrated = 0;
				lastpenalized = microsec_clock::universal_time();
			}

			if (prevGameState == PLAYER_SET)
			{
				lastplay = microsec_clock::universal_time();
			}
		}
		else if (gameState == PLAYER_INITIAL)
		{
			if (gameState != prevGameState)
				calibrated = 0;
		}
		else if (gameState == PLAYER_READY)
		{
			/*if (gameState != prevGameState)
			{
				calibrated = 0;
			}*/
		}
		else if (gameState == PLAYER_SET)
		{

		}
		else if (gameState == PLAYER_FINISHED)
		{
			;
		}
		else if (gameState == PLAYER_PENALISED)
		{
		}
	}
}


void HeadController::GetPosition()
{
	if(wim != 0)
		if(wim.get() != 0)
		{
			robot_x = wim->myposition().x();
			robot_y = wim->myposition().y();
			robot_phi = wrapToPi( wim->myposition().phi() );
			robot_confidence = wim->myposition().confidence();
		}

	return;
}




void HeadController::CheckForBall()
{


	if(wim != 0)
	{
		if (wim->balls_size() > 0)
		{
			bx = wim->balls(0).relativex() + wim->balls(0).relativexspeed() * 0.200;
			by = wim->balls(0).relativey() + wim->balls(0).relativeyspeed() * 0.200;
			bd = sqrt(pow(bx, 2) + pow(by, 2));
			bb = atan2(by, bx);
		}
	}

	if (bmsg != 0)
	{
		if (bmsg->radius() > 0)
		{
			seeballmessage=1;
			lastball = microsec_clock::universal_time();
			seeballtrust = 1;
		}
		else
		{
			seeballmessage=0;
			if (lastball + milliseconds(600) < microsec_clock::universal_time())
			{
				seeballtrust = 0;
			}
		}
	}
	else
		seeballmessage=0;

	return;
}

int HeadController::MakeHeadAction()
{
	//std::cout<<"HeadAction"<<targetYaw<<":"<<targetPitch<<std::endl;
	hmot.set_command("setHead");
	hmot.set_parameter(0, targetYaw);
	hmot.set_parameter(1, targetPitch);
	_blk.publishSignal(hmot, "motion");
	waiting=0;
	return 1;
}


void HeadController::HeadScanStepHigh(float yaw_limit)
{
	static  bool middle=true;
	static int sign=1;
	if(startscan==true)
	{
		startscan=false;
		middle=true;
		targetPitch=-0.55;
		targetYaw=0;
		sign =(HeadYaw.sensorvalue()>0?1:-1);
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



	//std:cout<<"enum"<<state<<" "<<phase<<std::endl;

	if (startscan)
	{
		ysign = HeadYaw.sensorvalue() > 0 ? +1 : -1; //Side
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
		waiting = 0;

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

	return;
}


void HeadController::HeadTrackIntelligent()
{
	static enum {BALL1, OPPG, BALL2, OWNG} state = BALL1;
	HeadYaw = allsm->jointdata(KDeviceLists::HEAD + KDeviceLists::YAW);
	HeadPitch = allsm->jointdata(KDeviceLists::HEAD + KDeviceLists::PITCH);
	waiting++;

	if ( ( reachedTargetHead()) || (waiting >= WAITFOR) )
	{
		waiting = 0;

		switch (state)
		{
		case BALL1:
			targetYaw = lookAtPointRelativeYaw(bx, by);
			targetPitch = lookAtPointRelativePitch(bx, by);
			state = OPPG;
			break;

		case OPPG:
			targetYaw = robot_phi - lookAtPointRelativeYaw(oppGoalX - robot_x, oppGoalY - robot_y);
			targetYaw = (targetYaw < 0) ? targetYaw - 0.2 : targetYaw + 0.2;
			targetPitch = lookAtPointRelativePitch(oppGoalX - robot_x, oppGoalY - robot_y);
			//if (targetYaw < 1.57)
			//targetPitch = (0.145 * fabs(headpos)) - 0.752;
			//else
			//targetPitch = (-0.0698 * (fabs(headpos) - 1.57)) - 0.52;
			state = BALL2;
			break;

		case BALL2:
			targetYaw = lookAtPointRelativeYaw(bx, by);
			targetYaw = (targetYaw < 0) ? targetYaw - 0.2 : targetYaw + 0.2;
			targetPitch = lookAtPointRelativePitch(bx, by);
			state = OWNG;
			break;

		case OWNG:
			targetYaw = robot_phi - lookAtPointRelativeYaw(ownGoalX - robot_x, ownGoalY - robot_y);
			targetPitch = lookAtPointRelativePitch(ownGoalX - robot_x, ownGoalY - robot_y);
			//if (targetYaw < 1.57)
			//targetPitch = (0.145 * fabs(headpos)) - 0.752;
			//else
			//targetPitch = (-0.0698 * (fabs(headpos) - 1.57)) - 0.52;
			state = BALL1;
			break;
		}

		//		cout << " OwnX: " << ownGoalX-robot_x << " OwnY: " << ownGoalY-robot_y << " OppX: " << oppGoalX-robot_x << " OppY: " << oppGoalY-robot_y << endl;
		//		cout << state << " Yaw: " << targetYaw << " Pitch: " << targetPitch << endl;
		hmot.set_command("setHead");
		hmot.set_parameter(0, targetYaw);
		hmot.set_parameter(1, targetPitch);
		_blk.publishSignal(hmot, "motion");
	}

	return;
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



/* Vision Calibration */

void HeadController::calibrate()
{
	KCalibrateCam v;
	v.set_status(0);
	_blk.publishState(v, "vision");
	calibrated = 1;
}


/* Read Configuration Functions */

bool HeadController::readConfiguration(const std::string& file_name)
{
	XMLConfig config(file_name);

	if (!config.QueryElement("player", playerNumber))
		Logger::Instance().WriteMsg("HeadController", "Configuration file has no player, setting to default value: " + _toString(playerNumber), Logger::Error);

	std::string color;

	if (teamColor == TEAM_BLUE)
		color = "blue";
	else if (teamColor == TEAM_RED)
		color = "red";

	if (!config.QueryElement("default_team_color", color))
		Logger::Instance().WriteMsg("HeadController", "Configuration file has no team_color, setting to default value: " + color, Logger::Error);

	if (color == "blue")
		teamColor = TEAM_BLUE;
	else if (color == "red")
		teamColor = TEAM_RED;
	else
		Logger::Instance().WriteMsg("HeadController", "Undefined color in configuration, setting to default value: " + teamColor, Logger::Error);

	return true;
}


bool HeadController::readRobotConfiguration(const std::string& file_name)
{
	if ( (playerNumber < 1) || (4 < playerNumber) )
	{
		Logger::Instance().WriteMsg("HeadController",  " readRobotConfiguration: Invalid player number "  , Logger::Error);
		return false;
	}

	readRobotConf = true;
	XML config(file_name);
	typedef std::vector<XMLNode<std::string, float, std::string> > NodeCont;
	NodeCont teamPositions, robotPosition ;
	Logger::Instance().WriteMsg("HeadController",  " readRobotConfiguration "  , Logger::Info);

	for (int i = 0; i < 2; i++)
	{
		string kickoff = (i == 0) ? "KickOff" : "noKickOff";	//KICKOFF==0, NOKICKOFF == 1
		bool found = false;
		teamPositions = config.QueryElement<std::string, float, std::string>(kickoff);

		if (teamPositions.size() != 0)
			robotPosition = config.QueryElement<std::string, float, std::string>("robot", &(teamPositions[0]));

		for (NodeCont::iterator it = robotPosition.begin(); it != robotPosition.end(); it++)
		{
			if (it->attrb["number"] == playerNumber)
			{

				found = true;
			}
		}

		if (!found)
		{
			Logger::Instance().WriteMsg("HeadController",  " readRobotConfiguration: Unable to find initial " + kickoff + " position for player number " + _toString(playerNumber) , Logger::Error);
			readRobotConf = false;
		}
	}

	return readRobotConf;
}


bool HeadController::readGoalConfiguration(const std::string& file_name)
{
	TiXmlDocument doc2(file_name.c_str());
	bool loadOkay = doc2.LoadFile();

	if (!loadOkay)
	{
		Logger::Instance().WriteMsg("HeadController",  " readGoalConfiguration: cannot read file " + file_name , Logger::Info);
		return false;
	}

	TiXmlNode * Ftr;
	TiXmlElement * Attr;
	double x, y;
	string ID;

	for (Ftr = doc2.FirstChild()->NextSibling(); Ftr != 0; Ftr = Ftr->NextSibling())
	{
		if(Ftr->ToComment() == NULL)
		{
			Attr = Ftr->ToElement();
			Attr->Attribute("x", &x);
			Attr->Attribute("y", &y);
			ID = Attr->Attribute("ID");

			if (ID == "YellowGoal")
			{
				oppGoalX = x;
				oppGoalY = y;
				ownGoalX = -oppGoalX;
				ownGoalY = -oppGoalY;
			}

			if (ID == "YellowLeft")
			{
				oppGoalLeftX = x;
				oppGoalLeftY = y;
				ownGoalLeftX = -oppGoalLeftX;
				ownGoalLeftY = -oppGoalLeftY;
			}

			if (ID == "YellowRight")
			{
				oppGoalRightX = x;
				oppGoalRightY = y;
				ownGoalRightX = -oppGoalRightX;
				ownGoalRightY = -oppGoalRightY;
			}
		}
	}

	return true;
}

float HeadController::dist(float x1, float y1, float x2, float y2)
{
	return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}



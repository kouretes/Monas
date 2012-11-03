#include "Behavior.h"

#include <math.h>

using namespace KMath;
using namespace boost::posix_time;

ACTIVITY_REGISTER(Behavior);
using namespace std;


/**
 *	Returns a double random number (0 - 100.0)
 */
double behaviorRand()
{
	return (rand() % 100) / 100.0;
}


/**
 *	Behavior Initialization
 */
void Behavior::UserInit()
{
	_blk.updateSubscription("vision", msgentry::SUBSCRIBE_ON_TOPIC);
	_blk.updateSubscription("sensors", msgentry::SUBSCRIBE_ON_TOPIC);
	_blk.updateSubscription("worldstate", msgentry::SUBSCRIBE_ON_TOPIC);
	_blk.updateSubscription("obstacle", msgentry::SUBSCRIBE_ON_TOPIC);
	_blk.updateSubscription("behavior", msgentry::SUBSCRIBE_ON_TOPIC);
	wmot = new MotionWalkMessage();
	wmot->add_parameter(0.0f);
	wmot->add_parameter(0.0f);
	wmot->add_parameter(0.0f);
	wmot->add_parameter(0.0f);
	amot = new MotionActionMessage();
	locReset = new LocalizationResetMessage();
	pprm = new PathPlanningRequestMessage();
	fom = new ObstacleMessage();
	hcontrol = new HeadControlMessage();
	ballfound = false;
	pathOK = true;
	kickoff = false;

	for (int i = 0; i < 2; i++)
	{
		initX[i] = 0.0;
		initY[i] = 0.0;
		initPhi[i] = 0.0;
	}
	cX = 0.0;
	cY = 0.0;
	ct = 0.0;
	bd = 0.0;
	bb = 0.0;
	bx = 0.0;
	by = 0.0;
	side = +1;
	robot_x = 0.0;
	robot_y = 0.0;
	robot_phi = 0.0;
	robot_confidence = 1.0;
	readytokick = false;
	direction = 1;
	orientation = 0;
	gameState = PLAYER_INITIAL;
	teamColor = TEAM_BLUE;
	role = ATTACKER;
	readConfiguration(ArchConfig::Instance().GetConfigPrefix() + "/team_config.xml");		// reads playerNumber, teamColor
	readRobotConfiguration(ArchConfig::Instance().GetConfigPrefix() + "/robotConfig.xml");	// reads initX, initY, initPhi
	readGoalConfiguration(ArchConfig::Instance().GetConfigPrefix() + "/Features.xml");		// reads blueGoal*, yellowGoal*
	srand(time(0));
	lastwalk = microsec_clock::universal_time();
	lastplay = microsec_clock::universal_time();
	lastpenalized = microsec_clock::universal_time();
	//    generateFakeObstacles();
	Logger::Instance().WriteMsg("Behavior", "Initialized: My number is " + _toString(playerNumber) + " and my color is " + _toString(teamColor), Logger::Info);
	Reset();

}


/**
	TODO
	Works only for goalie!!!
	default value??
*/
void Behavior::Reset(){
	config.ur = atof(_xml.findValueForKey("behavior.ur").front().c_str());
	Logger::Instance().WriteMsg("Behavior", "Reset done", Logger::Warning);
}


/* Behavior Main Execution Function */

int Behavior::Execute()
{
	read_messages();
	GetGameState();
	GetPosition();

    if (gameState == PLAYER_INITIAL){
         hcontrol->mutable_task()->set_action(HeadControlMessage::FROWN);
        _blk.publishState(*hcontrol, "behavior");
    }
	else if (gameState == PLAYER_PLAYING)
	{
		if(bfm != 0) {
			if(bfm.get() != 0) {
				ballfound = bfm->ballfound();
			}
		}

		UpdateOrientation();
		//checkForPenaltyArea();
		readytokick = false;

		if(playerNumber == 1 || role == GOALIE) { // goalie role if number 1
			Goalie();
		}
		else { // not goalie behavior
			//TODO goalie must go to his position, not walk strait :P
			if (lastpenalized + seconds(4) > microsec_clock::universal_time())
			{
				hcontrol->mutable_task()->set_action(HeadControlMessage::LOCALIZE_FAR);
				_blk.publishState(*hcontrol, "behavior");
				return 0;
			}

			// Publish message to head controller to run check for ball
			hcontrol->mutable_task()->set_action(HeadControlMessage::SMART_SELECT);
			_blk.publishState(*hcontrol, "behavior");
			if (ballfound == 1)
			{
				side = (bb > 0) ? 1 : -1;
				posx = 0.1, posy = 0.03; // Desired ball position for kick
				double epsx = 0.025, epsy = 0.025; // Desired precision

				if ( (fabs( bx - posx ) < epsx)  && (fabs( by - (side * posy) ) < epsy) && (bmsg != 0) && (bmsg->radius() > 0) )
				{
					readytokick = true;
					Kick(side);
					direction = (side == +1) ? -1 : +1;
				}

				if (!readytokick)
				{
					//Define roles
					if(ClosestRobot())
					{
						role = ATTACKER;
					}
					else
					{
						role = CENTER_FOR;
					}
					//Logger::Instance().WriteMsg("BehaviorTest", "Role: " + _toString(role), Logger::Info);
					approachBallRoleDependent();
				}
			}
			if (ballfound == 0)
			{
				/* New exploration */

				//walk straight for some seconds after the scan has ended (lastpenalized+seconds(12))
				//and then start turning around to search for ball.
				if (lastpenalized + seconds(14) > microsec_clock::universal_time())
				{
					pathPlanningRequestAbsolute(0.2, 0.0, 0.0);
				}
				else if ( (fabs(robot_x) < 2.0) && (fabs(robot_y) < 2.0) )
					pathPlanningRequestAbsolute(0.45, 0.45 * direction, M_PI_4 * direction);
				else
					pathPlanningRequestAbsolute(0.1, 0.1 * direction, M_PI_4 * direction);

				/* End of New Exploration */
				//HeadScanStepSmart();
			}

		} // not goalie behavior end
	}
	else if (gameState == PLAYER_READY)
	{
		//HeadScanStepSmart();
		hcontrol->mutable_task()->set_action(HeadControlMessage::LOCALIZE);
		_blk.publishState(*hcontrol, "behavior");
		int p = (kickoff) ? 0 : 1;
		gotoPosition( initX[p], initY[p], initPhi[p] );
		return 0;
	}
	else if (gameState == PLAYER_SET)
	{
		hcontrol->mutable_task()->set_action(HeadControlMessage::SCAN_AND_TRACK_FOR_BALL);
		_blk.publishState(*hcontrol, "behavior");

		//if (ballfound == 0)
			//HeadScanStepSmart();
	}
	else if(gameState == PLAYER_PENALISED)
	{
		hcontrol->mutable_task()->set_action(HeadControlMessage::FROWN);
		_blk.publishState(*hcontrol, "behavior");
	}

	return 0;
}


/**
	Read Incoming Messages from declared topics...use Message objects to get the data.
  */

void Behavior::read_messages()
{
	gsm  = _blk.readState<GameStateMessage> ("worldstate");
	bmsg = _blk.readSignal<BallTrackMessage> ("vision");
	allsm = _blk.readData<AllSensorValuesMessage> ("sensors");
	om   = _blk.readState<ObstacleMessageArray> ("obstacle");
	wim  = _blk.readData<WorldInfo> ("worldstate");
	swim = _blk.readData<SharedWorldInfo> ("worldstate");
	bfm = _blk.readState<BallFoundMessage> ("behavior");

	if(wim != 0)
	{
	    if(wim.get() != 0)
		{
            if (wim->balls_size() > 0)
            {
                bx = wim->balls(0).relativex() + wim->balls(0).relativexspeed() * 0.200;
                by = wim->balls(0).relativey() + wim->balls(0).relativeyspeed() * 0.200;
                bd = sqrt(pow(bx, 2) + pow(by, 2));
                bb = atan2(by, bx);
            }
		}
	}
}


/* Information Gathering Functions */

void Behavior::GetGameState()
{
	if (gsm != 0)
	{
		int prevGameState = gameState;
		gameState = gsm->player_state();
		teamColor = gsm->team_color();
		playerNumber = gsm->player_number();

		if (gameState == PLAYER_PLAYING)
		{
			if (prevGameState == PLAYER_PENALISED)
			{
				direction = 1;
				lastpenalized = microsec_clock::universal_time();
				locReset->set_type(PLAYER_PENALISED);
				locReset->set_kickoff(kickoff);
				_blk.publishSignal(*locReset, "worldstate");
			}

			if (prevGameState == PLAYER_SET)
			{
				lastplay = microsec_clock::universal_time();
			}
		}
		else if (gameState == PLAYER_INITIAL)
		{
		}
		else if (gameState == PLAYER_READY)
		{
			if (gameState != prevGameState)
			{
				stopRobot();
				locReset->set_type(PLAYER_READY);
				locReset->set_kickoff(kickoff);
				_blk.publishSignal(*locReset, "worldstate");
			}
		}
		else if (gameState == PLAYER_SET)
		{
			kickoff = gsm->kickoff();
			locReset->set_type(PLAYER_SET);
			locReset->set_kickoff(kickoff);
			_blk.publishSignal(*locReset, "worldstate");

			if (gameState != prevGameState)
				stopRobot();
		}
		else if (gameState == PLAYER_FINISHED)
		{
			;
		}
		else if (gameState == PLAYER_PENALISED)
		{
			kickoff = gsm->kickoff();
		}
	}
}

bool Behavior::ClosestRobot()
{
	double epsx = 0.005, epsy = 0.005; // Desired precision
	if(swim != 0)
	{
		if(swim.get() != 0)
		{
			//            Logger::Instance().WriteMsg("SharedWorldModel", "Closest robot x: " + _toString(swim->playerclosesttoball().x()) +
			//                                " y: " + _toString(swim->playerclosesttoball().y()), Logger::Info);
			double closest_robot_x = swim->playerclosesttoball().x();
			double closest_robot_y = swim->playerclosesttoball().y();

			if(((fabs(robot_x - closest_robot_x)) < epsx) && (fabsf(robot_y - closest_robot_y) < epsy))
				return true;
			else
				return false;
		}
	}
	else
		return true;
}

void Behavior::GetPosition()
{
	if(wim != 0){
		if(wim.get() != 0)
		{
			robot_x = wim->myposition().x();
			robot_y = wim->myposition().y();
			robot_phi = wrapToPi( wim->myposition().phi() );
			robot_confidence = wim->myposition().confidence();
		}
	}
}


void Behavior::UpdateOrientation()
{
	double loppgb = anglediff2(atan2(oppGoalLeftY - robot_y, oppGoalLeftX - robot_x), robot_phi);
	double roppgb = anglediff2(atan2(oppGoalRightY - robot_y, oppGoalRightX - robot_x), robot_phi);
	double cone = anglediff2(loppgb, roppgb);
	double oppgb = wrapToPi(roppgb + cone / 2.0);

	if ( (oppgb <= M_PI_4) && (oppgb > -M_PI_4) )
	{
		orientation = 0;
	}
	else if ( (oppgb > M_PI_4) && (oppgb <= (M_PI - M_PI_4) ) )
	{
		orientation = 1;
	}
	else if ( (oppgb > (M_PI - M_PI_4) ) || (oppgb <= -(M_PI - M_PI_4) ) )
	{
		orientation = 2;
	}
	else if ( (oppgb <= -M_PI_4 ) && (oppgb > -(M_PI - M_PI_4) ) )
	{
		orientation = 3;
	}
}

/* Kicking */

void Behavior::Kick(int side)
{
	if ( kickoff && (microsec_clock::universal_time() <= lastplay + seconds(25)) )
	{
		if (behaviorRand() < 0.75)
		{
			littleWalk(0.2, 0.0, 0.0);
		}
		else
		{
			if (side == 1)
				amot->set_command("KickSideLeftStable3.xar");
			else
				amot->set_command("KickSideRightStable3.xar");

			_blk.publishSignal(*amot, "motion");
		}
	}
	else
	{
		if (orientation == 0)
		{
			if (by > 0.0)
				amot->set_command("KickForwardLeft2.xar"); //LeftKick
			else
				amot->set_command("KickForwardRight2.xar"); //RightKick
		}
		else if (orientation == 3)
		{
			amot->set_command("KickSideLeftStable3.xar"); //"HardLeftSideKick" KickSideLeftFast
			direction = -1;
		}
		else if (orientation == 1)
		{
			amot->set_command("KickSideRightStable3.xar"); //"HardRightSideKick" KickSideRightFast
			direction = +1;
		}
		else if (orientation == 2)
		{
			if (by > 0.0)
				amot->set_command("KickSideLeftStable3.xar"); //LeftBackHigh_carpet KickBackLeft KickBackLeftPierris
			else
				amot->set_command("KickSideRightStable3.xar"); //RightBackHigh_carpet KickBackRight KickBackRightPierris
		}
		else
		{
			if (by > 0.0)
				amot->set_command("KickSideLeftStable3.xar");
			else
				amot->set_command("KickSideRightStable3.xar");
		}

		_blk.publishSignal(*amot, "motion");
	}
}

/* Locomotion Functions */

void Behavior::velocityWalk(double ix, double iy, double it, double f)
{
	double x, y, t;
	x = ix;
	y = iy;
	t = it;

	/* BEGIN - Basic Obstacle Avoidance Code */
	if ( (om != 0) && (playerNumber == 2) )
	{
		if ( (om->distance(2) <= 0.4) && (om->distance(0) <= 0.4) )
		{
			if (x > 0.0)
			{
				x = 0.0;
				t = 0.0;
			}
		}
		else if (om->distance(2) <= 0.4)
		{
			if (x > 0.0)
			{
				x = 0.0;
				y = 0.5;
				t = 0.0;
			}
		}
		else if (om->distance(0) <= 0.4)
		{
			if (x > 0.0)
			{
				x = 0.0;
				y = -0.5;
				t = 0.0;
			}
		}
	}

	/* END - Basic Obstacle Avoidance Code */
	wmot->set_command("setWalkTargetVelocity");

	if ( (x == 0.0) && (y == 0.0) && (t == 0.0) )
	{
		cX = 0.0;
		cY = 0.0;
		ct = 0.0;
	}
	else
	{
		if( lastwalk + milliseconds(200) > microsec_clock::universal_time() )
			return;

		x = (x > +1.0) ? +1.0 : x;
		x = (x < -1.0) ? -1.0 : x;
		y = (y > +1.0) ? +1.0 : y;
		y = (y < -1.0) ? -1.0 : y;
		t = (t > +1.0) ? +1.0 : t;
		t = (t < -1.0) ? -1.0 : t;
		cX = 0.25 * cX + 0.75 * x;
		cY = 0.25 * cY + 0.75 * y;
		ct = 0.25 * ct + 0.75 * t;
	}

	wmot->set_parameter(0, cX);
	wmot->set_parameter(1, cY);
	wmot->set_parameter(2, ct);
	wmot->set_parameter(3, f);
	_blk.publishSignal(*wmot, "motion");
	lastwalk = microsec_clock::universal_time();
}


void Behavior::littleWalk(double x, double y, double th)
{
	wmot->set_command("walkTo");
	wmot->set_parameter(0, x);
	wmot->set_parameter(1, y);
	wmot->set_parameter(2, th);
	_blk.publishSignal(*wmot, "motion");
}

void Behavior::approachBall()
{
	if (bd > 0.3)
    {
        int pathSide = (bb > 0) ? 1 : -1;
        pathPlanningRequestRelative(bx, by, pathSide * M_PI_2);
    }
    else if(robot_phi > (float) (M_PI_4/2.0)){
        pathPlanningRequestAbsolute(0.1, 0.55, (float)(-3*M_PI_4/2.0));
    }
    else if(robot_phi < (float) (-M_PI_4/2.0)){
        littleWalk(0.1, -0.55, (float)(3*M_PI_4/2.0));
    }
    else
        littleWalk(bx - posx, by - side * posy, bb);
}

void Behavior::approachBallRoleDependent()
{
    if (orientation == 1)
		side = -1;
	else if (orientation == 3)
		side = +1;

	if(role == ATTACKER)
	{
		approachBall();

	}
	else if(role == CENTER_FOR)
	{
		if (bd > 0.7)
		{
			int pathSide = (bb > 0) ? 1 : -1;
			pathPlanningRequestAbsolute(bx - posx, by - side * posy, bb);
		}
		else
			stopRobot();
	}
}

void Behavior::stopRobot()
{
	//velocityWalk(0.0, 0.0, 0.0, 1.0);
	amot->set_command("InitPose.xar");
	_blk.publishSignal(*amot, "motion");
}

void Behavior::pathPlanningRequestRelative(float target_x, float target_y, float target_phi)
{
	pprm->set_gotox(target_x);
	pprm->set_gotoy(target_y);
	pprm->set_gotoangle(target_phi);
	pprm->set_mode("relative");
	_blk.publishSignal(*pprm, "obstacle");
}

void Behavior::pathPlanningRequestAbsolute(float target_x, float target_y, float target_phi)
{
	pprm->set_gotox(target_x);
	pprm->set_gotoy(target_y);
	pprm->set_gotoangle(target_phi);
	pprm->set_mode("absolute");
	_blk.publishSignal(*pprm, "obstacle");
}

void Behavior::gotoPosition(float target_x, float target_y, float target_phi)
{
	double targetDistance = sqrt((target_x - robot_x) * (target_x - robot_x) + (target_y - robot_y) * (target_y - robot_y));
	double targetAngle = anglediff2(atan2(target_y - robot_y, target_x - robot_x), robot_phi);
	double targetOrientation = anglediff2(target_phi, robot_phi);

	if ( (targetDistance > 0.25) || (fabs(targetOrientation) > M_PI_4) )
		pathPlanningRequestAbsolute(toCartesianX(targetDistance, targetAngle),
		                            toCartesianY(targetDistance, targetAngle),
		                            targetOrientation);
	else
		stopRobot();
}

/* Read Configuration Functions */

bool Behavior::readConfiguration(const std::string& file_name)
{
	XMLConfig config(file_name);

	if (!config.QueryElement("player", playerNumber))
		Logger::Instance().WriteMsg("Behavior", "Configuration file has no player, setting to default value: " + _toString(playerNumber), Logger::Error);

	std::string color;

	if (teamColor == TEAM_BLUE)
		color = "blue";
	else if (teamColor == TEAM_RED)
		color = "red";

	if (!config.QueryElement("default_team_color", color))
		Logger::Instance().WriteMsg("Behavior", "Configuration file has no team_color, setting to default value: " + color, Logger::Error);

	if (color == "blue")
		teamColor = TEAM_BLUE;
	else if (color == "red")
		teamColor = TEAM_RED;
	else
		Logger::Instance().WriteMsg("Behavior", "Undefined color in configuration, setting to default value: " + color, Logger::Error);

	return true;
}


bool Behavior::readRobotConfiguration(const std::string& file_name)
{
	if ( (playerNumber < 1) || (4 < playerNumber) )
	{
		Logger::Instance().WriteMsg("Behavior",  " readRobotConfiguration: Invalid player number "  , Logger::Error);
		return false;
	}

	XML config(file_name);
	typedef std::vector<XMLNode<std::string, float, std::string> > NodeCont;
	NodeCont teamPositions, robotPosition ;
	Logger::Instance().WriteMsg("Behavior",  " readRobotConfiguration "  , Logger::Info);

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
				initPhi[i] = 0.0;
				initX[i] = (it->attrb["posx"]);
				initY[i] = (it->attrb["posy"]);
				Logger::Instance().WriteMsg("Behavior", " readConf INIT X " + kickoff + " " + _toString(initX[i]) + " INITY " + _toString(initY[i]) + " INITPHI " + _toString(initPhi[i]), Logger::Info);
				found = true;
			}
		}

		if (!found)
		{
			Logger::Instance().WriteMsg("Behavior",  " readRobotConfiguration: Unable to find initial " + kickoff + " position for player number " + _toString(playerNumber) , Logger::Error);
			return false;
		}
	}

	return true;
}


bool Behavior::readGoalConfiguration(const std::string& file_name)
{
	TiXmlDocument doc2(file_name.c_str());
	bool loadOkay = doc2.LoadFile();

	if (!loadOkay)
	{
		Logger::Instance().WriteMsg("Behavior",  " readGoalConfiguration: cannot read file " + file_name , Logger::Info);
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

void Behavior::generateFakeObstacles()
{
	float tmpX = -3.0 + ObstacleRadius, tmpY = -1.1 + ObstacleRadius;

	for(int j = 0; j < numOfFakeObstacles; j++)
	{
		fakeObstacles[j][0] = INIT_VALUE;
		fakeObstacles[j][1] = INIT_VALUE;
	}

	int i = 0;

	while(tmpX < -2.4)
	{
		fakeObstacles[i][0] = tmpX;
		fakeObstacles[i][1] = tmpY;
		fakeObstacles[i + 1][0] = tmpX;
		fakeObstacles[i + 1][1] = -tmpY;
		tmpX += 2 * ObstacleRadius;
		i += 2;
	}

	tmpX = -2.4 + ObstacleRadius;

	while(tmpY < 1.1)
	{
		fakeObstacles[i][0] = tmpX;
		fakeObstacles[i][1] = tmpY;
		tmpY += 2 * ObstacleRadius + 0.02;
		i++;
	}
}

void Behavior::checkForPenaltyArea()
{
	float fakeDist = 0.0, fakeDir = 0.0;

	for(int j = 0; j < numOfFakeObstacles; j++)
	{
		if(fakeObstacles[j][0] == INIT_VALUE)
			continue;
		else
		{
			fakeDist = DISTANCE(robot_x, robot_y, fakeObstacles[j][0], fakeObstacles[j][1]);

			if(fakeDist < MapRadius)
			{
				//send fake obstacle message
				//               fakeDir = anglediff2(atan2(fakeObstacles[j][1]-robot_y,fakeObstacles[j][0]-robot_x), robot_phi);
				fakeDir = 2 * M_PI - wrapTo0_2Pi(robot_phi) - atan2(fakeObstacles[j][1] - robot_y, fakeObstacles[j][0] - robot_x);
				fom->set_direction(fakeDir);
				fom->set_distance(fakeDist);
				fom->set_certainty(1);
				_blk.publishSignal(*fom, "obstacle");
			}
		}
	}
}

/*------------------------------- GOALIE -------------------------------*/

void Behavior::Goalie()
{
	role = GOALIE;
	if(ballfound == 1) {

		fall = toFallOrNotToFall();

		if(fall == 1) // extend left foot
		{
			amot->set_command("goalieLeftFootExtened.xar");
			_blk.publishSignal(*amot, "motion");
		}
		else if(fall == -1) // extend right foot
		{
			amot->set_command("goalieRightFootExtened.xar");
			_blk.publishSignal(*amot, "motion");
		}
		else // scan and track for ball
		{
			hcontrol->mutable_task()->set_action(HeadControlMessage::SCAN_AND_TRACK_FOR_BALL);
			_blk.publishState(*hcontrol, "behavior");
		}

	}
	else if(ballfound == 0) {
		hcontrol->mutable_task()->set_action(HeadControlMessage::SCAN_AND_TRACK_FOR_BALL);
		_blk.publishState(*hcontrol, "behavior");
	}

	return;
}

int Behavior::toFallOrNotToFall()
{
	float x1, y1, dk, ubx, uby, ub;

	if(wim == 0)   // the two last observation messages
		return 0;

	if(wim->balls_size() == 0)
		return 0;

	// observation of (x,y) position of the ball and x,y speed
	x1 = wim->balls(0).relativex();
	y1 = wim->balls(0).relativey();
	ubx = wim->balls(0).relativexspeed();
	uby = wim->balls(0).relativeyspeed();


	if(ubx < 0.0)
	{
		//Logger::Instance().WriteMsg("toFallOrNotToFall", "ubx<0", Logger::Info);
		dk = (ubx * y1 - uby * x1) / ubx ; // dk is the projection of the ball's route towards the robot/goalpost
		//Logger::Instance().WriteMsg("toFallOrNotToFall","DK:"+_toString(dk), Logger::Info);
		if(fabs(dk) <= 0.3) //if dk is shorter than the robot's foot can extend
		{
			// ur old value 0.1 / 1.4
			ub = sqrt(ubx * ubx + uby * uby);
			//Logger::Instance().WriteMsg("toFallOrNotToFall","UB:"+_toString(ub), Logger::Info);
			//Logger::Instance().WriteMsg("toFallOrNotToFall","UR:"+_toString(config.ur), Logger::Info);
			if(fabs(ub) > config.ur)
			{
				if(dk > 0)
					return 1;  // left
				else
					return -1;  // right
			}
		}
	}

	return 0;
}

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
	wmot.add_parameter(0.0f);
	wmot.add_parameter(0.0f);
	wmot.add_parameter(0.0f);
	wmot.add_parameter(0.0f);
	ballfound = false;
	pathOK = true;
	kickoff = false;
	cX = 0.0;
	cY = 0.0;
	ct = 0.0;
	ball_dist = 0.0;
	ball_bearing = 0.0;
	goalieApproachStarted = false;
	ball_x = 0.0;
	ball_y = 0.0;
	side =+ 1;
	robot_x = 0.0;
	robot_y = 0.0;
	robot_phi = 0.0;
	robot_confidence = 1.0;
	readytokick = false;
	direction = 1;
	orientation = 0;
	gameState = PLAYER_INITIAL;
	role = ATTACKER;
	Reset();
	Logger::Instance().WriteMsg("Behavior", "Initialized: My number is " + _toString(config.playerNumber) + " and my color is " + _toString(config.teamColor), Logger::Info);
	srand(time(0));
	lastwalk = microsec_clock::universal_time();
	lastplay = microsec_clock::universal_time();
	lastpenalised = microsec_clock::universal_time();
    hcontrol.mutable_task()->set_action(HeadControlMessage::FROWN);
    _blk.publishState(hcontrol, "behavior");
	// generateFakeObstacles();
}


/**
	Reset function that reads all xml configuration data and update the config struct
	of behavior. All new xml reads MUST be added here, dont make other functions!
*/
void Behavior::Reset(){

	// === read team configuration xml data from teamConfig.xml===
	config.teamNumber = atoi(_xml.findValueForKey("teamConfig.team_number").c_str());
	config.playerNumber = atoi(_xml.findValueForKey("teamConfig.player").c_str());
	config.maxPlayers = atoi(_xml.findValueForKey("teamConfig.team_max_players").c_str());

	std::string color;
	color = _xml.findValueForKey("teamConfig.default_team_color").c_str();
	if(color.compare("blue") == 0)
		config.teamColor = TEAM_BLUE;
	else
		config.teamColor == TEAM_RED;

	// === read behavior configuration xml data from behavior.xml===
	config.posx = atof(_xml.findValueForKey("behavior.posx").c_str());
	config.posy = atof(_xml.findValueForKey("behavior.posy").c_str());
	config.epsx = atof(_xml.findValueForKey("behavior.epsx").c_str());
	config.epsy = atof(_xml.findValueForKey("behavior.epsy").c_str());
	config.kicks.KickForwardLeft = _xml.findValueForKey("behavior.KickForwardLeft").c_str();
	config.kicks.KickForwardRight = _xml.findValueForKey("behavior.KickForwardRight").c_str();
	config.kicks.KickSideLeft = _xml.findValueForKey("behavior.KickSideLeft").c_str();
	config.kicks.KickSideRight = _xml.findValueForKey("behavior.KickSideRight").c_str();
	config.kicks.KickBackLeft = _xml.findValueForKey("behavior.KickBackLeft").c_str();
	config.kicks.KickBackRight = _xml.findValueForKey("behavior.KickBackRight").c_str();
	config.ur = atof(_xml.findValueForKey("behavior.ur").c_str());

	// === read robot configuration xml data from playerConfig.xml===
	if ( (config.playerNumber < 1) || (config.playerNumber > config.maxPlayers) )
		Logger::Instance().WriteMsg("Behavior", "Behavior Reset: Invalid player number", Logger::Error);

	for(int i = 0 ; i < 2 ; i++)
	{
		std::string kickOff = (i == 0) ? "KickOff" : "noKickOff";	// KICKOFF == 0, NOKICKOFF == 1

		for(int r = 0 ; r < config.maxPlayers ; r++) // for each robot on the node
		{
			if( atoi(_xml.findValueForKey("playerConfig."+kickOff+".robot~"+_toString(r)+".$number").c_str()) == config.playerNumber )
			{
				config.initPhi[i] = 0.0;
				config.initX[i] = atof(_xml.findValueForKey("playerConfig."+kickOff+".robot~"+_toString(r)+".$posx").c_str());
				config.initY[i] = atof(_xml.findValueForKey("playerConfig."+kickOff+".robot~"+_toString(r)+".$posy").c_str());
				break;
			}
		}
	}

	// === read goal configuration xml data from Fearures.xml ===
	std::string ID;
	for(int v = 0 ; v < _xml.numberOfNodesForKey("features.ftr") ; v++)
	{
		ID = _xml.findValueForKey("features.ftr~"+_toString(v)+".ID").c_str();
		if(ID == "YellowGoal")
		{
			config.oppGoalX = atof(_xml.findValueForKey("features.ftr~"+_toString(v)+".x").c_str());
			config.oppGoalY = atof(_xml.findValueForKey("features.ftr~"+_toString(v)+".y").c_str());
			config.ownGoalX = -config.oppGoalX;
			config.ownGoalY = -config.oppGoalY;
		}
		else if(ID == "YellowLeft")
		{
			config.oppGoalLeftX = atof(_xml.findValueForKey("features.ftr~"+_toString(v)+".x").c_str());
			config.oppGoalLeftY = atof(_xml.findValueForKey("features.ftr~"+_toString(v)+".y").c_str());
			config.ownGoalLeftX = -config.oppGoalLeftX;
			config.ownGoalLeftY = -config.oppGoalLeftY;
		}
		else if(ID == "YellowRight")
		{
			config.oppGoalRightX = atof(_xml.findValueForKey("features.ftr~"+_toString(v)+".x").c_str());
			config.oppGoalRightY = atof(_xml.findValueForKey("features.ftr~"+_toString(v)+".y").c_str());
			config.ownGoalRightX = -config.oppGoalRightX;
			config.ownGoalRightY = -config.oppGoalRightY;
		}
	}
	gameMode = atoi(_xml.findValueForKey("teamConfig.game_mode").c_str()) == 1 ? true : false;
	Logger::Instance().WriteMsg("Behavior", "Reset done", Logger::Info);
}


/* Behavior Main Execution Function */

int Behavior::Execute()
{
	read_messages();
	GetGameState();
	GetPosition();

    if (gameState == PLAYER_INITIAL){
		if(prevGameState != PLAYER_INITIAL){
        	hcontrol.mutable_task()->set_action(HeadControlMessage::FROWN);
        	_blk.publishState(hcontrol, "behavior");
		}
    }
	else if (gameState == PLAYER_PLAYING)
	{
		if(prevGameState == PLAYER_PENALISED){
			lastpenalised = microsec_clock::universal_time();
			//Check if the penalized was a wrong decision
			if(microsec_clock::universal_time() - penalisedStarted > seconds(10) && gameMode){
				direction = 1;
				locReset.set_type(LocalizationResetMessage::PENALISED);
				locReset.set_kickoff(kickoff);
				_blk.publishSignal(locReset, "worldstate");
			}
		}

		if (prevGameState == PLAYER_SET)
		{
			lastplay = microsec_clock::universal_time();
		}

		if(bfm != 0 && bfm.get() != 0) {
			ballfound = bfm->ballfound();
		}
			
		UpdateOrientation();
		readytokick = false;

		if(config.playerNumber == 1 || role == GOALIE) { // goalie role if number 1
			Goalie();
		}
		else { // not goalie behavior

			//TODO goalie must go to his position, not walk strait :P
			if (lastpenalised + seconds(4) > microsec_clock::universal_time())
			{
				hcontrol.mutable_task()->set_action(HeadControlMessage::LOCALIZE_FAR);
				_blk.publishState(hcontrol, "behavior");
				return 0;
			}

			// Publish message to head controller to run check for ball
			hcontrol.mutable_task()->set_action(HeadControlMessage::SMART_SELECT);
			_blk.publishState(hcontrol, "behavior");
			if (ballfound == 1)
			{
				side = (ball_bearing > 0) ? 1 : -1;
				//posx = 0.1, posy = 0.03; // Desired ball position for kick
				//double epsx = 0.025, epsy = 0.025; // Desired precision

				if ( (fabs( ball_x - config.posx ) < config.epsx)  && (fabs( ball_y - (side * config.posy) ) < config.epsy) && (bmsg != 0) && (bmsg->radius() > 0) )
				{
					readytokick = true;
					Kick();
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
				//walk straight for some seconds after the scan has ended (lastpenalised+seconds(12))
				//and then start turning around to search for ball.
				if (lastpenalised + seconds(14) > microsec_clock::universal_time())
				{
					pathPlanningRequestAbsolute(0.2, 0.0, 0.0);
				}
				else if ( (fabs(robot_x) < 2.0) && (fabs(robot_y) < 2.0) )
					pathPlanningRequestAbsolute(0.45, 0.45 * direction, M_PI_4 * direction);
				else
					pathPlanningRequestAbsolute(0.1, 0.1 * direction, M_PI_4 * direction);
			}

		} // not goalie behavior end
	}
	else if (gameState == PLAYER_READY)
	{
		if (gameState != prevGameState)
		{
			if(prevGameState != PLAYER_PLAYING){
				locReset.set_type(LocalizationResetMessage::READY);
				locReset.set_kickoff(kickoff);
				_blk.publishSignal(locReset, "worldstate");
				stopRobot();
			}
			hcontrol.mutable_task()->set_action(HeadControlMessage::LOCALIZE);
			_blk.publishState(hcontrol, "behavior");
		}
		int p = (kickoff) ? 0 : 1;
		gotoPosition(config.initX[p], config.initY[p], config.initPhi[p] );
		return 0;
	}
	else if (gameState == PLAYER_SET)
	{
		kickoff = gsm->kickoff();

		if (gameState != prevGameState){

			//Reset Loc
			locReset.set_type(LocalizationResetMessage::SET);
			locReset.set_kickoff(kickoff);
			_blk.publishSignal(locReset, "worldstate");

			stopRobot();

			hcontrol.mutable_task()->set_action(HeadControlMessage::SCAN_AND_TRACK_FOR_BALL);
			_blk.publishState(hcontrol, "behavior");
		}
	}
	else if(gameState == PLAYER_PENALISED)
	{
		kickoff = gsm->kickoff();
		if (gameState != prevGameState){
			penalisedStarted = microsec_clock::universal_time();
			hcontrol.mutable_task()->set_action(HeadControlMessage::FROWN);
			_blk.publishState(hcontrol, "behavior");
		}
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

	if(wim != 0 && wim.get() != 0 && wim->balls_size() > 0)
	{
		ball_x = wim->balls(0).relativex() + wim->balls(0).relativexspeed() * 0.200;
		ball_y = wim->balls(0).relativey() + wim->balls(0).relativeyspeed() * 0.200;
		ball_dist = sqrt(pow(ball_x, 2) + pow(ball_y, 2));
		ball_bearing = atan2(ball_y, ball_x);
	}
}


/* Information Gathering Functions */

void Behavior::GetGameState()
{
	if (gsm != 0)
	{
		prevGameState = gameState;
		gameState = gsm->player_state();
		config.teamColor = gsm->team_color();
		config.playerNumber = gsm->player_number();

	}
}

bool Behavior::ClosestRobot()
{
	double robot_epsx = 0.005, robot_epsy = 0.005; // Desired precision
	if(swim != 0 && swim.get() != 0)
	{
		double closest_robot_x = swim->playerclosesttoball().x();
		double closest_robot_y = swim->playerclosesttoball().y();

		if(((fabs(robot_x - closest_robot_x)) < robot_epsx) && (fabsf(robot_y - closest_robot_y) < robot_epsy))
			return true;
		else
			return false;
	}
	else
		return true;
}

void Behavior::GetPosition()
{
	if(wim != 0 && wim.get() != 0){
		robot_x = wim->myposition().x();
		robot_y = wim->myposition().y();
		robot_phi = wrapToPi( wim->myposition().phi() );
		robot_confidence = wim->myposition().confidence();
	}
}


void Behavior::UpdateOrientation()
{
	double loppgb = anglediff2(atan2(config.oppGoalLeftY - robot_y, config.oppGoalLeftX - robot_x), robot_phi);
	double roppgb = anglediff2(atan2(config.oppGoalRightY - robot_y, config.oppGoalRightX - robot_x), robot_phi);
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

void Behavior::Kick()
{
	if ( kickoff && (microsec_clock::universal_time() <= lastplay + seconds(0/*25*/)) )
	{
		if (behaviorRand() < 0.75)
		{
			littleWalk(0.2, 0.0, 0.0);
		}
		else
		{
			if (side == 1)
				amot.set_command(config.kicks.KickSideLeft);
			else
				amot.set_command(config.kicks.KickSideRight);

			_blk.publishSignal(amot, "motion");
		}
	}
	else
	{
        cout << "Kick orientation = " << orientation << "\n";
		if (orientation == 0)
		{
			if (ball_y > 0.0)
				amot.set_command(config.kicks.KickForwardLeft); // Left Kick
			else
				amot.set_command(config.kicks.KickForwardRight); // Right Kick
		}
		else if (orientation == 3)
		{
			amot.set_command(config.kicks.KickSideLeft); //  HardLeftSideKick KickSideLeftFast
			direction = -1;
		}
		else if (orientation == 1)
		{
			amot.set_command(config.kicks.KickSideRight); // HardRightSideKick  KickSideRightFast
			direction = +1;
		}
		else if (orientation == 2)
		{
			if (ball_y > 0.0)
				amot.set_command(config.kicks.KickSideLeft); // LeftBackHigh_carpet KickBackLeft KickBackLeftPierris
			else
				amot.set_command(config.kicks.KickSideRight); // RightBackHigh_carpet KickBackRight KickBackRightPierris
		}
		else
		{
			if (ball_y > 0.0)
				amot.set_command(config.kicks.KickSideLeft);
			else
				amot.set_command(config.kicks.KickSideRight);
		}

		_blk.publishSignal(amot, "motion");
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
	if ( (om != 0) && (config.playerNumber == 2) )
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
	wmot.set_command("setWalkTargetVelocity");

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

	wmot.set_parameter(0, cX);
	wmot.set_parameter(1, cY);
	wmot.set_parameter(2, ct);
	wmot.set_parameter(3, f);
	_blk.publishSignal(wmot, "motion");
	lastwalk = microsec_clock::universal_time();
}


void Behavior::littleWalk(double x, double y, double th)
{
	wmot.set_command("walkTo");
	wmot.set_parameter(0, x);
	wmot.set_parameter(1, y);
	wmot.set_parameter(2, th);
	_blk.publishSignal(wmot, "motion");
}

void Behavior::approachBall()
{

    double loppgb = anglediff2(atan2(config.oppGoalLeftY - robot_y, config.oppGoalLeftX - robot_x), robot_phi);
	double roppgb = anglediff2(atan2(config.oppGoalRightY - robot_y, config.oppGoalRightX - robot_x), robot_phi);
	double cone = anglediff2(loppgb, roppgb);
	double oppgb = wrapToPi(roppgb + cone / 2.0);

    std::cout << oppgb << "\n";
	if (ball_dist > 0.3){
        int pathSide = (ball_bearing > 0) ? 1 : -1;
//        pathPlanningRequestRelative(ball_x, ball_y, pathSide * M_PI_2);
//        velocityWalk(ball_x,ball_y,ball_bearing,1.0);
        pathPlanningRequestAbsolute(ball_x - config.posx, ball_y - side * config.posy, ball_bearing);
    }
    else if((ball_bearing > M_PI_4) || (ball_bearing < -M_PI_4)){
        littleWalk(0.0, 0.0, (float)(side*M_PI_4/2.0));
    }
    else if(oppgb > (float) (M_PI_4)){
        velocityWalk(0.0, -0.7, (float)(M_PI_4/2),1.0);
    }
    else if(oppgb < (float) (-M_PI_4)){
        velocityWalk(0.0, 0.7, (float)(-M_PI_4/2),1.0);
    }
    else{
        pathPlanningRequestAbsolute(ball_x - config.posx, ball_y - side * config.posy, ball_bearing);

    }
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
		if (ball_dist > 0.7)
		{
			int pathSide = (ball_bearing > 0) ? 1 : -1;
			pathPlanningRequestAbsolute(ball_x - config.posx, ball_y - side * config.posy, ball_bearing);
		}
		else
			stopRobot();
	}
}

void Behavior::stopRobot()
{
	//velocityWalk(0.0, 0.0, 0.0, 1.0);
	amot.set_command("InitPose.xar");
	_blk.publishSignal(amot, "motion");
}

void Behavior::pathPlanningRequestRelative(float target_x, float target_y, float target_phi)
{
	pprm.set_gotox(target_x);
	pprm.set_gotoy(target_y);
	pprm.set_gotoangle(target_phi);
	pprm.set_mode("relative");
	_blk.publishSignal(pprm, "obstacle");
}

void Behavior::pathPlanningRequestAbsolute(float target_x, float target_y, float target_phi)
{
	pprm.set_gotox(target_x);
	pprm.set_gotoy(target_y);
	pprm.set_gotoangle(target_phi);
	pprm.set_mode("absolute");
	_blk.publishSignal(pprm, "obstacle");
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

//TODO not working
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
				//fakeDir = anglediff2(atan2(fakeObstacles[j][1]-robot_y,fakeObstacles[j][0]-robot_x), robot_phi);
				fakeDir = 2 * M_PI - wrapTo0_2Pi(robot_phi) - atan2(fakeObstacles[j][1] - robot_y, fakeObstacles[j][0] - robot_x);
				fom.set_direction(fakeDir);
				fom.set_distance(fakeDist);
				fom.set_certainty(1);
				_blk.publishSignal(fom, "obstacle");
			}
		}
	}
}

/*------------------------------------ GOALIE FUNCTIONS -----------------------------------------*/

/**
 * Function used by goalie basic behavior to decide when to fall for a ball save and
 * when to go on the ball and kick it away from the goal post (TODO).
 */
void Behavior::Goalie()
{
	role = GOALIE;
	if(ballfound == 1) {

		fall = toFallOrNotToFall();

		if(fall == 1) // extend left foot
		{
			amot.set_command("goalieLeftFootExtened.xar");
			_blk.publishSignal(amot, "motion");	
			return;
		}
		else if(fall == -1) // extend right foot
		{
			amot.set_command("goalieRightFootExtened.xar");
			_blk.publishSignal(amot, "motion");
			return;
		}
		else // scan and track for ball
		{
			hcontrol.mutable_task()->set_action(HeadControlMessage::SCAN_AND_TRACK_FOR_BALL);
			_blk.publishState(hcontrol, "behavior");
		}

		if(ball_dist < 0.65) // check if ball is to close to the goal post
		{
			goalieApproachStarted = true;
			pathPlanningRequestAbsolute(ball_x - config.posx, ball_y - side * config.posy, ball_bearing);
			if ( (fabs(ball_x - config.posx) < config.epsx)  && (fabs( ball_y - (side * config.posy) ) < config.epsy) && (bmsg != 0) && (bmsg->radius() > 0) ) {
				if (ball_y > 0.0)
					amot.set_command(config.kicks.KickForwardLeft); // Left Kick
				else
					amot.set_command(config.kicks.KickForwardRight); // Right Kick

				_blk.publishSignal(amot, "motion");	
			}
		}else if(goalieApproachStarted == true){
			stopRobot();
			goalieApproachStarted = false;
		}

	}
	else if(ballfound == 0) {
		if(goalieApproachStarted == true){
			stopRobot();
			goalieApproachStarted = false;
		}
		hcontrol.mutable_task()->set_action(HeadControlMessage::SCAN_AND_TRACK_FOR_BALL);
		_blk.publishState(hcontrol, "behavior");
	}

	return;
}

/**
 * Function used by Goalie function above to determine when and where to extend robot foot
 * for a ball save.
 */
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
		dk = (ubx * y1 - uby * x1) / ubx ; // dk is the projection of the ball's route towards the robot/goalpost

		if(fabs(dk) <= 0.3) // if dk is shorter than the robot's foot can extend
		{
			ub = sqrt(ubx * ubx + uby * uby);

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

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
 * Behavior initialization function.
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
	ballFound = false;
	pathOK = true;
	kickOff = false;
	cX = 0.0;
	cY = 0.0;
	ct = 0.0;
	ballDist = 0.0;
	ballBearing = 0.0;
	goalieApproachStarted = false;
	ballX = 0.0;
	ballY = 0.0;
	side =+ 1;
	robotX = 0.0;
	robotY = 0.0;
	robotPhi = 0.0;
	robotConfidence = 1.0;
	readyToKick = false;
	scanAfterKick = false;
	direction = 1;
	orientation = 0;
	gameState = PLAYER_INITIAL;
	currentRobotAction = MotionStateMessage::IDLE;
	role = ATTACKER;
	Reset();
	Logger::Instance().WriteMsg("Behavior", "Initialized: My number is " + _toString(config.playerNumber) + " and my color is " + _toString(config.teamColor), Logger::Info);
	fGen.Init(config.maxPlayers);
	sendDebugMessages();
	srand(time(0));
	lastWalk = microsec_clock::universal_time();
	lastPlay = microsec_clock::universal_time();
	lastPenalised = microsec_clock::universal_time();
	lastFormation = microsec_clock::universal_time();
    hcontrol.mutable_task()->set_action(HeadControlMessage::FROWN);
    _blk.publishState(hcontrol, "behavior");
	// generateFakeObstacles();
}


/**
 * Reset function that reads all xml configuration data and update the config struct
 * of behavior. All new xml reads MUST be added here, dont make other functions!
 */
void Behavior::Reset(){

	// === read team configuration xml data from teamConfig.xml===
	config.teamNumber = atoi(_xml.findValueForKey("teamConfig.team_number").c_str());
	config.playerNumber = atoi(_xml.findValueForKey("teamConfig.player").c_str());
	config.maxPlayers = atoi(_xml.findValueForKey("teamConfig.team_max_players").c_str());
	config.isPenaltyMode = atoi(_xml.findValueForKey("teamConfig.penalty_mode").c_str()) == 1 ? true : false;

	std::string color;
	color = _xml.findValueForKey("teamConfig.default_team_color").c_str();
	if(color.compare("blue") == 0)
		config.teamColor = TEAM_BLUE;
	else
		config.teamColor == TEAM_RED;

	// === read behavior configuration xml data from behavior.xml===
	config.posX = atof(_xml.findValueForKey("behavior.posx").c_str());
	config.posY = atof(_xml.findValueForKey("behavior.posy").c_str());
	config.epsX = atof(_xml.findValueForKey("behavior.epsx").c_str());
	config.epsY = atof(_xml.findValueForKey("behavior.epsy").c_str());
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
			if( atoi(_xml.findValueForKey("playerConfig."+kickOff+".player~"+_toString(r)+".number").c_str()) == config.playerNumber )
			{
				config.initX[i] = atof(_xml.findValueForKey("playerConfig."+kickOff+".player~"+_toString(r)+".x").c_str());
				config.initY[i] = atof(_xml.findValueForKey("playerConfig."+kickOff+".player~"+_toString(r)+".y").c_str());
				config.initPhi[i] = TO_RAD(atof(_xml.findValueForKey("playerConfig."+kickOff+".player~"+_toString(r)+".phi").c_str()));
				break;
			}
		}
	}

	// === read field configuration xml data from field.xml used for formation generator ===
	// update the Field struct on formation generator header
	fGen.Field.MaxX = atof(_xml.findValueForKey("field.FieldMaxX").c_str());
	fGen.Field.MinX = atof(_xml.findValueForKey("field.FieldMinX").c_str());
	fGen.Field.MaxY = atof(_xml.findValueForKey("field.FieldMaxY").c_str());
	fGen.Field.MinY = atof(_xml.findValueForKey("field.FieldMinY").c_str());

	fGen.Field.LeftPenaltyAreaMaxX = atof(_xml.findValueForKey("field.LeftPenaltyAreaMaxX").c_str());
	fGen.Field.LeftPenaltyAreaMinX = atof(_xml.findValueForKey("field.LeftPenaltyAreaMinX").c_str());
	fGen.Field.LeftPenaltyAreaMaxY = atof(_xml.findValueForKey("field.LeftPenaltyAreaMaxY").c_str());
	fGen.Field.LeftPenaltyAreaMinY = atof(_xml.findValueForKey("field.LeftPenaltyAreaMinY").c_str());

	fGen.Field.RightPenaltyAreaMaxX = atof(_xml.findValueForKey("field.RightPenaltyAreaMaxX").c_str());
	fGen.Field.RightPenaltyAreaMinX = atof(_xml.findValueForKey("field.RightPenaltyAreaMinX").c_str());
	fGen.Field.RightPenaltyAreaMaxY = atof(_xml.findValueForKey("field.RightPenaltyAreaMaxY").c_str());
	fGen.Field.RightPenaltyAreaMinY = atof(_xml.findValueForKey("field.RightPenaltyAreaMinY").c_str());

	fGen.Field.LeftGoalPost = atof(_xml.findValueForKey("field.LeftGoalAreaMaxY").c_str());
	fGen.Field.RightGoalPost = atof(_xml.findValueForKey("field.LeftGoalAreaMinY").c_str());

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
	readMessages();
	getBallData();
	getGameState();
	getPosition();
	getMotionData();

    if (gameState == PLAYER_INITIAL){
		if(prevGameState != PLAYER_INITIAL){
        	hcontrol.mutable_task()->set_action(HeadControlMessage::FROWN);
        	_blk.publishState(hcontrol, "behavior");
		}
    }
	else if (gameState == PLAYER_PLAYING)
	{

		if(lastFormation + seconds(10) < microsec_clock::universal_time()) {

			fGen.Generate(globalBallX, globalBallY);
			if(!gameMode){
				sendDebugMessages();
			}
			lastFormation = microsec_clock::universal_time();
		}

		if(prevGameState == PLAYER_PENALISED){
			lastPenalised = microsec_clock::universal_time();
			//Check if the penalized was a wrong decision
			if(microsec_clock::universal_time() - penalisedStarted > seconds(10) || !gameMode){
				direction = 1;
				locReset.set_type(LocalizationResetMessage::PENALISED);
				locReset.set_kickoff(kickOff);
				_blk.publishSignal(locReset, "worldstate");
			}

			if(penaltyMode) {
				locReset.set_type(LocalizationResetMessage::PENALTY_MODE);
				_blk.publishSignal(locReset, "worldstate");
			}
		}

		if (prevGameState == PLAYER_SET)
		{
			lastPlay = microsec_clock::universal_time();
		}

		if(bfm != 0 && bfm.get() != 0) {
			ballFound = bfm->ballfound();
		}

		updateOrientation();
		readyToKick = false;

		if(config.playerNumber == 1 || role == GOALIE) { // goalie role if number 1
			goalie();
		}
		else { // not goalie behavior

			//TODO goalie must go to his positcurrention, not walk strait :P
			if (lastPenalised + seconds(4) > microsec_clock::universal_time())
			{
				hcontrol.mutable_task()->set_action(HeadControlMessage::LOCALIZE_FAR);
				_blk.publishState(hcontrol, "behavior");
				return 0;
			}

			// Publish message to head controller to run check for ball
			hcontrol.mutable_task()->set_action(HeadControlMessage::SMART_SELECT);
			_blk.publishState(hcontrol, "behavior");

			if (ballFound == 1)
			{
				side = (ballBearing > 0) ? 1 : -1;
				//posx = 0.1, posy = 0.03; // Desired ball position for kick
				//double epsx = 0.025, epsy = 0.025; // Desired precision

                double loppgb = anglediff2(atan2(config.oppGoalLeftY - robotY, config.oppGoalLeftX - robotX), robotPhi);
                double roppgb = anglediff2(atan2(config.oppGoalRightY - robotY, config.oppGoalRightX - robotX), robotPhi);
                double cone = anglediff2(loppgb, roppgb);
                double oppgb = wrapToPi(roppgb + cone / 2.0);

				if ( (fabs( ballX - config.posX ) < config.epsX)  && (fabs( ballY - (side * config.posY) ) < config.epsY) && (bmsg != 0) && (bmsg->radius() > 0) && (oppgb < M_PI_4) && (oppgb > -M_PI_4) )
				{
					readyToKick = true;
					scanAfterKick = true;
					kick();
					direction = (side == +1) ? -1 : +1;
					//hcontrol.mutable_task()->set_action(HeadControlMessage::SMART_SELECT);
					//_blk.publishState(hcontrol, "behavior");
				}

				if (!readyToKick)
				{
					//Define roles
					if(closestRobot())
					{
						role = ATTACKER;
					}
					else
					{
						role = CENTER_FOR;
					}
					//Logger::Instance().WriteMsg("BehaviorTest", "Role: " + _toString(role), Logger::Info);
					if(penaltyMode)
						pathPlanningRequestAbsolute(ballX - (config.posX + 0.025), ballY - side * config.posY, ballBearing);	// 2.5cm offset from the ball!
					else
						approachBallRoleDependent();
				}

			}
			if (ballFound == 0)
			{
				if(currentRobotAction == MotionStateMessage::WALKING && scanAfterKick == true) {
					scanAfterKick = false;
					stopRobot();
					hcontrol.mutable_task()->set_action(HeadControlMessage::SMART_SELECT);
					_blk.publishState(hcontrol, "behavior");
				}

				//walk straight for some seconds after the scan has ended (lastpenalised+seconds(12))
				//and then start turning around to search for ball.
				if (lastPenalised + seconds(14) > microsec_clock::universal_time())
				{
					//pathPlanningRequestAbsolute(0.2, 0.0, 0.0);
                    if(swim!=0 && swim.get()!=0 && swim->globalballs_size()>0)
					    goToPosition(SharedGlobalBallX, SharedGlobalBallY, 0.0);
                    else
                        pathPlanningRequestAbsolute(0.2, 0.0, 0.0);
				}
				/*
				else if ( (fabs(robotX) < 4.5f) && (fabs(robotY) < 3.0f) )
					pathPlanningRequestAbsolute(0.45, 0.45 * direction, M_PI_4 * direction);
				else
					pathPlanningRequestAbsolute(0.1, 0.1 * direction, M_PI_4 * direction);
					*/

                if(swim!=0 && swim.get()!=0 && swim->globalballs_size()>0)
                    goToPosition(SharedGlobalBallX, SharedGlobalBallY, 0.0);
                else if ( (fabs(robotX) < 4.5f) && (fabs(robotY) < 3.0f) )
					pathPlanningRequestAbsolute(0.45, 0.45 * direction, M_PI_4 * direction);
				else
					pathPlanningRequestAbsolute(0.1, 0.1 * direction, M_PI_4 * direction);
			}

		} // not goalie behavior end
	}
	else if (gameState == PLAYER_READY)
	{
	 	/*
	 	if (gameState != prevGameState)
		{
			if(prevGameState != PLAYER_PLAYING){
				locReset.set_type(LocalizationResetMessage::READY);
				locReset.set_kickoff(kickOff);
				_blk.publishSignal(locReset, "worldstate");
				stopRobot();
			}
			hcontrol.mutable_task()->set_action(HeadControlMessage::LOCALIZE);
			_blk.publishState(hcontrol, "behavior");
		}
		int p = (kickOff) ? 0 : 1;
		goToPosition(config.initX[p], config.initY[p], config.initPhi[p] );
		*/
		stopRobot();
		return 0;
	}
	else if (gameState == PLAYER_SET)
	{
		kickOff = gsm->kickoff();

		if (gameState != prevGameState){

			//Reset Loc
			locReset.set_type(LocalizationResetMessage::SET);
			locReset.set_kickoff(kickOff);
			_blk.publishSignal(locReset, "worldstate");

			stopRobot();

			hcontrol.mutable_task()->set_action(HeadControlMessage::SCAN_AND_TRACK_FOR_BALL);
			_blk.publishState(hcontrol, "behavior");
		}
	}
	else if(gameState == PLAYER_PENALISED)
	{
		kickOff = gsm->kickoff();
		if (gameState != prevGameState){
			penalisedStarted = microsec_clock::universal_time();
			hcontrol.mutable_task()->set_action(HeadControlMessage::FROWN);
			_blk.publishState(hcontrol, "behavior");
		}
	}

	return 0;
}


/**
 * Read incoming messages from declared topics. Use message objects to get the data.
 */
void Behavior::readMessages() {

	gsm  = _blk.readState<GameStateMessage> ("worldstate");
	bmsg = _blk.readSignal<BallTrackMessage> ("vision");
	allsm = _blk.readData<AllSensorValuesMessage> ("sensors");
	om   = _blk.readState<ObstacleMessageArray> ("obstacle");
	wim  = _blk.readData<WorldInfo> ("worldstate");
	swim = _blk.readData<SharedWorldInfo> ("worldstate");
	bfm = _blk.readState<BallFoundMessage> ("behavior");
	sm = _blk.readState<MotionStateMessage>("worldstate");
}

/* --------------------------------- Information gathering functions from messages ---------------------------------- */

void Behavior::getGameState() {

	if (gsm != 0) {
		prevGameState = gameState;
		gameState = gsm->player_state();
		config.teamColor = gsm->team_color();
		config.playerNumber = gsm->player_number();

		if( gsm->penalty() || config.isPenaltyMode )
			penaltyMode = true;
		else
			penaltyMode = false;
	}
}

void Behavior::getPosition() {

	if(wim != 0 && wim.get() != 0) {
		robotX = wim->myposition().x();
		robotY = wim->myposition().y();
		robotPhi = wrapToPi( wim->myposition().phi() );
		robotConfidence = wim->myposition().confidence();
	}
}

void Behavior::getBallData() {

	if(wim != 0 && wim.get() != 0 && wim->balls_size() > 0)
	{
		ballX = wim->balls(0).relativex() + wim->balls(0).relativexspeed() * 0.200;
		ballY = wim->balls(0).relativey() + wim->balls(0).relativeyspeed() * 0.200;
		ballDist = sqrt(pow(ballX, 2) + pow(ballY, 2));
		ballBearing = atan2(ballY, ballX);

		// global ball relative to robot
		globalBallX = (wim->myposition().x() + wim->balls (0).relativex() * cos(wim->myposition().phi()) - wim->balls (0).relativey() * sin(wim->myposition().phi()));
		globalBallY = (wim->myposition().y() + wim->balls (0).relativex() * sin(wim->myposition().phi()) + wim->balls (0).relativey() * cos(wim->myposition().phi()));
	}

	if(swim != 0 && swim.get() != 0 && swim->globalballs_size() > 0) {
		SharedGlobalBallX = swim->globalballs(0).x();
		SharedGlobalBallY = swim->globalballs(0).y();
	}
}

void Behavior::getMotionData() {

	if(sm != 0) {
		currentRobotAction = sm->type();
	}
}

void Behavior::sendDebugMessages() {

	Logger::Instance().WriteMsg("Behavior", "BallX: "+_toString(globalBallX)+" BallY: "+_toString(globalBallY), Logger::Info);

	for(unsigned int i = 0 ; i < fGen.getFormation()->size() ; i++) {

		Logger::Instance().WriteMsg("Behavior", "Role: "+_toString(fGen.getFormation()->at(i).role)+
											" X: "+_toString(fGen.getFormation()->at(i).X)+
											" Y: "+_toString(fGen.getFormation()->at(i).Y), Logger::Info);

		if(fdg.positions_size() < (int)(i+1))
			fdg.add_positions();
		fdg.mutable_positions(i)->set_x(fGen.getFormation()->at(i).X);
		fdg.mutable_positions(i)->set_y(fGen.getFormation()->at(i).Y);
		fdg.mutable_positions(i)->set_role(fGen.getFormation()->at(i).role);
	}
	_blk.publishSignal(fdg, "debug");
}

/* -------------------------------------------------------------------------------------------------------------------- */

bool Behavior::closestRobot() {

	if(swim != 0 && swim.get() != 0) {
        if(swim->playerclosesttoball()==config.playerNumber){
            //Logger::Instance().WriteMsg("Behavior", config.playerNumber, Logger::Info);
            return true;
        }
        else{
            return false;
        }
	}
	else
		return true;
}

void Behavior::updateOrientation() {

	double loppgb = anglediff2(atan2(config.oppGoalLeftY - robotY, config.oppGoalLeftX - robotX), robotPhi);
	double roppgb = anglediff2(atan2(config.oppGoalRightY - robotY, config.oppGoalRightX - robotX), robotPhi);
	double cone = anglediff2(loppgb, roppgb);
	double oppgb = wrapToPi(roppgb + cone / 2.0);

	if ( (oppgb <= M_PI_4) && (oppgb > -M_PI_4) ) {
		orientation = 0;
	}
	else if ( (oppgb > M_PI_4) && (oppgb <= (M_PI - M_PI_4) ) ) {
		orientation = 1;
	}
	else if ( (oppgb > (M_PI - M_PI_4) ) || (oppgb <= -(M_PI - M_PI_4) ) ) {
		orientation = 2;
	}
	else if ( (oppgb <= -M_PI_4 ) && (oppgb > -(M_PI - M_PI_4) ) ) {
		orientation = 3;
	}
}

/**
 * Function used for kicking the ball.
 */
void Behavior::kick()
{
	if ( kickOff && (microsec_clock::universal_time() <= lastPlay + seconds(0/*25*/)) ) {
		if (behaviorRand() < 0.75) {
			littleWalk(0.2, 0.0, 0.0);
		}
		else {
			if (side == 1)
				amot.set_command(config.kicks.KickSideLeft);
			else
				amot.set_command(config.kicks.KickSideRight);

			_blk.publishSignal(amot, "motion");
		}
	}
	else {

		if (orientation == 0) {
			if (ballY > 0.0)
				amot.set_command(config.kicks.KickForwardLeft); // Left Kick
			else
				amot.set_command(config.kicks.KickForwardRight); // Right Kick
		}
		else if (orientation == 3) {
			amot.set_command(config.kicks.KickSideLeft); //  HardLeftSideKick KickSideLeftFast
			direction = -1;
		}
		else if (orientation == 1) {
			amot.set_command(config.kicks.KickSideRight); // HardRightSideKick KickSideRightFast
			direction = +1;
		}
		else if (orientation == 2) {
			if (ballY > 0.0)
				amot.set_command(config.kicks.KickSideLeft); // LeftBackHigh_carpet KickBackLeft KickBackLeftPierris
			else
				amot.set_command(config.kicks.KickSideRight); // RightBackHigh_carpet KickBackRight KickBackRightPierris
		}
		else {
			if (ballY > 0.0)
				amot.set_command(config.kicks.KickSideLeft);
			else
				amot.set_command(config.kicks.KickSideRight);
		}

		_blk.publishSignal(amot, "motion");
	}
}

/**
 * Locomotion Functions
 */
void Behavior::velocityWalk(double ix, double iy, double it, double f) {

	double x, y, t;
	x = ix;
	y = iy;
	t = it;

	// BEGIN - Basic Obstacle Avoidance Code
	if ( (om != 0) && (config.playerNumber == 2) ) {
		if ( (om->distance(2) <= 0.4) && (om->distance(0) <= 0.4) ) {
			if (x > 0.0) {
				x = 0.0;
				t = 0.0;
			}
		}
		else if (om->distance(2) <= 0.4) {
			if (x > 0.0) {
				x = 0.0;
				y = 0.5;
				t = 0.0;
			}
		}
		else if (om->distance(0) <= 0.4) {
			if (x > 0.0) {
				x = 0.0;
				y = -0.5;
				t = 0.0;
			}
		}
	}

	// END - Basic Obstacle Avoidance Code
	wmot.set_command("setWalkTargetVelocity");

	if ( (x == 0.0) && (y == 0.0) && (t == 0.0) ) {
		cX = 0.0;
		cY = 0.0;
		ct = 0.0;
	}
	else {
		if( lastWalk + milliseconds(200) > microsec_clock::universal_time() )
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
	lastWalk = microsec_clock::universal_time();
}


void Behavior::littleWalk(double x, double y, double th) {

	wmot.set_command("walkTo");
	wmot.set_parameter(0, x);
	wmot.set_parameter(1, y);
	wmot.set_parameter(2, th);
	_blk.publishSignal(wmot, "motion");
}

void Behavior::approachBall() {

    double loppgb = anglediff2(atan2(config.oppGoalLeftY - robotY, config.oppGoalLeftX - robotX), robotPhi);
	double roppgb = anglediff2(atan2(config.oppGoalRightY - robotY, config.oppGoalRightX - robotX), robotPhi);
	double cone = anglediff2(loppgb, roppgb);
	double oppgb = wrapToPi(roppgb + cone / 2.0);

	if (ballDist > 0.3) {
        int pathSide = (ballBearing > 0) ? 1 : -1;
		// pathPlanningRequestRelative(ball_x, ball_y, pathSide * M_PI_2);
		// velocityWalk(ball_x,ball_y,ball_bearing,1.0);
        pathPlanningRequestAbsolute(ballX - config.posX, ballY - side * config.posY, ballBearing);
    }
//    else if((ballBearing > M_PI_4/2.0f) || (ballBearing < -M_PI_4/2.0f)) {
//        littleWalk(0.0, 0.0, (float)(-side*M_PI_4/2.0));
//    }
    else if(oppgb > (float) (M_PI_4)) {
        velocityWalk(0.0, -0.7, (float)(M_PI_4/2),1.0);
    }
    else if(oppgb < (float) (-M_PI_4)) {
        velocityWalk(0.0, 0.7, (float)(-M_PI_4/2),1.0);
    }
    else{
		pathPlanningRequestAbsolute(ballX - (config.posX + 0.025), ballY - side * config.posY, ballBearing); // 2.5cm offset from the ball!
    }
}

void Behavior::approachBallRoleDependent() {

    if (orientation == 1)
		side = -1;
	else if (orientation == 3)
		side = +1;

	if(role == ATTACKER) {
		approachBall();
	}
	else if(role == CENTER_FOR) {
		if (ballDist > 0.7) {
			int pathSide = (ballBearing > 0) ? 1 : -1;
			pathPlanningRequestAbsolute(ballX - config.posX, ballY - side * config.posY, ballBearing);
		}
		else
			stopRobot();
	}
}

void Behavior::stopRobot()
{
	// velocityWalk(0.0, 0.0, 0.0, 1.0);
	amot.set_command("InitPose.xar");
	_blk.publishSignal(amot, "motion");
}

void Behavior::pathPlanningRequestRelative(float targetX, float targetY, float targetPhi) {

	pprm.set_gotox(targetX);
	pprm.set_gotoy(targetY);
	pprm.set_gotoangle(targetPhi);
	pprm.set_mode("relative");
	_blk.publishSignal(pprm, "obstacle");
}

void Behavior::pathPlanningRequestAbsolute(float targetX, float targetY, float targetPhi) {

	pprm.set_gotox(targetX);
	pprm.set_gotoy(targetY);
	pprm.set_gotoangle(targetPhi);
	pprm.set_mode("absolute");
	_blk.publishSignal(pprm, "obstacle");
}

void Behavior::goToPosition(float targetX, float targetY, float targetPhi) {

	double targetDistance = sqrt((targetX - robotX) * (targetX - robotX) + (targetY - robotY) * (targetY - robotY));
	double targetAngle = anglediff2(atan2(targetY - robotY, targetX - robotX), robotPhi);
	double targetOrientation = anglediff2(targetPhi, robotPhi);

	if ( (targetDistance > 0.25) )//|| (fabs(targetOrientation) > M_PI_4) )
		pathPlanningRequestAbsolute(toCartesianX(targetDistance, targetAngle),
		                            toCartesianY(targetDistance, targetAngle),
		                            targetOrientation);
	else
		stopRobot();
}


void Behavior::generateFakeObstacles() {

	float tmpX = -3.0 + ObstacleRadius, tmpY = -1.1 + ObstacleRadius;

	for(int j = 0; j < numOfFakeObstacles; j++) {
		fakeObstacles[j][0] = INIT_VALUE;
		fakeObstacles[j][1] = INIT_VALUE;
	}

	int i = 0;

	while(tmpX < -2.4) {
		fakeObstacles[i][0] = tmpX;
		fakeObstacles[i][1] = tmpY;
		fakeObstacles[i + 1][0] = tmpX;
		fakeObstacles[i + 1][1] = -tmpY;
		tmpX += 2 * ObstacleRadius;
		i += 2;
	}

	tmpX = -2.4 + ObstacleRadius;

	while(tmpY < 1.1) {
		fakeObstacles[i][0] = tmpX;
		fakeObstacles[i][1] = tmpY;
		tmpY += 2 * ObstacleRadius + 0.02;
		i++;
	}
}

// TODO not working
void Behavior::checkForPenaltyArea()
{
	float fakeDist = 0.0, fakeDir = 0.0;

	for(int j = 0; j < numOfFakeObstacles; j++) {
		if(fakeObstacles[j][0] == INIT_VALUE)
			continue;
		else {
			fakeDist = DISTANCE(robotX, robotY, fakeObstacles[j][0], fakeObstacles[j][1]);

			if(fakeDist < MapRadius) {
				//send fake obstacle message
				//fakeDir = anglediff2(atan2(fakeObstacles[j][1]-robot_y,fakeObstacles[j][0]-robot_x), robot_phi);
				fakeDir = 2 * M_PI - wrapTo0_2Pi(robotPhi) - atan2(fakeObstacles[j][1] - robotY, fakeObstacles[j][0] - robotX);
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
void Behavior::goalie() {

	role = GOALIE;
	if(ballFound == 1) {

		fall = toFallOrNotToFall();

		if(fall == 1) { // extend left foot
			amot.set_command("goalieLeftFootExtened.xar");
			_blk.publishSignal(amot, "motion");
			return;
		}
		else if(fall == -1) { // extend right foot
			amot.set_command("goalieRightFootExtened.xar");
			_blk.publishSignal(amot, "motion");
			return;
		}
		else { // scan and track for ball
			hcontrol.mutable_task()->set_action(HeadControlMessage::SCAN_AND_TRACK_FOR_BALL);
			_blk.publishState(hcontrol, "behavior");
		}

		if(ballDist < 0.65) { // check if ball is to close to the goal post
			goalieApproachStarted = true;
			pathPlanningRequestAbsolute(ballX - config.posX, ballY - side * config.posY, ballBearing);
			if ( (fabs(ballX - config.posX) < config.epsX)  && (fabs( ballY - (side * config.posY) ) < config.epsY) && (bmsg != 0) && (bmsg->radius() > 0) ) {
				if (ballY > 0.0)
					amot.set_command(config.kicks.KickForwardLeft); // Left Kick
				else
					amot.set_command(config.kicks.KickForwardRight); // Right Kick

				_blk.publishSignal(amot, "motion");
			}
		}
		else if(goalieApproachStarted == true) {
			stopRobot();
			goalieApproachStarted = false;
		}

	}
	else if(ballFound == 0) {
		if(goalieApproachStarted == true) {
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
int Behavior::toFallOrNotToFall() {

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

	if(ubx < 0.0) {
		dk = (ubx * y1 - uby * x1) / ubx ; // dk is the projection of the ball's route towards the robot/goalpost

		if(fabs(dk) <= 0.3) { // if dk is shorter than the robot's foot can extend

			ub = sqrt(ubx * ubx + uby * uby);

			if(fabs(ub) > config.ur) {
				if(dk > 0)
					return 1;  // left
				else
					return -1;  // right
			}
		}
	}

	return 0;
}

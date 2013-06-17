#include "Behavior.h"

#include <math.h>

using namespace KMath;
using namespace KStandard;
using namespace boost::posix_time;
using namespace std;
using namespace FormationParameters;
using namespace Utility;
using KMath::Specific::permutationsOfCombinations;

ACTIVITY_REGISTER(Behavior);

/**
 *	Returns a double random number (0 - 100.0)
 */
double behaviorRand() {
	return (rand() % 100) / 100.0;
}


/**
 * Behavior initialization function.
 */
void Behavior::UserInit() {
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
	sharedBallFound = false;
	pathOK = true;
	kickOff = false;
	cX = 0.0;
	cY = 0.0;
	ct = 0.0;
	count = 0;
	ballDist = 0.0;
	ballBearing = 0.0;
	goalieApproachStarted = false;
	formationFlag = false;
	ballX = 0.0;
	ballY = 0.0;
	relativeBallX = 0.0;
	relativeBallY = 0.0;
	side =+ 1;
	robotX = 0.0;
	robotY = 0.0;
	robotPhi = 0.0;
	robotConfidence = 1.0;
	readyToKick = false;
	scanAfterKick = false;
	goToPositionFlag = true;
	direction = 1;
	orientation = 0;
	numOfRobots = 0;
	gameState = PLAYER_INITIAL;
	currentRobotAction = MotionStateMessage::IDLE;
	Reset();
	Logger::Instance().WriteMsg("Behavior", "Initialized: My number is " + _toString(config.playerNumber) + " and my color is " + _toString(config.teamColor), Logger::Info);
	srand(time(0));
	lastWalk = microsec_clock::universal_time();
	lastPlay = microsec_clock::universal_time();
	lastPenalised = microsec_clock::universal_time();
	lastFormation = microsec_clock::universal_time();

	dispTimer = microsec_clock::universal_time();
	sharedBallTimer = microsec_clock::universal_time();

	lastGoToCenter = microsec_clock::universal_time() - seconds(10);
	lastBallFound = microsec_clock::universal_time() - seconds(20);

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
	config.teamNumber = atoi(Configurator::Instance().findValueForKey("teamConfig.team_number").c_str());
	config.playerNumber = atoi(Configurator::Instance().findValueForKey("teamConfig.player").c_str());
	config.maxPlayers = atoi(Configurator::Instance().findValueForKey("teamConfig.team_max_players").c_str());
	config.isPenaltyMode = atoi(Configurator::Instance().findValueForKey("teamConfig.penalty_mode").c_str()) == 1 ? true : false;

	std::string color;
	color = Configurator::Instance().findValueForKey("teamConfig.default_team_color").c_str();
	if(color.compare("blue") == 0)
		config.teamColor = TEAM_BLUE;
	else
		config.teamColor = TEAM_RED;

	// === read behavior configuration xml data from behavior.xml===
	config.posX = atof(Configurator::Instance().findValueForKey("behavior.posx").c_str());
	config.posY = atof(Configurator::Instance().findValueForKey("behavior.posy").c_str());
	config.epsX = atof(Configurator::Instance().findValueForKey("behavior.epsx").c_str());
	config.epsY = atof(Configurator::Instance().findValueForKey("behavior.epsy").c_str());
	config.kicks.KickForwardLeft = Configurator::Instance().findValueForKey("behavior.KickForwardLeft").c_str();
	config.kicks.KickForwardRight = Configurator::Instance().findValueForKey("behavior.KickForwardRight").c_str();
	config.kicks.KickSideLeft = Configurator::Instance().findValueForKey("behavior.KickSideLeft").c_str();
	config.kicks.KickSideRight = Configurator::Instance().findValueForKey("behavior.KickSideRight").c_str();
	config.kicks.KickBackLeft = Configurator::Instance().findValueForKey("behavior.KickBackLeft").c_str();
	config.kicks.KickBackRight = Configurator::Instance().findValueForKey("behavior.KickBackRight").c_str();
	config.ur = atof(Configurator::Instance().findValueForKey("behavior.ur").c_str());

	// === read robot configuration xml data from playerConfig.xml===
	if ( (config.playerNumber < 1) || (config.playerNumber > config.maxPlayers) )
		Logger::Instance().WriteMsg("Behavior", "Behavior Reset: Invalid player number", Logger::Error);

	// === read field configuration xml data from field.xml used for formation generator ===
	// update the Field struct on formation generator header
	fGen.Field.MaxX = atof(Configurator::Instance().findValueForKey("field.FieldMaxX").c_str());
	fGen.Field.MinX = atof(Configurator::Instance().findValueForKey("field.FieldMinX").c_str());
	fGen.Field.MaxY = atof(Configurator::Instance().findValueForKey("field.FieldMaxY").c_str());
	fGen.Field.MinY = atof(Configurator::Instance().findValueForKey("field.FieldMinY").c_str());

	fGen.Field.LeftPenaltyAreaMaxX = atof(Configurator::Instance().findValueForKey("field.LeftPenaltyAreaMaxX").c_str());
	fGen.Field.LeftPenaltyAreaMinX = atof(Configurator::Instance().findValueForKey("field.LeftPenaltyAreaMinX").c_str());
	fGen.Field.LeftPenaltyAreaMaxY = atof(Configurator::Instance().findValueForKey("field.LeftPenaltyAreaMaxY").c_str());
	fGen.Field.LeftPenaltyAreaMinY = atof(Configurator::Instance().findValueForKey("field.LeftPenaltyAreaMinY").c_str());

	fGen.Field.RightPenaltyAreaMaxX = atof(Configurator::Instance().findValueForKey("field.RightPenaltyAreaMaxX").c_str());
	fGen.Field.RightPenaltyAreaMinX = atof(Configurator::Instance().findValueForKey("field.RightPenaltyAreaMinX").c_str());
	fGen.Field.RightPenaltyAreaMaxY = atof(Configurator::Instance().findValueForKey("field.RightPenaltyAreaMaxY").c_str());
	fGen.Field.RightPenaltyAreaMinY = atof(Configurator::Instance().findValueForKey("field.RightPenaltyAreaMinY").c_str());

	fGen.Field.LeftGoalPost = atof(Configurator::Instance().findValueForKey("field.LeftGoalAreaMaxY").c_str());
	fGen.Field.RightGoalPost = atof(Configurator::Instance().findValueForKey("field.LeftGoalAreaMinY").c_str());

	fGen.Field.DiameterCCircle = atof(Configurator::Instance().findValueForKey("field.DiameterCCircle").c_str());

	// === read goal configuration xml data from Fearures.xml ===
	std::string ID;
	for(int v = 0 ; v < Configurator::Instance().numberOfNodesForKey("features.ftr") ; v++)
	{
		ID = Configurator::Instance().findValueForKey("features.ftr~"+_toString(v)+".ID").c_str();
		if(ID == "YellowGoal")
		{
			config.oppGoalX = atof(Configurator::Instance().findValueForKey("features.ftr~"+_toString(v)+".x").c_str());
			config.oppGoalY = atof(Configurator::Instance().findValueForKey("features.ftr~"+_toString(v)+".y").c_str());
			config.ownGoalX = -config.oppGoalX;
			config.ownGoalY = -config.oppGoalY;
		}
		else if(ID == "YellowLeft")
		{
			config.oppGoalLeftX = atof(Configurator::Instance().findValueForKey("features.ftr~"+_toString(v)+".x").c_str());
			config.oppGoalLeftY = atof(Configurator::Instance().findValueForKey("features.ftr~"+_toString(v)+".y").c_str());
			config.ownGoalLeftX = -config.oppGoalLeftX;
			config.ownGoalLeftY = -config.oppGoalLeftY;
		}
		else if(ID == "YellowRight")
		{
			config.oppGoalRightX = atof(Configurator::Instance().findValueForKey("features.ftr~"+_toString(v)+".x").c_str());
			config.oppGoalRightY = atof(Configurator::Instance().findValueForKey("features.ftr~"+_toString(v)+".y").c_str());
			config.ownGoalRightX = -config.oppGoalRightX;
			config.ownGoalRightY = -config.oppGoalRightY;
		}
	}
	gameMode = atoi(Configurator::Instance().findValueForKey("teamConfig.game_mode").c_str()) == 1 ? true : false;
	Logger::Instance().WriteMsg("Behavior", "Reset done", Logger::Info);
}


void Behavior::generateFakeBalls() {

	if(sharedBallTimer + seconds(5) < microsec_clock::universal_time() && microsec_clock::universal_time() < sharedBallTimer + seconds(6)) {
		SharedGlobalBallX = 0.639;
		SharedGlobalBallY = -1.224;
		sharedBallFound = true;
	}
	else if(sharedBallTimer + seconds(10) < microsec_clock::universal_time() && microsec_clock::universal_time() < sharedBallTimer + seconds(11)) {
		SharedGlobalBallX = 2.089;
		SharedGlobalBallY = -0.150;
		sharedBallFound = true;
	}
	else if(sharedBallTimer + seconds(20) < microsec_clock::universal_time() && microsec_clock::universal_time() < sharedBallTimer + seconds(21)) {
		SharedGlobalBallX = -1.219;
		SharedGlobalBallY = 1.255;
		sharedBallFound = true;
	}
	else if(sharedBallTimer + seconds(30) < microsec_clock::universal_time() && microsec_clock::universal_time() < sharedBallTimer + seconds(31)) {
		SharedGlobalBallX = -1.083;
		SharedGlobalBallY = 0.055;
		sharedBallFound = true;
	}
	else {
		sharedBallFound = false;
	}
}

void Behavior::generateFakeRobots() {

	numOfRobots = robots.size();
	std::cout<<_toString(numOfRobots)<<std::endl;
	for(unsigned int r = 0 ; r < robots.size() ; r++) {
		robots.at(r).robotId = r + 1;
		if(r == 0) {
			robots.at(r).robotX = -2.8f;
			robots.at(r).robotY = 0.0f;
			robots.at(r).robotPhi = M_PI/6.0f;
			robots.at(r).robotStability = 0;
		}
		else if(r == 1) {
			robots.at(r).robotX = 2.0f;
			robots.at(r).robotY = 0.5f;
			robots.at(r).robotPhi = M_PI_2;
			robots.at(r).robotStability = 0;
		}
		else if(r == 2) {
			robots.at(r).robotX = 1.0f;
			robots.at(r).robotY = 0.5;
			robots.at(r).robotPhi = (7.0f*M_PI)/4.0f;
			robots.at(r).robotStability = 0;
		}
		else if(r == 3) {
			robots.at(r).robotX = 2.5f;
			robots.at(r).robotY = 1.0f;
			robots.at(r).robotPhi = (5.0f*M_PI)/4.0f;
			robots.at(r).robotStability = 0;
		}
		else if(r == 4) {
			robots.at(r).robotX = 1.5f;
			robots.at(r).robotY = 1.2f;
			robots.at(r).robotPhi = (3.0f*M_PI)/2.0f;
			robots.at(r).robotStability = 0;
		}
	}
	//sortRobotsbyId(robots);
}

/* Behavior Main Execution Function */

int Behavior::Execute() {

	readMessages();
	getBallData();
	getGameState();
	getPosition();
	getMotionData();
	getTeamPositions();

	/*
	if(dispTimer + seconds(10) < microsec_clock::universal_time()) {
		std::cout << "ROBOTS: "+_toString(numOfRobots) << std::endl;
		std::cout << "==========================================" << std::endl;
		for(int i = 0 ; i < robots.size() ; i++) {
			std::cout << "Id: "+_toString(robots[i].robotId) << std::endl;
			std::cout << "X: "+_toString(robots[i].robotX) << std::endl;
			std::cout << "Y: "+_toString(robots[i].robotY) << std::endl;
			std::cout << "Phi: "+_toString(robots[i].robotPhi) << std::endl;
			std::cout << "Stab: "+_toString(robots[i].robotStability) << std::endl;
			std::cout << "==========================================" << std::endl;
		}
		dispTimer = microsec_clock::universal_time();
	}
	*/
    if (gameState == PLAYER_INITIAL) {
		if(prevGameState != PLAYER_INITIAL) {
        	hcontrol.mutable_task()->set_action(HeadControlMessage::FROWN);
        	_blk.publishState(hcontrol, "behavior");
		}
    }
	else if (gameState == PLAYER_PLAYING) {

		if(prevGameState == PLAYER_PENALISED) {
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

		if (prevGameState == PLAYER_SET) {
			lastPlay = microsec_clock::universal_time();
		}

		if(bfm != 0 && bfm.get() != 0) {
			ballFound = bfm->ballfound();
		}

		updateOrientation();
		readyToKick = false;
			
		if(sharedBallFound == true) {
			if( (gsm != 0 && gsm.get() != 0 && gsm->secs_remaining()%10 == 0) || (lastFormation + seconds(10) < microsec_clock::universal_time()) ) {
				
				if(gsm!=0)
					std::cout << "SECS REMAINING: " << _toString(gsm->secs_remaining()) << std::endl;
					
				std::cout << "TIME: "+_toString(count++) << std::endl;

				fGen.Generate(SharedGlobalBallX, SharedGlobalBallY, true); // if shared world ball does not exist??? TODO
				if(!gameMode){
					sendDebugMessages();
				}
				lastFormation = microsec_clock::universal_time();

				if(config.playerNumber != 1)
					Coordinate();

				goToPositionFlag = false;
			}
		}

		if (lastPenalised + seconds(4) > microsec_clock::universal_time()) {
			hcontrol.mutable_task()->set_action(HeadControlMessage::LOCALIZE_FAR);
			_blk.publishState(hcontrol, "behavior");
			return 0;
		}

		// Publish message to head controller to run check for ball
		hcontrol.mutable_task()->set_action(HeadControlMessage::SMART_SELECT);
		_blk.publishState(hcontrol, "behavior");
		
		if(config.playerNumber == 1) { // goalie role if number 1
			if(formationFlag == true) {
				if(goToPosition(currentRole.X, currentRole.Y, 0.0) == false)
					return 0;
				else
					formationFlag = false;
			}
			goalie();
		}
		else { // not goalie behavior

			if(currentRole.role == FormationParameters::ONBALL) {

				if(goToPositionFlag == false && ballFound == 0) {
					if(goToPosition(currentRole.X, currentRole.Y, 0.0) == false) {
						return 0;
					}
					else
						goToPositionFlag = true;
				}
				else if(ballFound == 1) {

		            lastBallFound = microsec_clock::universal_time();
					side = (ballBearing > 0) ? 1 : -1;

		            double loppgb = anglediff2(atan2(config.oppGoalLeftY - robotY, config.oppGoalLeftX - robotX), robotPhi);
		            double roppgb = anglediff2(atan2(config.oppGoalRightY - robotY, config.oppGoalRightX - robotX), robotPhi);
		            double cone = anglediff2(loppgb, roppgb);
		            double oppgb = wrapToPi(roppgb + cone / 2.0);

					if(fabs(ballX - config.posX) < config.epsX && fabs(ballY - (side*config.posY)) < config.epsY && bmsg != 0 && bmsg->radius() > 0 && oppgb < M_PI_4 &&
						oppgb > -M_PI_4) {

						readyToKick = true;
						scanAfterKick = true;
						kick();
						direction = (side == +1) ? -1 : +1;
					}

					if(!readyToKick) {
						if(penaltyMode)
							pathPlanningRequestAbsolute(ballX - (config.posX + 0.025), ballY - side * config.posY, ballBearing);	// 2.5cm offset from the ball!
						else
							approachBall();
					}
				}
				else if(ballFound == 0) {

					if(currentRobotAction == MotionStateMessage::WALKING && scanAfterKick == true) {
						scanAfterKick = false;
						stopRobot();
						hcontrol.mutable_task()->set_action(HeadControlMessage::SMART_SELECT);
						_blk.publishState(hcontrol, "behavior");
					}

					// walk straight for some seconds after the scan has ended and then start turning around to search for ball.
					if(lastPenalised + seconds(20) > microsec_clock::universal_time()) {
                    	pathPlanningRequestAbsolute(3.0, 0.0, 0.0);
					}
					else {
		            	if(sharedBallFound == true)
		                	goToPosition(SharedGlobalBallX, SharedGlobalBallY, 0.0);
						else {
							if(lastGoToCenter + seconds(10) > microsec_clock::universal_time()) {
		                            if(robotX < 0.0)
		                                goToPosition(0.0, 0.0, 0.0);
		                            else
		                                goToPosition(fGen.Field.MaxX/2.0f, 0.0, 0.0);
							}
		                    else {
		                        if(searchFlag) {
		                            lastBallFound = microsec_clock::universal_time();
		                            searchFlag = false;
		                        }

		                        if (lastBallFound + seconds(20) > microsec_clock::universal_time())
		                            littleWalk(0.0, 0.0, (float)(-direction*M_PI_4/2.0));
		                        else{
		                            lastGoToCenter = microsec_clock::universal_time();
		                            searchFlag = true;
		                        }
		                    }
						}
					}
				}

			}
			else { // role is not attacker

				if(goToPositionFlag == false) {
					if(goToPosition(currentRole.X, currentRole.Y, 0.0) == false) {
						return 0;
					}
					else
						goToPositionFlag = true;
				}
				else if(ballFound == 1) {
					direction = (ballBearing > 0) ? 1 : -1;
					if(fabs(ballBearing) > M_PI/6)
						littleWalk(0.0, 0.0, ballBearing);
				}
				else if(sharedBallFound == 1) {
					direction = (SharedBallBearing > 0) ? 1 : -1;
					if(fabs(SharedBallBearing) > M_PI/6)
						littleWalk(0.0, 0.0, SharedBallBearing);
				}
				else if(ballFound == 0 && sharedBallFound == 0) {
					littleWalk(0.0, 0.0, (float)(-direction*M_PI_4/2.0));
				}
			}

		} // not goalie behavior end
	}
	else if (gameState == PLAYER_READY) {
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
	else if (gameState == PLAYER_SET) {

		kickOff = gsm->kickoff();

		if (gameState != prevGameState) {

			//Reset Loc
			locReset.set_type(LocalizationResetMessage::SET);
			locReset.set_kickoff(kickOff);
			_blk.publishSignal(locReset, "worldstate");

			stopRobot();

			hcontrol.mutable_task()->set_action(HeadControlMessage::SCAN_AND_TRACK_FOR_BALL);
			_blk.publishState(hcontrol, "behavior");
		}
	}
	else if(gameState == PLAYER_PENALISED) {

		kickOff = gsm->kickoff();

		if(prevGameState == PLAYER_INITIAL) {
			std::cout << "INITIAL FORMATION CALCULATED!" << std::endl;
			fGen.Init(config.maxPlayers, true);
			sendDebugMessages();
			currentRole = fGen.getFormation()->at(config.playerNumber - 1);
			//std::cout << "I CHOOSE TO BE: " << getRoleString(currentRole.role) << std::endl;
			lastFormation = microsec_clock::universal_time();
			formationFlag = true;
			goToPositionFlag = true;
		}

		if (gameState != prevGameState) {
			penalisedStarted = microsec_clock::universal_time();
			hcontrol.mutable_task()->set_action(HeadControlMessage::FROWN);
			_blk.publishState(hcontrol, "behavior");
		}
	}

	return 0;
}

void Behavior::Coordinate() {

//		fGen.Generate(SharedGlobalBallX, SharedGlobalBallY, true); // if shared world ball does not exist??? TODO
//		if(!gameMode){
//			sendDebugMessages();
//		}
//		lastFormation = microsec_clock::universal_time();

		for(unsigned int i = 0 ; i < fGen.getFormation()->size() ; i++) {
			if(fGen.getFormation()->at(i).role != FormationParameters::GOALIE)
				roles.insert(roles.end(), fGen.getFormation()->at(i).role);
		}

		//print(roles, "Behavior");
		mappings = permutationsOfCombinations(roles, numOfRobots);
		//std::cout << "ALL POSSIBLE MAPPINGS ARE: " << std::endl;
		//print(mappings, "Behavior");
		roles.clear();

		// search for optimal mapping
		maxU = 0;
		for(unsigned int map = 0 ; map < mappings.size() ; map++) {
			mapCost = 0;
			for(unsigned int r = 0 ; r < numOfRobots ; r++) { // for all except goalie robots
				currentRobotPos = fGen.findRoleInfo(mappings[map].at(r));
				mapCost = mapCost + fieldUtility(currentRobotPos.X, currentRobotPos.Y, SharedGlobalBallY, fGen, fGen.getFormationType()) -

						distance(robots[r].robotX, robots[r].robotY, currentRobotPos.X, currentRobotPos.Y, fGen.Field.MaxX, fGen.Field.MaxY) -

				  		minRotation(robots[r].robotX, robots[r].robotY, currentRobotPos.X, currentRobotPos.Y, robots[r].robotPhi) -

				  		collisions(mappings[map], robots, fGen, r, robots[r].robotX, robots[r].robotY);

				if(currentRobotPos.role == FormationParameters::ONBALL) {
					mapCost = mapCost - robotStability(robots[r].robotStability);
				}
			}

			if(Max(mapCost, maxU) == mapCost) {
				maxU = mapCost;
				index = map;
			}
			//std::cout << "MAPPING: ";
			//print(mappings[map], "Behavior");
			//std::cout << "COST: " << _toString(mapCost) << std::endl;
		}

		currentRole = fGen.findRoleInfo(mappings[index][getRobotIndex(robots, config.playerNumber)]);
		std::cout << "OPTIMAL MAP IS: ";
		print(mappings[index], "Behavior");
		std::cout << "MY OPTIMAL ROLE IS: " << getRoleString(currentRole.role) << std::endl;
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

		// check for penalty mode
		if( gsm->penalty() || config.isPenaltyMode )
			penaltyMode = true;
		else
			penaltyMode = false;
	}
}

// MAYBE WE DONT NEED IT ANYMORE!
void Behavior::getPosition() {

	if(wim != 0 && wim.get() != 0) {
		robotX = wim->myposition().x();
		robotY = wim->myposition().y();
		robotPhi = wrapToPi( wim->myposition().phi() );
	}
}

void Behavior::getTeamPositions() {

	if(swim != 0 && swim.get() != 0 && swim->teammateposition_size() > 0) {
		robots.clear(); // MAYBE NOT HERE!!
		robots.resize(swim->teammateposition_size());
		for(int i = 0 ; i < swim->teammateposition_size() ; i++) {
			robots[i].robotId = swim->teammateposition(i).robotid();
			robots[i].robotX = swim->teammateposition(i).pose().x();
			robots[i].robotY = swim->teammateposition(i).pose().y();
			robots[i].robotPhi = wrapToPi(swim->teammateposition(i).pose().phi());
			robots[i].robotStability = swim->teammateposition(i).stability();
		}
		sortRobotsbyId(robots); // sorting robots by player number
		if(robots[0].robotId == 1)
			robots.erase(robots.begin());
		numOfRobots = robots.size();
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
		relativeBallX = (wim->myposition().x() + wim->balls (0).relativex() * cos(wim->myposition().phi()) - wim->balls (0).relativey() * sin(wim->myposition().phi()));
		relativeBallY = (wim->myposition().y() + wim->balls (0).relativex() * sin(wim->myposition().phi()) + wim->balls (0).relativey() * cos(wim->myposition().phi()));
	}

	if(swim != 0 && swim.get() != 0 && swim->globalballs_size() > 0) {
		SharedGlobalBallX = swim->globalballs(0).x();
		SharedGlobalBallY = swim->globalballs(0).y();
		SharedBallBearing = atan2(SharedGlobalBallY, SharedGlobalBallX);
		sharedBallFound = true;
	}
	else
		sharedBallFound = false;
}

void Behavior::getMotionData() {

	if(sm != 0) {
		currentRobotAction = sm->type();
	}
}

void Behavior::sendDebugMessages() {

	Logger::Instance().WriteMsg("Behavior", "BallX: "+_toString(SharedGlobalBallX)+" BallY: "+_toString(SharedGlobalBallY), Logger::Info);

	for(unsigned int i = 0 ; i < fGen.getFormation()->size() ; i++) {

		Logger::Instance().WriteMsg("Behavior", "Role: "+_toString(getRoleString(fGen.getFormation()->at(i).role))+
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
        if(swim->playerclosesttoball() == config.playerNumber) {
            return true;
        }
        else {
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
void Behavior::kick() {

	if(kickOff && (microsec_clock::universal_time() <= lastPlay + seconds(0) ) ) {
		if (behaviorRand() < 0.75) {
			littleWalk(0.2, 0.0, 0.0);
		}
		else {
			if(side == 1)
				amot.set_command(config.kicks.KickSideLeft);
			else
				amot.set_command(config.kicks.KickSideRight);

			_blk.publishSignal(amot, "motion");
		}
	}
	else {

		if(orientation == 0) {
			if(ballY > 0.0)
				amot.set_command(config.kicks.KickForwardLeft); // Left Kick
			else
				amot.set_command(config.kicks.KickForwardRight); // Right Kick
		}
		else if(orientation == 3) {
			amot.set_command(config.kicks.KickSideLeft);
			direction = -1;
		}
		else if(orientation == 1) {
			amot.set_command(config.kicks.KickSideRight);
			direction = +1;
		}
		else if(orientation == 2) {
			if(ballY > 0.0)
				amot.set_command(config.kicks.KickSideLeft);
			else
				amot.set_command(config.kicks.KickSideRight);
		}
		else {
			if(ballY > 0.0)
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


void Behavior::littleWalk(double x, double y, double theta) {

	wmot.set_command("walkTo");
	wmot.set_parameter(0, x);
	wmot.set_parameter(1, y);
	wmot.set_parameter(2, theta);
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
    else if((ballBearing > M_PI_4) || (ballBearing < -M_PI_4)) {
        littleWalk(0.0, 0.0, (float)(side*M_PI_4/2.0));
    }
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

bool Behavior::goToPosition(float targetX, float targetY, float targetPhi) {

	double targetDistance = sqrt((targetX - robotX) * (targetX - robotX) + (targetY - robotY) * (targetY - robotY));
	double targetAngle = anglediff2(atan2(targetY - robotY, targetX - robotX), robotPhi);
	double targetOrientation = anglediff2(targetPhi, robotPhi);

	// TODO if the robot make it to position, stop checking for distance and check only orientation!
	if(targetDistance > 0.25) {
		pathPlanningRequestAbsolute(toCartesianX(targetDistance, targetAngle),
		                            toCartesianY(targetDistance, targetAngle),
		                            targetOrientation);
		return false;
	}
	else if(fabs(targetOrientation) > M_PI_4) {
		if(targetOrientation > 0)
			velocityWalk(0.0, 0.0, 0.3, 1);
		else
			velocityWalk(0.0, 0.0, -0.3, 1);
		return false;
	}
	else {
		//stopRobot();
		return true;
	}
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

	if(ballFound == 1) {

		if(!goalieApproachStarted)
			stopRobot();

		littleWalk(0.0, 0.0, ballBearing);
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

		if(ballDist < 1.0f) { // check if ball is to close to the goal post
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

		amot.set_command("PoseInitial.xar");
		_blk.publishSignal(amot, "motion");

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

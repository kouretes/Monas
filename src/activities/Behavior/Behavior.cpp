#include "Behavior.h"

#include "core/include/Logger.hpp"
#include "core/elements/math/Common.hpp"
#include "core/elements/math/Specific.hpp"
#include "core/elements/KStandard.hpp"
#include <math.h>

using namespace KMath;
using namespace KStandard;
using namespace boost::posix_time;
using namespace std;
using namespace FormationParameters;
using namespace Utility;
using namespace PSO;
using KMath::Specific::permutationsOfCombinations;

ACTIVITY_REGISTER(Behavior);

Behavior::Behavior(Blackboard &b) :
	IActivity(b), prf("Behavior")
{
	;
}

/**
 * Behavior initialization function.
 */
void Behavior::UserInit() {
	 
	_blk.updateSubscription("vision", msgentry::SUBSCRIBE_ON_TOPIC);
	_blk.updateSubscription("sensors", msgentry::SUBSCRIBE_ON_TOPIC);
	_blk.updateSubscription("worldstate", msgentry::SUBSCRIBE_ON_TOPIC);
	_blk.updateSubscription("pathplanning", msgentry::SUBSCRIBE_ON_TOPIC);
	_blk.updateSubscription("behavior", msgentry::SUBSCRIBE_ON_TOPIC);
	_blk.updateSubscription("buttonevents", msgentry::SUBSCRIBE_ON_TOPIC);
	_blk.updateSubscription("external", msgentry::SUBSCRIBE_ON_TOPIC, msgentry::HOST_ID_ANY_HOST); // used for PSO synchronization
	_blk.updateSubscription("communication", msgentry::SUBSCRIBE_ON_TOPIC);

	wmot.add_parameter(0.0f);
	wmot.add_parameter(0.0f);
	wmot.add_parameter(0.0f);
	wmot.add_parameter(0.0f);

	gameState = PLAYER_INITIAL;

	ballFound = false, sharedBallFound = false;
	robotStopped = false;
	kickOff = false;
	dist = false;
	gsmtime = false;
	synchronization = true;
	cX = 0.0;
	cY = 0.0;
	ct = 0.0;
	coordinationCycle = 0;
	ballX = 0.0, ballY = 0.0, ballDist = 0.0, ballBearing = 0.0;
	SharedGlobalBallX = 0.0, SharedGlobalBallY = 0.0, lastSharedBallX = 0.0, lastSharedBallY = 0.0;
	goalieApproachStarted = false;
	side =+ 1;
	robotX = 0.0, robotY = 0.0, robotPhi = 0.0;
	readyToKick = false, scanAfterKick = false;
	PSOflag = false, PSOhasRun = false;
	direction = 1;
	orientation = 0;
	numOfRobots = 0;
	currentRobotAction = MotionStateMessage::IDLE;
	Reset();
	fGen.Init(config.positions);
			
	LogEntry(LogLevel::Info, GetName())<<"Initialized: My number is " << (config.playerNumber) << " and my color is " <<(config.teamColor);
	
	if(config.playerNumber != 1)
		currentRole.role = FormationParameters::ONBALL; // default role
	else
		currentRole.role = FormationParameters::GOALIE;
		
	if(penaltyMode)
		goToPositionFlag = true;
	else
		goToPositionFlag = false;
		
	srand(time(0));
	lastWalk = microsec_clock::universal_time();
	lastPlay = microsec_clock::universal_time();
	lastPenalised = microsec_clock::universal_time();
	lastFormation = microsec_clock::universal_time();
	dispTimer = microsec_clock::universal_time();
    scanKickTime = microsec_clock::universal_time();
    lastBumperPressed = microsec_clock::universal_time();
	lastGoToCenter = microsec_clock::universal_time() - seconds(10);
	lastBallFound = microsec_clock::universal_time() - seconds(20);
	lastScan = microsec_clock::universal_time() + seconds(3);
    hcontrol.mutable_task()->set_action(HeadControlMessage::FROWN);
    _blk.publishState(hcontrol, "behavior");
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
	
	config.positions = (unsigned int)atoi(Configurator::Instance().findValueForKey("playerConfig.positions").c_str());
	
	// === read robot configuration xml data from playerConfig.xml===
	if ( (config.playerNumber < 1) || (config.playerNumber > config.maxPlayers) )
		LogEntry(LogLevel::Error, GetName())<< "Behavior Reset: Invalid player number";

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
	LogEntry(LogLevel::Info, GetName())<<"Reset done";
}

/* Behavior Main Execution Function */

int Behavior::Execute() {
	
	KPROF_SCOPE(prf, "Execute");
	
	gBestm.Clear();
	
	readMessages();
	getBallData();
	getGameState();
	getPosition();
	getMotionData();
	getTeamInfo();
	checkIfBumperPressed();
	
	if(PSOflag == true && PSOhasRun == true) {
		getPSOData();
	}
		
    if (gameState == PLAYER_INITIAL) {
		if(prevGameState != PLAYER_INITIAL) {
        	hcontrol.mutable_task()->set_action(HeadControlMessage::FROWN);
        	_blk.publishState(hcontrol, "behavior");
		}
    }
	else if (gameState == PLAYER_PLAYING) {

		if(prevGameState == PLAYER_PENALISED) {
			lastPenalised = microsec_clock::universal_time();
			// Check if the penalized was a wrong decision
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
		else if(prevGameState == PLAYER_SET) {
			lastPlay = microsec_clock::universal_time();
		}

		if(swim == 0 && config.playerNumber != 1) {
			LogEntry(LogLevel::Info, GetName()) << "NO SWI";
			currentRole.role = FormationParameters::ONBALL;
			goToPositionFlag = true;
		}

		updateOrientation();
		readyToKick = false;

		if(sharedBallFound == true && !penaltyMode) {
		LogEntry(LogLevel::Info, GetName()) << "SHARED BALL";
			if( (gsmtime = (gsm != 0 && gsm.get() != 0 && gsm->secs_remaining()%20 == 19)) ||
				(lastFormation + seconds(20) < microsec_clock::universal_time()) ||
				(dist = (DISTANCE(SharedGlobalBallX, lastSharedBallX, SharedGlobalBallY, lastSharedBallY) >= 0.7f)) ) {
				
				coordinationCycle++;
				
				if(dist) {
					dist = false;
					LogEntry(LogLevel::Info, GetName()) << "DISTANCE";
				}
				else if(gsmtime) {
					LogEntry(LogLevel::Info, GetName()) << "GSM SECS REMAINING: " << _toString(gsm->secs_remaining());
				}
				else
					LogEntry(LogLevel::Info, GetName()) << "LAST FORMATION TIMER";

				LogEntry(LogLevel::Info, GetName()) << "ROBOTS: "+_toString(numOfRobots);
				LogEntry(LogLevel::Info, GetName()) << "==========================================";
				for(unsigned int i = 0 ; i < robots.size() ; i++) {
					LogEntry(LogLevel::Info, GetName()) << "Id: "+_toString(robots[i].robotId);
					LogEntry(LogLevel::Info, GetName()) << "X: "+_toString(robots[i].robotX);
					LogEntry(LogLevel::Info, GetName()) << "Y: "+_toString(robots[i].robotY);
					LogEntry(LogLevel::Info, GetName()) << "Phi: "+_toString(robots[i].robotPhi);
					LogEntry(LogLevel::Info, GetName()) << "Stab: "+_toString(robots[i].robotStability);
					LogEntry(LogLevel::Info, GetName()) << "==========================================";
				}

				lastSharedBallX = SharedGlobalBallX;
				lastSharedBallY = SharedGlobalBallY;
								
				fGen.Generate(SharedGlobalBallX, SharedGlobalBallY);
				
				if(!gameMode){
					sendFormationDebugMessages();
				}

				if(config.playerNumber != 1) {
					Coordinate();
				}
				else {
					LogEntry(LogLevel::Info, GetName()) << "GOALIE: NO COORDINATION";
					currentRole = fGen.findRoleInfo(FormationParameters::GOALIE);
					goToPositionFlag = false;
				}
					
				lastFormation = microsec_clock::universal_time();
			}
		}

		if (lastPenalised + seconds(4) > microsec_clock::universal_time()) {
			hcontrol.mutable_task()->set_action(HeadControlMessage::LOCALIZE_FAR);
			_blk.publishState(hcontrol, "behavior");
			return 0;
		}

        if(currentRobotAction == MotionStateMessage::WALKING && scanAfterKick == true) {
            if (scanKickTime + seconds(2) < microsec_clock::universal_time())
			    scanAfterKick = false;

            stopRobot();
			hcontrol.mutable_task()->set_action(HeadControlMessage::SCAN);
			_blk.publishState(hcontrol, "behavior");
            return 0;
		}

		// Publish message to head controller to run check for ball
		hcontrol.mutable_task()->set_action(HeadControlMessage::SMART_SELECT);
		_blk.publishState(hcontrol, "behavior");

		if(config.playerNumber == 1) { // goalie role if number 1
			LogEntry(LogLevel::Info, GetName()) << "GOALIE BEHAVIOR";
			if(goToPositionFlag == false) {
				if(goToPosition(currentRole.X, currentRole.Y, 0.0) == false)
					return 0;
				else
					goToPositionFlag = true;
			}
			goalie();
		}
		else { // not goalie behavior
			
			if(currentRole.role == FormationParameters::ONBALL) {

				if(goToPositionFlag == false && ballFound == 0) {
					LogEntry(LogLevel::Info, GetName()) << "ATTACKER BEHAVIOR: GO TO POSITION";
					if(goToPosition(currentRole.X, currentRole.Y, 0.0) == false) {
						return 0;
					}
					else
						goToPositionFlag = true;
				}
				else if(ballFound == 1) {

					LogEntry(LogLevel::Info, GetName()) << "ATTACKER BEHAVIOR: BALL FOUND";

					goToPositionFlag = true;

		            lastBallFound = microsec_clock::universal_time();
					side = (ballBearing > 0) ? 1 : -1;

		            double loppgb = anglediff2(atan2(config.oppGoalLeftY - robotY, config.oppGoalLeftX - robotX), robotPhi);
		            double roppgb = anglediff2(atan2(config.oppGoalRightY - robotY, config.oppGoalRightX - robotX), robotPhi);
		            double cone = anglediff2(loppgb, roppgb);
		            double oppgb = wrapToPi(roppgb + cone / 2.0);

					/*
					if(fabs(ballY) < config.epsX){
						littleWalk(0.0, side, 0.0);
						return 0;
					}*/

					if(ballX < config.posX && ((ballY < config.posY) || (ballY < -config.posY))
						&& oppgb < M_PI_4
						&& oppgb > -M_PI_4
						&& lastBumperPressed + milliseconds(700) < microsec_clock::universal_time()){
						
						readyToKick = true;
						scanAfterKick = true;
                        scanKickTime = microsec_clock::universal_time();
						//return 0; // do nothing!!!
						kick();
						direction = (side == +1) ? -1 : +1;
					}

					if(!readyToKick) {
						if(penaltyMode)
							pathPlanningRequest(ballX - (config.posX + 0.025), ballY - side * config.posY, ballBearing, false);	// 2.5cm offset from the ball!
						else
							approachBall();
					}
				}
				else if(ballFound == 0) {

					LogEntry(LogLevel::Info, GetName()) << "ATTACKER BEHAVIOR: BALL NOT FOUND";

					// walk straight for some seconds after the scan has ended and then start turning around to search for ball.
					if(lastPenalised + seconds(20) > microsec_clock::universal_time()) {
                    	pathPlanningRequest(3.0, 0.0, 0.0, false);
					}
					else {
		            	if(sharedBallFound == true)
		                	goToPosition(SharedGlobalBallX, SharedGlobalBallY, 0.0);
						else {
							if(lastGoToCenter + seconds(10) > microsec_clock::universal_time()) {
		                            if(robotX < 0.0)
		                                goToPosition(-fGen.Field.MaxX/2.0f, 0.0, 0.0);
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
					LogEntry(LogLevel::Info, GetName()) << "OTHER BEHAVIOR: GO TO POSITION";
					if(goToPosition(currentRole.X, currentRole.Y, 0.0) == false)
						return 0;
					else
						goToPositionFlag = true;
				}
				else if(lastPenalised + seconds(20) > microsec_clock::universal_time()) {
                    	pathPlanningRequest(3.0, 0.0, 0.0, false);
				}
				else if(ballFound == 1) {
					LogEntry(LogLevel::Info, GetName()) << "OTHER BEHAVIOR: BALL FOUND";
					direction = (ballBearing > 0) ? 1 : -1;
					if(fabs(ballBearing) > M_PI/6) {
						littleWalk(0.0, 0.0, ballBearing);
						robotStopped = false;
					}
					else if(!robotStopped) {
						robotStopped = true;
						stopRobot();
					}

				}
				else if(sharedBallFound == 1) {
					LogEntry(LogLevel::Info, GetName()) << "OTHER BEHAVIOR: SHARED BALL FOUND";
					direction = (SharedBallBearing > 0) ? 1 : -1;
					if(fabs(SharedBallBearing) > M_PI/6) {
						littleWalk(0.0, 0.0, SharedBallBearing);
						robotStopped = false;
					}
					else if(!robotStopped) {
						robotStopped = true;
						stopRobot();
					}
				}
				else if(ballFound == 0 && sharedBallFound == 0) {
					LogEntry(LogLevel::Info, GetName()) << "OTHER BEHAVIOR: DEN VLEPW TIPOTA";

					if(lastScan + seconds(3) < microsec_clock::universal_time() && lastScan + seconds(8) > microsec_clock::universal_time()) {
						littleWalk(0.0, 0.0, (float)(-direction*M_PI_4/2.0));
						return 0;
					}
					else if(lastScan + seconds(8) < microsec_clock::universal_time()) {
						lastScan = microsec_clock::universal_time();
					}

					stopRobot();
				}

				if(currentRole.role == FormationParameters::DEFENDER ||
					currentRole.role == FormationParameters::DEFENDER_L ||
					currentRole.role == FormationParameters::DEFENDER_R) {

					if(ballFound == 1 || sharedBallFound == 1)
						defender();
				}
			}

		} // not goalie behavior end
	}
	else if (gameState == PLAYER_READY) {

	 	if(gsm != 0 && gsm.get() != 0)
	 		kickOff = gsm->kickoff();

	 	if(gameState != prevGameState) {
			fGen.XmlInitFormation(kickOff);
			if(!gameMode)
				sendFormationDebugMessages();
			currentRole = fGen.getFormation()->at(config.playerNumber - 1);
			lastFormation = microsec_clock::universal_time();
			locReset.set_type(LocalizationResetMessage::READY);
			locReset.set_kickoff(kickOff);
			_blk.publishSignal(locReset, "worldstate");
			stopRobot();
		}

		hcontrol.mutable_task()->set_action(HeadControlMessage::LOCALIZE);
		_blk.publishState(hcontrol, "behavior");

		if(goToPosition(currentRole.X, currentRole.Y, 0.0) == false)
			return 0;

		stopRobot();
		return 0;
	}
	else if (gameState == PLAYER_SET) {

		if(gsm != 0 && gsm.get() != 0)
			kickOff = gsm->kickoff();
		
		if(prevGameState != PLAYER_READY) {
			fGen.XmlInitFormation(kickOff);
			if(!gameMode)
				sendFormationDebugMessages();
			currentRole = fGen.getFormation()->at(config.playerNumber - 1);
			lastFormation = microsec_clock::universal_time();
		}
		
		if(gameState != prevGameState) {
			// Reset Loc
			locReset.set_type(LocalizationResetMessage::SET);
			locReset.set_kickoff(kickOff);
			_blk.publishSignal(locReset, "worldstate");
		}

		hcontrol.mutable_task()->set_action(HeadControlMessage::SCAN_AND_TRACK_FOR_BALL);
		_blk.publishState(hcontrol, "behavior");

		stopRobot();
	}
	else if(gameState == PLAYER_PENALISED) {

		if(prevGameState == PLAYER_INITIAL) {
			fGen.XmlInitFormation(kickOff);
			//fGen.DynamicInitFormation(kickOff);
			if(!gameMode)
				sendFormationDebugMessages();
			currentRole = fGen.getFormation()->at(config.playerNumber - 1);
		}

		if(gameState != prevGameState) {
			penalisedStarted = microsec_clock::universal_time();
			hcontrol.mutable_task()->set_action(HeadControlMessage::FROWN);
			_blk.publishState(hcontrol, "behavior");
		}
	}
	
	#ifdef KPROFILING_ENABLED
		prf.generate_report(1);
	#endif

	return 0;
}

void Behavior::Coordinate() {
		KPROF_SCOPE(prf, "Coordinate");
		
		int robotIndex = -1;
		
		if(PSOflag == true) {
			
			particle bestParticle;
			
			bestParticle = runPSO(10, 10, fGen, robots, SharedGlobalBallY);
				
			sendGlobalBest(bestParticle, gBestCost);
			
			if(!gameMode) {
				sendPSODebugMessages(bestParticle);
			}
			PSOhasRun = true;
		}
		else {
			
			std::string complexRole;
			
			for(unsigned int i = 0 ; i < fGen.getFormation()->size() ; i++) {
				if(fGen.getFormation()->at(i).role != FormationParameters::GOALIE) {
					complexRole = _toString(fGen.getFormation()->at(i).role);
					complexRole = complexRole + _toString(fGen.getFormation()->at(i).id);
					roles.insert(roles.end(), atoi(complexRole.c_str()));
				}
			}

			print(roles, "Behavior");
			mappings = permutationsOfCombinations(roles, numOfRobots);
			LogEntry(LogLevel::Info, GetName()) << "ALL POSSIBLE MAPPINGS ARE: ";
			print(mappings, "Behavior");
			
			roles.clear();

			LogEntry(LogLevel::Info, GetName()) << "CHECKING MAPPINGS... (" << mappings.size() << ")";

			// search for optimal mapping
			maxU = -INF;
			for(unsigned int map = 0 ; map < mappings.size() ; map++) {
				mapCost = 0.0f;
				LogEntry(LogLevel::Info, GetName()) << "MAP: " << map;
				for(unsigned int r = 0 ; r < numOfRobots ; r++) { // for all except goalie robots
					currentRobotPos = fGen.findRoleInfo(mappings[map].at(r));
					mapCost = mapCost + fieldUtility(currentRobotPos.X, currentRobotPos.Y, SharedGlobalBallY, fGen) -

							distance(robots[r].robotX, robots[r].robotY, currentRobotPos.X, currentRobotPos.Y, fGen.Field.MaxX, fGen.Field.MaxY) -

					  		minRotation(robots[r].robotX, robots[r].robotY, currentRobotPos.X, currentRobotPos.Y, robots[r].robotPhi) -

					  		collisions(mappings[map], robots, fGen, r) -
					  		
					  		sparse(mappings[map], numOfRobots, fGen, r);

					if(currentRobotPos.role == FormationParameters::ONBALL) {
						mapCost = mapCost - robotStability(robots[r].robotStability);
					}
				}

				if(Max(mapCost, maxU) == mapCost) {
					maxU = mapCost;
					index = map;
				}
				print(mappings[map], "Behavior");
				LogEntry(LogLevel::Info, GetName()) << "COST: " << mapCost;
			}

			robotIndex = getRobotIndex(robots, config.playerNumber);
			LogEntry(LogLevel::Info, GetName()) << "INDEX IS : " << robotIndex;
			
			if(robotIndex != -1)
				currentRole = fGen.findRoleInfo(mappings[index][robotIndex]);
			else
				currentRole.role = FormationParameters::ONBALL;
			
			LogEntry(LogLevel::Info, GetName()) << "OPTIMAL MAP IS: ";
			print(mappings[index], "Behavior");
			LogEntry(LogLevel::Info, GetName()) << "OPTIMAL COST IS: " << maxU;
			LogEntry(LogLevel::Info, GetName()) << "MY OPTIMAL ROLE IS: " << getRoleString(currentRole.role);
			
			if(!gameMode) {
				sendBFDebugMessages(mappings[index]);
			}
			
			goToPositionFlag = false;
		}
}

/**
 * Read incoming messages from declared topics. Use message objects to get the data.
 */
void Behavior::readMessages() {
	bm = _blk.readSignal<ButtonMessage>("buttonevents");
	gsm  = _blk.readState<GameStateMessage> ("worldstate");
	allsm = _blk.readData<AllSensorValuesMessage> ("sensors");
	om   = _blk.readState<ObstacleMessageArray> ("pathplanning");
	wim  = _blk.readData<WorldInfo> ("worldstate");
	swim = _blk.readData<SharedWorldInfo> ("worldstate");
	sm = _blk.readState<MotionStateMessage>("worldstate");
	h = _blk.readState<KnownHosts>("communication");
}

/* --------------------------------- Information gathering functions from messages ---------------------------------- */
void Behavior::checkIfBumperPressed(){
	if(bm != 0){
		int lbump = bm->data(KDeviceLists::L_BUMPER_L) + bm->data(KDeviceLists::L_BUMPER_R);
		int rbump = bm->data(KDeviceLists::R_BUMPER_L) + bm->data(KDeviceLists::R_BUMPER_R);
		if(lbump == 1 || rbump == 1){
			lastBumperPressed = microsec_clock::universal_time();
		}
	}
}

void Behavior::getPSOData() {
	
	int robotIndex = -1;
	float d = 0.0f, minD = INF;
	vector<PSOData> mappings;
	
	gbm = _blk.readData<gBestMessage>("external", msgentry::HOST_ID_LOCAL_HOST);
		
	if(gbm != 0 && gbm.get() != 0) {
		PSOData newData;
		for(int i = 0 ; i < gbm->gstarparticle_size()-1 ; i++)
			newData.gBest.insert(newData.gBest.end(), gbm->gstarparticle(i));
		newData.value = gbm->value();
		newData.cycle = gbm->gstarparticle(gbm->gstarparticle_size()-1);
		mappings.insert(mappings.end(), newData);
    }
		
	if(!h.get() || (h && h->entrylist_size() == 0)) {
		;
	}
	else {
		const ::google::protobuf::RepeatedPtrField< HostEntry >& rf = h->entrylist();
		::google::protobuf::RepeatedPtrField< HostEntry >::const_iterator fit;

		for(fit = rf.begin(); fit != rf.end(); ++fit) {
	        gbm  = _blk.readData<gBestMessage>("external", (*fit).hostid());

	        if(gbm != 0 && gbm.get() != 0) {
	        	PSOData newData;
	       		for(int i = 0 ; i < gbm->gstarparticle_size()-1 ; i++)
					newData.gBest.insert(newData.gBest.end(), gbm->gstarparticle(i));
				newData.value = gbm->value();
				newData.cycle = gbm->gstarparticle(gbm->gstarparticle_size()-1);
				mappings.insert(mappings.end(), newData);
			}
		}
	}
	
	/*
	LogEntry(LogLevel::Info, GetName()) << "MAPPINGS RECEIVED: " << mappings.size();
	for(int k = 0 ; k < mappings.size() ; k++) {
		for(int l = 0 ; l < mappings[k].gBest.size() ; l++) {
			std::cout << mappings[k].gBest[l] << " ";
		}
		LogEntry(LogLevel::Info, GetName()) << "U_VALUE:" << mappings[k].value;
		LogEntry(LogLevel::Info, GetName()) << "TIMESTAMP:" << mappings[k].time;
	}*/
				
	for(unsigned int r = 0 ; r < mappings.size() ; r++) {
		if(r+1 < mappings.size()) {
			if(/*mappings[r].stamp - mappings[r+1].stamp).total_seconds() > 5*/ mappings[r].cycle != mappings[r+1].cycle 
				&& lastFormation + seconds(4) > microsec_clock::universal_time()) {
				synchronization = false;
				break;
			}
			else
				synchronization = true;
		}
	}
	
	if(synchronization == true) {
		sortPSOData(mappings);
	
		LogEntry(LogLevel::Info, GetName()) << "BEST PSO MAPPING:" << mappings[0].value;
	
		robotIndex = getRobotIndex(robots, config.playerNumber);
		currentRole.X = mappings[0].gBest[2*robotIndex];
		currentRole.Y = mappings[0].gBest[2*robotIndex + 1];
	
		for(unsigned int i = 1 ; i < fGen.getFormation()->size() ; i++) {
			d = DISTANCE(currentRole.X, fGen.getFormation()->at(i).X, currentRole.Y, fGen.getFormation()->at(i).Y);
			if(Min(d, minD) == d) {
				minD = d;
				index = i;
			}
		}
		currentRole.role = fGen.getFormation()->at(index).role;
		LogEntry(LogLevel::Info, GetName()) << "MY OPTIMAL ROLE IS: " << getRoleString(currentRole.role);
		if(!gameMode) {
			sendPSODebugMessages(mappings[0].gBest);
		}
		
		PSOhasRun = false;
		goToPositionFlag = false;
	}
}

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

void Behavior::getTeamInfo() {

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
		ballX = wim->balls(0).relativex();// + wim->balls(0).relativexspeed() * 0.200;
		ballY = wim->balls(0).relativey();// + wim->balls(0).relativeyspeed() * 0.200;
		ballDist = sqrt(pow(ballX, 2) + pow(ballY, 2));
		ballBearing = atan2(ballY, ballX);
		ballFound = true;
	}
	else
		ballFound = false;

	if(swim != 0 && swim.get() != 0 && swim->globalballs_size() > 0) {
		SharedGlobalBallX = swim->globalballs(0).x();
		SharedGlobalBallY = swim->globalballs(0).y();
		SharedBallBearing = atan2(SharedGlobalBallY - robotY, SharedGlobalBallX - robotX);
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

void Behavior::sendGlobalBest(PSO::particle &bestParticle, float gBestCost) {
	
	for(unsigned int i = 0 ; i < bestParticle.size() ; i++)
		gBestm.add_gstarparticle(bestParticle[i]);

	gBestm.set_value(gBestCost);
	gBestm.add_gstarparticle(coordinationCycle);
	_blk.publishData(gBestm, "external");
}

void Behavior::sendFormationDebugMessages() {
	
	LogEntry(LogLevel::Info, GetName())
		<< "BallX: "  << (SharedGlobalBallX)
		<< " BallY: " << (SharedGlobalBallY);

	for(unsigned int i = 0 ; i < fGen.getFormation()->size() ; i++) {

		LogEntry(LogLevel::Info, GetName())
			<< "Role: " << (getRoleString(fGen.getFormation()->at(i).role))
			<< " X: "   << (fGen.getFormation()->at(i).X)
			<< " Y: "   <<  (fGen.getFormation()->at(i).Y);

		if(fdg.positions_size() < (int)(i+1))
			fdg.add_positions();

		fdg.mutable_positions(i)->set_x(fGen.getFormation()->at(i).X);
		fdg.mutable_positions(i)->set_y(fGen.getFormation()->at(i).Y);
		fdg.mutable_positions(i)->set_role(fGen.getFormation()->at(i).role);
	}
	_blk.publishSignal(fdg, "debug");
}

void Behavior::sendPSODebugMessages(vector<float> &positions) {
	
	unsigned int nextPos = 0;
		
	for(unsigned int i = 0 ; i < numOfRobots ; i++) {

		if(psodg.positionspso_size() < (int)(i+1))
			psodg.add_positionspso();

		psodg.mutable_positionspso(i)->set_x(positions[nextPos]);
		psodg.mutable_positionspso(i)->set_y(positions[nextPos + 1]);
		nextPos += 2;
	}
	_blk.publishSignal(psodg, "debug");
}

void Behavior::sendBFDebugMessages(vector<unsigned int> optimalMapping) {
	
	posInfo nextPos;
	
	for(unsigned int i = 0 ; i < optimalMapping.size() ; i++) {
		nextPos = fGen.findRoleInfo(optimalMapping[i]);
		
		if(mdg.mapping_size() < (int)(i+1))
			mdg.add_mapping();
		
		mdg.mutable_mapping(i)->set_x(nextPos.X);
		mdg.mutable_mapping(i)->set_y(nextPos.Y);
		mdg.mutable_mapping(i)->set_role(nextPos.role);
		
	}
	_blk.publishSignal(mdg, "debug");
}

/* -------------------------------------------------------------------------------------------------------------------- */

void Behavior::updateOrientation() {

	double loppgb = anglediff2(atan2(config.oppGoalLeftY - robotY, config.oppGoalLeftX - robotX), robotPhi);
	double roppgb = anglediff2(atan2(config.oppGoalRightY - robotY, config.oppGoalRightX - robotX), robotPhi);
	double cone = anglediff2(loppgb, roppgb);
	double oppgb = wrapToPi(roppgb + cone / 2.0);

	if( (oppgb <= M_PI_4) && (oppgb > -M_PI_4) ) {
		orientation = 0;
	}
	else if( (oppgb > M_PI_4) && (oppgb <= (M_PI - M_PI_4) ) ) {
		orientation = 1;
	}
	else if( (oppgb > (M_PI - M_PI_4) ) || (oppgb <= -(M_PI - M_PI_4) ) ) {
		orientation = 2;
	}
	else if( (oppgb <= -M_PI_4 ) && (oppgb > -(M_PI - M_PI_4) ) ) {
		orientation = 3;
	}
}

void Behavior::kick() {

	if(kickOff && (microsec_clock::universal_time() <= lastPlay + seconds(0) ) ) {
		if(side == 1)
            amot.set_command(config.kicks.KickSideLeft);
        else
            amot.set_command(config.kicks.KickSideRight);

        _blk.publishSignal(amot, "motion");
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

void Behavior::velocityWalk(double ix, double iy, double it, double f) {

	double x, y, t;
	x = ix;
	y = iy;
	t = it;

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

	if (ballDist > 0.5) {
        pathPlanningRequest(ballX - config.posX, ballY - side * config.posY, ballBearing, false);
    }
    else if((ballBearing > M_PI_4) || (ballBearing < -M_PI_4)) {
        littleWalk(0.0, 0.0, (float)(side*M_PI_4/2.0));
    }
    else if(oppgb > (float) (M_PI_4)) {
        velocityWalk(0.0, -1.0, (float)(M_PI_4/2.0),1.0);
    }
    else if(oppgb < (float) (-M_PI_4)) {
        velocityWalk(0.0, 1.0, (float)(-M_PI_4/2.0),1.0);
    }
    else{
        velocityWalk(2*(ballX - config.posX)+0.03,ballY - side * config.posY, 0.0 ,1.0);
    }
}

void Behavior::stopRobot() {
	amot.set_command("InitPose.xar");
	_blk.publishSignal(amot, "motion");
}

void Behavior::pathPlanningRequest(float targetX, float targetY, float targetPhi, bool useSmallGrid) {

	pprm.set_targetx(targetX);
	pprm.set_targety(targetY);
	pprm.set_targetorientation(targetPhi);
	pprm.set_forceuseofsmallmap(useSmallGrid);
	_blk.publishSignal(pprm, "pathplanning");
}


bool Behavior::goToPosition(float targetX, float targetY, float targetPhi) {

	double targetDistance = sqrt((targetX - robotX) * (targetX - robotX) + (targetY - robotY) * (targetY - robotY));
	double targetAngle = anglediff2(atan2(targetY - robotY, targetX - robotX), robotPhi);
	double targetOrientation = anglediff2(targetPhi, robotPhi);

	// TODO if the robot make it to position, stop checking for distance and check only orientation!
	if(targetDistance > 0.25) {
		LogEntry(LogLevel::Info, GetName()) << "THELW NA PAW STH THESH: " << targetX << ", " << targetY;
		pathPlanningRequest(toCartesianX(targetDistance, targetAngle),
		                            toCartesianY(targetDistance, targetAngle),
		                            targetOrientation, false);
		return false;
	}
	else if(fabs(targetOrientation) > M_PI_4) {
		if(targetOrientation > 0)
			velocityWalk(0.0, 0.0, 0.3, 1);
		else
			velocityWalk(0.0, 0.0, -0.3, 1);
		return false;
	}
	else
		return true;
}

void Behavior::defender() {

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

	if(ballDist < 1.5f) { // check if ball is to close to the goal post
		velocityWalk(ballX - config.posX, ballY - side * config.posY, ballBearing, 1);
		if ( (fabs(ballX - config.posX) < config.epsX)  && (fabs( ballY - side*config.posY ) < config.epsY)) {
			if (ballY > 0.0)
				amot.set_command(config.kicks.KickForwardLeft); // Left Kick
			else
				amot.set_command(config.kicks.KickForwardRight); // Right Kick

			_blk.publishSignal(amot, "motion");
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
			velocityWalk(ballX - config.posX, ballY - side * config.posY, ballBearing, 1);
			if ( (fabs(ballX - config.posX) < config.epsX)  && (fabs( ballY - side*config.posY ) < config.epsY)) {
				if (ballY > 0.0)
					amot.set_command(config.kicks.KickForwardLeft); // Left Kick
				else
					amot.set_command(config.kicks.KickForwardRight); // Right Kick

				_blk.publishSignal(amot, "motion");
			}
		}
		else {
			littleWalk(0.0, 0.0, ballBearing);
		}

	}
	/*else if(sharedBallFound == 1) {
		littleWalk(0.0, 0.0, SharedBallBearing);
	}*/
	else if(ballFound == 0/* && sharedBallFound == 0*/) {

		stopRobot();

		hcontrol.mutable_task()->set_action(HeadControlMessage::SCAN_AND_TRACK_FOR_BALL);
		_blk.publishState(hcontrol, "behavior");
	}

	return;
}

int Behavior::toFallOrNotToFall() {

	float x, y, ballProjection, uBall, uBallX, uBallY;

	if(wim == 0)	// the two last observation messages
		return 0;

	if(wim->balls_size() == 0)
		return 0;

	// observation of (x,y) position of the ball and x,y speed
	x = wim->balls(0).relativex();
	y = wim->balls(0).relativey();
	uBallX = wim->balls(0).relativexspeed();
	uBallY = wim->balls(0).relativeyspeed();

	if(uBallX < 0.0) {
		ballProjection = (uBallX * y - uBallX * x) / uBallX ; // dk is the projection of the ball's route towards the robot/goalpost

		if(fabs(ballProjection) <= 0.3) { // if ball projection is shorter than the robot's foot can extend

			uBall = sqrt(uBallX * uBallX + uBallY * uBallY);

			if(fabs(uBall) > config.ur) {
				if(ballProjection > 0)
					return 1;  // left
				else
					return -1;  // right
			}
		}
	}

	return 0;
}

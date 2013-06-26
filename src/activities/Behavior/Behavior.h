#ifndef BEHAVIOR_H
#define BEHAVIOR_H

#include "core/include/IActivity.hpp"
#include "core/architecture/configurator/Configurator.hpp"

#include "messages/Motion.pb.h"
#include "messages/SensorsMessage.pb.h"
#include "messages/VisionObservations.pb.h"
#include "messages/Gamecontroller.pb.h"
#include "messages/PathPlanningMessages.pb.h"
#include "messages/WorldInfo.pb.h"
#include "messages/BehaviorMessages.pb.h"
#include "messages/RoboCupGameControlData.h"
#include "messages/Debug.pb.h"

#include "hal/robot/generic_nao/robot_consts.h"

#include <boost/date_time/posix_time/posix_time.hpp>

#include "FormationGenerator.h"
#include "Utility.h"

/**
 * @brief Monas activity intented to determine the Behavior of the robot.
 *
 * @author 2012-2013 Kouretes Team
 *
 * \file Behavior.h
 */

ACTIVITY_START

class Behavior: public IActivity {

public:

	ACTIVITY_CONSTRUCTOR(Behavior);

	/**
	 * @fn std::string ACTIVITY_VISIBLE GetName()
	 * @brief Used to get the name of the activity.
	 */
	std::string ACTIVITY_VISIBLE GetName() { return "Behavior"; }

	/**
	 * @fn void ACTIVITY_VISIBLE  UserInit()
	 * @brief Activity initialization.
	 */
	void ACTIVITY_VISIBLE UserInit();

	/**
	 * @fn void ACTIVITY_VISIBLE  Reset()
	 * @brief Activity reset capability. Reload xmls etc...
	 */
	void ACTIVITY_VISIBLE Reset();

	/**
	 * @fn int ACTIVITY_VISIBLE IEX_DIRECTIVE_HOT Execute()
	 * @brief Agents execute function, used to execute the activity.
	 */
	int ACTIVITY_VISIBLE IEX_DIRECTIVE_HOT Execute();

private:

	/**
	 * @struct Kick
	 * @brief Used to store all possible kicks. This struct is member of configuration struct.
	 */
	struct Kick {
		std::string KickForwardLeft;
		std::string KickForwardRight;
		std::string KickSideLeft;
		std::string KickSideRight;
		std::string KickBackLeft;
		std::string KickBackRight;
	};

	/**
	 * @struct configiration
	 * @brief Used to store and update all values that we read from the xml files. So everything
	 *	that is read from an xml should be included in this list!
	 */
	struct {

		// values from the team_config xml file
		unsigned int teamNumber, playerNumber, teamColor, maxPlayers;

		bool isPenaltyMode;

		// values from the features xml file
		double oppGoalX, oppGoalY, ownGoalX, ownGoalY;
		double oppGoalLeftX, oppGoalLeftY, oppGoalRightX, oppGoalRightY;
		double ownGoalLeftX, ownGoalLeftY, ownGoalRightX, ownGoalRightY;

		// values from the behavior xml file
		float posX, posY, epsX, epsY;
		struct Kick kicks;
		float ur; // used only by goalie

	} config;

	/* --------------------------------- Messages and Messaging Functions ---------------------------------- */

	/**
	 * Incoming Messages
	 */
	boost::shared_ptr<const AllSensorValuesMessage> allsm;
	boost::shared_ptr<const GameStateMessage>  gsm;
	boost::shared_ptr<const ObstacleMessageArray>  om;
	boost::shared_ptr<const WorldInfo>  wim;
	boost::shared_ptr<const SharedWorldInfo>  swim;
	boost::shared_ptr<const MotionStateMessage> sm;

	/**
	 * Outgoing Messages
	 */
	HeadControlMessage hcontrol;
	MotionWalkMessage wmot;
	MotionActionMessage amot;
	LocalizationResetMessage locReset;
	PathPlanningRequestMessage pprm;
	ObstacleMessage fom;	// fake obstacle message!
	FormationDataForGUI fdg;

	/**
	 * @fn void readMessages()
	 * @brief Read all incoming messages from declared topics. Called each time the
	 * activity is executed to update message objects.
	 */
	void readMessages();

	/**
	 * @fn void GetBallData()
	 * @brief Information gathering function, that reads the ball coordinates, bearing and distance.
	 */
	void getBallData();

	/**
	 * @fn void GetGameState()
	 * @brief Information gathering function, that reads the game state message
	 * and assign the robot game state variables (gameState, teamColor, playerNumber...)
	 */
	void getGameState();

	/**
	 * @fn void GetPosition()
	 * @brief Information gathering function, that reads the position, angle and robot confidence.
	 */
	void getPosition();

	/**
	 * @fn void getTeamPositions()
	 * @brief Information gathering function, that reads all reachable teammate positions, orientation and
	 * stability (including that robot).
	 */
	void getTeamInfo();

	/**
	 * @fn void GetMotionData()
	 * @brief Information gathering function, that reads motion state of the robot.
	 */
	void getMotionData();

	/**
	 * @fn void sendDebugMessages()
	 * @brief Send messages for debug.
     */
	void sendDebugMessages();

	/* --------------------------------- Roles functions ---------------------------------- */

	/**
	 * @fn int toFallOrNotToFall()
	 * @brief Function used by Goalie() function to determine when and where to extend foot
	 * for a ball save.
	 */
	int toFallOrNotToFall();

	/**
	 * @fn void Goalie()
	 * @brief Function used by goalie basic behavior to decide when to fall for a ball save and
 	 * when to go on the ball and kick it away from the goal post.
	 */
	void goalie();
	
	/**
	 * @fn void defender()
	 * @brief
	 */
	void defender();

	/* ------------------------------------------------------------------------------------- */

	/**
	 * @fn void updateOrientation()
	 * @brief Function used to determine which side the robot is facing.
	 */
	void updateOrientation();

	/**
	 * @fn void kick()
	 * @brief Use for approaching the ball on the right distance and kick the ball
	 * using the appropriate kick motion.
	 */
	void kick();

	/**
	 * @fn void velocityWalk(double ix, double iy, double it, double f)
	 * @brief (TODO)
	 */
	void velocityWalk(double ix, double iy, double it, double f);

	/**
	 * @fn void littleWalk(double x, double y, double th)
	 * @brief (TODO)
	 */
	void littleWalk(double x, double y, double theta);

	/**
	 * @fn void approachBall()
	 * @brief Use to approach the ball.
	 */
	void approachBall();

	/**
	 * @fn void stopRobot()
	 * @brief Stops the robot from moving.
	 */
	void stopRobot();

	/**
	 * @fn void pathPlanningRequest(float target_x, float target_y, float target_phi)
	 * @brief (TODO)
	 */
	void pathPlanningRequest(float targetX, float targetY, float targetPhi, bool useSmallGrid);

	/**
	 * @fn void goToPosition(float target_x, float target_y, float target_phi)
	 * @brief (TODO)
	 */
	bool goToPosition(float targetX, float targetY, float targetPhi);

	void Coordinate();


	/* --------------------------------- Behavior Variables ---------------------------------- */

	bool ballFound, sharedBallFound;	// variables that is true if we see the ball.

	int fall;	// variable for goalie role to check if he should fall or not and in which side.

	bool kickOff, searchFlag;

	float cX, cY, ct;

	float ballDist, ballBearing, ballX, ballY; // ball data coming from world info message.

	float SharedGlobalBallX, SharedGlobalBallY, SharedBallBearing, lastSharedBallX, lastSharedBallY; // global ball from shared world state!

	int side;

	float robotX, robotY, robotPhi; // robot coordinates, angle.

	std::vector<FormationParameters::Robot> robots; // team information.

	bool readyToKick, scanAfterKick;

	int direction;

	unsigned int orientation;

	int gameState, prevGameState;

	bool goalieApproachStarted, goToPositionFlag, robotStopped;

	bool gameMode, penaltyMode;

	MotionStateMessage::ActionType currentRobotAction;

	FormationGenerator fGen; // object that create and update the team formation

	boost::posix_time::ptime lastWalk, lastPlay, lastPenalised, penalisedStarted, lastFormation, lastBallFound, lastGoToCenter, dispTimer; // timers
	
	float mapCost, maxU;

	unsigned int index;
	
	bool dist, gsmtime;
	
	FormationParameters::posInfo currentRobotPos, currentRole;

	std::vector<FormationParameters::Role> roles;

	std::vector< std::vector<FormationParameters::Role> > mappings;

	unsigned int numOfRobots;

};

ACTIVITY_END

#endif


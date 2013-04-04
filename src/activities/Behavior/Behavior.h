#ifndef BEHAVIOR_H
#define BEHAVIOR_H

#include "core/include/IActivity.hpp"


#include "messages/motion.pb.h"
#include "messages/SensorsMessage.pb.h"
#include "messages/VisionObservations.pb.h"
#include "messages/Gamecontroller.pb.h"
#include "messages/ObstacleAvoidanceMessage.pb.h"
#include "messages/WorldInfo.pb.h"
#include "messages/BehaviorMessages.pb.h"
#include "messages/RoboCupGameControlData.h"
#include "messages/Debug.pb.h"

#include "hal/robot/generic_nao/robot_consts.h"

#include "tools/logger.h"
#include "tools/toString.h"
#include "core/elements/math/Common.hpp"
#include "tools/obstacleConst.h"

#include <boost/date_time/posix_time/posix_time.hpp>

#include "FormationGenerator.h"

#define INIT_VALUE -111.0
#define numOfFakeObstacles 15

/**
 * @brief Monas activity intented to determine the Behavior of the robot.
 * (Roles: Goalie, Attacker, Defender... )
 *
 * @author 2012 Kouretes team
 *
 * \file Behavior.h
 */

ACTIVITY_START

class Behavior: public IActivity
{

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
	 * @enum ROLES
	 * @brief enum to attach roles on the robot and determine their behavior.
	 */
	enum ROLES
	{
	    ATTACKER = 0, CENTER_FOR = 1, GOALIE = 2
	};

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
		int teamNumber, playerNumber, teamColor, maxPlayers;

		bool isPenaltyMode;

		// values from the playerConfig xml file, initial game positions on the field...
		float initX[2], initY[2], initPhi[2];

		// values from the features xml file
		double oppGoalX, oppGoalY, ownGoalX, ownGoalY;
		double oppGoalLeftX, oppGoalLeftY, oppGoalRightX, oppGoalRightY;
		double ownGoalLeftX, ownGoalLeftY, ownGoalRightX, ownGoalRightY;

		// values from the behavior xml file
		float posX, posY, epsX, epsY;
		struct Kick kicks;
		float ur; // used only by goalie. IF SOMEONE KNOWS WHAT ur IS, PLEASE CHANGE THAT NAME!

	} config;

	/* --------------------------------- Messages and Functions ---------------------------------- */

	/**
	 * Incoming Messages
	 */
	boost::shared_ptr<const AllSensorValuesMessage> allsm;
	boost::shared_ptr<const BallTrackMessage>  bmsg;
	boost::shared_ptr<const GameStateMessage>  gsm;
	boost::shared_ptr<const ObstacleMessageArray>  om;
	boost::shared_ptr<const WorldInfo>  wim;
	boost::shared_ptr<const SharedWorldInfo>  swim;
	boost::shared_ptr<const BallFoundMessage>  bfm;
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
	 * @fn void GetMotionData()
	 * @brief Information gathering function, that reads motion state of the robot.
	 */
	void getMotionData();

	/**
	 * @fn void sendDebugMessages()
	 * @brief Send messages for debug.
     */
	void sendDebugMessages();

	/* --------------------------------- Goalie functions ---------------------------------- */

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

	/* ------------------------------------------------------------------------------------- */

	/**
	 * @fn bool closestRobot()
	 * @brief Use to determine if the robot is the one closest to the ball.
	 * @return true if the robot is the closest or there is no shared world info message
	 * to get information, else returns false.
	 */
	bool closestRobot();

	/**
	 * @fn void updateOrientation()
	 * @brief (TODO)
	 */
	void updateOrientation();

	/**
	 * @fn void checkForPenaltyArea()
	 * @brief (TODO)
	 */
	void checkForPenaltyArea();

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
	void littleWalk(double x, double y, double th);

	/**
	 * @fn void approachBall()
	 * @brief Use to approach the ball.
	 */
	void approachBall();

	/**
	 * @fn void approachBallRoleDependent()
	 * @brief Use to determine if the robot is the one that should approach the
	 * the ball. Checks if the robot is the closest one to the ball and then
	 * calls the approachBall() function if needed.
	 */
	void approachBallRoleDependent();

	/**
	 * @fn void stopRobot()
	 * @brief Stops the robot from moving.
	 */
	void stopRobot();

	/**
	 * @fn void generateFakeObstacles()
	 * @brief (TODO)
	 */
	void generateFakeObstacles();

	/**
	 * @fn void pathPlanningRequestRelative(float target_x, float target_y, float target_phi)
	 * @brief (TODO)
	 */
	void pathPlanningRequestRelative(float targetX, float targetY, float targetPhi);

	/**
	 * @fn void pathPlanningRequestAbsolute(float target_x, float target_y, float target_phi)
	 * @brief (TODO)
	 */
	void pathPlanningRequestAbsolute(float targetX, float targetY, float targetPhi);

	/**
	 * @fn void goToPosition(float target_x, float target_y, float target_phi)
	 * @brief (TODO)
	 */
	void goToPosition(float targetX, float targetY, float targetPhi);

	/* --------------------------------- Behavior Variables ---------------------------------- */

	bool ballFound;	// variable that is true if we see the ball.

	int fall;	// variable for goalie role to check if he should fall or not and in which side.

	int role;	// variable that holds the role number of the robot (see enum ROLES).

	bool pathOK;

	bool kickOff;

	float cX, cY, ct;

	float ballDist, ballBearing, ballX, ballY; // ball data coming from world info message.
	
	float globalBallX, globalBallY; // transformation from relative robot ball x,y to relative field ball x,y USED UNTIL WE HAVE GLOBAL BALL ESTIMATION!!!
	
	float SharedGlobalBallX, SharedGlobalBallY; // global ball from shared world state!

	int side;

	float robotX, robotY, robotPhi, robotConfidence; // robot coordinates, angle and confidence.

	bool readyToKick;

	int direction;

	double orientation;

	int gameState, prevGameState;

	float fakeObstacles[numOfFakeObstacles][2]; // fake obstacles to avoid entering the penalty area.

	bool goalieApproachStarted;

	bool gameMode, penaltyMode;

	MotionStateMessage::ActionType currentRobotAction;

	FormationGenerator fGen; // object that create and update the team formation

	boost::posix_time::ptime lastWalk, lastPlay, lastPenalised, penalisedStarted, lastFormation; // timers.
};

ACTIVITY_END
#endif

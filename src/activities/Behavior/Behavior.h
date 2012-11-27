#ifndef BEHAVIOR_H
#define BEHAVIOR_H

#include "architecture/executables/IActivity.h"
#include "architecture/archConfig.h"

#include "messages/motion.pb.h"
#include "messages/SensorsMessage.pb.h"
#include "messages/VisionObservations.pb.h"
#include "messages/Gamecontroller.pb.h"
#include "messages/ObstacleAvoidanceMessage.pb.h"
#include "messages/WorldInfo.pb.h"
#include "messages/BehaviorMessages.pb.h"
#include "messages/RoboCupGameControlData.h"

#include "hal/robot/generic_nao/robot_consts.h"

#include "tools/logger.h"
#include "tools/toString.h"
#include "tools/mathcommon.h"
#include "tools/obstacleConst.h"

#include <boost/date_time/posix_time/posix_time.hpp>


#define INIT_VALUE -111.0
#define numOfFakeObstacles 15

/**
 * @brief Monas activity intented to determine the Behavior
	(Roles: Goalie, Attacker, Defender... ) for nao v3.3 robot.

 * @author 2012 kouretes team

 * \file Behavior.h
*/

ACTIVITY_START

class Behavior: public IActivity
{

public:

	ACTIVITY_CONSTRUCTOR(Behavior);

	/**
	 * @fn std::string ACTIVITY_VISIBLE GetName()
	 * @brief used get the name of the activity.
	 */
	std::string ACTIVITY_VISIBLE GetName() { return "Behavior"; }

	/**
	 * @fn void ACTIVITY_VISIBLE  UserInit()
	 * @brief Activity initialization...
	 */
	void ACTIVITY_VISIBLE UserInit();

	/**
	 * @fn void ACTIVITY_VISIBLE  Reset()
	 * @brief Activity reset capability. Reload xmls etc...
	 */
	void ACTIVITY_VISIBLE Reset();

	/**
	 * @fn int ACTIVITY_VISIBLE IEX_DIRECTIVE_HOT Execute()
	 * @brief Agents execute function, used to execute the activity...
	 */
	int ACTIVITY_VISIBLE IEX_DIRECTIVE_HOT Execute();

private:

	/**
	 * @enum ROLES
	 * @brief enum to attach roles on the robot.
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

		// values from the playerConfig xml file, initial game positions on the field...
		float initX[2], initY[2], initPhi[2];

		// values from the features xml file
		double oppGoalX, oppGoalY, ownGoalX, ownGoalY;
		double oppGoalLeftX, oppGoalLeftY, oppGoalRightX, oppGoalRightY;
		double ownGoalLeftX, ownGoalLeftY, ownGoalRightX, ownGoalRightY;

		// values from the behavior xml file
		float posx, posy, epsx, epsy;
		struct Kick kicks;
		float ur; // used only by goalie

	} config;

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

	/**
	 * Outgoing Messages
	 */
	HeadControlMessage hcontrol;
	MotionWalkMessage wmot;
	MotionActionMessage amot;
	LocalizationResetMessage locReset;
	PathPlanningRequestMessage pprm;
	ObstacleMessage fom;	// fake obstacle message!

	/**
	 * @fn void read_messages()
	 * @brief read incoming messages from declared topics.
	 */
	void read_messages();

	/**
	 * @fn void GetGameState()
	 * @brief information gathering function, that reads the game state message
	 * and assign the robot game state variables (gameState, teamColor, playerNumber...)
	 */
	void GetGameState();

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
	void Goalie();

	/* ------------------------------------------------------------------------------------- */

	/**
	 * @fn bool ClosestRobot()
	 * @brief (TODO)
	 */
	bool ClosestRobot();

	/**
	 * @fn void GetPosition()
	 * @brief (TODO)
	 */
	void GetPosition();

	/**
	 * @fn void UpdateOrientation()
	 * @brief (TODO)
	 */
	void UpdateOrientation();

	/**
	 * @fn void checkForPenaltyArea()
	 * @brief (TODO)
	 */
	void checkForPenaltyArea();

	/**
	 * @fn void Kick()
	 * @brief (TODO)
	 */
	void Kick();

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
	 * @brief (TODO)
	 */
	void approachBall();

	/**
	 * @fn void approachBallRoleDependent()
	 * @brief (TODO)
	 */
	void approachBallRoleDependent();

	/**
	 * @fn void stopRobot()
	 * @brief (TODO)
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
	void pathPlanningRequestRelative(float target_x, float target_y, float target_phi);

	/**
	 * @fn void pathPlanningRequestAbsolute(float target_x, float target_y, float target_phi)
	 * @brief (TODO)
	 */
	void pathPlanningRequestAbsolute(float target_x, float target_y, float target_phi);

	/**
	 * @fn void gotoPosition(float target_x, float target_y, float target_phi)
	 * @brief (TODO)
	 */
	void gotoPosition(float target_x, float target_y, float target_phi);

	bool ballfound;	// variable that is true if we see the ball.

	int fall;	// variable for goalie role to check if he should fall or not and in which side.

	int role;	// variable that holds the role number of the robot (see enum ROLES)...

	bool pathOK;

	bool kickoff;

	float cX, cY, ct;

	float ball_dist, ball_bearing, ball_x, ball_y;

	int side;

	float robot_x, robot_y, robot_phi, robot_confidence;

	bool readytokick;

	int direction;

	double orientation;

	int gameState, prevGameState;

	float fakeObstacles[numOfFakeObstacles][2]; // fake obstacles to avoid entering the penalty area.

	bool goalieApproachStarted;
	
	bool gameMode;

	boost::posix_time::ptime lastwalk, lastplay, lastpenalised, penalisedStarted;
};

ACTIVITY_END
#endif

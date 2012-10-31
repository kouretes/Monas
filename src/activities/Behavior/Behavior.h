#ifndef BEHAVIOR_H
#define BEHAVIOR_H

#include "architecture/executables/IActivity.h"

#include "messages/motion.pb.h"
#include "messages/SensorsMessage.pb.h"
#include "messages/VisionObservations.pb.h"
#include "messages/Kimage.pb.h"
#include "messages/Gamecontroller.pb.h"
#include "messages/ObstacleAvoidanceMessage.pb.h"
#include "messages/WorldInfo.pb.h"
#include "messages/BehaviorMessages.pb.h"
#include "tools/XML.h"
#include "tools/XMLConfig.h"
#include "architecture/archConfig.h"

#include <boost/date_time/posix_time/posix_time.hpp>

#define PITCHMIN -0.55
#define	PITCHMAX 0.33
#define YAWMIN 0.8
#define YAWMAX 1.35
#define PITCHSTEP 0.3
#define YAWSTEP 0.4

#define OVERSH 0.08
#define WAITFOR 40

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
	 * @brief Agents execute function...
	 */
	int ACTIVITY_VISIBLE IEX_DIRECTIVE_HOT Execute();
	
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
	
	/**
	 * @fn bool ClosestRobot()
	 * @brief
	 */
	bool ClosestRobot();
	void GetPosition();
	void UpdateOrientationPlus();
	int MakeTrackBallAction();
	int MakeTrackBallActionNoBmsg();
	void checkForPenaltyArea();

	void HeadScanStepRaster();
	void Kick(int side);

private:
	
	/**
	 * @enum ROLES
	 * @brief enum to attach roles on the robot.
	 */
	enum ROLES
	{
	    ATTACKER = 0, CENTER_FOR, GOALIE
	};

	int toFallOrNotToFall();

	void velocityWalk(double ix, double iy, double it, double f);
	void littleWalk(double x, double y, double th);
	void approachBall(double ballX, double ballY);
	void approachBallNewWalk(double ballX, double ballY);
	void approachBallRoleDependent(double ballX, double ballY);

	void stopRobot();
	void generateFakeObstacles();
	void pathPlanningRequestRelative(float target_x, float target_y, float target_phi);
	void pathPlanningRequestAbsolute(float target_x, float target_y, float target_phi);
	void gotoPosition(float target_x, float target_y, float target_phi);

	void calibrate();

	bool readConfiguration(const std::string& file_name); 		//this function reads team's configuration info from XML file
	bool readRobotConfiguration(const std::string& file_name); 	//this function reads robot's initial position in the field from XML file
	bool readGoalConfiguration(const std::string& file_name); 	//this function reads the position of the goals
	//bool readBehaviorConfiguration(const std::string& file_name); // this function reads the behavior data

	float dist(float x1, float y1, float x2, float y2);
	void test();

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
	HeadControlMessage* hcontrol;
	MotionWalkMessage* wmot;
	MotionHeadMessage* hmot;
	MotionActionMessage* amot;
	LocalizationResetMessage* locReset;
	PathPlanningRequestMessage* pprm;
	ObstacleMessage* fom; // fake obstacle message!

	int leftright;
	float headpos;

	bool ballfound;
	bool scanforball;
	bool startscan;
	bool scanOK;
	bool pathOK;
	float targetYaw;
	float targetPitch;
	SensorData HeadYaw;
	SensorData HeadPitch;
	float psign, ysign;
	unsigned waiting;
	int calibrated;

	int forball, forpost;
	
	//float ur; // value used by goalie function toFallOrNotToFall!!!
	
	int fall;	// variable for goalie role to check if he should fall or not.

	bool kickoff;
	float initX[2], initY[2], initPhi[2]; // initial game position in the field!!!!
	float fakeObstacles[numOfFakeObstacles][2]; // fake obstacles to avoid entering the penalty area.
	double oppGoalX, oppGoalY, ownGoalX, ownGoalY;
	double oppGoalLeftX, oppGoalLeftY, oppGoalRightX, oppGoalRightY;
	double ownGoalLeftX, ownGoalLeftY, ownGoalRightX, ownGoalRightY;
	float cX, cY, ct;
	float ur; //Better names plz :P
	float bd, bb, bx, by, posx, posy;
	float trackYaw, trackPitch;
	int side;
	float robot_x, robot_y, robot_phi, robot_confidence;

	bool readytokick;
	int back;
	int direction;
	double orientation;

	int gameState;
	int teamColor;
	int playerNumber;
	int role;

	bool readRobotConf;

	boost::posix_time::ptime lastmove, lastball, lastwalk, lastplay, lastpenalized, ballseen, lastrolechange;
};

ACTIVITY_END
#endif

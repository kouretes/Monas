#ifndef NoxOpenChallenge_H
#define NoxOpenChallenge_H 1

#include "architecture/IActivity.h"

#include "architecture/narukom/pub_sub/publisher.h"

#include "messages/motion.pb.h"
#include "messages/SensorsMessage.pb.h"
#include "messages/VisionObservations.pb.h"
#include "messages/Gamecontroller.pb.h"
#include "messages/ObstacleAvoidanceMessage.pb.h"
#include "messages/BehaviorMessages.pb.h"
#include "messages/WorldInfo.pb.h"
#include "tools/XML.h"
#include "tools/XMLConfig.h"
#include "architecture/archConfig.h"

#include <boost/date_time/posix_time/posix_time.hpp>





class NoxOpenChallenge: public IActivity {

	public:
		NoxOpenChallenge();
		std::string GetName() {
			return "NoxOpenChallenge";
		}

		void UserInit();
		int Execute();

		void read_messages();

		void GetGameState();
		void GetPosition();
		void UpdateOrientation();
		void CheckForBall();
		int MakeTrackBallAction();

		void HeadScanStep();
		void HighHeadScanStep(float yaw_limit);

		void Kick(int side);
		bool WideWalk();

	private:

		void velocityWalk(double ix, double iy, double it, double f);
		void littleWalk(double x, double y, double th);
		void gotoPosition(float target_x,float target_y, float target_phi);

		void calibrate();

		bool readConfiguration(const std::string& file_name); 		//this function reads team's configuration info from XML file
		bool readRobotConfiguration(const std::string& file_name); 	//this function reads robot's initial position in the field from XML file
		bool readGoalConfiguration(const std::string& file_name); 	//this function reads the position of the goals
		bool readOpenChallengeConf();

		void test();

		/* Incoming Messages */
		boost::shared_ptr<const AllSensorValuesMessage> allsm;
		boost::shared_ptr<const BallTrackMessage>  bmsg;
		boost::shared_ptr<const GameStateMessage>  gsm;
		//boost::shared_ptr<const ObservationMessage>  obsm;
		boost::shared_ptr<const ObstacleMessage>  om;
		boost::shared_ptr<const WorldInfo>  wim;

		/* Outgoing Messages */
		MotionWalkMessage* wmot;
		MotionHeadMessage* hmot;
		MotionActionMessage* amot;
		LocalizationResetMessage* locReset;

		int leftright;
		float headpos;

		short ballfound;
		bool scanforball;
		bool startscan;
		float targetYaw;
		float targetPitch;
		SensorData HeadYaw;
		SensorData HeadPitch;
		float psign,ysign;
		unsigned waiting;
		int calibrated;

		int forball, forpost;

		bool kickoff;
		float initX[2][2], initY[2][2], initPhi[2][2]; //initial game position in the field!!!!
		double blueGoalX, blueGoalY, yellowGoalX, yellowGoalY;
		double oppGoalX, oppGoalY, ownGoalX, ownGoalY;
		float cX, cY, ct;
		float bd, bb, bx, by, posx, posy;
		double epsx,epsy;
		int side;
		float robot_x,robot_y,robot_phi,robot_confidence;

		bool readytokick;
		int back;
		int direction;
		bool obstacleFront;
		double orientation;

		int gameState;
		int teamColor;
		int playerNumber;

		bool readRobotConf;

		
		bool wideWalk;
		boost::posix_time::ptime lastmove, lastball, lastwalk, lastplay, lastOpenConfRead, lastWideWalk;

		std::string _spName;
		std::string _wwName;
};

#endif

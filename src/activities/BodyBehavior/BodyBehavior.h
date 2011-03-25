#ifndef BODY_BEHAVIOR_H
#define BODY_BEHAVIOR_H

#include "architecture/IActivity.h"

#include "messages/motion.pb.h"
#include "messages/SensorsMessage.pb.h"
#include "messages/VisionObservations.pb.h"
#include "messages/Gamecontroller.pb.h"
#include "messages/ObstacleAvoidanceMessage.pb.h"
#include "messages/BehaviorMessages.pb.h"



#ifndef TO_RAD
#define TO_RAD 0.01745329f
#endif


#define DONOTHING 0
#define CALIBRATE 1
#define SCANFORBALL 2
#define SCANFORPOST 3
#define BALLTRACK 4

class BodyBehavior: public IActivity {

	public:
		BodyBehavior();
		int Execute();
		void UserInit();
		void read_messages();
		std::string GetName() {
			return "BodyBehavior";
		}

	private:

		short ballfound;
		bool scancompleted;

		MotionWalkMessage* wmot;
		MotionActionMessage* amot;
		BToHeadMessage* bhmsg;
		int pitchdirection;
		int yawdirection;
		SensorData HeadYaw;
		SensorData HeadPitch;

		boost::shared_ptr<const BallTrackMessage> bmsg;
		boost::shared_ptr<const GameStateMessage> gsm;
		boost::shared_ptr<const ObservationMessage> obsm;
		//	boost::shared_ptr<const ObstacleMessage>  om;
		boost::shared_ptr<const HeadToBMessage> hbm;
		boost::shared_ptr<const ScanMessage> scm;

		ObservationMessage* lastObsm;

		int calibrated;
		bool play;
		bool kickoff;
		bool isScaning;
		bool readytokick;
		int back;
		int direction;
		bool obstacleFront;
		int gameState;
		int oldGameState;
		int curraction;
		int prevaction;

		int teamColor;
		int playernum;
		int orientation;


		double mglRand();
		void velocityWalk(double x, double y, double th, double f);
		void littleWalk(double x, double y, double th, int s);
		bool readConfiguration(const std::string& file_name);
};

#endif

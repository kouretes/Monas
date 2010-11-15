#ifndef BODY_BEHAVIOR_H
#define BODY_BEHAVIOR_H

#include "architecture/IActivity.h"
#include "architecture/archConfig.h"
#include "architecture/narukom/pub_sub/publisher.h"

#include "messages/motion.pb.h"
#include "messages/SensorsMessage.pb.h"
#include "messages/VisionObservations.pb.h"
#include "messages/Gamecontroller.pb.h"
#include "messages/ObstacleAvoidanceMessage.pb.h"
#include "messages/HeadBehaviorMessage.pb.h"

#include "tools/logger.h"
#include "tools/toString.h"
#include "messages/RoboCupGameControlData.h"

#ifndef TO_RAD
#define TO_RAD 0.01745329f
#endif

#define LIMITUP -0.63
#define	LIMITDOWN 0.43
#define	LIMITLEFT 0.30
#define	LIMITRIGHT -0.30
#define STEPVER 0.35
#define STEPHOR 0.35

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
		bool turnforscan;
		bool scanforball;

		MotionWalkMessage* wmot;
		MotionActionMessage* amot;

		int pitchdirection;
		int yawdirection;
		SensorPair HeadYaw;
		SensorPair HeadPitch;

		boost::shared_ptr<const BallTrackMessage> bmsg;
		boost::shared_ptr<const GameStateMessage> gsm;
		boost::shared_ptr<const ObservationMessage> obsm;
		//	boost::shared_ptr<const ObstacleMessage>  om;
		boost::shared_ptr<const HeadBehaviorMessage> hbm;

		int calibrated;
		bool play;
		bool kickoff;

		bool readytokick;
		int back;
		int direction;
		bool turning;
		bool obstacleFront;
		int gameState;

		int teamColor;
		int playernum;
		int orientation;

		double mglRand();
		void velocityWalk(double x, double y, double th, double f);
		void littleWalk(double x, double y, double th, int s);
		bool readConfiguration(const std::string& file_name);
};

#endif

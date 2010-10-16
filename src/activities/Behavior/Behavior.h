#ifndef BEHAVIOR_H
#define BEHAVIOR_H

#include "architecture/IActivity.h"

#include "architecture/narukom/pub_sub/publisher.h"

#include "messages/motion.pb.h"
#include "messages/SensorsMessage.pb.h"
#include "messages/VisionObservations.pb.h"
#include "messages/Gamecontroller.pb.h"
#include "messages/ObstacleAvoidanceMessage.pb.h"

#include "alptr.h"

#ifndef TO_RAD
#define TO_RAD 0.01745329f
#endif

#define LIMITUP -0.63
#define	LIMITDOWN 0.43
#define	LIMITLEFT 0.30
#define	LIMITRIGHT -0.30
#define STEPVER 0.35
#define STEPHOR 0.35

namespace AL {
	class ALMotionProxy;
	class ALMemoryProxy;
}

class Behavior: public IActivity, public Publisher {

	public:
		Behavior();
		int Execute();
		void UserInit();
		void read_messages();
		int MakeTrackBallAction();
		void HeadScanStep();
		std::string GetName() {
			return "Behavior";
		}
		void mgltest();

	private:
		AL::ALPtr<AL::ALMotionProxy> motion;
		AL::ALPtr<AL::ALMemoryProxy> memory;
		short ballfound;
		MotionWalkMessage* wmot;
		MotionHeadMessage* hmot;
		MotionActionMessage* amot;

		int pitchdirection;
		int yawdirection;
		SensorPair HeadYaw;
		SensorPair HeadPitch;

		bool startscan;
		bool scanforball;
		short scandirectionpitch;
		short scandirectionyaw;

		bool reachedlimitup;
		bool reachedlimitdown;
		bool reachedlimitleft;
		bool reachedlimitright;

		short balllastseendirection;
		HeadJointSensorsMessage* hjsm;
		BallTrackMessage* bmsg;
		GameStateMessage* gsm;
		ObservationMessage* obsm;
		ObstacleMessageArray* om;
		int calibrated;
		bool play;
		bool kickoff;

		bool stopped;
		bool readytokick;
		int back;
		int direction;
		bool turning;
		int count;
		bool obstacleFront;
		int gameState;
		
		int teamColor; 
		int orientation; 
		
		double mglRand();
		void velocityWalk(double x, double y, double th, double f);
		void littleWalk(double x, double y, double th, int s);
		void calibrate();

};

#endif

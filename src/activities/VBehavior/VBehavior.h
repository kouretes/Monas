#ifndef VBEHAVIOR_H
#define VBEHAVIOR_H

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

#define PITCHMIN -0.35
#define	PITCHMAX 0.3
#define YAWMIN  1.2
#define	YAWMAX 1
#define PITCHSTEP 0.2
#define YAWSTEP 0.5

#define OVERSH 0.1
#define WAITFOR 20

namespace AL {
	class ALMotionProxy;
	class ALMemoryProxy;
}

class VBehavior: public IActivity {

	public:
		VBehavior();
		int Execute();
		void UserInit();
		void read_messages();
		int MakeTrackBallAction();
		void HeadScanStep();
		std::string GetName() {
			return "VBehavior";
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
		SensorData HeadYaw;
		SensorData HeadPitch;

		bool startscan;
		bool scanforball;
		float targetYaw;
		float targetPitch;
		float psign,ysign;
		unsigned waiting;

		short balllastseendirection;
		boost::shared_ptr<const AllSensorValues> allsm;
		boost::shared_ptr<const BallTrackMessage>  bmsg;
		boost::shared_ptr<const GameStateMessage>  gsm;
		boost::shared_ptr<const ObservationMessage>  obsm;
		boost::shared_ptr<const ObstacleMessage>  om;
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
		double orientation;

		double mglRand();
		void velocityWalk(double x, double y, double th, double f);
		void littleWalk(double x, double y, double th);
		void calibrate();
		float cX,cY,cth;//Commanded

};

#endif

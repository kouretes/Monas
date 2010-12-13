#ifndef VBEHAVIOR_H
#define VBEHAVIOR_H

#include "architecture/IActivity.h"

#include "architecture/narukom/pub_sub/publisher.h"

#include "messages/motion.pb.h"
#include "messages/SensorsMessage.pb.h"
#include "messages/VisionObservations.pb.h"
#include "messages/Gamecontroller.pb.h"
#include "messages/ObstacleAvoidanceMessage.pb.h"

#ifndef TO_RAD
#define TO_RAD 0.01745329f
#endif

#define LIMITUP -0.53
#define	LIMITDOWN 0.15
#define	LIMITLEFT 0.55
#define	LIMITRIGHT -0.55
#define STEPVER 0.34
#define STEPHOR 0.24


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
		boost::shared_ptr<const HeadJointSensorsMessage> hjsm;
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
		void littleWalk(double x, double y, double th, int s);
		void calibrate();
		float cX,cY,cth;//Commanded

};

#endif

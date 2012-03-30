#ifndef LBEHAVIOR_H
#define LBEHAVIOR_H

#include "architecture/IActivity.h"

#include "messages/motion.pb.h"
#include "messages/SensorsMessage.pb.h"
#include "messages/VisionObservations.pb.h"
#include "messages/Gamecontroller.pb.h"
#include "messages/ObstacleAvoidanceMessage.pb.h"
#include "messages/WorldInfo.pb.h"
#include "tools/XML.h"
#include "tools/XMLConfig.h"
#include "architecture/archConfig.h"

#include <boost/date_time/posix_time/posix_time.hpp>

using namespace boost::posix_time;

#ifndef TO_RAD
#define TO_RAD 0.01745329f
#endif

#define PITCHMIN -0.55
#define	PITCHMAX 0.33
#define YAWMIN 0.8
#define YAWMAX 1.35
#define PITCHSTEP 0.3
#define YAWSTEP 0.4

#define OVERSH 0.08
#define WAITFOR 40

//namespace AL {
//	class ALMotionProxy;
//	class ALMemoryProxy;
//}

class LBehavior: public IActivity {

	public:
		LBehavior();
		int Execute();
		void UserInit();
		void read_messages();
		int MakeTrackBallAction();
		void HeadScanStep();
		void ScanForGoalposts(int step);
		std::string GetName() {
			return "LBehavior";
		}
		void test();

	private:
		short ballfound;
		MotionWalkMessage* wmot;
		MotionHeadMessage* hmot;
		MotionActionMessage* amot;
		LocalizationResetMessage* locReset;

		float initX[2][2], initY[2][2], initPhi[2][2]; //initial game position in the field!!!!
		int playernum;
		bool readRobotConf;

		int pitchdirection;
		int yawdirection;

		SensorData HeadYaw;
		SensorData HeadPitch;

		bool startscan;
		bool scanforball;
		float targetYaw;
		float targetPitch;
		float psign,ysign;
		//bool pitchonly;
		unsigned waiting;

		short balllastseendirection;
		boost::shared_ptr<const AllSensorValuesMessage> allsm;
		boost::shared_ptr<const BallTrackMessage>  bmsg;
		boost::shared_ptr<const GameStateMessage>  gsm;
		//boost::shared_ptr<const ObservationMessage>  obsm;
		boost::shared_ptr<const ObstacleMessage>  om;
		boost::shared_ptr<const WorldInfo>  wim;
		boost::posix_time::ptime timeStart,timeStop;
		int step;

		bool scanForGoals;

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

		ptime lastball, lastwalk,lastrun,lastkick,wimtime;

		int leftright;
		float headpos;

		double Rand();
		void velocityWalk(double x, double y, double th, double f);
		void littleWalk(double x, double y, double th);
		void calibrate();

		bool readConfiguration(const std::string& file_name); //this function reads team's configuration info from XML file
		bool readRobotConfiguration(const std::string& file_name); //this function reads robot's initial position in the field from XML file
		void gotoPosition(float target_x,float target_y, float target_phi);
		float cX,cY,ct;//Commanded

};

#endif

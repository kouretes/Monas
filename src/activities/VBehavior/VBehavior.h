#ifndef VBEHAVIOR_H
#define VBEHAVIOR_H

#include "architecture/IActivity.h"

#include "architecture/narukom/pub_sub/publisher.h"

#include "messages/motion.pb.h"
#include "messages/SensorsMessage.pb.h"
#include "messages/VisionObservations.pb.h"
#include "messages/Gamecontroller.pb.h"
#include "messages/ObstacleAvoidanceMessage.pb.h"
#include "tools/XML.h"
#include "tools/XMLConfig.h"
#include "architecture/archConfig.h"

#ifndef TO_RAD
#define TO_RAD 0.01745329f
#endif

#define PITCHMIN -0.55
#define	PITCHMAX 0.33
#define YAWMIN 0.8
#define YAWMAX 1.35
#define PITCHSTEP 0.3
#define YAWSTEP 0.4

#define OVERSH 0.06
#define WAITFOR 40

//namespace AL {
//	class ALMotionProxy;
//	class ALMemoryProxy;
//}

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
		//////////////////////added by Aggeliki
		float initX, initY, initPhi; //initial game position in the field!!!!
		int playernum;
		bool readRobotConf;
		//////////////////////////
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
		///////////////added by Aggeliki
		bool readConfiguration(const std::string& file_name); //this function reads team's configuration info from XML file
		bool readRobotConfiguration(const std::string& file_name, bool kickoff); //this function reads robot's initial position in the field from XML file
		///////////////////////////////////////////////
		float cX,cY,cth;//Commanded

};

#endif

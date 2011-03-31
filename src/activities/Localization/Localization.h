#ifndef Localization_H
#define Localization_H

#include "architecture/IActivity.h"

#include "messages/SensorsMessage.pb.h"
#include "messages/VisionObservations.pb.h"
#include "messages/Gamecontroller.pb.h"
#include "messages/WorldInfo.pb.h"
#include "messages/motion.pb.h"
#include "messages/BehaviorMessages.pb.h"

#include "KLocalization.h"
#include "BallFilter.h"
#include "PracticalSocket.h"
#include <string>
class Localization: public IActivity, public KLocalization
{

	public:
		Localization();
		~Localization()
		{
			if (serverpid != -1)
				pthread_cancel(serverpid);
			delete sock;
		}
		int Execute();
		void UserInit();
		void process_messages();
		belief LocalizationStepSIR(KMotionModel & MotionModel, vector<KObservationModel> & Observations, double rangemaxleft, double rangemaxright);
		void RobotPositionMotionModel(KMotionModel & MModel);
		std::string GetName()
		{
			return "Localization";
		}

	private:

		int count;
		int serverpid;
		WorldInfo MyWorld;

		//RtTime rtm;
		belief mypos;

		vector<KObservationModel> currentObservation;
		KMotionModel robotmovement;

		partcl TrackPoint;
		partcl TrackPointRobotPosition;

		int leftright;
		float headpos;
		LocalizationData DebugData;
		float maxrangeleft;
		float maxrangeright;
		SensorData PosX;
		SensorData PosY;
		SensorData Angle;

		SensorData HeadYaw;
		SensorData HeadPitch;
		BallTrackMessage lastballseen;
		//BallFilter myBall;

		MotionHeadMessage hmot;
		MotionWalkMessage wmot;
		partcl target;

		boost::shared_ptr<const GameStateMessage> gsm;
		boost::shared_ptr<const ObservationMessage> obsm;
		boost::shared_ptr<const RobotPositionMessage> rpsm;

		BToHeadMessage* bhmsg;
		//bool firstrun;

		//boost::posix_time::ptime time;
		//boost::posix_time::time_duration duration;

		//For Debug!
		void SimpleBehaviorStep();
		static void * StartServer(void * kati);
		pthread_t acceptthread;
		static TCPSocket *sock;

		int LocalizationData_Load(parts & Particles, vector<KObservationModel> & Observation, KMotionModel & MotionModel);
		void Send_LocalizationData();
		int DebugMode_Receive();
		static bool debugmode;

		header incommingheader;
		header outgoingheader;

		int size;
		char *data;

};

#endif

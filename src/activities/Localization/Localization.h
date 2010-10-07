#ifndef Localization_H
#define Localization_H

#include "architecture/IActivity.h"

#include "architecture/narukom/pub_sub/publisher.h"

#include "messages/SensorsMessage.pb.h"
#include "messages/VisionObservations.pb.h"
#include "messages/Gamecontroller.pb.h"
#include "messages/WorldInfo.pb.h"

#include "KLocalization.h"
#include "Kutils.h"

#include <udt.h>
#include <string>
class Localization: public IActivity, public Publisher, public KLocalization {

	public:
		Localization();
		int Execute();
		void UserInit();
		void read_messages();
		belief LocalizationStepSIR(KMotionModel & MotionModel, vector<KObservationModel> & Observations, double rangemaxleft, double rangemaxright) ;
		void RobotPositionMotionModel(KMotionModel & MModel);
		std::string GetName() {
			return "Localization";
		}

	private:

		int count;

		WorldInfo MyWorld;

		//RtTime rtm;
		belief mypos;

		vector<KObservationModel> currentObservation;
		KMotionModel robotmovement;

		partcl TrackPoint;
		partcl TrackPointRobotPosition;

		LocalizationData DebugData;
		float maxrangeleft;
		float maxrangeright;

		SensorPair PosX;
		SensorPair PosY;
		SensorPair Angle;

		SensorPair HeadYaw;
		SensorPair HeadPitch;
		BallTrackMessage lastballseen;

		GameStateMessage* gsm;
		ObservationMessage* obsm;
		RobotPositionSensorMessage* rpsm;

		//For Debug!
		static void * StartServer(void * kati);
		pthread_t acceptthread;
		static UDTSOCKET recver;

		int LocalizationData_Load(parts & Particles, vector<KObservationModel> & Observation,KMotionModel & MotionModel );
		void Send_LocalizationData();

		static bool debugmode;

		header incommingheader;
		header outgoingheader;

		int size;
		char *data;

};

#endif

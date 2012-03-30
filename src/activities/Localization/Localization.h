#ifndef Localization_H
#define Localization_H

#include "architecture/IActivity.h"

#include "messages/SensorsMessage.pb.h"
#include "messages/VisionObservations.pb.h"
#include "messages/Gamecontroller.pb.h"
#include "messages/WorldInfo.pb.h"
#include "messages/motion.pb.h"
#include <boost/date_time/posix_time/posix_time.hpp>
//#define KPROFILING_ENABLED

#include "tools/profiler.hpp"
#include "BallFilter.h"
#include "KLocalization.h"
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
	void Reset(int,bool,bool);
	void process_messages();
	belief LocalizationStepSIR(KMotionModel & MotionModel, vector<KObservationModel> & Observations, vector<KObservationModel> & AmbigiusObservations, double rangemaxleft, double rangemaxright);
	void RobotPositionMotionModel(KMotionModel & MModel);
	std::string GetName()
	{
		return "Localization";
	}
	void calculate_ball_estimate(KMotionModel const & MModel);
private:

	int count;
	int serverpid;
	WorldInfo MyWorld;
	mutable KProfiling::profiler vprof;

	//RtTime rtm;
	belief mypos;

	vector<KObservationModel> currentObservation;
	vector<KObservationModel> currentAbigiusObservation;
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
	BallFilter myBall;

	MotionHeadMessage hmot;
	MotionWalkMessage wmot;
	partcl target;

	boost::shared_ptr<const GameStateMessage> gsm;
	boost::shared_ptr<const ObservationMessage> obsm;
	boost::shared_ptr<const RobotPositionMessage> rpsm;
	boost::shared_ptr<const LocalizationResetMessage> lrm;
	boost::shared_ptr<const MotionStateMessage> sm;

	boost::posix_time::ptime timeStart,timeStop;

	bool firstrun;
	MotionStateMessage::ActionType currentRobotAction;
	bool fallBegan;
	
	boost::posix_time::ptime last_observation_time;
	boost::posix_time::ptime last_filter_time;
	boost::posix_time::ptime now;

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

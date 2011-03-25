#ifndef SENSORS_H
#define SENSORS_H
#include "architecture/narukom/pub_sub/publisher.h"


#include "hal/robot/generic_nao/aldebaran-sensors.h"
#include "messages/SensorsMessage.pb.h"
#include <vector>
#include <map>
#include <string>

#include "architecture/IActivity.h"
#include "tools/stat/cumulative.h"
#include "hal/robot/generic_nao/robot_consts.h"

//#define NUMBER_OF_SENSORS 46//TODO Please check the number devices
// Use DCM proxy

//#define USE_POINTERS


class Sensors: public IActivity, public Publisher/*, public Subscriber*/
{
	public:
		Sensors();
		int Execute();

		void UserInit();
		std::string GetName()
		{
			return "Sensors";
		}
		~Sensors()
		{
			AL::ALValue commands;

			commands.arraySetSize(3);
			commands[0] = std::string("Device/SubDeviceList/US/Actuator/Value");
			commands[1] = std::string("Merge");
			commands[2].arraySetSize(1);
			commands[2][0].arraySetSize(2);
			commands[2][0][0] = 0.0;
			commands[2][0][1] = dcm->getTime(1000);

			dcm->set(commands);

		}
		RobotPositionMessage RPM;

		AllSensorValuesMessage ASM;

	private:
		AL::ALPtr<AL::DCMProxy> dcm;
		AL::ALPtr<AL::ALMotionProxy> motion;
		AL::ALPtr<AL::ALMemoryProxy> memory;
		//	AL::ALPtr<AL::ALMemoryFastAccess> MemoryFastAccess;

		void initialisation();

		std::vector<std::string> jointKeys,sensorKeys;
		sample_counter sc;
		cumulative_central_moments<float> gyravg[KDeviceLists::GYR_SIZE];
		cumulative_central_moments<float> accnorm;



#ifdef KROBOT_IS_REMOTE_OFF
		void initFastAccess();
		void synchronisedDCMcallback();
		std::vector<float *> jointPtr,sensorPtr;//Used by DCM callbacks
		RtTime rtmfast;
		unsigned int timedifffast;
#else

	std::vector<float> jointValues,sensorValues;

#endif /* KROBOT_IS_REMOTE_OFF*/
	RtTime rtm;
	unsigned int timediff;

	float smoothness; //sensordata = 90%*value + 10%*oldvalue

};

#endif


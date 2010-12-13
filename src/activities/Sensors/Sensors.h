#ifndef SENSORS_H
#define SENSORS_H
#include "architecture/narukom/pub_sub/publisher.h"

#include "hal/robot/generic_nao/robot_consts.h"
#include "hal/robot/aldebaran-sensors.h"
#include "messages/SensorsMessage.pb.h"
#include <vector>
#include <map>
#include <string>

#include "architecture/IActivity.h"

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

		InertialSensorsMessage ISM;
		FSRSensorsMessage FSM;
		UltaSoundSensorsMessage USSM;
		HeadJointSensorsMessage HJSM;
		BodyJointSensorsMessage BJSM;
		RobotPositionSensorMessage RPSM;

		AllSensorValues ASM;

	private:
		AL::ALPtr<AL::DCMProxy> dcm;
		AL::ALPtr<AL::ALMotionProxy> motion;
		AL::ALPtr<AL::ALMemoryProxy> memory;
		//	AL::ALPtr<AL::ALMemoryFastAccess> MemoryFastAccess;

		void initialisation();
		void initFastAccess();
		void synchronisedDCMcallback();

		std::map<DeviceNames, float *> SensorDataPtr;

		std::map<std::string, std::vector<std::string> > devicesInChains;
		std::map<std::string, std::vector<std::string> > devicesNames;

		std::map<std::string, std::vector<float> > Values;

		RtTime rtm;
		RtTime rtmfast;
		unsigned int timediff;
		unsigned int timedifffast;

		unsigned int period;

		float smoothness; //sensordata = 90%*value + 10%*oldvalue
		//Indexing according initialization
		std::vector<float> devicesValues;
		template<typename T> void fillSensorMsg(T &msg, int start, int end);
};

#endif


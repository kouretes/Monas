#ifndef SENSORS_H
#define SENSORS_H
#include "architecture/narukom/pub_sub/publisher.h"
#include "messages/TestMessage.pb.h"

#include "messages/SensorsMessage.pb.h"
#include <vector>
#include <map>
#include <string>

#include "architecture/IActivity.h"
#include "almotionproxy.h"
#include "almemoryproxy.h"
#include "alptr.h"
#include "alxplatform.h"
#include <albroker.h>
#include <alproxy.h>

#include <rttime.h>
#include "dcmproxy.h"

//#define NUMBER_OF_SENSORS 46//TODO Please check the number devices
// Use DCM proxy

//#define USE_POINTERS


class Sensors: public IActivity/*, public Subscriber*/{
	public:
		Sensors();
		int Execute();
		void process_messages();
		void UserInit();
		std::string GetName() {
			return "Sensors";
		}
		~Sensors() {
			;

			AL::ALValue commands;

			commands.arraySetSize(3);
			commands[0] = std::string("Device/SubDeviceList/US/Actuator/Value");
			commands[1] = std::string("Merge");
			commands[2].arraySetSize(1);
			commands[2][0].arraySetSize(2);
			commands[2][0][0] = 0.0;
			commands[2][0][1] = dcm->getTime(10);

			dcm->set(commands);

		}

		InertialSensorsMessage ISM;
		FSRSensorsMessage FSM;
		UltaSoundSensorsMessage USSM;
		HeadJointSensorsMessage HJSM;
		BodyJointSensorsMessage BJSM;
		RobotPositionSensorMessage RPSM;

	private:
		AL::ALPtr<AL::DCMProxy> dcm;
		AL::ALPtr<AL::ALMotionProxy> motion;
		AL::ALPtr<AL::ALMemoryProxy> memory;

		void initialisation();

		std::map<std::string, std::vector<float *> > SensorDataPtr;

		std::map<std::string, std::vector<std::string> > devicesInChains;
		std::map<std::string, std::vector<std::string> > devicesNames;

		std::map<std::string, std::vector<float> > Values;

		RtTime rtm;
		unsigned int timediff;

		unsigned int period;

		float smoothness; //sensordata = 90%*value + 10%*oldvalue
		//Indexing according initialization
		std::vector<float> devicesValues;
};

#endif


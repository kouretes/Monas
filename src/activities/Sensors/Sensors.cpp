#include "Sensors.h"

#include "hal/robot/generic_nao/kAlBroker.h"

#include "tools/logger.h"
#include "tools/toString.h"

using namespace AL;
using namespace std;
#define MODULO 2

namespace {
	ActivityRegistrar<Sensors>::Type temp("Sensors");
}

Sensors::Sensors() :
	Publisher("Sensors")//,Subscriber("SensorsController")
{
	;
}

void Sensors::UserInit() {
	Logger::Instance().WriteMsg("Sensors", "Sensors UserInit", Logger::Info);

	try {
		dcm = KAlBroker::Instance().GetBroker()->getDcmProxy();
	} catch (AL::ALError& e) {
		Logger::Instance().WriteMsg("Sensors", "Error in getting dcm proxy", Logger::FatalError);
	}

	try {
		memory = KAlBroker::Instance().GetBroker()->getMemoryProxy();
	} catch (AL::ALError& e) {
		Logger::Instance().WriteMsg("Sensors", "Error in getting ALmemory proxy", Logger::FatalError);
	}

	try {
		motion = KAlBroker::Instance().GetBroker()->getMotionProxy();
	} catch (AL::ALError& e) {
		Logger::Instance().WriteMsg("Sensors", "Error in getting motion proxy", Logger::FatalError);
	}

	_com->get_message_queue()->add_publisher(this);

	//Starting US Sensors
	ALValue commands;

	commands.arraySetSize(3);
	commands[0] = string("Device/SubDeviceList/US/Actuator/Value");
	commands[1] = string("Merge");
	commands[2].arraySetSize(1);
	commands[2][0].arraySetSize(2);
	commands[2][0][0] = 68.0;
	commands[2][0][1] = dcm->getTime(10);

	dcm->set(commands);

	initialisation();
	rtm.start();
	period = 0;
	Logger::Instance().WriteMsg("Sensors", "Sensor Controller Initialized", Logger::Info);
	//cout << "Sensor Controller Initialized" << endl;
}

int Sensors::Execute() {

	unsigned int counter = 0;
	unsigned int i = 0;
	unsigned int j = 0;
	Values["Head"] = memory->getListData(devicesInChains["Head"]);
	Values["LeftLeg"] = memory->getListData(devicesInChains["LeftLeg"]);
	Values["RightLeg"] = memory->getListData(devicesInChains["RightLeg"]);
	Values["Inertial"] = memory->getListData(devicesInChains["Inertial"]);
	rtm.stop();
	if (period % MODULO == 0) {

		Values["FSR"] = memory->getListData(devicesInChains["FSR"]);
		Values["USoundLeft"] = memory->getListData(devicesInChains["USoundLeft"]);
		Values["USoundRight"] = memory->getListData(devicesInChains["USoundRight"]);
		Values["RobotPosition"] = motion->getRobotPosition(true);
	}
	Values["RightArm"] = memory->getListData(devicesInChains["RightArm"]);
	Values["LeftArm"] = memory->getListData(devicesInChains["LeftArm"]);

	timediff = rtm.diffNs();
	rtm.start();
	for (i = 0; i < devicesInChains["Head"].size(); i++) { //Values["Head"][i] = *(SensorDataPtr["Head"][i]);
		HJSM.mutable_sensordata(i)->set_sensorvalue(Values["Head"][i]);
		HJSM.mutable_sensordata(i)->set_sensorvaluediff(Values["Head"][i] - devicesValues[counter]);
		HJSM.mutable_sensordata(i)->set_sensortimediff(timediff);
		devicesValues[counter] = Values["Head"][i];
		counter++;
	}

	j = 0;
	for (i = 0; i < devicesInChains["LeftArm"].size(); i++) {
		BJSM.mutable_sensordata(j)->set_sensorvalue(Values["LeftArm"][i]);

		BJSM.mutable_sensordata(j)->set_sensorvaluediff(Values["LeftArm"][i] - devicesValues[counter]);
		BJSM.mutable_sensordata(j)->set_sensortimediff(timediff);
		devicesValues[counter] = Values["LeftArm"][i];
		counter++;
		j++;
	}

	for (i = 0; i < devicesInChains["LeftLeg"].size(); i++) {
		BJSM.mutable_sensordata(j)->set_sensorvalue(Values["LeftLeg"][i]);

		BJSM.mutable_sensordata(j)->set_sensorvaluediff(Values["LeftLeg"][i] - devicesValues[counter]);
		BJSM.mutable_sensordata(j)->set_sensortimediff(timediff);
		devicesValues[counter] = Values["LeftLeg"][i];
		counter++;
		j++;
	}

	for (i = 0; i < devicesInChains["RightLeg"].size(); i++) {
		BJSM.mutable_sensordata(j)->set_sensorvalue(Values["RightLeg"][i]);

		BJSM.mutable_sensordata(j)->set_sensorvaluediff(Values["RightLeg"][i] - devicesValues[counter]);
		BJSM.mutable_sensordata(j)->set_sensortimediff(timediff);
		devicesValues[counter] = Values["RightLeg"][i];
		counter++;
		j++;
	}

	for (i = 0; i < devicesInChains["RightArm"].size(); i++) {
		BJSM.mutable_sensordata(j)->set_sensorvalue(Values["RightArm"][i]);

		BJSM.mutable_sensordata(j)->set_sensorvaluediff(Values["RightArm"][i] - devicesValues[counter]);
		BJSM.mutable_sensordata(j)->set_sensortimediff(timediff);
		devicesValues[counter] = Values["RightArm"][i];
		counter++;
		j++;
	}
	publish(&HJSM, "sensors");
	publish(&BJSM, "sensors");
	//	for (unsigned int i = 0; i < devicesInChains["Body"].size(); i++) {
	//		BJSM.mutable_sensordata(i)->set_sensorname(devicesNames["Body"][i]);
	//		BJSM.mutable_sensordata(i)->set_sensorvalue(Values[i]);
	//		BJSM.mutable_sensordata(i)->set_sensorvaluediff(Values[i] - devicesValues[counter]);
	//		BJSM.mutable_sensordata(i)->set_sensortimediff(timediff);
	//		devicesValues[counter] = Values[i];
	//		counter++;
	//	}
	for (i = 0; i < devicesInChains["Inertial"].size(); i++) {
		ISM.mutable_sensordata(i)->set_sensorvalue(Values["Inertial"][i]);
		ISM.mutable_sensordata(i)->set_sensorvaluediff(Values["Inertial"][i] - devicesValues[counter]);
		ISM.mutable_sensordata(i)->set_sensortimediff(timediff);
		devicesValues[counter] = Values["Inertial"][i];
		counter++;
	}
	publish(&ISM, "sensors");

	if (period % MODULO == 0) {

		for (i = 0; i < devicesInChains["FSR"].size(); i++) {
			FSM.mutable_sensordata(i)->set_sensorvalue(Values["FSR"][i]);
			FSM.mutable_sensordata(i)->set_sensorvaluediff(Values["FSR"][i] - devicesValues[counter]);
			FSM.mutable_sensordata(i)->set_sensortimediff(timediff);
			devicesValues[counter] = Values["FSR"][i];
			counter++;
		}

		for (i = 0; i < devicesInChains["USoundLeft"].size(); i++) {
			USSM.mutable_sensordata(i)->set_sensorvalue(Values["USoundLeft"][i]);
			USSM.mutable_sensordata(i)->set_sensorvaluediff(Values["USoundLeft"][i] - devicesValues[counter]);
			USSM.mutable_sensordata(i)->set_sensortimediff(timediff);
			devicesValues[counter] = Values["USoundLeft"][i];
			counter++;
		}

		for (j = 0; j < devicesInChains["USoundRight"].size(); j++) {
			USSM.mutable_sensordata(i)->set_sensorvalue(Values["USoundRight"][j]);
			USSM.mutable_sensordata(i)->set_sensorvaluediff(Values["USoundRight"][j] - devicesValues[counter]);
			USSM.mutable_sensordata(i)->set_sensortimediff(timediff);
			devicesValues[counter] = Values["USoundRight"][j];
			counter++;
			i++;
		}

		publish(&FSM, "sensors");
		publish(&USSM, "sensors");
		//
		//		//A vector containing the World Absolute Robot Position. (Absolute Position X, Absolute Position Y, Absolute Angle Z)
		for (unsigned int i = 0; i < Values["RobotPosition"].size(); i++) {
			//RPSM.mutable_sensordata(i)->set_sensorname(devicesNames["RobotPosition"][i]);
			RPSM.mutable_sensordata(i)->set_sensorvalue(Values["RobotPosition"][i]);
			RPSM.mutable_sensordata(i)->set_sensorvaluediff(Values["RobotPosition"][i] - devicesValues[counter]);
			RPSM.mutable_sensordata(i)->set_sensortimediff(timediff);
			devicesValues[counter] = Values["RobotPosition"][i];
			counter++;
		}
		////
		publish(&RPSM, "sensors");
		period = 0;
	}
	////
	period++;

	//cout << "Sended Values" << endl;
	//cout << "Sensors: Counter " << counter << endl;

	return 0;

}

void Sensors::initialisation() {
	unsigned int i, j;

	HJSM.set_topic("sensors"); //HeadJointSensorsMessage
	HJSM.set_timeout(250);

	BJSM.set_topic("sensors"); //BodyJointSensorsMessage
	BJSM.set_timeout(250);

	ISM.set_topic("sensors"); //InertialSensorsMessage
	ISM.set_timeout(250);

	FSM.set_topic("sensors"); //FSRSensorsMessage
	FSM.set_timeout(250);

	USSM.set_topic("sensors"); //UltraSoundSensorsMessage
	USSM.set_timeout(250);

	RPSM.set_topic("sensors"); //RobotPositionMessage
	RPSM.set_timeout(250);

	RPSM.add_sensordata(); //X
	RPSM.add_sensordata(); //Y
	RPSM.add_sensordata(); //Angle

	devicesNames["RobotPosition"].push_back(std::string("X"));
	devicesNames["RobotPosition"].push_back(std::string("Y"));
	devicesNames["RobotPosition"].push_back(std::string("Angle"));

	devicesInChains["Head"].push_back(std::string("Device/SubDeviceList/HeadYaw/Position/Sensor/Value"));
	devicesInChains["Head"].push_back(std::string("Device/SubDeviceList/HeadPitch/Position/Sensor/Value"));
	///SMALL STRING NAMES
	devicesNames["Head"].push_back(std::string("HeadYaw"));
	devicesNames["Head"].push_back(std::string("HeadPitch"));

	for (i = 0; i < devicesInChains["Head"].size(); i++) {
		HJSM.add_sensordata();
		//HJSM.mutable_sensordata(i)->set_sensorname(devicesNames["Head"][i]);

		//SensorDataPtr["Head"].push_back((float *) memory->getDataPtr(devicesInChains["Head"][i]));
		//cout << " dsfsdf " << (float)(*(SensorDataPtr["Head"][i])) << endl;
		//Values["Head"].push_back(*(SensorDataPtr["Head"][i]));
	}

	devicesInChains["LeftArm"].push_back(std::string("Device/SubDeviceList/LShoulderPitch/Position/Sensor/Value"));
	devicesInChains["LeftArm"].push_back(std::string("Device/SubDeviceList/LShoulderRoll/Position/Sensor/Value"));
	devicesInChains["LeftArm"].push_back(std::string("Device/SubDeviceList/LElbowYaw/Position/Sensor/Value"));
	devicesInChains["LeftArm"].push_back(std::string("Device/SubDeviceList/LElbowRoll/Position/Sensor/Value"));
	devicesInChains["LeftLeg"].push_back(std::string("Device/SubDeviceList/LHipYawPitch/Position/Sensor/Value"));
	devicesInChains["LeftLeg"].push_back(std::string("Device/SubDeviceList/LHipPitch/Position/Sensor/Value"));
	devicesInChains["LeftLeg"].push_back(std::string("Device/SubDeviceList/LHipRoll/Position/Sensor/Value"));
	devicesInChains["LeftLeg"].push_back(std::string("Device/SubDeviceList/LKneePitch/Position/Sensor/Value"));
	devicesInChains["LeftLeg"].push_back(std::string("Device/SubDeviceList/LAnklePitch/Position/Sensor/Value"));
	devicesInChains["LeftLeg"].push_back(std::string("Device/SubDeviceList/LAnkleRoll/Position/Sensor/Value"));
	devicesInChains["RightLeg"].push_back(std::string("Device/SubDeviceList/RHipYawPitch/Position/Sensor/Value"));
	devicesInChains["RightLeg"].push_back(std::string("Device/SubDeviceList/RHipPitch/Position/Sensor/Value"));
	devicesInChains["RightLeg"].push_back(std::string("Device/SubDeviceList/RHipRoll/Position/Sensor/Value"));
	devicesInChains["RightLeg"].push_back(std::string("Device/SubDeviceList/RKneePitch/Position/Sensor/Value"));
	devicesInChains["RightLeg"].push_back(std::string("Device/SubDeviceList/RAnklePitch/Position/Sensor/Value"));
	devicesInChains["RightLeg"].push_back(std::string("Device/SubDeviceList/RAnkleRoll/Position/Sensor/Value"));
	devicesInChains["RightArm"].push_back(std::string("Device/SubDeviceList/RShoulderPitch/Position/Sensor/Value"));
	devicesInChains["RightArm"].push_back(std::string("Device/SubDeviceList/RShoulderRoll/Position/Sensor/Value"));
	devicesInChains["RightArm"].push_back(std::string("Device/SubDeviceList/RElbowYaw/Position/Sensor/Value"));
	devicesInChains["RightArm"].push_back(std::string("Device/SubDeviceList/RElbowRoll/Position/Sensor/Value"));

	//devicesInChains["Body"].push_back(std::string("Device/SubDeviceList/HeadYaw/Position/Sensor/Value"));
	//devicesInChains["Body"].push_back(std::string("Device/SubDeviceList/HeadPitch/Position/Sensor/Value"));
	devicesInChains["Body"].push_back(std::string("Device/SubDeviceList/LShoulderPitch/Position/Sensor/Value"));
	devicesInChains["Body"].push_back(std::string("Device/SubDeviceList/LShoulderRoll/Position/Sensor/Value"));
	devicesInChains["Body"].push_back(std::string("Device/SubDeviceList/LElbowYaw/Position/Sensor/Value"));
	devicesInChains["Body"].push_back(std::string("Device/SubDeviceList/LElbowRoll/Position/Sensor/Value"));
	devicesInChains["Body"].push_back(std::string("Device/SubDeviceList/LHipYawPitch/Position/Sensor/Value"));
	devicesInChains["Body"].push_back(std::string("Device/SubDeviceList/LHipPitch/Position/Sensor/Value"));
	devicesInChains["Body"].push_back(std::string("Device/SubDeviceList/LHipRoll/Position/Sensor/Value"));
	devicesInChains["Body"].push_back(std::string("Device/SubDeviceList/LKneePitch/Position/Sensor/Value"));
	devicesInChains["Body"].push_back(std::string("Device/SubDeviceList/LAnklePitch/Position/Sensor/Value"));
	devicesInChains["Body"].push_back(std::string("Device/SubDeviceList/LAnkleRoll/Position/Sensor/Value"));
	devicesInChains["Body"].push_back(std::string("Device/SubDeviceList/RHipYawPitch/Position/Sensor/Value"));
	devicesInChains["Body"].push_back(std::string("Device/SubDeviceList/RHipPitch/Position/Sensor/Value"));
	devicesInChains["Body"].push_back(std::string("Device/SubDeviceList/RHipRoll/Position/Sensor/Value"));
	devicesInChains["Body"].push_back(std::string("Device/SubDeviceList/RKneePitch/Position/Sensor/Value"));
	devicesInChains["Body"].push_back(std::string("Device/SubDeviceList/RAnklePitch/Position/Sensor/Value"));
	devicesInChains["Body"].push_back(std::string("Device/SubDeviceList/RAnkleRoll/Position/Sensor/Value"));
	devicesInChains["Body"].push_back(std::string("Device/SubDeviceList/RShoulderPitch/Position/Sensor/Value"));
	devicesInChains["Body"].push_back(std::string("Device/SubDeviceList/RShoulderRoll/Position/Sensor/Value"));
	devicesInChains["Body"].push_back(std::string("Device/SubDeviceList/RElbowYaw/Position/Sensor/Value"));
	devicesInChains["Body"].push_back(std::string("Device/SubDeviceList/RElbowRoll/Position/Sensor/Value"));

	Logger::Instance().WriteMsg("Sensors", "Size of devicesInChains[\"Body\"]: " + _toString(devicesInChains["Body"].size()), Logger::Info);
	//cout << "Size of devicesInChains[\"Body\"]: " << devicesInChains["Body"].size() << endl;


	devicesNames["LeftArm"].push_back(std::string("LShoulderPitch"));
	devicesNames["LeftArm"].push_back(std::string("LShoulderRoll"));
	devicesNames["LeftArm"].push_back(std::string("LElbowYaw"));
	devicesNames["LeftArm"].push_back(std::string("LElbowRoll"));
	devicesNames["LeftLeg"].push_back(std::string("LHipYawPitch"));
	devicesNames["LeftLeg"].push_back(std::string("LHipPitch"));
	devicesNames["LeftLeg"].push_back(std::string("LHipRoll"));
	devicesNames["LeftLeg"].push_back(std::string("LKneePitch"));
	devicesNames["LeftLeg"].push_back(std::string("LAnklePitch"));
	devicesNames["LeftLeg"].push_back(std::string("LAnkleRoll"));
	devicesNames["RightLeg"].push_back(std::string("RHipYawPitch"));
	devicesNames["RightLeg"].push_back(std::string("RHipPitch"));
	devicesNames["RightLeg"].push_back(std::string("RHipRoll"));
	devicesNames["RightLeg"].push_back(std::string("RKneePitch"));
	devicesNames["RightLeg"].push_back(std::string("RAnklePitch"));
	devicesNames["RightLeg"].push_back(std::string("RAnkleRoll"));
	devicesNames["RightArm"].push_back(std::string("RShoulderPitch"));
	devicesNames["RightArm"].push_back(std::string("RShoulderRoll"));
	devicesNames["RightArm"].push_back(std::string("RElbowYaw"));
	devicesNames["RightArm"].push_back(std::string("RElbowRoll"));

	//devicesNames["Body"].push_back(std::string("HeadYaw"));
	//devicesNames["Body"].push_back(std::string("HeadPitch"));
	devicesNames["Body"].push_back(std::string("LShoulderPitch"));
	devicesNames["Body"].push_back(std::string("LShoulderRoll"));
	devicesNames["Body"].push_back(std::string("LElbowYaw"));
	devicesNames["Body"].push_back(std::string("LElbowRoll"));
	devicesNames["Body"].push_back(std::string("LHipYawPitch"));
	devicesNames["Body"].push_back(std::string("LHipPitch"));
	devicesNames["Body"].push_back(std::string("LHipRoll"));
	devicesNames["Body"].push_back(std::string("LKneePitch"));
	devicesNames["Body"].push_back(std::string("LAnklePitch"));
	devicesNames["Body"].push_back(std::string("LAnkleRoll"));
	devicesNames["Body"].push_back(std::string("RHipYawPitch"));
	devicesNames["Body"].push_back(std::string("RHipPitch"));
	devicesNames["Body"].push_back(std::string("RHipRoll"));
	devicesNames["Body"].push_back(std::string("RKneePitch"));
	devicesNames["Body"].push_back(std::string("RAnklePitch"));
	devicesNames["Body"].push_back(std::string("RAnkleRoll"));
	devicesNames["Body"].push_back(std::string("RShoulderPitch"));
	devicesNames["Body"].push_back(std::string("RShoulderRoll"));
	devicesNames["Body"].push_back(std::string("RElbowYaw"));
	devicesNames["Body"].push_back(std::string("RElbowRoll"));

	//	for (unsigned int i = 0; i < devicesInChains["Body"].size(); i++) {
	//		BJSM.add_sensordata();
	//		//BE CAREFULL THE DEVICES INDEX MUST BE IN THE SAME ORDER
	//		BJSM.mutable_sensordata(j)->set_sensorname(devicesNames["Body"][i]);
	//
	//		//	SensorDataPtr["Body"].push_back((float *) memory->getDataPtr(devicesInChains["Body"][i]));
	//		//	Values["Body"].push_back(*(SensorDataPtr["Body"][i]));
	//	}
	//BE CAREFULL THE DEVICES INDEX MUST BE IN THE SAME ORDER
	j = 0;
	for (i = 0; i < devicesInChains["LeftArm"].size(); i++) {
		BJSM.add_sensordata();
		//BJSM.mutable_sensordata(j)->set_sensorname(devicesNames["LeftArm"][i]);
		j++;
	}
	for (i = 0; i < devicesInChains["LeftLeg"].size(); i++) {
		BJSM.add_sensordata();
		//BJSM.mutable_sensordata(j)->set_sensorname(devicesNames["LeftLeg"][i]);
		j++;
	}
	for (i = 0; i < devicesInChains["RightLeg"].size(); i++) {
		BJSM.add_sensordata();
		//BJSM.mutable_sensordata(j)->set_sensorname(devicesNames["RightLeg"][i]);
		j++;
	}
	for (i = 0; i < devicesInChains["RightArm"].size(); i++) {
		BJSM.add_sensordata();
		//BJSM.mutable_sensordata(j)->set_sensorname(devicesNames["RightArm"][i]);
		j++;
	}

	devicesInChains["Inertial"].push_back(std::string("Device/SubDeviceList/InertialSensor/AccX/Sensor/Value"));
	devicesInChains["Inertial"].push_back(std::string("Device/SubDeviceList/InertialSensor/AccY/Sensor/Value"));
	devicesInChains["Inertial"].push_back(std::string("Device/SubDeviceList/InertialSensor/AccZ/Sensor/Value"));
	devicesInChains["Inertial"].push_back(std::string("Device/SubDeviceList/InertialSensor/GyrX/Sensor/Value"));
	devicesInChains["Inertial"].push_back(std::string("Device/SubDeviceList/InertialSensor/GyrY/Sensor/Value"));
	devicesInChains["Inertial"].push_back(std::string("Device/SubDeviceList/InertialSensor/AngleX/Sensor/Value"));
	devicesInChains["Inertial"].push_back(std::string("Device/SubDeviceList/InertialSensor/AngleY/Sensor/Value"));

	devicesNames["Inertial"].push_back(std::string("AccX"));
	devicesNames["Inertial"].push_back(std::string("AccY"));
	devicesNames["Inertial"].push_back(std::string("AccZ"));
	devicesNames["Inertial"].push_back(std::string("GyrX"));
	devicesNames["Inertial"].push_back(std::string("GyrY"));
	devicesNames["Inertial"].push_back(std::string("AngleX"));
	devicesNames["Inertial"].push_back(std::string("AngleY"));

	for (i = 0; i < devicesInChains["Inertial"].size(); i++) {
		ISM.add_sensordata();
		//ISM.mutable_sensordata(i)->set_sensorname(devicesNames["Inertial"][i]);
	}
	//TODO This is not real FSR
	devicesInChains["FSR"].push_back(std::string("Device/SubDeviceList/LFoot/FSR/CenterOfPressure/X/Sensor/Value"));
	devicesInChains["FSR"].push_back(std::string("Device/SubDeviceList/LFoot/FSR/CenterOfPressure/Y/Sensor/Value"));
	devicesInChains["FSR"].push_back(std::string("Device/SubDeviceList/LFoot/FSR/TotalWeight/Sensor/Value"));
	devicesInChains["FSR"].push_back(std::string("Device/SubDeviceList/RFoot/FSR/CenterOfPressure/X/Sensor/Value"));
	devicesInChains["FSR"].push_back(std::string("Device/SubDeviceList/RFoot/FSR/CenterOfPressure/Y/Sensor/Value"));
	devicesInChains["FSR"].push_back(std::string("Device/SubDeviceList/RFoot/FSR/TotalWeight/Sensor/Value"));
	devicesNames["FSR"].push_back(std::string("LFoot_FSR_CenterOfPressure_X"));
	devicesNames["FSR"].push_back(std::string("LFoot_FSR_CenterOfPressure_Y"));
	devicesNames["FSR"].push_back(std::string("LFoot_FSR_TotalWeight"));
	devicesNames["FSR"].push_back(std::string("RFoot_FSR_CenterOfPressure_X"));
	devicesNames["FSR"].push_back(std::string("RFoot_FSR_CenterOfPressure_Y"));
	devicesNames["FSR"].push_back(std::string("RFoot_FSR_TotalWeight"));

	for (i = 0; i < devicesInChains["FSR"].size(); i++) {
		FSM.add_sensordata();
		//FSM.mutable_sensordata(i)->set_sensorname(devicesNames["FSR"][i]);
	}
	devicesInChains["USoundLeft"].push_back(std::string("Device/SubDeviceList/US/Left/Sensor/Value"));
	devicesInChains["USoundLeft"].push_back(std::string("Device/SubDeviceList/US/Left/Sensor/Value1"));
	devicesInChains["USoundLeft"].push_back(std::string("Device/SubDeviceList/US/Left/Sensor/Value2"));
	devicesInChains["USoundLeft"].push_back(std::string("Device/SubDeviceList/US/Left/Sensor/Value3"));
	devicesInChains["USoundLeft"].push_back(std::string("Device/SubDeviceList/US/Left/Sensor/Value4"));
	devicesInChains["USoundLeft"].push_back(std::string("Device/SubDeviceList/US/Left/Sensor/Value5"));
	devicesInChains["USoundLeft"].push_back(std::string("Device/SubDeviceList/US/Left/Sensor/Value6"));
	devicesInChains["USoundLeft"].push_back(std::string("Device/SubDeviceList/US/Left/Sensor/Value7"));
	devicesInChains["USoundLeft"].push_back(std::string("Device/SubDeviceList/US/Left/Sensor/Value8"));
	devicesInChains["USoundLeft"].push_back(std::string("Device/SubDeviceList/US/Left/Sensor/Value9"));

	devicesInChains["USoundRight"].push_back(std::string("Device/SubDeviceList/US/Right/Sensor/Value"));
	devicesInChains["USoundRight"].push_back(std::string("Device/SubDeviceList/US/Right/Sensor/Value1"));
	devicesInChains["USoundRight"].push_back(std::string("Device/SubDeviceList/US/Right/Sensor/Value2"));
	devicesInChains["USoundRight"].push_back(std::string("Device/SubDeviceList/US/Right/Sensor/Value3"));
	devicesInChains["USoundRight"].push_back(std::string("Device/SubDeviceList/US/Right/Sensor/Value4"));
	devicesInChains["USoundRight"].push_back(std::string("Device/SubDeviceList/US/Right/Sensor/Value5"));
	devicesInChains["USoundRight"].push_back(std::string("Device/SubDeviceList/US/Right/Sensor/Value6"));
	devicesInChains["USoundRight"].push_back(std::string("Device/SubDeviceList/US/Right/Sensor/Value7"));
	devicesInChains["USoundRight"].push_back(std::string("Device/SubDeviceList/US/Right/Sensor/Value8"));
	devicesInChains["USoundRight"].push_back(std::string("Device/SubDeviceList/US/Right/Sensor/Value9"));

	devicesNames["USoundLeft"].push_back(std::string("USoundLeft"));
	devicesNames["USoundLeft"].push_back(std::string("USoundLeft1"));
	devicesNames["USoundLeft"].push_back(std::string("USoundLeft2"));
	devicesNames["USoundLeft"].push_back(std::string("USoundLeft3"));
	devicesNames["USoundLeft"].push_back(std::string("USoundLeft4"));
	devicesNames["USoundLeft"].push_back(std::string("USoundLeft5"));
	devicesNames["USoundLeft"].push_back(std::string("USoundLeft6"));
	devicesNames["USoundLeft"].push_back(std::string("USoundLeft7"));
	devicesNames["USoundLeft"].push_back(std::string("USoundLeft8"));
	devicesNames["USoundLeft"].push_back(std::string("USoundLeft9"));

	devicesNames["USoundRight"].push_back(std::string("USoundRight"));
	devicesNames["USoundRight"].push_back(std::string("USoundRight1"));
	devicesNames["USoundRight"].push_back(std::string("USoundRight2"));
	devicesNames["USoundRight"].push_back(std::string("USoundRight3"));
	devicesNames["USoundRight"].push_back(std::string("USoundRight4"));
	devicesNames["USoundRight"].push_back(std::string("USoundRight5"));
	devicesNames["USoundRight"].push_back(std::string("USoundRight6"));
	devicesNames["USoundRight"].push_back(std::string("USoundRight7"));
	devicesNames["USoundRight"].push_back(std::string("USoundRight8"));
	devicesNames["USoundRight"].push_back(std::string("USoundRight9"));

	for (i = 0; i < devicesInChains["USoundLeft"].size(); i++) {
		USSM.add_sensordata();
		//USSM.mutable_sensordata(i)->set_sensorname(devicesNames["USoundLeft"][i]);
	}
	for (j = 0; j < devicesInChains["USoundRight"].size(); j++) {
		USSM.add_sensordata();
		//USSM.mutable_sensordata(i)->set_sensorname(devicesNames["USoundRight"][j]);
	}
	int allchainsSize = 0;

	for (std::map<std::string, std::vector<std::string> >::const_iterator iter = devicesInChains.begin(); iter != devicesInChains.end(); ++iter) {
		allchainsSize += (iter->second).size();
	}
	devicesValues.assign(allchainsSize, 0);
	cout << " Number of devices distibuted " << allchainsSize << endl;
}

void Sensors::process_messages() {
	Logger::Instance().WriteMsg("Sensors", "Sensor process messages", Logger::ExtraInfo);
	//cout << "Sensor process messages" << endl;
	//	MessageBuffer* sub_buf = Subscriber::getBuffer();
	//	TestMessage* tm = (TestMessage*) sub_buf->remove_head();
	//	cout << "I received a message from " << tm->publisher() << " with counter "
	//			<< tm->counter() << endl;
	//	tm->set_counter(tm->counter() + 1);
	//	tm->set_topic("behavior"));
	//	publish(tm);
}

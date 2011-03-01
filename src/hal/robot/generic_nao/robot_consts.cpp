/**
 * robot_consts.cpp
 *
 *  Created on: Feb 6, 2011
 *      Author: trs
 */
#include "robot_consts.h"

void KDeviceLists::fillJointNames(std::map<DeviceNames, std::string> &Keys, std::string arg1, std::string arg2, std::string arg3)
{
	//std::map<DeviceNames, std::string> Keys;
	// Joints  list
	Keys[HEAD_PITCH] = "Device/SubDeviceList/HeadPitch/" + arg1 + "/" + arg2 + "/" + arg3;
	Keys[HEAD_YAW] = std::string("Device/SubDeviceList/HeadYaw/") + arg1 + "/" + arg2 + "/" + arg3;
	Keys[L_ANKLE_PITCH] = std::string("Device/SubDeviceList/LAnklePitch/") + arg1 + "/" + arg2 + "/" + arg3;
	Keys[L_ANKLE_ROLL] = std::string("Device/SubDeviceList/LAnkleRoll/") + arg1 + "/" + arg2 + "/" + arg3;
	Keys[L_ELBOW_ROLL] = std::string("Device/SubDeviceList/LElbowRoll/") + arg1 + "/" + arg2 + "/" + arg3;
	Keys[L_ELBOW_YAW] = std::string("Device/SubDeviceList/LElbowYaw/") + arg1 + "/" + arg2 + "/" + arg3;
	//Keys[L_HAND] = std::string("Device/SubDeviceList/LHand")+arg1+"/"+arg2+"/"+arg3;
	Keys[L_HIP_PITCH] = std::string("Device/SubDeviceList/LHipPitch/") + arg1 + "/" + arg2 + "/" + arg3;
	Keys[L_HIP_ROLL] = std::string("Device/SubDeviceList/LHipRoll/") + arg1 + "/" + arg2 + "/" + arg3;
	Keys[L_HIP_YAW_PITCH] = std::string("Device/SubDeviceList/LHipYawPitch/") + arg1 + "/" + arg2 + "/" + arg3;
	Keys[L_KNEE_PITCH] = std::string("Device/SubDeviceList/LKneePitch/") + arg1 + "/" + arg2 + "/" + arg3;
	Keys[L_SHOULDER_PITCH] = std::string("Device/SubDeviceList/LShoulderPitch/") + arg1 + "/" + arg2 + "/" + arg3;
	Keys[L_SHOULDER_ROLL] = std::string("Device/SubDeviceList/LShoulderRoll/") + arg1 + "/" + arg2 + "/" + arg3;
	//Keys[L_WRIST_YAW] = std::string("Device/SubDeviceList/LWristYaw")+arg1+"/"+arg2+"/"+arg3;
	Keys[R_ANKLE_PITCH] = std::string("Device/SubDeviceList/RAnklePitch/") + arg1 + "/" + arg2 + "/" + arg3;
	Keys[R_ANKLE_ROLL] = std::string("Device/SubDeviceList/RAnkleRoll/") + arg1 + "/" + arg2 + "/" + arg3;
	Keys[R_ELBOW_ROLL] = std::string("Device/SubDeviceList/RElbowRoll/") + arg1 + "/" + arg2 + "/" + arg3;
	Keys[R_ELBOW_YAW] = std::string("Device/SubDeviceList/RElbowYaw/") + arg1 + "/" + arg2 + "/" + arg3;
	//Keys[R_HAND] = std::string("Device/SubDeviceList/RHand")+arg1+"/"+arg2+"/"+arg3;
	Keys[R_HIP_PITCH] = std::string("Device/SubDeviceList/RHipPitch/") + arg1 + "/" + arg2 + "/" + arg3;
	Keys[R_HIP_ROLL] = std::string("Device/SubDeviceList/RHipRoll/") + arg1 + "/" + arg2 + "/" + arg3;
	Keys[R_HIP_YAW_PITCH] = std::string("Device/SubDeviceList/RHipYawPitch/") + arg1 + "/" + arg2 + "/" + arg3;
	Keys[R_KNEE_PITCH] = std::string("Device/SubDeviceList/RKneePitch/") + arg1 + "/" + arg2 + "/" + arg3;
	Keys[R_SHOULDER_PITCH] = std::string("Device/SubDeviceList/RShoulderPitch/") + arg1 + "/" + arg2 + "/" + arg3;
	Keys[R_SHOULDER_ROLL] = std::string("Device/SubDeviceList/RShoulderRoll/") + arg1 + "/" + arg2 + "/" + arg3;
	//Keys[R_WRIST_YAW] = std::string("Device/SubDeviceList/RWristYaw")+arg1+"/"+arg2+"/"+arg3;

	//return Keys;
}

std::map<DeviceNames, std::string> KDeviceLists::fillHardnessActuatorNames()
{
	std::map<DeviceNames, std::string> fHardnessActuatorKeys;

	//		// Joints Actuator list
	fillJointNames(fHardnessActuatorKeys, "Hardness", "Actuator", "Value");
	return fHardnessActuatorKeys;
}

std::map<DeviceNames, std::string> KDeviceLists::fillPositionActuatorNames()
{
	std::map<DeviceNames, std::string> fPositionActuatorKeys;
	//		// Joints Actuator list
	fillJointNames(fPositionActuatorKeys, "Position", "Actuator", "Value");
	return fPositionActuatorKeys;
}

std::map<DeviceNames, std::string> KDeviceLists::fillSensorNames()
{
	std::map<DeviceNames, std::string> fSensorKeys;
	// Joints Sensor list
	fillJointNames(fSensorKeys, "Position", "Sensor", "Value");

	// Inertial sensors
	fSensorKeys[ACC_X] = std::string("Device/SubDeviceList/InertialSensor/AccX/Sensor/Value");
	fSensorKeys[ACC_Y] = std::string("Device/SubDeviceList/InertialSensor/AccY/Sensor/Value");
	fSensorKeys[ACC_Z] = std::string("Device/SubDeviceList/InertialSensor/AccZ/Sensor/Value");
	fSensorKeys[GYR_X] = std::string("Device/SubDeviceList/InertialSensor/GyrX/Sensor/Value");
	fSensorKeys[GYR_Y] = std::string("Device/SubDeviceList/InertialSensor/GyrY/Sensor/Value");
	fSensorKeys[ANGLE_X] = std::string("Device/SubDeviceList/InertialSensor/AngleX/Sensor/Value");
	fSensorKeys[ANGLE_Y] = std::string("Device/SubDeviceList/InertialSensor/AngleY/Sensor/Value");

	// Some FSR sensors
	fSensorKeys[L_COP_X] = std::string("Device/SubDeviceList/LFoot/FSR/CenterOfPressure/X/Sensor/Value");
	fSensorKeys[L_COP_Y] = std::string("Device/SubDeviceList/LFoot/FSR/CenterOfPressure/Y/Sensor/Value");
	fSensorKeys[L_TOTAL_WEIGHT] = std::string("Device/SubDeviceList/LFoot/FSR/TotalWeight/Sensor/Value");
	fSensorKeys[R_COP_X] = std::string("Device/SubDeviceList/RFoot/FSR/CenterOfPressure/X/Sensor/Value");
	fSensorKeys[R_COP_Y] = std::string("Device/SubDeviceList/RFoot/FSR/CenterOfPressure/Y/Sensor/Value");
	fSensorKeys[R_TOTAL_WEIGHT] = std::string("Device/SubDeviceList/RFoot/FSR/TotalWeight/Sensor/Value");

	fSensorKeys[L_FSR_FL] = std::string("Device/SubDeviceList/LFoot/FSR/FrontLeft/Sensor/Value");
	fSensorKeys[L_FSR_FR] = std::string("Device/SubDeviceList/LFoot/FSR/FrontRight/Sensor/Value");
	fSensorKeys[L_FSR_RL] = std::string("Device/SubDeviceList/LFoot/FSR/RearLeft/Sensor/Value");
	fSensorKeys[L_FSR_RR] = std::string("Device/SubDeviceList/LFoot/FSR/RearRight/Sensor/Value");

	fSensorKeys[R_FSR_FL] = std::string("Device/SubDeviceList/RFoot/FSR/FrontLeft/Sensor/Value");
	fSensorKeys[R_FSR_FR] = std::string("Device/SubDeviceList/RFoot/FSR/FrontRight/Sensor/Value");
	fSensorKeys[R_FSR_RL] = std::string("Device/SubDeviceList/RFoot/FSR/RearLeft/Sensor/Value");
	fSensorKeys[R_FSR_RR] = std::string("Device/SubDeviceList/RFoot/FSR/RearRight/Sensor/Value");

	return fSensorKeys;
}
const std::map<DeviceNames, std::string> & KDeviceLists::SensorNames()
{
	static std::map<DeviceNames, std::string> ret = fillSensorNames();
	return ret;
}

const std::map<DeviceNames, std::string> & KDeviceLists::PositionActuatorKeys()
{
	static std::map<DeviceNames, std::string> ret = fillPositionActuatorNames();
	return ret;
}

const std::map<DeviceNames, std::string> & KDeviceLists::HardnessActuatorKeys()
{
	static std::map<DeviceNames, std::string> ret = fillHardnessActuatorNames();
	return ret;
}

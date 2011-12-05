/**
 * robot_consts.cpp
 *
 *  Created on: Feb 6, 2011
 *      Author: trs
 */
#include "robot_consts.h"

static const std::string preFix("Device/SubDeviceList/");

namespace KDeviceLists
{
	std::vector<std::string> generateJointNames();
	std::map<std::string, int> generateJointIDs();
	void fillJointKeys(std::vector<std::string> &Keys, std::string const & postFix);

	std::vector<std::string>  generateHardnessActuatorKeys();
	std::vector<std::string>  generatePositionActuatorKeys();
	std::vector<std::string>  generateJointKeys();


	std::vector<std::string> generateSensorNames();
	std::vector< std::string> generateSensorKeys();

	std::vector< std::string> generateButtonKeys();


}

std::vector<std::string> KDeviceLists::generateJointNames()
{

	std::vector<std::string> Keys;
	Keys.resize(NUMOFJOINTS);
	Keys[HEAD+YAW] = "HeadYaw";
	Keys[HEAD+PITCH] = "HeadPitch";

	Keys[L_LEG+HIP_YAW_PITCH] = "LHipYawPitch";
	Keys[L_LEG+HIP_ROLL] = "LHipRoll";
	Keys[L_LEG+HIP_PITCH] = "LHipPitch";
	Keys[L_LEG+KNEE_PITCH] = "LKneePitch";
	Keys[L_LEG+ANKLE_PITCH] = "LAnklePitch";
	Keys[L_LEG+ANKLE_ROLL] = "LAnkleRoll";

	Keys[R_LEG+HIP_YAW_PITCH] = "RHipYawPitch";
	Keys[R_LEG+HIP_ROLL] = "RHipRoll";
	Keys[R_LEG+HIP_PITCH] = "RHipPitch";
	Keys[R_LEG+KNEE_PITCH] = "RKneePitch";
	Keys[R_LEG+ANKLE_PITCH] = "RAnklePitch";
	Keys[R_LEG+ANKLE_ROLL] = "RAnkleRoll";

	Keys[L_ARM+SHOULDER_PITCH] = "LShoulderPitch";
	Keys[L_ARM+SHOULDER_ROLL] = "LShoulderRoll";
	Keys[L_ARM+ELBOW_ROLL] = "LElbowRoll";
	Keys[L_ARM+ELBOW_YAW] = "LElbowYaw";
	//Keys[L_ARM+WRIST_YAW] = "LWristYaw";
	//Keys[L_ARM+HAND] = "LHand";

	Keys[R_ARM+SHOULDER_PITCH] = "RShoulderPitch";
	Keys[R_ARM+SHOULDER_ROLL] = "RShoulderRoll";
	Keys[R_ARM+ELBOW_ROLL] = "RElbowRoll";
	Keys[R_ARM+ELBOW_YAW] = "RElbowYaw";
	//Keys[R_ARM+WRIST_YAW] = "RWristYaw";
	//Keys[R_ARM+HAND] = "RHand";
	return Keys;
}

std::map<std::string, int> KDeviceLists::generateJointIDs()
{
	std::map<std::string, int> JointsIDs;

	JointsIDs.insert(std::pair<std::string, int>("HeadYaw", HEAD+YAW));
	JointsIDs.insert(std::pair<std::string, int>("HeadPitch", HEAD+PITCH));

	JointsIDs.insert(std::pair<std::string, int>("LHipYawPitch", L_LEG+HIP_YAW_PITCH));
	JointsIDs.insert(std::pair<std::string, int>("LHipRoll", L_LEG+HIP_ROLL));
	JointsIDs.insert(std::pair<std::string, int>("LHipPitch", L_LEG+HIP_PITCH));
	JointsIDs.insert(std::pair<std::string, int>("LKneePitch", L_LEG+KNEE_PITCH));
	JointsIDs.insert(std::pair<std::string, int>("LAnklePitch", L_LEG+ANKLE_PITCH));
	JointsIDs.insert(std::pair<std::string, int>("LAnkleRoll", L_LEG+ANKLE_ROLL));

	JointsIDs.insert(std::pair<std::string, int>("RHipYawPitch", R_LEG+HIP_YAW_PITCH));
	JointsIDs.insert(std::pair<std::string, int>("RHipRoll", R_LEG+HIP_ROLL));
	JointsIDs.insert(std::pair<std::string, int>("RHipPitch", R_LEG+HIP_PITCH));
	JointsIDs.insert(std::pair<std::string, int>("RKneePitch", R_LEG+KNEE_PITCH));
	JointsIDs.insert(std::pair<std::string, int>("RAnklePitch", R_LEG+ANKLE_PITCH));
	JointsIDs.insert(std::pair<std::string, int>("RAnkleRoll", R_LEG+ANKLE_ROLL));

	JointsIDs.insert(std::pair<std::string, int>("LShoulderPitch", L_ARM+SHOULDER_PITCH));
	JointsIDs.insert(std::pair<std::string, int>("LShoulderRoll", L_ARM+SHOULDER_ROLL));
	JointsIDs.insert(std::pair<std::string, int>("LElbowRoll", L_ARM+ELBOW_ROLL));
	JointsIDs.insert(std::pair<std::string, int>("LElbowYaw", L_ARM+ELBOW_YAW));
	//JointsIDs.insert(std::pair<std::string, int>("LWristYaw", L_ARM+WRIST_YAW));
	//JointsIDs.insert(std::pair<std::string, int>("LHand", L_ARM+HAND));

	JointsIDs.insert(std::pair<std::string, int>("RShoulderPitch", R_ARM+SHOULDER_PITCH));
	JointsIDs.insert(std::pair<std::string, int>("RShoulderRoll", R_ARM+SHOULDER_ROLL));
	JointsIDs.insert(std::pair<std::string, int>("RElbowRoll", R_ARM+ELBOW_ROLL));
	JointsIDs.insert(std::pair<std::string, int>("RElbowYaw", R_ARM+ELBOW_YAW));
	//JointsIDs.insert(std::pair<std::string, int>("RWristYaw", R_ARM+WRIST_YAW));
	//JointsIDs.insert(std::pair<std::string, int>("RHand", R_ARM+HAND));

	return JointsIDs;
}
std::vector<std::string> KDeviceLists::generateSensorNames()
{
	std::vector<std::string> fSensorKeys;
	fSensorKeys.resize(NUMOFSENSORS);
	fSensorKeys[ACC+AXIS_X] = "AccX";
	fSensorKeys[ACC+AXIS_Y] = "AccY";
	fSensorKeys[ACC+AXIS_Z] = "AccZ";
	fSensorKeys[GYR+AXIS_X] = "GyrX";
	fSensorKeys[GYR+AXIS_Y] = "GyrY";
	fSensorKeys[GYR+AXIS_Z] = "GyrRef";
	//fSensorKeys[ANGLE+AXIS_X] = "AngleX";
	//fSensorKeys[ANGLE+AXIS_Y] = "AngleY";

	// Some FSR sensors

	fSensorKeys[L_FSR+FSR_FL] = "LFSR_FrontLeft";
	fSensorKeys[L_FSR+FSR_FR] = "LFSR_FrontRight";
	fSensorKeys[L_FSR+FSR_RL] = "LFSR_RearLeft";
	fSensorKeys[L_FSR+FSR_RR] = "LFSR_RearRight";
	fSensorKeys[L_FSR+COP_X] = "LFSR_CenterOfPressureX";
	fSensorKeys[L_FSR+COP_Y] = "LFSR_CenterOfPressureY";
	fSensorKeys[L_FSR+TOTAL_WEIGHT] = "LFSR_TotalWeight";


	fSensorKeys[R_FSR+FSR_FL] = "RFSR_FrontLeft";
	fSensorKeys[R_FSR+FSR_FR] = "RFSR_FrontRight";
	fSensorKeys[R_FSR+FSR_RL] = "RFSR_RearLeft";
	fSensorKeys[R_FSR+FSR_RR] = "RFSR_RearRight";
	fSensorKeys[R_FSR+COP_X] = "RFSR_CenterOfPressureX";
	fSensorKeys[R_FSR+COP_Y] = "RFSR_CenterOfPressureY";
	fSensorKeys[R_FSR+TOTAL_WEIGHT] = "RFSR_TotalWeight";


	fSensorKeys[L_US+US_VALUE]="LUS_Value";
	fSensorKeys[L_US+US_VALUE1]="LUS_Value1";
	fSensorKeys[L_US+US_VALUE2]="LUS_Value2";
	fSensorKeys[L_US+US_VALUE3]="LUS_Value3";
	fSensorKeys[L_US+US_VALUE4]="LUS_Value4";
	fSensorKeys[L_US+US_VALUE5]="LUS_Value5";
	fSensorKeys[L_US+US_VALUE6]="LUS_Value6";
	fSensorKeys[L_US+US_VALUE7]="LUS_Value7";
	fSensorKeys[L_US+US_VALUE8]="LUS_Value8";
	fSensorKeys[L_US+US_VALUE9]="LUS_Value9";

	fSensorKeys[R_US+US_VALUE]="RUS_Value";
	fSensorKeys[R_US+US_VALUE1]="RUS_Value1";
	fSensorKeys[R_US+US_VALUE2]="RUS_Value2";
	fSensorKeys[R_US+US_VALUE3]="RUS_Value3";
	fSensorKeys[R_US+US_VALUE4]="RUS_Value4";
	fSensorKeys[R_US+US_VALUE5]="RUS_Value5";
	fSensorKeys[R_US+US_VALUE6]="RUS_Value6";
	fSensorKeys[R_US+US_VALUE7]="RUS_Value7";
	fSensorKeys[R_US+US_VALUE8]="RUS_Value8";
	fSensorKeys[R_US+US_VALUE9]="RUS_Value9";
	return fSensorKeys;

}
std::vector<std::string> const& KDeviceLists::getJointNames()
{
	static const std::vector<std::string> r=generateJointNames();
	return r;
}
std::map<std::string,int> const& KDeviceLists::getJointIDs()
{
	static const std::map<std::string,int> r=generateJointIDs();
	return r;
}
std::vector<std::string> const& KDeviceLists::getSensorNames()
{
	static const std::vector<std::string> r=generateSensorNames();
	return r;

}

void KDeviceLists::fillJointKeys(std::vector<std::string> &Keys, std::string const & postFix)
{
	//std::vector< std::string> Keys;
	// Joints  list

	std::vector<std::string> const& Names=getJointNames();
	Keys.resize(Names.size());
	for(unsigned i=0;i<Names.size();i++)
		Keys[i] = preFix+Names[i]+postFix;

	//return Keys;
}

std::vector< std::string> KDeviceLists::generateHardnessActuatorKeys()
{
	std::vector< std::string> fHardnessActuatorKeys;
	//		// Joints Actuator list
	fillJointKeys(fHardnessActuatorKeys, "/Hardness/Actuator/Value");
	return fHardnessActuatorKeys;
}

std::vector< std::string> KDeviceLists::generatePositionActuatorKeys()
{
	std::vector< std::string> fPositionActuatorKeys;
	//		// Joints Actuator list
	fillJointKeys(fPositionActuatorKeys, "/Position/Actuator/Value");
	return fPositionActuatorKeys;
}

std::vector< std::string> KDeviceLists::generateJointKeys()
{
	std::vector< std::string> fPositionActuatorKeys;
	//		// Joints Actuator list
	fillJointKeys(fPositionActuatorKeys, "/Position/Sensor/Value");
	return fPositionActuatorKeys;
}

std::vector< std::string> KDeviceLists::generateSensorKeys()
{
	std::vector<std::string> fSensorKeys;
	fSensorKeys.resize(NUMOFSENSORS);

	// Inertial sensors
	fSensorKeys[ACC+AXIS_X] = preFix+"InertialSensor/AccX/Sensor/Value";
	fSensorKeys[ACC+AXIS_Y] = preFix+"InertialSensor/AccY/Sensor/Value";
	fSensorKeys[ACC+AXIS_Z] = preFix+"InertialSensor/AccZ/Sensor/Value";
	fSensorKeys[GYR+AXIS_X] = preFix+"InertialSensor/GyrX/Sensor/Value";
	fSensorKeys[GYR+AXIS_Y] = preFix+"InertialSensor/GyrY/Sensor/Value";
	fSensorKeys[GYR+AXIS_Z] = preFix+"InertialSensor/GyrRef/Sensor/Value";
	//fSensorKeys[ANGLE+AXIS_X] = preFix+"InertialSensor/AngleX/Sensor/Value";
	//fSensorKeys[ANGLE+AXIS_Y] = preFix+"InertialSensor/AngleY/Sensor/Value";

	// Some FSR sensors

	fSensorKeys[L_FSR+FSR_FL] = preFix+"LFoot/FSR/FrontLeft/Sensor/Value";
	fSensorKeys[L_FSR+FSR_FR] = preFix+"LFoot/FSR/FrontRight/Sensor/Value";
	fSensorKeys[L_FSR+FSR_RL] = preFix+"LFoot/FSR/RearLeft/Sensor/Value";
	fSensorKeys[L_FSR+FSR_RR] = preFix+"LFoot/FSR/RearRight/Sensor/Value";
	fSensorKeys[L_FSR+COP_X] = preFix+"LFoot/FSR/CenterOfPressure/X/Sensor/Value";
	fSensorKeys[L_FSR+COP_Y] = preFix+"LFoot/FSR/CenterOfPressure/Y/Sensor/Value";
	fSensorKeys[L_FSR+TOTAL_WEIGHT] = preFix+"LFoot/FSR/TotalWeight/Sensor/Value";


	fSensorKeys[R_FSR+FSR_FL] = preFix+"RFoot/FSR/FrontLeft/Sensor/Value";
	fSensorKeys[R_FSR+FSR_FR] = preFix+"RFoot/FSR/FrontRight/Sensor/Value";
	fSensorKeys[R_FSR+FSR_RL] = preFix+"RFoot/FSR/RearLeft/Sensor/Value";
	fSensorKeys[R_FSR+FSR_RR] = preFix+"RFoot/FSR/RearRight/Sensor/Value";
	fSensorKeys[R_FSR+COP_X] = preFix+"RFoot/FSR/CenterOfPressure/X/Sensor/Value";
	fSensorKeys[R_FSR+COP_Y] = preFix+"RFoot/FSR/CenterOfPressure/Y/Sensor/Value";
	fSensorKeys[R_FSR+TOTAL_WEIGHT] = preFix+"RFoot/FSR/TotalWeight/Sensor/Value";

	fSensorKeys[L_US+US_VALUE]=preFix+"US/Left/Sensor/Value";
	fSensorKeys[L_US+US_VALUE1]=preFix+"US/Left/Sensor/Value1";
	fSensorKeys[L_US+US_VALUE2]=preFix+"US/Left/Sensor/Value2";
	fSensorKeys[L_US+US_VALUE3]=preFix+"US/Left/Sensor/Value3";
	fSensorKeys[L_US+US_VALUE4]=preFix+"US/Left/Sensor/Value4";
	fSensorKeys[L_US+US_VALUE5]=preFix+"US/Left/Sensor/Value5";
	fSensorKeys[L_US+US_VALUE6]=preFix+"US/Left/Sensor/Value6";
	fSensorKeys[L_US+US_VALUE7]=preFix+"US/Left/Sensor/Value7";
	fSensorKeys[L_US+US_VALUE8]=preFix+"US/Left/Sensor/Value8";
	fSensorKeys[L_US+US_VALUE9]=preFix+"US/Left/Sensor/Value9";

	fSensorKeys[R_US+US_VALUE]=preFix+"US/Right/Sensor/Value";
	fSensorKeys[R_US+US_VALUE1]=preFix+"US/Right/Sensor/Value1";
	fSensorKeys[R_US+US_VALUE2]=preFix+"US/Right/Sensor/Value2";
	fSensorKeys[R_US+US_VALUE3]=preFix+"US/Right/Sensor/Value3";
	fSensorKeys[R_US+US_VALUE4]=preFix+"US/Right/Sensor/Value4";
	fSensorKeys[R_US+US_VALUE5]=preFix+"US/Right/Sensor/Value5";
	fSensorKeys[R_US+US_VALUE6]=preFix+"US/Right/Sensor/Value6";
	fSensorKeys[R_US+US_VALUE7]=preFix+"US/Right/Sensor/Value7";
	fSensorKeys[R_US+US_VALUE8]=preFix+"US/Right/Sensor/Value8";
	fSensorKeys[R_US+US_VALUE9]=preFix+"US/Right/Sensor/Value9";

	return fSensorKeys;
}


std::vector< std::string> KDeviceLists::generateButtonKeys()
{
	std::vector<std::string> fKeys;
	fKeys.resize(NUMOFBUTTONS);

	fKeys[CHEST_BUTTON] = preFix+"ChestBoard/Button/Sensor/Value";
	fKeys[L_BUMPER_L] = preFix+"LFoot/Bumper/Left/Sensor/Value";
	fKeys[L_BUMPER_R] = preFix+"LFoot/Bumper/Right/Sensor/Value";
	fKeys[R_BUMPER_L] = preFix+"RFoot/Bumper/Left/Sensor/Value";
	fKeys[R_BUMPER_R] = preFix+"RFoot/Bumper/Right/Sensor/Value";


	return fKeys;
}



std::vector<std::string> const& KDeviceLists::getSensorKeys()
{
	static const std::vector< std::string> ret = generateSensorKeys();
	return ret;
}

std::vector<std::string> const& KDeviceLists::getPositionActuatorKeys()
{
	static const std::vector< std::string> ret = generatePositionActuatorKeys();
	return ret;
}

std::vector<std::string> const& KDeviceLists::getHardnessActuatorKeys()
{
	static const std::vector< std::string> ret = generateHardnessActuatorKeys();
	return ret;
}

std::vector<std::string> const& KDeviceLists::getJointKeys()
{
	static const std::vector< std::string> ret = generateJointKeys();
	return ret;
}

std::vector<std::string> const& KDeviceLists::getButtonKeys()
{
	static const std::vector< std::string> ret = generateButtonKeys();
	return ret;
}





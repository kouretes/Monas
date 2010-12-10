/**
 * robot_consts.h
 *
 *  Created on: Dec 9, 2010
 *      Author: trs
 */

#ifndef ROBOT_CONSTS_H_
#define ROBOT_CONSTS_H_




enum ChainHeadNames {
	 YAW, PITCH, HEADSIZE
};
enum ChainLegNames {
	HIP_YAW_PITCH, HIP_ROLL, HIP_PITCH, KNEE_PITCH, ANKLE_PITCH, ANKLE_ROLL, LEGSIZE
};

enum ChainArmNames {
	SHOULDER_PITCH, SHOULDER_ROLL, ELBOW_ROLL, ELBOW_YAW, /*WRIST_YAW, HAND,*/ARMSIZE
};

enum ChainAccessNames {
	X, Y, Z
};

enum DeviceNames {
	HEAD, HEAD_YAW = HEAD, HEAD_PITCH,

	L_LEG, L_HIP_YAW_PITCH = L_LEG,

	L_HIP_ROLL, L_HIP_PITCH, L_KNEE_PITCH, L_ANKLE_PITCH, L_ANKLE_ROLL,

	R_LEG, R_HIP_YAW_PITCH = R_LEG,

	R_HIP_ROLL, R_HIP_PITCH, R_KNEE_PITCH, R_ANKLE_PITCH, R_ANKLE_ROLL,

	L_ARM, L_SHOULDER_PITCH = L_ARM, L_SHOULDER_ROLL, L_ELBOW_ROLL, L_ELBOW_YAW, /*L_WRIST_YAW, L_HAND,*/

	R_ARM, R_SHOULDER_PITCH = R_ARM, R_SHOULDER_ROLL, R_ELBOW_ROLL, R_ELBOW_YAW, /*R_WRIST_YAW, R_HAND,*/

	ACC, ACC_X = ACC, ACC_Y, ACC_Z,

	GYR, GYR_X = GYR, GYR_Y,

	ANGLE, ANGLE_X = ANGLE, ANGLE_Y,

	L_COP_X, L_COP_Y, L_TOTAL_WEIGHT,

	R_COP_X, R_COP_Y, R_TOTAL_WEIGHT


} ;

	/// Returns a map from joint ids (enum Joint names) to almemory string "path" names
namespace KDeviceLists {

	std::map<DeviceNames, std::string> fillSensorNames() {
		std::map<DeviceNames, std::string> fSensorKeys;

		// Joints Sensor list
		fSensorKeys[HEAD_PITCH] = "Device/SubDeviceList/HeadPitch/Position/Sensor/Value";
		fSensorKeys[HEAD_YAW] = std::string("Device/SubDeviceList/HeadYaw/Position/Sensor/Value");
		fSensorKeys[L_ANKLE_PITCH] = std::string("Device/SubDeviceList/LAnklePitch/Position/Sensor/Value");
		fSensorKeys[L_ANKLE_ROLL] = std::string("Device/SubDeviceList/LAnkleRoll/Position/Sensor/Value");
		fSensorKeys[L_ELBOW_ROLL] = std::string("Device/SubDeviceList/LElbowRoll/Position/Sensor/Value");
		fSensorKeys[L_ELBOW_YAW] = std::string("Device/SubDeviceList/LElbowYaw/Position/Sensor/Value");
		//fSensorKeys[L_HAND] = std::string("Device/SubDeviceList/LHand/Position/Sensor/Value");
		fSensorKeys[L_HIP_PITCH] = std::string("Device/SubDeviceList/LHipPitch/Position/Sensor/Value");
		fSensorKeys[L_HIP_ROLL] = std::string("Device/SubDeviceList/LHipRoll/Position/Sensor/Value");
		fSensorKeys[L_HIP_YAW_PITCH] = std::string("Device/SubDeviceList/LHipYawPitch/Position/Sensor/Value");
		fSensorKeys[L_KNEE_PITCH] = std::string("Device/SubDeviceList/LKneePitch/Position/Sensor/Value");
		fSensorKeys[L_SHOULDER_PITCH] = std::string("Device/SubDeviceList/LShoulderPitch/Position/Sensor/Value");
		fSensorKeys[L_SHOULDER_ROLL] = std::string("Device/SubDeviceList/LShoulderRoll/Position/Sensor/Value");
		//fSensorKeys[L_WRIST_YAW] = std::string("Device/SubDeviceList/LWristYaw/Position/Sensor/Value");
		fSensorKeys[R_ANKLE_PITCH] = std::string("Device/SubDeviceList/RAnklePitch/Position/Sensor/Value");
		fSensorKeys[R_ANKLE_ROLL] = std::string("Device/SubDeviceList/RAnkleRoll/Position/Sensor/Value");
		fSensorKeys[R_ELBOW_ROLL] = std::string("Device/SubDeviceList/RElbowRoll/Position/Sensor/Value");
		fSensorKeys[R_ELBOW_YAW] = std::string("Device/SubDeviceList/RElbowYaw/Position/Sensor/Value");
		//fSensorKeys[R_HAND] = std::string("Device/SubDeviceList/RHand/Position/Sensor/Value");
		fSensorKeys[R_HIP_PITCH] = std::string("Device/SubDeviceList/RHipPitch/Position/Sensor/Value");
		fSensorKeys[R_HIP_ROLL] = std::string("Device/SubDeviceList/RHipRoll/Position/Sensor/Value");
		fSensorKeys[R_KNEE_PITCH] = std::string("Device/SubDeviceList/RKneePitch/Position/Sensor/Value");
		fSensorKeys[R_SHOULDER_PITCH] = std::string("Device/SubDeviceList/RShoulderPitch/Position/Sensor/Value");
		fSensorKeys[R_SHOULDER_ROLL] = std::string("Device/SubDeviceList/RShoulderRoll/Position/Sensor/Value");
		//fSensorKeys[R_WRIST_YAW] = std::string("Device/SubDeviceList/RWristYaw/Position/Sensor/Value");

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
		return fSensorKeys;
	}
	const std::map<DeviceNames, std::string> SensorNames() {
		static std::map<DeviceNames, std::string> ret = fillSensorNames();
		return ret;
	}
	std::map<DeviceNames, std::string> ActuatorName;

}
;

#endif /* ROBOT_CONSTS_H_ */

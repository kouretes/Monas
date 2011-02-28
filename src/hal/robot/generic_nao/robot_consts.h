/**
 * robot_consts.h
 *
 *  Created on: Dec 9, 2010
 *      Author: trs
 */

#ifndef ROBOT_CONSTS_H_
#define ROBOT_CONSTS_H_
#include <map>
#include <string>


enum ChainHeadNames
{
	YAW, PITCH, HEADSIZE
};
enum ChainLegNames
{
	HIP_YAW_PITCH, HIP_ROLL, HIP_PITCH, KNEE_PITCH, ANKLE_PITCH, ANKLE_ROLL, LEGSIZE
};

enum ChainArmNames
{
	SHOULDER_PITCH, SHOULDER_ROLL, ELBOW_ROLL, ELBOW_YAW, /*WRIST_YAW, HAND,*/ARMSIZE
};

enum ChainAccessNames
{
	X, Y, Z
};

enum DeviceNames
{
	HEAD, HEAD_YAW = HEAD, HEAD_PITCH,

	L_LEG, L_HIP_YAW_PITCH = L_LEG,

	L_HIP_ROLL, L_HIP_PITCH, L_KNEE_PITCH, L_ANKLE_PITCH, L_ANKLE_ROLL,

	R_LEG, R_HIP_YAW_PITCH = R_LEG,

	R_HIP_ROLL, R_HIP_PITCH, R_KNEE_PITCH, R_ANKLE_PITCH, R_ANKLE_ROLL,

	L_ARM, L_SHOULDER_PITCH = L_ARM, L_SHOULDER_ROLL, L_ELBOW_ROLL, L_ELBOW_YAW, /*L_WRIST_YAW, L_HAND,*/

	R_ARM, R_SHOULDER_PITCH = R_ARM, R_SHOULDER_ROLL, R_ELBOW_ROLL, R_ELBOW_YAW, /*R_WRIST_YAW, R_HAND,*/

	numofjoints,

	ACC = numofjoints, ACC_X = numofjoints, ACC_Y, ACC_Z,

	GYR, GYR_X = GYR, GYR_Y,

	ANGLE, ANGLE_X = ANGLE, ANGLE_Y,

	L_COP, L_COP_X = L_COP, L_COP_Y, L_TOTAL_WEIGHT,

	R_COP, R_COP_X = R_COP, R_COP_Y, R_TOTAL_WEIGHT,

	L_FSR, L_FSR_FL = L_FSR, L_FSR_FR, L_FSR_RL, L_FSR_RR,

	R_FSR, R_FSR_FL = R_FSR, R_FSR_FR, R_FSR_RL, R_FSR_RR

};

/// Returns a map from joint ids (enum Joint names) to almemory string "path" names
namespace KDeviceLists
{
	void fillJointNames(std::map<DeviceNames, std::string> &Keys, std::string arg1, std::string arg2, std::string arg3);


	std::map<DeviceNames, std::string> fillHardnessActuatorNames();

	std::map<DeviceNames, std::string> fillPositionActuatorNames();

	std::map<DeviceNames, std::string> fillSensorNames();

	const std::map<DeviceNames, std::string> & SensorNames();

	const std::map<DeviceNames, std::string> &PositionActuatorKeys();

	const std::map<DeviceNames, std::string> &HardnessActuatorKeys();

	//std::map<DeviceNames, std::string> ActuatorName;

};

#endif /* ROBOT_CONSTS_H_ */

/**
 * robot_consts.h
 *
 *  Created on: Dec 9, 2010
 *      Author: trs
 */

#ifndef ROBOT_CONSTS_H_
#define ROBOT_CONSTS_H_
#include <vector>
#include <map>
#include <string>
#include "core/elements/Singleton.hpp"

/// Returns a map from joint ids (enum Joint names) to almemory string "path" names
namespace KDeviceLists
{


	enum ChainHeadNames
	{
	    YAW = 0, PITCH, HEAD_SIZE
	};

	enum ChainArmNames
	{
	    SHOULDER_PITCH = 0, SHOULDER_ROLL, ELBOW_YAW, ELBOW_ROLL, /*WRIST_YAW, HAND,*/ARM_SIZE
	};

	enum ChainLegNames
	{
	    HIP_YAW_PITCH = 0, HIP_ROLL, HIP_PITCH, KNEE_PITCH, ANKLE_PITCH, ANKLE_ROLL, LEG_SIZE
	};

	enum ChainAccessNames
	{
	    AXIS_X = 0, AXIS_Y, AXIS_Z , AXIS_SIZE
	};

	enum ChainIniertialSizes
	{
	    ACC_SIZE = AXIS_SIZE, GYR_SIZE = AXIS_SIZE, ANGLE_SIZE = AXIS_Z
	};

	enum ChainFSRNames
	{
	    FSR_FL = 0, FSR_FR, FSR_RL, FSR_RR, COP_X, COP_Y, TOTAL_WEIGHT, FSR_SIZE
	};
	enum ChainUltraSonicNames
	{
	    US_VALUE, US_VALUE1, US_VALUE2, US_VALUE3, US_VALUE4, US_VALUE5, US_VALUE6, US_VALUE7, US_VALUE8, US_VALUE9, US_SIZE
	};
	enum RobotPositionNames
	{
	    ROBOT_X = 0, ROBOT_Y, ROBOT_ANGLE, ROBOTPOSITION_SIZE
	};
	enum ChainsNames
	{
		CHAIN_HEAD=0, CHAIN_L_ARM, CHAIN_L_LEG, CHAIN_R_LEG,CHAIN_R_ARM, CHAINS_SIZE
	};

	enum JointNames
	{
	    HEAD = 0,	L_ARM = HEAD_SIZE, L_LEG = L_ARM + ARM_SIZE, R_LEG = L_LEG + LEG_SIZE, R_ARM = R_LEG + LEG_SIZE ,

	    NUMOFJOINTS = R_ARM + ARM_SIZE

	};

	enum ButtonNames
	{

	    CHEST_BUTTON = 0, L_BUMPER_L, L_BUMPER_R, R_BUMPER_L, R_BUMPER_R, BUTTONS_SIZE, NUMOFBUTTONS = BUTTONS_SIZE
	};

	enum SupportLeg
	{
		SUPPORT_LEG_NONE = 0, SUPPORT_LEG_LEFT, SUPPORT_LEG_RIGHT, SUPPORT_LEG_BOTH
	};

	enum SensorNames
	{

	    ACC = 0,

	    GYR = ACC + ACC_SIZE,

	    //ANGLE=GYR+GYR_SIZE,
	    //L_FSR=ANGLE+ANGLE_SIZE,
	    L_FSR = GYR + GYR_SIZE,
	    R_FSR = L_FSR + FSR_SIZE,
	    L_US = R_FSR + FSR_SIZE,
	    R_US = L_US + US_SIZE,
	    //BUTTONS=BUTTONS_SIZE,
	    NUMOFSENSORS = R_US + US_SIZE

	};

	enum ComputedSensorsNames
	{
	    ANGLE = 0,
		SUPPORT_LEG = ANGLE + ANGLE_SIZE,
	    NUMOFCOMPUTEDSENSORS = SUPPORT_LEG +1
	};

	struct Interpret
	{
		static const float GRAVITY_PULL = 9.81f; //(m/s^2)

		static const float GYR_Z_REF = 1230; //expected value of GYR_Z
		static const float GYR_Z_RAW = -1680; //expected value of GYR_Z_RAW
		static const float ACC_NORM = 58; //expected value of GYR_Z_RAW
		static const float GYR_GAIN = (-1.0 / 2.0) * 0.017453; //(1 / (2mv/deg/sec))* gyr ref


		static const float BUTTON_PRESSED = 0.0; //Normally on switches
		static const float BUTTON_RELEASED = 1.0; //Normally on switches
		static const float ROBOT_WEIGHT = 4.879f;
	};
	std::vector<std::string> const& getJointNames();

	std::vector<std::string> const& getJointKeys();
	std::vector<std::string> const& getPositionActuatorKeys();
	std::vector<std::string> const& getHardnessActuatorKeys();

	std::vector<std::string> const& getSensorNames();
	std::vector<std::string> const& getSensorKeys();


	//std::vector<std::string> const& getButtonNames();
	std::vector<std::string> const& getButtonKeys();

	std::map<std::string, int> const& getJointIDs();
	// Not Implemented yet :P
	//std::map<std::string,SensorNames> const& getSensorIDs();


	//std::vector< std::string> ActuatorName;

};
#endif /* ROBOT_CONSTS_H_ */

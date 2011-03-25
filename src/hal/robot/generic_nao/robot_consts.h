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

/// Returns a map from joint ids (enum Joint names) to almemory string "path" names
namespace KDeviceLists
{


	enum ChainHeadNames
	{
		YAW=0, PITCH, HEAD_SIZE
	};
	enum ChainLegNames
	{
		HIP_YAW_PITCH=0, HIP_ROLL, HIP_PITCH, KNEE_PITCH, ANKLE_PITCH, ANKLE_ROLL, LEG_SIZE
	};

	enum ChainArmNames
	{
		SHOULDER_PITCH=0, SHOULDER_ROLL, ELBOW_ROLL, ELBOW_YAW, /*WRIST_YAW, HAND,*/ARM_SIZE
	};

	enum ChainAccessNames
	{
		AXIS_X=0, AXIS_Y, AXIS_Z , AXIS_SIZE
	};

	enum ChainIniertialSizes
	{
		ACC_SIZE=AXIS_SIZE, GYR_SIZE=AXIS_SIZE,ANGLE_SIZE=AXIS_Z
	};

	enum ChainFSRNames
	{
		FSR_FL=0, FSR_FR, FSR_RL,FSR_RR, COP_X, COP_Y,TOTAL_WEIGHT,FSR_SIZE
	};
	enum ChainUltraSonicNames
	{
		US_VALUE,US_VALUE1,US_VALUE2,US_VALUE3,US_VALUE4,US_VALUE5,US_VALUE6,US_VALUE7,US_VALUE8,US_VALUE9,US_SIZE
	};
	enum RobotPositionNames
	{
		ROBOT_X=0,ROBOT_Y,ROBOT_ANGLE,ROBOTPOSITION_SIZE
	};
	enum JointNames
	{
		HEAD=0,	L_LEG=HEAD_SIZE , R_LEG=L_LEG+LEG_SIZE,L_ARM=R_LEG+LEG_SIZE,R_ARM=L_ARM+ARM_SIZE ,

		NUMOFJOINTS=R_ARM+ARM_SIZE

	};

	enum SensorNames
	{

		ACC = 0,

		GYR=ACC+ACC_SIZE,

		//ANGLE=GYR+GYR_SIZE,

		//L_FSR=ANGLE+ANGLE_SIZE,
		L_FSR=GYR+GYR_SIZE,
		R_FSR=L_FSR+FSR_SIZE,

		L_US=R_FSR+FSR_SIZE,
		R_US=L_US+US_SIZE,

		NUMOFSENSORS=R_US+US_SIZE

	};

	struct Interpret
	{
		static const float GRAVITY_PULL=9.81f; //(m/s^2)
		static const float ACC_OFFSET=0;
		static const float ACC_GAIN=9.81/55.555555f; //(56 Unites per g)

		static const float GYR_OFFSET=22000;
		static const float GYR_Z_REF=1230; //expected value of GYR_Z
		static const float GYR_GAIN=(-1/2.0f)*0.017453; //(1 / (2mv/deg/sec))* gyr ref
	};
	std::vector<std::string> const& getJointNames();

	std::vector<std::string> const& getJointKeys();
	std::vector<std::string> const& getPositionActuatorKeys();
	std::vector<std::string> const& getHardnessActuatorKeys();

	std::vector<std::string> const& getSensorNames();
	std::vector<std::string> const& getSensorKeys();

	std::map<std::string,JointNames> const& getJointIDs();
	std::map<std::string,SensorNames> const& getSensorIDs();

	//std::vector< std::string> ActuatorName;

};

#endif /* ROBOT_CONSTS_H_ */

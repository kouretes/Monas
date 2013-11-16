/*! \file LowLevelPlanner.h
 *	\brief A Monas Activity that first plans the trajectories needed by the Walk Engine
 and executes the desired walking gait!
 *
 */

#ifndef LOWLEVELPLANNER_H
#define LOWLEVELPLANNER_H
#include "core/include/IActivity.hpp"
#include "core/architecture/configurator/Configurator.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#include "messages/Motion.pb.h"

#include <iostream>
#include <string>
#include "Stepplanner.h"
#include "ControlThread.h"
#include "FootTrajectoryPlanner.h"
#include "ZMPTrajectoryPlanner.h"

#include "hal/robot/generic_nao/NAOKinematics.h"
#include "core/elements/math/KMat.hpp"
#include "hal/robot/generic_nao/aldebaran-motion.h"


/**
 Useful NameSpace
 **/
namespace LEG
{
	enum
	{
		LEFT_LEG = 0, RIGHT_LEG
	};
}
;

enum {
	DO_NOTHING =0, INIT_WALK, DO_STEPS, FINAL_STEP, WAIT_TO_FINISH, DCM_RUN, DCM_STOP
};



enum
{
	ANKL_ROLL = 0, ANKL_PITCH, ANKL_CTRL_CHAIN_SIZE
};

enum
{
	HIP_RLL = 0, HIP_PTCH, HIP_CTRL_CHAIN_SIZE
};

/**
 * \class LowLevelPlanner
 *
 * \file LowLevelPlanner.h
 **/

ACTIVITY_START
class LowLevelPlanner: public IActivity
{
	public:
		/**
		 * @brief Does nothing
		 */

		ACTIVITY_CONSTRUCTOR(LowLevelPlanner);
		/**
		 * @brief
		 * @return 0
		 */
		int ACTIVITY_VISIBLE IEX_DIRECTIVE_HOT Execute();
		/**
		 * @brief Here the Activity initializes.
		 */
		void ACTIVITY_VISIBLE UserInit();
		void ACTIVITY_VISIBLE Reset();
		std::string ACTIVITY_VISIBLE GetName()
		{
			return "LowLevelPlanner";
		}
		private:
		/**
		 Message providing Torso Velocity
		 */
		boost::shared_ptr<const MotionWalkMessage> wm;

		/**
		 From High LVL
		 **/
		std::vector<float> speed;

		/**
		 Main object instances used by KWalk
		 **/
		RobotParameters NaoRobot;
		ZMPTrajectoryPlanner NaoZmpTrajectoryPlanner;
		FootTrajectoryPlanner NaoFootTrajectoryPlanner;
		Stepplanner NaoPlanner;
		PlanePoint foot;
		GroundPoint zmp;
		KWalkMat KWalkMath;
		bool finalStep;

		float FeetTrajectory[2][SIZE_OF_TRAJECTORY_ELEMENTS][2][MAX_TRAJECTORY_LENGTH];
		float ZmpTrajectory[2][2][MAX_TRAJECTORY_LENGTH];
		CircularBuffer<float> *ZmpBuffer[2];
		int dcm_counters[2];
		int dcm_length[2];
		int next_2B_inserted,last,current_buffer;

		//Zmp

		/**
		 Used by DCM callbacks
		 **/
		std::vector<float *> jointPtr, sensorPtr;
		boost::shared_ptr<AL::ALMemoryProxy> memory;
		AL::ALMotionProxy *motion;
		AL::DCMProxy *dcm;
		NAOKinematics *nkin;
		std::vector<float> alljoints;
		AL::ALValue commands;
		/****/
		float fsr_position[4][2][2];

		unsigned int dcm_counter;

		/** Initialise the DCM part **/
		void initialise_devices();

		/** function bound to the DCM **/
		int DCMcallback();

		/** Create DCM Position Actuator Alias **/
		void createJointsPositionActuatorAlias();

		/** Prepare Command ALValue to send command to actuator **/
		void prepareJointsPositionActuatorCommand();

		/** Set one hardness value to all Body joints **/
		void setStiffness(const float &stiffnessValue);

		/** Computation of the Target Center of Mass wrt the inertial frame **/
		void Calculate_Desired_COM();
		std::vector<float> Calculate_IK();
		void Calculate_Tragectories();
		/** Leg Controllers **/
		LIPMPreviewController * NaoLIPMx ,*NaoLIPMy;

		int state, dcm_state;
		/** Real odometry **/
		NAOKinematics::kmatTable Tis,Tssprime;//Odometry, from supportleg to inertial, transformation from support leg to other leg
		NAOKinematics::kmatTable Tilerror,Tirerror;
        KDeviceLists::SupportLeg supportleg;
        KDeviceLists::ChainsNames chainsupport; //Corresponds to this supportleg
        bool double_support;
        float double_support_progress;//Progress of double support, zero in single supports
        /** FSR Transform **/
        KMath::KMat::GenMatrix<double,3,4> fsrposl,fsrposr;
        KMath::KMat::GenMatrix<double,4,1> fsrlbias,fsrrbias;

        KVecFloat2 getCoP();
	};
	ACTIVITY_END
#endif


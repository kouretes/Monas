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
#include "WalkEngine.hpp"

#include "core/elements/math/KMat.hpp"
#include "hal/robot/generic_nao/aldebaran-motion.h"


/**
 Useful NameSpace
 **/

enum {
	DO_NOTHING =0, INIT_WALK, DO_STEPS, FINAL_STEP, WAIT_TO_FINISH, DCM_RUN, DCM_STOP
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
		Stepplanner NaoPlanner;
		WalkEngine* 	engine;
		/**
		 Used by DCM callbacks
		 **/
		std::vector<float *> jointPtr, sensorPtr;
		boost::shared_ptr<AL::ALMemoryProxy> memory;
		AL::ALMotionProxy *motion;
		AL::DCMProxy *dcm;
		std::vector<float> alljoints;
		AL::ALValue commands;
		/****/
		float fsr_position[4][2][2];

		unsigned int dcm_counter;

		/** Initialise the DCM part **/
		void initialise_devices();

		/** function bound to the DCM **/
		int DCMcallback();

		/** Create DCM hardness Actuator Alias **/
		void createHardnessActuatorAlias();

		/** Create DCM Position Actuator Alias **/
		void createJointsPositionActuatorAlias();

		/** Prepare Command ALValue to send command to actuator **/
		void prepareJointsPositionActuatorCommand();

		/** Set one hardness value to all Body joints **/
		void setStiffness(const float &stiffnessValue);

		void setStiffnessDCM(const float &stiffnessValue);

		int state, dcm_state;
	};
	ACTIVITY_END
#endif


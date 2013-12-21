/*! \file LowLevelPlanner.h
 *	\brief A Monas Activity that first plans the trajectories needed by the Walk Engine
 and executes the desired walking gait!
 *
 */

#ifndef KinematicsDemo2_H
#define KinematicsDemo2_H
#include "core/include/IActivity.hpp"
#include "core/architecture/configurator/Configurator.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#include "messages/Motion.pb.h"
#include "messages/SensorsMessage.pb.h"

#include <iostream>
#include <string>
#include "core/elements/math/KMat.hpp"
#include "hal/robot/generic_nao/aldebaran-motion.h"
#include "hal/robot/generic_nao/NAOKinematics.h"



/**
 * \class KinematicsDemo2
 *
 * \file KinematicsDemo2.h
 **/

ACTIVITY_START
class KinematicsDemo2: public IActivity
{
	public:
		/**
		 * @brief Does nothing
		 */

		ACTIVITY_CONSTRUCTOR(KinematicsDemo2);
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
			return "KinematicsDemo2";
		}
		private:



		NAOKinematics nkin;
		/**
		 Used by DCM callbacks
		 **/
		std::vector<float *> jointPtr, sensorPtr;
	boost::shared_ptr<const AllSensorValuesMessage> allsm;
		boost::shared_ptr<AL::ALMemoryProxy> memory;
		AL::ALMotionProxy *motion;
		AL::DCMProxy *dcm;
		std::vector<float> alljoints;
		AL::ALValue commands;

		double angX, angY;
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

	};
	ACTIVITY_END
#endif


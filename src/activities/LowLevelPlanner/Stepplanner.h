#ifndef __STEPPLANNER_H__
#define __STEPPLANNER_H__
#include "RobotParameters.h"
#include <core/elements/math/KMat.hpp>
#include "WalkEngine.hpp"
#include <queue>


class Stepplanner
{
  private:
	RobotParameters Robot;
	/** Pelvis Rotation **/
	KMath::KMat::GenMatrix<float, 2, 2> RotPelvisZ;
	/** Pelvis and ankle Position wrt inertial frame of reference **/
	KVecFloat3 Pelvis, anklel, ankler;
	/** Desired Pelvis velocity **/
	KVecFloat3 lastvelocity, velocity, tv;
	/** Support Leg **/
	KDeviceLists::SupportLeg support;
  public:
	/** Designed step buffer **/
	std::queue<WalkInstruction> inst;
	//Stepplanner();

	/** @fn void initialize(RobotParameters )
	 *  @brief initiliazes the Stepplanner
	 *  by adding two zero speed,
	 *  and a double support instructions
	 */

	void initialize(RobotParameters );

	/** @fn void oneStep(std::vector<float> )
     *  @brief adds a walking instruction
     *  with a desired speed
     */

	void oneStep(std::vector<float> );

};
#endif

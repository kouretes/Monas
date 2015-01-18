#ifndef __WALKENGINE_HPP__
#define __WALKENGINE_HPP__
#include <core/elements/math/KMat.hpp>
#include "hal/robot/nao/generic_nao/robot_consts.h"
#include "core/elements/buffer/LockedBuffer.hpp"
#include  "ControlThread.h"
#include "hal/robot/nao/generic_nao/NAOKinematics.h"
#include "RobotParameters.h"
#include "KWalkMat.h"
#include "core/elements/math/Common.hpp"


/** A class with the necessary data
 * for the walking procedure
 **/
class WalkInstruction
{
public:

	KVecFloat3 target;
	KDeviceLists::SupportLeg targetSupport, targetZMP;
	float ttlspeed;
	unsigned steps;

};

/** Walking Instruction Buffer, with incoming walking instructions
 * from the Stepplanner
 **/
typedef LockedBuffer<WalkInstruction> WalkBuffer;


class WalkEngine
{

public:
	/** WalkEngine Constructor **/
	WalkEngine(RobotParameters &rp);

	/** Necessary Instances by WalkEngine **/
	RobotParameters &NaoRobot;
	KWalkMat interp;
	NAOKinematics nkin;
	WalkBuffer walkbuffer;



	/** Arm joint data **/
	KMath::KMat::GenMatrix<double, 4, 1> armangles;

	/** WalkEngine Public functions **/

	/** @fn std::vector<float> runStep()
	 *  @brief Main Walking Engine function
	 *   computes the desired trajectories and
	 *   the joints
	 **/
	std::vector<float> runStep();
	/** @fn void Reset()
	 *  @brief resets the Walking Engine
	 **/
	void Reset();
	/** @fn void addInit()
	 *  @brief initializes the Walking Engine
	 **/
	void addInit();

	/** @fn void setFSR(KMath::KMat::GenMatrix<double, 4, 1> l, KMath::KMat::GenMatrix<double, 4, 1> r)
	 *  @brief Sets the values of the FSRs
	 **/
	void setFSR(KMath::KMat::GenMatrix<double, 4, 1> l, KMath::KMat::GenMatrix<double, 4, 1> r);

    /** @fn void initFSR(KMath::KMat::GenMatrix<double, 3, 4> l, KMath::KMat::GenMatrix<double, 3, 4> r)
     *  @brief initializes the FSR posistions
     **/
	void initFSR(KMath::KMat::GenMatrix<double, 3, 4> l, KMath::KMat::GenMatrix<double, 3, 4> r)
	{
		fsrposl = l;
		fsrposr = r;
	};

private:


	/** FSR position matrices and FSR Data vectors **/
	KMath::KMat::GenMatrix<double, 3, 4> fsrposl, fsrposr;
	KMath::KMat::GenMatrix<double, 4, 1> fsrl, fsrr;

	/** Real odometry Data **/
	//Odometry, from supportleg to inertial, transformation from support leg to other leg
	NAOKinematics::kmatTable Tis, Tsp, Tssprime, Til, Tir, Tilold, Tirold, Tilerror, Tirerror ;

	/** The Inverse Kinematics Targets **/
	NAOKinematics::kmatTable Tpprimel,Tpprimer;

	/** Starting interpolating foot points,
	 * points generated from interpolation,
	 * and target foot poings at the end of cycle
	 * **/
	KVecFloat3 startL, startR, dl, dr, planL, planR, predicterror;

	/** Support Leg **/
	KDeviceLists::SupportLeg supportleg;

	KDeviceLists::ChainsNames chainsupport;

	/** Double support Indicator **/
	bool double_support;

	/** Measuring the progress of the double support phase, zero in single supports **/
	float double_support_progress;

	/**
	 * Current Walking Instruction at the walking cycle
	 * Planned Walking Instruction at the end of the walking cycle
	**/
	WalkInstruction ci, planned;

	/** Current Walking Command counter **/
	unsigned currentstep;

	/** Walking Instruction to be executed **/
	std::queue<WalkInstruction> qbuffer;

	/** MPC Controller **/
	LIPMPreviewController NaoLIPM;

	/** ZMP Buffer for the Prediction Horizon **/
	KVecFloat3 destZMP;
	CircularBuffer<KVecFloat3> 	Zbuffer;

	/** Measured Center of Mass from the joint encoders **/
	KVecDouble3 measuredcom;
	int balance;

   /** @fn void feed()
    *  @brief Filling the Command and ZMP buffers
   **/
	void feed();

	/** @fn void Calculate_Desired_COM()
	 *  @brief Computation of the Target Center of Mass wrt the Inertial Frame of Reference
    **/
	void Calculate_Desired_COM(int balance);

	/** @fn std::vector<float> Calculate_IK()
	 *  @brief Computation of leg joint angles
	**/
	std::vector<float> Calculate_IK();

	/** @fn void Calculate_Tragectories()
	 *  @brief Interpolation function
	**/
	void Calculate_Tragectories();

	/** @fn void planInstruction(KVecFloat3 destZMP, unsigned steps)
	 *  @brief ZMP Interpolation procedure
     **/
	void planInstruction(KVecFloat3 destZMP, unsigned steps);

	/** @fn KVecFloat2 getCoP()
	 *  @brief Computation of Center of Pressure, x,y data
	**/

	KVecFloat2 getCoP();

	/** @fn KVecFloat3 getPositionInertial(NAOKinematics::Effectors ef)
	 *  @brief Compute the Position x, y and orientation omega
	 *   wrt Inertial Frame of Reference
	**/
	inline KVecFloat3 getPositionInertial(NAOKinematics::Effectors ef)
	{
		KVecFloat3 t, r;
		/** Do not change the parenthesis **/
		NAOKinematics::kmatTable m = Tis*(Tsp*nkin.getForwardEffector(ef));
		t = m.getTranslation();
		r(0) = t(0) / 1000.0;
		r(1) = t(1) / 1000.0;
		r(2) = m.getEulerAngles()(2);
		return r;

	};

	/** @fn NAOKinematics::kmatTable getTransformation(KVecFloat3 p, float z)
	 *  @brief Compute a Transformation matrix without the pitch roll angles
	**/
	inline NAOKinematics::kmatTable getTransformation(KVecFloat3 p, float z)
	{
		NAOKinematics::kmatTable t;
		KMath::KMat::transformations::makeTransformation(t,
			(double) p(0) * 1000,
			(double) p(1) * 1000,
			(double) z * 1000,
			0.0,
			0.0,
			(double) p(2)
			);
		return t;
	};

	/** @fn float anglemean(float l, float r) const
	 *  @brief Computation of mean angle
	**/
	inline float anglemean(float l, float r) const
	{
		return (float) ((double) l + KMath::anglediff2( (double) r, (double) l) / 2.0);
	}


};
#endif

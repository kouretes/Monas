#ifndef __WALKENGINE_HPP__
#define __WALKENGINE_HPP__
#include <core/elements/math/KMat.hpp>
#include "hal/robot/generic_nao/robot_consts.h"
#include "core/elements/buffer/LockedBuffer.hpp"
#include  "ControlThread.h"
#include "hal/robot/generic_nao/NAOKinematics.h"
#include "RobotParameters.h"
#include "KWalkMat.h"
#include "core/elements/math/Common.hpp"
class WalkInstruction
{
	public:

	KVecFloat3 target;
	KDeviceLists::SupportLeg targetSupport,targetZMP;

	unsigned steps;

};
typedef LockedBuffer<WalkInstruction> WalkBuffer;


class WalkEngine
{



public:
	WalkEngine(RobotParameters &rp);
	RobotParameters &NaoRobot;
	KWalkMat interp;
	NAOKinematics nkin;
		WalkBuffer walkbuffer;
	void setFSR(KMath::KMat::GenMatrix<double,4,1> l,KMath::KMat::GenMatrix<double,4,1> r);
	void initFSR(KMath::KMat::GenMatrix<double,3,4> l,KMath::KMat::GenMatrix<double,3,4> r)
	{
		fsrposl=l;
		fsrposr=r;
	};
	std::vector<float> runStep();

	void Reset();
	void addInit();

	KMath::KMat::GenMatrix<double,4,1> armangles;

private:
	void feed();

		/** Computation of the Target Center of Mass wrt the inertial frame **/
	void Calculate_Desired_COM();
	std::vector<float> Calculate_IK();
	void Calculate_Tragectories();



	KVecFloat2 getCoP();


	/** Real odometry **/


	NAOKinematics::kmatTable Tis,Tsp,Tssprime;//Odometry, from supportleg to inertial, transformation from support leg to other leg
	KVecFloat3 startL,startR;
	NAOKinematics::kmatTable Tilold,Tirold,Tilerror,Tirerror;

	/** FSR Transform **/
	KMath::KMat::GenMatrix<double,3,4> fsrposl,fsrposr;
	KMath::KMat::GenMatrix<double,4,1> fsrl,fsrr;





	KDeviceLists::SupportLeg supportleg;
	KDeviceLists::ChainsNames chainsupport; /// Corresponds to this supportleg
    bool double_support;
    float double_support_progress;  ///Progress of double support, zero in single supports
	WalkInstruction ci;
	KVecFloat3 predicterror;
	unsigned currentstep;
	std::queue<WalkInstruction> qbuffer;

	/** Leg Controllers **/
	LIPMPreviewController NaoLIPM;//NaoLIPMy;
	CircularBuffer<KVecFloat3> 	Zbuffer;
	KVecFloat3 planL,planR;
	WalkInstruction planned;

	float comzmeasured;



	inline KVecFloat3 getPositionInertial(NAOKinematics::Effectors ef)
	{
		KVecFloat3 t,r;
		//The following mult parenthesis makes all the difference in the world
		NAOKinematics::kmatTable m=Tis*(Tsp*nkin.getForwardEffector(ef));
		t=m.getTranslation();
		r(0)=t(0)/1000.0;
		r(1)=t(1)/1000.0;
		r(2)=m.getEulerAngles()(2);
		return r;

	};
	NAOKinematics::kmatTable getTransformation(KVecFloat3 p,float z)
	{
		NAOKinematics::kmatTable t;
		KMath::KMat::transformations::makeTransformation(t,
				(double)p(0)*1000,
				(double)p(1)*1000,
				(double)z*1000,
				0.0,
				0.0,
				(double)p(2)
				);
		return t;
	};

	float anglemean(float l, float r) const
	{
		return l+KMath::anglediff2(r,l)/2;
	}


};



#endif //__WALKENGINE_HPP__

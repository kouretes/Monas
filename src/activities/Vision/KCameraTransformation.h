#ifndef KCAMERATRANSFORMATION_H
#define KCAMERATRANSFORMATION_H

#include <boost/date_time/posix_time/posix_time.hpp>
#include "hal/robot/generic_nao/aldebaran-kme.h"


#include "core/elements/math/KMat.hpp"



#define sqrd(x) ( (x)*(x) )
//#include "boost/date_time/local_time/local_time.hpp"


typedef  struct measurement_struct
{
	float mean;
	float var;
}  measurement;


class CameraT : public KMath::KMat::transformations
{

	static const double TAU = 33.33333333333333333333333333333333; //Skew constant of CMOS sensor in ms
	//v is velocity components
	//tau is time per line. constant of CMOS sensors
	template<typename T> static void skew2d(KMath::KMat::ATMatrix<T, 3> t, const KMath::KMat::GenMatrix<T, 2, 1>&  v, const T tau)
	{
		t.identity();
		t.AisIdentity = false;
		t.BisZero = false;
		t.A(1, 2) = v(1) * tau;
		t.A(2, 2) = 1 + v(2) * tau;
		t.B.copyFrom(v);
		v.scalar_mult(tau);
		v.scalar_mult((T)0.5);
	};
};
//double KMat::TAU=33.33333333333333333333333333333333;//Skew constant of CMOS sensor in ms




typedef struct cameraPose
{
	float angX;
	float angY;
	//float VangX;
	//float VangY;
	float yaw;
	float pitch;
	//float Vyaw;
	//float Vpitch;
	float cameraPitch;
	//unsigned  timediff;
	//boost::posix_time::ptime time;//time of sensors
	float focallength;//in pixels
	float cameraX;
	float cameraY;
	float cameraZ;
	//float imcomp;//Image compensation time in ms;

} cpose;


class   KCameraTranformation
{

public:

	KCameraTranformation();
	void Init();

	void setPose(cpose p);

	std::vector<float>  getKinematics(const std::string toAsk) const;
	float cot(float theta) const;
	KVecFloat3  camera2dTo3d(const KVecFloat2 &coords) const;
	KVecFloat2  camera3dTo2d(const KVecFloat3 &coords) const;
	KVecFloat3  cameraToGround(const KVecFloat3 &c3d) const;
	KVecFloat3  camera2dToTorso(const KVecFloat2 &coords) const;
	KVecFloat3  camera2dToGround(const KVecFloat2 &c2d) const;
	KVecFloat3  cameraToGroundProjection(const KVecFloat3 &c3d, float height) const;
	KVecFloat3  camera2dToGroundProjection(const KVecFloat2 &c2d, float height) const;
	KVecFloat2  groundToCamera2d(const KVecFloat3 &g) const;
	float vectorAngle(const KVecFloat2 &v1, const KVecFloat2 &v2) const;
	measurement  angularDistance(const KVecFloat2 &v1, const KVecFloat2 &v2, float realsize) const;
	measurement  angularDistanceProjected(const KVecFloat2 &v1, const KVecFloat2 &v2, float realsize) const;
	measurement *projectionDistance(KVecFloat2 &v, float height) const;
	float getPitch() const;
	float getRoll() const;

private:
	cpose thepose;
	KMath::KMat::ATMatrix<float, 4> cameraChain; //Camera to ground transformation
	KMath::KMat::ATMatrix<float, 4> cameraTorsoChain; //CameraToTorso;
	KMath::KMat::ATMatrix<float, 4> cameraChainInv; //Inverse
	//KMat::ATMatrix<float,4> cameraYawless;
	boost::shared_ptr<AL::ALMotionProxy> motion;

};

#endif

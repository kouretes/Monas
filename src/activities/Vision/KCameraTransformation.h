#ifndef KCAMERA_TRANSFORMATION_H
#define KCAMERA_TRANSFORMATION_H

#include "KMat.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include "hal/robot/generic_nao/aldebaran-kme.h"

#define sqrd(x) ( (x)*(x) )
//#include "boost/date_time/local_time/local_time.hpp"
typedef struct measurement_struct
{
    float mean;
    float var;
} measurement;

class CameraT : public KMat::transformations
{

		static const double TAU=33.33333333333333333333333333333333;//Skew constant of CMOS sensor in ms
		//v is velocity components
		//tau is time per line. constant of CMOS sensors
		template<typename T> static void skew2d(KMat::ATMatrix<T,3> t,const KMat::GenMatrix<T,2,1>&  v,const T tau)
		{
			t.identity();
			t.AisIdentity=false;
			t.BisZero=false;
			t.A(1,2)=v(1)*tau;
			t.A(2,2)=1+v(2)*tau;
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
	float VangX;
	float VangY;
	float yaw;
	float pitch;
	float Vyaw;
	float Vpitch;
	float cameraPitch;
	unsigned  timediff;
	boost::posix_time::ptime time;//time of sensors
    float focallength;//in pixels
    float cameraX;
    float cameraY;
    float cameraZ;
	//float imcomp;//Image compensation time in ms;

} cpose;


class KCameraTranformation
{

	public:

		KCameraTranformation();
        void Init();

        void setPose(cpose p);
        //float getPitch();
        //float getRoll();




		std::vector<float>  getKinematics(const std::string toAsk);
		float cot(float theta);
		KVecFloat3  camera2dTo3d(const KVecFloat2 & coords);
		KVecFloat2  camera3dTo2d(const KVecFloat3 & coords);
		KVecFloat3  cameraToGround(const KVecFloat3 & c3d);
		KVecFloat3  camera2dToTorso(const KVecFloat2 & coords);
		KVecFloat3  camera2dToGround(const KVecFloat2& c2d);
        KVecFloat3  cameraToGroundProjection(const KVecFloat3 & c3d,float height);
		KVecFloat3  camera2dToGroundProjection(const KVecFloat2& c2d,float height);
		KVecFloat2  groundToCamera2d(const KVecFloat3& g);
		float vectorAngle(const KVecFloat2 & v1,const KVecFloat2 & v2);
		measurement  angularDistance(const KVecFloat2 & v1,const KVecFloat2 & v2,float realsize);
        measurement  angularDistanceProjected(const KVecFloat2 & v1,const KVecFloat2 & v2,float realsize);
        measurement* projectionDistance(KVecFloat2 &v,float height);
        float getPitch();
        float getRoll();

	private:
        cpose thepose;
        KMat::ATMatrix<float,4> cameraChain;//Camera to ground transformation
        KMat::ATMatrix<float,4> cameraTorsoChain;//CameraToTorso;
        KMat::ATMatrix<float,4> cameraChainInv;//Inverse
        //KMat::ATMatrix<float,4> cameraYawless;
		AL::ALPtr<AL::ALMotionProxy> motion;

};



#endif

#ifndef KCAMERA_TRANSFORMATION_H
#define KCAMERA_TRANSFORMATION_H

#include "KMat.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include "hal/robot/aldebaran-kme.h"

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
		template<typename T> static void skew2d(KMat::ATMatrix<T,3> t,const KMat::HCoords<T,2>&  v,const T tau)
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
		KMat::HCoords<float,3>  camera2dTo3d(const KMat::HCoords<float,2> & coords);
		KMat::HCoords<float,2>  camera3dTo2d(const KMat::HCoords<float,3> & coords);
		KMat::HCoords<float,3>  cameraToGround(const KMat::HCoords<float,3> & c3d);
		KMat::HCoords<float,3>  camera2dToTorso(const KMat::HCoords<float,2> & coords);
		KMat::HCoords<float,3>  camera2dToGround(const KMat::HCoords<float,2>& c2d);
        KMat::HCoords<float,3>  cameraToGroundProjection(const KMat::HCoords<float,3> & c3d,float height);
		KMat::HCoords<float,3>  camera2dToGroundProjection(const KMat::HCoords<float,2>& c2d,float height);
		KMat::HCoords<float,2>  groundToCamera2d(const KMat::HCoords<float,3>& g);
		float vectorAngle(const KMat::HCoords<float,2> & v1,const KMat::HCoords<float,2> & v2);
		measurement  angularDistance(const KMat::HCoords<float,2> & v1,const KMat::HCoords<float,2> & v2,float realsize);
        measurement  angularDistanceProjected(const KMat::HCoords<float,2> & v1,const KMat::HCoords<float,2> & v2,float realsize);
        measurement* projectionDistance(KMat::HCoords<float,2> &v,float height);
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

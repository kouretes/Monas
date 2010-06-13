#ifndef KCAMERA_TRANSFORMATION_H
#define KCAMERA_TRANSFORMATION_H
#include "KMat.h"
#include <boost/date_time/posix_time/posix_time.hpp>
//#include "boost/date_time/local_time/local_time.hpp"


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
  unsigned  timediff;
  boost::posix_time::ptime time;//time of sensors
  float imcomp;//Image compensation time in ms;

} cpose;




#endif

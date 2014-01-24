#ifndef __Kouretes_Walk_Engine__Kalman__
#define __Kouretes_Walk_Engine__Kalman__
#include "core/elements/math/KMat.hpp"
#include "RobotParameters.h"
#include <queue>

class Kalman{

private:
    KMath::KMat::GenMatrix<float,2,1> Ckalman;
    KMath::KMat::GenMatrix<float,1,2> Kgain;
    KMath::KMat::GenMatrix<float,2,2>s,MeasurementNoise;
    KMath::KMat::GenMatrix<float,1,1>P, ProcessNoise,Bkalman;
    RobotParameters &OurRobot;
public:
    KMath::KMat::GenMatrix<float,1,1> StateKalman,StatePredict;
    std::queue<float> uBuffer,combuffer;
    KMath::KMat::GenMatrix<float,2,1> ykalman;
    void Filter(float ZMPMeasured,float CoMMeasured);
    Kalman(RobotParameters &robot);

};
#endif

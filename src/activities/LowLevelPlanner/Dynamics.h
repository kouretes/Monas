#ifndef __Kouretes_Walk_Engine__Dynamics__
#define __Kouretes_Walk_Engine__Dynamics__
#include "core/elements/math/KMat.hpp"
#define LagN 15
#define PreviewWindow 101
#include "RobotParameters.h"
class Dynamics
{
    private:
    KMath::KMat::GenMatrix<float, 4, 4> Ad;
    KMath::KMat::GenMatrix<float,4,1> Bd,Stateold,temp;
    KMath::KMat::GenMatrix<float, 3, 1> Cd;
    KMath::KMat::GenMatrix<float, 4, 2>  L;
    RobotParameters &OurRobot;
    public:
    KMath::KMat::GenMatrix<float,4,1> State,State_e;
    float predictedError,zmpstate,zmpstateNew;
    Dynamics(RobotParameters &robot);
    void AugmentState();
    void Update(float u,KVecFloat2 error);


};
#endif

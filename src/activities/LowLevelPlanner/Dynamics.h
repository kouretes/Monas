#ifndef __Kouretes_Walk_Engine__Dynamics__
#define __Kouretes_Walk_Engine__Dynamics__
#include "core/elements/math/KMat.hpp"

#include "RobotParameters.h"
class Dynamics
{
    private:
    KMath::KMat::GenMatrix<float, 4, 4> Ad;
    KMath::KMat::GenMatrix<float,4,1> Bd,Stateold,temp;

    KMath::KMat::GenMatrix<float, 4, 2>  L;
    RobotParameters &OurRobot;
    int startup;
    public:
    KMath::KMat::GenMatrix<float,4,1> State,State_e;
    KMath::KMat::GenMatrix<float, 3, 1> Cd;
    float predictedError,zmpstate,zmpstateNew;
    Dynamics(RobotParameters &robot);
    void AugmentState();
    void Update(float u,KVecFloat2 error);

    void RolloutUpdate(float u);

};
#endif
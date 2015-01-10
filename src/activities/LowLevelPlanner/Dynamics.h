#ifndef __DYNAMICS_H__
#define __DYNAMICS_H__
#include "RobotParameters.h"
#include "core/elements/math/KMat.hpp"

class Dynamics
{
    private:

	KMath::KMat::GenMatrix<float, 4, 4> Ad;

    KMath::KMat::GenMatrix<float,4,1> Bd,Stateold,temp;

    KMath::KMat::GenMatrix<float, 4, 2>  L;

    RobotParameters &OurRobot;

    int startup;

    public:

    KMath::KMat::GenMatrix<float, 3, 1> Cd;

    KMath::KMat::GenMatrix<float,4,1> State,State_e;

    float predictedError,zmpstate,zmpstateNew;

    Dynamics(RobotParameters &robot);
    /** @fn AugmentState()
     *  @brief Computes the augmented state vector
     */
    void AugmentState();

    /** @fn Update(float u, KVecFloat2 error)
     *  @brief Update the Linear Dynamics
     * of the Cart and Table
    **/
    void Update(float u,KVecFloat2 error);


};
#endif

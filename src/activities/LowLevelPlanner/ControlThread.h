//
//  ControlThread.h
//  Kouretes Walk Engine
//
//  Created by Stelios Piperakis on 8/14/13.
//  Copyright (c) 2013 SP. All rights reserved.
//
#include "core/elements/math/KMat.hpp"
#include <vector>
#include "RobotParameters.h"
#ifndef __Kouretes_Walk_Engine__ControlThread__
#define __Kouretes_Walk_Engine__ControlThread__
class LIPMPreviewController{
    private:
    KMath::KMat::GenMatrix<float, 3, 3> Aobs,Ad;
    KMath::KMat::GenMatrix<float, 3, 1> Bd,Cd,L,State,temp;
    KMath::KMat::GenMatrix<float,1,3> Gx;
    KMath::KMat::GenMatrix<float, 50, 1> Gd;
    KMath::KMat::GenMatrix<float,51,1> ZMPReference;
    float Gi,Integrationfb,Statefb,Predictionfb,Com,ZMPMeasured,u;
    int counter;
    RobotParameters OurRobot;
    public:
    LIPMPreviewController(RobotParameters robot);
    void LIPMComPredictor(std::vector<float> ZMP);
    std::vector<float> COM;

};


#endif /* defined(__Kouretes_Walk_Engine__ControlThread__) */

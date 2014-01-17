//
//  ControlThread.h
//  Kouretes Walk Engine
//
//  Created by Stelios Piperakis on 8/14/13.
//  Copyright (c) 2013 SP. All rights reserved.
//

#ifndef __Kouretes_Walk_Engine__ControlThread__
#define __Kouretes_Walk_Engine__ControlThread__
#include "core/elements/math/KMat.hpp"
#include <vector>
#include <queue>
#include <iostream>
#include "CircularBuffer.hpp"
#include "RobotParameters.h"
#include "Kalman.h"
#include "Dynamics.h"
#define PreviewWindow 101
#define LagN 15
class LIPMPreviewController{
    private:
    KMath::KMat::GenMatrix<float,LagN,4> Kx;
    KMath::KMat::GenMatrix<float,LagN,PreviewWindow-1> Ky;
    KMath::KMat::GenMatrix<float,PreviewWindow-1,1> ZMPReferenceX,ZMPReferenceY;
    /** DMPC **/
    KMath::KMat::GenMatrix<float, LagN, LagN>  Omega;
    KMath::KMat::GenMatrix<float, PreviewWindow-1, LagN>  Phi;
    KMath::KMat::GenMatrix<float, PreviewWindow-1, 4>  Tau;
    KMath::KMat::GenMatrix<float, LagN, LagN>  Al;
    KMath::KMat::GenMatrix<float, LagN, 1>  L0,httaX,httaY;

    void generateLaguerre();

 	public:
 	    RobotParameters &OurRobot;
 	    Kalman KalmanX,KalmanY;
 	    Dynamics DynamicsX,DynamicsY;
		LIPMPreviewController(RobotParameters&);
		void LIPMComPredictor(CircularBuffer<float> & ZmpBufferX,CircularBuffer<float> & ZmpBufferY,float CoMmeasuredX,float CoMmeasuredY,float ZMPMeasuredX,float ZMPMeasuredY);
		void DMPC();
        float DeltauX,DeltauY,uX,uY;
		float predictedErrorX,predictedErrorY;
		float d;
		KVecFloat2 COM;
};


#endif /* defined(__Kouretes_Walk_Engine__ControlThread__) */

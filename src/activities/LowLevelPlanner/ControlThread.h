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
#include "RobotParameters.h"
#include <queue>
#include <iostream>
#include "CircularBuffer.hpp"

#define PreviewWindow 101
#define LagN 15
class LIPMPreviewController{
    private:
    KMath::KMat::GenMatrix<float, 4, 4> Ad;
    KMath::KMat::GenMatrix<float,4,1>Bd,State,Stateold,temp;
    KMath::KMat::GenMatrix<float, 3, 1> Cd;
    KMath::KMat::GenMatrix<float, 4, 2>  L;

    /** DMPC **/
    KMath::KMat::GenMatrix<float, LagN, LagN>  Omega;
    KMath::KMat::GenMatrix<float, LagN, PreviewWindow-1>  Phi;
    KMath::KMat::GenMatrix<float, LagN, 4>  Psi;
    KMath::KMat::GenMatrix<float, 4, 4>  Ae;
    KMath::KMat::GenMatrix<float, 4, 1>  Be;
    KMath::KMat::GenMatrix<float, 1, 4>  Ce;
    KMath::KMat::GenMatrix<float, LagN, LagN>  Al;
    KMath::KMat::GenMatrix<float, LagN, 1>  L0,htta;
    KMath::KMat::GenMatrix<float, 4, 1> State_e;



    std::queue<float> uBuffer,combuffer;
	KMath::KMat::GenMatrix<float,2,1> Ckalman;
	KMath::KMat::GenMatrix<float,1,2> Kgain;
	KMath::KMat::GenMatrix<float, 2, 3>  C;
	KMath::KMat::GenMatrix<float,2,2>s,MeasurementNoise;
	KMath::KMat::GenMatrix<float,1,1>P, ProcessNoise;
	KMath::KMat::GenMatrix<float,2,1> ykalman;
	KMath::KMat::GenMatrix<float,1,1>StateKalman,StatePredict,Bkalman;

    KMath::KMat::GenMatrix<float,LagN,4> Kx;
    KMath::KMat::GenMatrix<float,LagN,PreviewWindow-1> Ky;
    KMath::KMat::GenMatrix<float,PreviewWindow-1,1> ZMPReference;
    RobotParameters OurRobot;

 	public:
		LIPMPreviewController(RobotParameters robot);
		void LIPMComPredictor(CircularBuffer<float> & ZmpBuffer,float CoMmeasured,float ZMPMeasured);
		void DMPC();
        float Deltau,u;
		float Com;
		float predictedError;
		float d;
};


#endif /* defined(__Kouretes_Walk_Engine__ControlThread__) */

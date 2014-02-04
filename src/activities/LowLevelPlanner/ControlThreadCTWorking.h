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

class LIPMPreviewController{
    private:
    KMath::KMat::GenMatrix<float, 4, 4> Ad;
    KMath::KMat::GenMatrix<float,4,1>Bd,State,temp;
    KMath::KMat::GenMatrix<float, 3, 1> Cd;
    KMath::KMat::GenMatrix<float, 4, 2>  L;

    std::queue<float> uBuffer,combuffer;
	KMath::KMat::GenMatrix<float,2,1> Ckalman;
	KMath::KMat::GenMatrix<float,1,2> Kgain;
	KMath::KMat::GenMatrix<float, 2, 3>  C;
	KMath::KMat::GenMatrix<float,2,2>s,MeasurementNoise;
	KMath::KMat::GenMatrix<float,1,1>P, ProcessNoise;
	KMath::KMat::GenMatrix<float,2,1> ykalman;
	KMath::KMat::GenMatrix<float,1,1>StateKalman,StatePredict,Bkalman;

    KMath::KMat::GenMatrix<float,1,3> Gx;
    KMath::KMat::GenMatrix<float, PreviewWindow-1, 1> Gd;
    KMath::KMat::GenMatrix<float,PreviewWindow,1> ZMPReference;
    float Gi,Integrationfb,Statefb,Predictionfb,u;
    RobotParameters OurRobot;

 	public:
		LIPMPreviewController(RobotParameters robot);
		void LIPMComPredictor(CircularBuffer<float> & ZmpBuffer,float CoMmeasured,float ZMPMeasured);

		float Com;
		float predictedError;
		float d;
};


#endif /* defined(__Kouretes_Walk_Engine__ControlThread__) */

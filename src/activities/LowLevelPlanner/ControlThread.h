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
#define KPROFILING_ENABLED
#include "tools/profiler.hpp"
#include "matlog.h"
#include "Kalman.h"
#include "Dynamics.h"
#define PreviewWindow 101

class LIPMPreviewController{
    private:
        KMath::KMat::GenMatrix<float,1,3> Gx;
        KMath::KMat::GenMatrix<float, PreviewWindow-1, 1> Gd;
        KMath::KMat::GenMatrix<float,PreviewWindow,1> ZMPReferenceX,ZMPReferenceY;
        float Gi,Integrationfbx,Statefbx,Predictionfbx,Integrationfby,Statefby,Predictionfby;
        mutable KProfiling::profiler walkprof;
 	public:
        RobotParameters &OurRobot;
        Kalman KalmanX,KalmanY;
        Dynamics DynamicsX,DynamicsY;
		LIPMPreviewController(RobotParameters &);
        void LIPMComPredictor(CircularBuffer<KVecFloat3> & ZmpBuffer,float CoMmeasuredX,float CoMmeasuredY,float ZMPMeasuredX,float ZMPMeasuredY);
		float uX,uY;
		float predictedErrorX,predictedErrorY;
		KVecFloat2 COM;
		mat_log flog;
        bool isDoubleSupport;
};


#endif /* defined(__Kouretes_Walk_Engine__ControlThread__) */

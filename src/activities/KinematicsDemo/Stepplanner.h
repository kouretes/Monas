//
//  Stepplanner.h
//  Kouretes Walk Engine
//
//  Created by Stelios Piperakis on 8/14/13.
//  Copyright (c) 2013 SP. All rights reserved.
//
#ifndef STEPPLANNER_H_
#define STEPPLANNER_H_
#include "RobotParameters.h"
#include <core/elements/math/KMat.hpp>
#include "WalkEngine.hpp"
#include <queue>





class Stepplanner
{
	private:
		RobotParameters Robot;
		bool  rightsupport;
        KMath::KMat::GenMatrix<float,2,1> tempVec;
        KMath::KMat::GenMatrix<float,2,2> RotFootZ,RotPelvisZ,RotStepZ;
        KVecFloat3 Pelvis,anklel,ankler;
        KDeviceLists::SupportLeg support;
        KMath::KMat::GenMatrix<float,2,5> Foot;
        float dx,dy,dtheta,Time;
	public:
		std::queue<WalkInstruction> inst;
		Stepplanner();
		int getStepcounter();
		void oneStep(std::vector<float> v);
		void initialize(RobotParameters );


  	};
#endif

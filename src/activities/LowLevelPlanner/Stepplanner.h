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
#include <queue>



enum
{
	X = 0, Y, Z, Theta, SIZE_OF_TRAJECTORY_ELEMENTS
};
enum
{
	LEFT = 0, RIGHT
};

struct GroundPoint{
		float x,y;
};

struct PlanePoint{
		float x,y,theta;
};


class Stepplanner
{
	private:
		PlanePoint PrevAnkle,Ankle,Step,Pelvis;
		RobotParameters Robot;
        GroundPoint ZMP;
		int stepcounter,LEG;
        bool firstRun,RIGHTSUPPORT;
        KMath::KMat::GenMatrix<float,2,1> tempVec;
        KMath::KMat::GenMatrix<float,2,2> RotFootZ,RotPelvisZ,RotStepZ;
        KMath::KMat::GenMatrix<float,2,5> Foot;
        float dx,dy,dtheta,Time;
	public:
		std::queue<GroundPoint> ZmpQ;
		std::queue<PlanePoint> FootQ;
		Stepplanner();
		int getStepcounter();
		void initialize(RobotParameters );
		PlanePoint getPelvis();
		void oneStep(std::vector<float> );
        void afterStep();
        void finalStep();
        void clear();
        bool IfFirstRightSupportleg();
        const std::queue<PlanePoint>& getFootQ() const
        {
            return FootQ;
        }



        const std::queue<GroundPoint>& getZmpQ() const
        {
            return ZmpQ;
        }

  	};
#endif

//
//  RobotParameters.h
//  Kouretes Walk Engine
//
//  Created by Stelios Piperakis on 8/14/13.
//  Copyright (c) 2013 SP. All rights reserved.
//
#ifndef ROBOTPARAMETERS_H_
#define ROBOTPARAMETERS_H_
#include <string>
#include <iostream>
#include <cmath>

#define MAX_TRAJECTORY_LENGTH 250

enum
{
	StepX=0, StepY, StepZ, H0,HX,ComZ, Tstep,Tinit, Tds, Tss, Ts, MaxStepX, MaxStepY, MaxStepTheta,g,AdaptiveStepTolx,AdaptiveStepToly,CoMZModelError, ParametersSize
};

class RobotParameters
{

	private:
		float WalkParameters[ParametersSize];

	public:
		RobotParameters()
		{
			WalkParameters[Ts] = 0.01;
			WalkParameters[StepX] = 0.155;
			WalkParameters[StepY] = 0.085;
			WalkParameters[StepZ] = 0.015;
			WalkParameters[H0] = 0.05;
			WalkParameters[HX] = -0.02;//225
			WalkParameters[ComZ] = 0.265;
			WalkParameters[CoMZModelError] = 1.25;
			WalkParameters[Tstep] = 0.45;//0.28
            WalkParameters[Tinit]= 0.50;
			WalkParameters[Tds] = 0.40;// ceil(0.25 * WalkParameters[Tstep]/WalkParameters[Ts])*WalkParameters[Ts];//0.35
			WalkParameters[Tss] = 0.60; //WalkParameters[Tstep]-WalkParameters[Tds];
			WalkParameters[MaxStepX] = 0.0400;
			WalkParameters[MaxStepY] = 0.0400;
			WalkParameters[MaxStepTheta] = 0.6980;
			WalkParameters[g] = 9.81;
			WalkParameters[AdaptiveStepTolx]=0.05;
            WalkParameters[AdaptiveStepToly]=0.1;


		}
		;
		float getWalkParameter(int);
};
#endif

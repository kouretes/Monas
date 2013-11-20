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
	StepX=0, StepY, StepZ, H0,HX,ComZ, Tstep, Tds, Tss, Ts, MaxStepX, MaxStepY, MaxStepTheta,g,AdaptiveStepTol, ParametersSize
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
			WalkParameters[StepZ] = 0.02;
			WalkParameters[H0] = 0.05;
			WalkParameters[HX] = -0.0225;
			WalkParameters[ComZ] = 0.250;
			WalkParameters[Tstep] = 0.38;

			WalkParameters[Tds] = ceil(0.35 * WalkParameters[Tstep]/WalkParameters[Ts])*WalkParameters[Ts];
			WalkParameters[Tss] =  WalkParameters[Tstep]-WalkParameters[Tds];
			WalkParameters[MaxStepX] = 0.0400;
			WalkParameters[MaxStepY] = 0.0400;
			WalkParameters[MaxStepTheta] = 0.6980;
			WalkParameters[g] = 9.81;
			WalkParameters[AdaptiveStepTol]=0.01;


		}
		;
		float getWalkParameter(int);
};
#endif

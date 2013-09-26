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

#define MAX_TRAJECTORY_LENGTH 250

enum
{
	StepX=0, StepY, StepZ, H0, ComZ, Tstep, Tds, Tss, Ts, MaxStepX, MaxStepY, MaxStepTheta,g, ParametersSize
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
			WalkParameters[StepZ] = 0.0353;
			WalkParameters[H0] = 0.060;
			WalkParameters[ComZ] = 0.260;
			WalkParameters[Tstep] = 0.4500;
			WalkParameters[Tds] = 0.300 * WalkParameters[Tstep];
			WalkParameters[Tss] = 0.700 * WalkParameters[Tstep];
			WalkParameters[MaxStepX] = 0.0400;
			WalkParameters[MaxStepY] = 0.0400;
			WalkParameters[MaxStepTheta] = 0.6980;
			WalkParameters[g] = 9.81;


		}
		;
		float getWalkParameter(int);
};
#endif

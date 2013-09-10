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
class RobotParameters
{

	private:
		float StepX;
		float StepY;
		float StepZ;
		float H0;
		float ComZ;
		float Tstep;
		float Tds;
		float Tss;
        float Ts;
		float MaxStepX;
		float MaxStepY;
		float MaxStepTheta;
    public:
		RobotParameters()
		{
            		Ts=0.01;
			StepX = 0.155;
			StepY = 0.085;
			StepZ = 0.0353;
			H0 = 0.060;
			ComZ = 0.260;
			Tstep=0.600;//Tstep = 0.35;//0.4;
			Tds = 0.250*Tstep;//0.300 * Tstep;
			Tss = 0.7500 * Tstep;
			MaxStepX = 0.0400;
			MaxStepY = 0.0400;
			MaxStepTheta = 0.6980;
		};
		float getRobotParameter(std::string );
};
#endif

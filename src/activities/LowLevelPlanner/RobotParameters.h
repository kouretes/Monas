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
			WalkParameters[StepZ] = 0.020;
			WalkParameters[H0] = 0.047;
			WalkParameters[HX] = -0.022;//225
			WalkParameters[ComZ] = 0.262;
			WalkParameters[CoMZModelError] = 0.82;
			WalkParameters[Tstep] = 0.4;//0.28
            WalkParameters[Tinit]= 0.35;
			WalkParameters[Tds] = 0.25;// ceil(0.25 * WalkParameters[Tstep]/WalkParameters[Ts])*WalkParameters[Ts];//0.35
			WalkParameters[Tss] = 0.75; //WalkParameters[Tstep]-WalkParameters[Tds];
			WalkParameters[MaxStepX] = 0.0500;
			WalkParameters[MaxStepY] = 0.0500;
			WalkParameters[MaxStepTheta] = 0.6980;
			WalkParameters[g] = 9.81;
			WalkParameters[AdaptiveStepTolx]=0.05;
            WalkParameters[AdaptiveStepToly]=0.1;


		}
		;
		float getWalkParameter(int);
};
#endif

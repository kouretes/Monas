//
//  RobotParameters.cpp
//  Kouretes Walk Engine
//
//  Created by Stelios Piperakis on 8/14/13.
//  Copyright (c) 2013 SP. All rights reserved.
//
#include "RobotParameters.h"
float RobotParameters::getRobotParameter(std::string p)
{
	float returnVal;
	if (p == "StepX")
	{
		returnVal = StepX;
	}
    else if (p == "StepY")
	{
		returnVal = StepY;
	}
	else if (p == "ComZ")
	{
		returnVal = ComZ;
	}
	else if (p == "H0")
	{
		returnVal = H0;
	}
	else if (p == "Tstep")
	{
		returnVal = Tstep;
	}
	else if (p == "Tds")
	{

		returnVal = Tds;
	}
    else if (p=="Tss")
	{
		returnVal = Tss;

	}
    else if (p=="MaxStepX")
    {
	  returnVal=MaxStepX;
    }
    else if (p=="MaxStepY")
    {
        returnVal=MaxStepY;
    }
    else if (p=="MaxStepTheta")
    {
        returnVal=MaxStepTheta;
    }
    else if (p=="Ts")
    {
        returnVal=Ts;
    }
    else
    {
        returnVal=StepZ;
    }
    return returnVal;
}

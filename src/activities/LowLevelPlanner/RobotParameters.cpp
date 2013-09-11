//
//  RobotParameters.cpp
//  Kouretes Walk Engine
//
//  Created by Stelios Piperakis on 8/14/13.
//  Copyright (c) 2013 SP. All rights reserved.
//
#include "RobotParameters.h"

float RobotParameters::getWalkParameter(int p)
{
	if(p>ParametersSize || p < 0)
	{
		std::cerr << "Error Invalid parameter" << std::endl;
		return 0;
	}
    return WalkParameters[p];
}

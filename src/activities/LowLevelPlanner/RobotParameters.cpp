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

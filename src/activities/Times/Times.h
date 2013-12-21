#ifndef TIMES_H
#define TIMES_H

#include <boost/date_time/posix_time/posix_time.hpp>

#include "core/include/IActivity.hpp"
#include "core/elements/math/Common.hpp"
#include "core/architecture/configurator/Configurator.hpp"

#include "hal/robot/generic_nao/NAOKinematics.h"
#include "hal/robot/generic_nao/KinematicsDefines.h"
#include "hal/smart_timer.h"

#include <string>
#include <math.h>
#include <iostream>
#include <fstream>
#include <vector>

#include "core/include/Logger.hpp"

using namespace boost::posix_time;
using namespace std;
using namespace KSystem;

ACTIVITY_START
class Times : public IActivity
{

public:

	Times(Blackboard &b);

	int ACTIVITY_VISIBLE IEX_DIRECTIVE_HOT Execute();
	void ACTIVITY_VISIBLE UserInit();
	void ACTIVITY_VISIBLE Reset();

	std::string ACTIVITY_VISIBLE GetName()
	{
		return "Times";
	}


private:
	smart_timer timer;

};

ACTIVITY_END

#endif

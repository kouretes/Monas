#include "THEALSP.h"

#include "hal/robot/generic_nao/kAlBroker.h"
#include "tools/logger.h"

namespace
{
	SpecialActionRegistrar<THEALSP>::Type temp("THEALSP");
}

THEALSP::THEALSP()
{
	Initialize();
	Logger::Instance().WriteMsg("SP:THEALSP", "Initialized!", Logger::ExtraInfo);

	try
	{
		motion = KAlBroker::Instance().GetBroker()->getMotionProxy();
	}
	catch (AL::ALError& e)
	{
		Logger::Instance().WriteMsg("SP:THEALSP", "Error in getting motion proxy", Logger::FatalError);
	}
}




void THEALSP::Initialize()
{
	//Add exported code here
}

int THEALSP::ExecutePost()
{
	return  motion->post.angleInterpolationBezier(names, times, keys);
}

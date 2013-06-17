#include "ALstandUpCross2011.h"

#include "hal/robot/generic_nao/kAlBroker.h"
#include "core/include/Logger.hpp"

namespace
{
	SpecialActionRegistrar<ALstandUpCross2011>::Type temp("ALstandUpCross2011");
}

ALstandUpCross2011::ALstandUpCross2011()
{
	Initialize();
	LogEntry(LogLevel::ExtraInfo,"SP:ALstandUpCross2011")<< "Initialized!";

	try
	{
		motion = new AL::ALMotionProxy(boost::shared_ptr<AL::ALBroker>(KAlBroker::Instance().GetBroker()));
	}
	catch (AL::ALError& e)
	{
		LogEntry(LogLevel::FatalError,"SP:ALstandUpCross2011")<<"Error in getting motion proxy";
	}
}




void ALstandUpCross2011::Initialize()
{
	names.reserve(19);
	times.arraySetSize(19);
	keys.arraySetSize(19);
	names.push_back("LShoulderPitch");
	times[0].arraySetSize(1);
	keys[0].arraySetSize(1);
	times[0][0] = 0.500000;
	keys[0][0] = AL::ALValue::array(1.59992, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));
	names.push_back("LShoulderRoll");
	times[1].arraySetSize(1);
	keys[1].arraySetSize(1);
	times[1][0] = 0.500000;
	keys[1][0] = AL::ALValue::array(0.374254, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));
	names.push_back("LElbowYaw");
	times[2].arraySetSize(1);
	keys[2].arraySetSize(1);
	times[2][0] = 0.500000;
	keys[2][0] = AL::ALValue::array(-0.139636, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));
	names.push_back("LElbowRoll");
	times[3].arraySetSize(1);
	keys[3].arraySetSize(1);
	times[3][0] = 0.500000;
	keys[3][0] = AL::ALValue::array(-0.0349066, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));
	names.push_back("RShoulderPitch");
	times[4].arraySetSize(1);
	keys[4].arraySetSize(1);
	times[4][0] = 0.500000;
	keys[4][0] = AL::ALValue::array(1.57699, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));
	names.push_back("RShoulderRoll");
	times[5].arraySetSize(1);
	keys[5].arraySetSize(1);
	times[5][0] = 0.500000;
	keys[5][0] = AL::ALValue::array(-0.478650, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));
	names.push_back("RElbowYaw");
	times[6].arraySetSize(1);
	keys[6].arraySetSize(1);
	times[6][0] = 0.500000;
	keys[6][0] = AL::ALValue::array(0.125746, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));
	names.push_back("RElbowRoll");
	times[7].arraySetSize(1);
	keys[7].arraySetSize(1);
	times[7][0] = 0.500000;
	keys[7][0] = AL::ALValue::array(0.0460620, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));
	names.push_back("LHipYawPitch");
	times[8].arraySetSize(1);
	keys[8].arraySetSize(1);
	times[8][0] = 0.500000;
	keys[8][0] = AL::ALValue::array(0.0368580, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));
	names.push_back("LHipRoll");
	times[9].arraySetSize(1);
	keys[9].arraySetSize(1);
	times[9][0] = 0.500000;
	keys[9][0] = AL::ALValue::array(-0.0367741, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));
	names.push_back("LHipPitch");
	times[10].arraySetSize(1);
	keys[10].arraySetSize(1);
	times[10][0] = 0.500000;
	keys[10][0] = AL::ALValue::array(4.19617e-05, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));
	names.push_back("LKneePitch");
	times[11].arraySetSize(1);
	keys[11].arraySetSize(1);
	times[11][0] = 0.500000;
	keys[11][0] = AL::ALValue::array(-0.0923279, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));
	names.push_back("LAnklePitch");
	times[12].arraySetSize(1);
	keys[12].arraySetSize(1);
	times[12][0] = 0.500000;
	keys[12][0] = AL::ALValue::array(0.921892, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));
	names.push_back("LAnkleRoll");
	times[13].arraySetSize(1);
	keys[13].arraySetSize(1);
	times[13][0] = 0.500000;
	keys[13][0] = AL::ALValue::array(-0.0199001, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));
	names.push_back("RHipRoll");
	times[14].arraySetSize(1);
	keys[14].arraySetSize(1);
	times[14][0] = 0.500000;
	keys[14][0] = AL::ALValue::array(0.0153820, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));
	names.push_back("RHipPitch");
	times[15].arraySetSize(1);
	keys[15].arraySetSize(1);
	times[15][0] = 0.500000;
	keys[15][0] = AL::ALValue::array(0.0122300, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));
	names.push_back("RKneePitch");
	times[16].arraySetSize(1);
	keys[16].arraySetSize(1);
	times[16][0] = 0.500000;
	keys[16][0] = AL::ALValue::array(-0.0923279, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));
	names.push_back("RAnklePitch");
	times[17].arraySetSize(1);
	keys[17].arraySetSize(1);
	times[17][0] = 0.500000;
	keys[17][0] = AL::ALValue::array(0.931180, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));
	names.push_back("RAnkleRoll");
	times[18].arraySetSize(1);
	keys[18].arraySetSize(1);
	times[18][0] = 0.500000;
	keys[18][0] = AL::ALValue::array(0.00771196, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));
}

int ALstandUpCross2011::ExecutePost()
{
	return  motion->post.angleInterpolationBezier(names, times, keys);
}

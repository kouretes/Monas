#include "ALstandUpCross.h"

#include "hal/robot/generic_nao/kAlBroker.h"

namespace
{
	SpecialActionRegistrar<ALstandUpCross>::Type temp("ALstandUpCross");
}

ALstandUpCross::ALstandUpCross()
{
	Initialize();
	Logger::Instance().WriteMsg("SP:ALstandUpCross", "Initialized!", Logger::ExtraInfo);

	try
	{
		motion = KAlBroker::Instance().GetBroker()->getMotionProxy();
	}
	catch (AL::ALError& e)
	{
		Logger::Instance().WriteMsg("SP:ALstandUpCross", "Error in getting motion proxy", Logger::FatalError);
	}
}




void ALstandUpCross::Initialize()
{
	jointCodes.arraySetSize(22);
	angles.arraySetSize(22);
	times.arraySetSize(22);
	jointCodes[0] = "HeadPitch";
	angles[0].arraySetSize(2);
	times[0].arraySetSize(2);
	angles[0][0] = 0.00000f;
	times[0][0] = 1.90000f;
	angles[0][1] = 0.00000f;
	times[0][1] = 2.90000f;
	jointCodes[1] = "HeadYaw";
	angles[1].arraySetSize(2);
	times[1].arraySetSize(2);
	angles[1][0] = 0.00000f;
	times[1][0] = 1.90000f;
	angles[1][1] = 0.00000f;
	times[1][1] = 2.90000f;
	jointCodes[2] = "LAnklePitch";
	angles[2].arraySetSize(2);
	times[2].arraySetSize(2);
	angles[2][0] = 0.52360f;
	times[2][0] = 1.90000f;
	angles[2][1] = 0.52360f;
	times[2][1] = 2.90000f;
	jointCodes[3] = "LAnkleRoll";
	angles[3].arraySetSize(2);
	times[3].arraySetSize(2);
	angles[3][0] = 0.00000f;
	times[3][0] = 1.90000f;
	angles[3][1] = 0.00000f;
	times[3][1] = 2.90000f;
	jointCodes[4] = "LElbowRoll";
	angles[4].arraySetSize(2);
	times[4].arraySetSize(2);
	angles[4][0] = 0.00000f;
	times[4][0] = 1.90000f;
	angles[4][1] = 0.00000f;
	times[4][1] = 2.90000f;
	jointCodes[5] = "LElbowYaw";
	angles[5].arraySetSize(2);
	times[5].arraySetSize(2);
	angles[5][0] = 0.00000f;
	times[5][0] = 1.90000f;
	angles[5][1] = 0.00000f;
	times[5][1] = 2.90000f;
	jointCodes[7] = "LHipPitch";
	angles[7].arraySetSize(2);
	times[7].arraySetSize(2);
	angles[7][0] = 0.00000f;
	times[7][0] = 1.90000f;
	angles[7][1] = 0.00000f;
	times[7][1] = 2.90000f;
	jointCodes[8] = "LHipRoll";
	angles[8].arraySetSize(2);
	times[8].arraySetSize(2);
	angles[8][0] = 0.00000f;
	times[8][0] = 1.90000f;
	angles[8][1] = 0.00000f;
	times[8][1] = 2.90000f;
	jointCodes[9] = "LHipYawPitch";
	angles[9].arraySetSize(2);
	times[9].arraySetSize(2);
	angles[9][0] = 0.00000f;
	times[9][0] = 1.90000f;
	angles[9][1] = 0.00000f;
	times[9][1] = 2.90000f;
	jointCodes[10] = "LKneePitch";
	angles[10].arraySetSize(2);
	times[10].arraySetSize(2);
	angles[10][0] = 0.00000f;
	times[10][0] = 1.90000f;
	angles[10][1] = 0.00000f;
	times[10][1] = 2.90000f;
	jointCodes[11] = "LShoulderPitch";
	angles[11].arraySetSize(2);
	times[11].arraySetSize(2);
	angles[11][0] = 0.00000f;
	times[11][0] = 1.90000f;
	angles[11][1] = 0.00000f;
	times[11][1] = 2.90000f;
	jointCodes[12] = "LShoulderRoll";
	angles[12].arraySetSize(2);
	times[12].arraySetSize(2);
	angles[12][0] = 1.57080f;
	times[12][0] = 1.90000f;
	angles[12][1] = 1.57080f;
	times[12][1] = 2.90000f;
	jointCodes[14] = "RAnklePitch";
	angles[14].arraySetSize(2);
	times[14].arraySetSize(2);
	angles[14][0] = 0.52360f;
	times[14][0] = 1.90000f;
	angles[14][1] = 0.52360f;
	times[14][1] = 2.90000f;
	jointCodes[15] = "RAnkleRoll";
	angles[15].arraySetSize(2);
	times[15].arraySetSize(2);
	angles[15][0] = 0.00000f;
	times[15][0] = 1.90000f;
	angles[15][1] = 0.00000f;
	times[15][1] = 2.90000f;
	jointCodes[16] = "RElbowRoll";
	angles[16].arraySetSize(2);
	times[16].arraySetSize(2);
	angles[16][0] = 0.00000f;
	times[16][0] = 1.90000f;
	angles[16][1] = 0.00000f;
	times[16][1] = 2.90000f;
	jointCodes[17] = "RElbowYaw";
	angles[17].arraySetSize(2);
	times[17].arraySetSize(2);
	angles[17][0] = 0.00000f;
	times[17][0] = 1.90000f;
	angles[17][1] = 0.00000f;
	times[17][1] = 2.90000f;
	jointCodes[19] = "RHipPitch";
	angles[19].arraySetSize(2);
	times[19].arraySetSize(2);
	angles[19][0] = 0.00000f;
	times[19][0] = 1.90000f;
	angles[19][1] = 0.00000f;
	times[19][1] = 2.90000f;
	jointCodes[20] = "RHipRoll";
	angles[20].arraySetSize(2);
	times[20].arraySetSize(2);
	angles[20][0] = 0.00000f;
	times[20][0] = 1.90000f;
	angles[20][1] = 0.00000f;
	times[20][1] = 2.90000f;
	jointCodes[6] = "RKneePitch";
	angles[6].arraySetSize(2);
	times[6].arraySetSize(2);
	angles[6][0] = 0.00000f;
	times[6][0] = 1.90000f;
	angles[6][1] = 0.00000f;
	times[6][1] = 2.90000f;
	jointCodes[13] = "RShoulderPitch";
	angles[13].arraySetSize(2);
	times[13].arraySetSize(2);
	angles[13][0] = 0.00000f;
	times[13][0] = 1.90000f;
	angles[13][1] = 0.00000f;
	times[13][1] = 2.90000f;
	jointCodes[18] = "RShoulderRoll";
	angles[18].arraySetSize(2);
	times[18].arraySetSize(2);
	angles[18][0] = -1.57080f;
	times[18][0] = 1.90000f;
	angles[18][1] = -1.57080f;
	times[18][1] = 2.90000f;
	jointCodes[21] = "RHipYawPitch";
	angles[21].arraySetSize(2);
	times[21].arraySetSize(2);
	angles[21][0] = 0.00000f;
	times[21][0] = 1.90000f;
	angles[21][1] = 0.00000f;
	times[21][1] = 2.90000f;
}

int ALstandUpCross::ExecutePost()
{
	motion->post.angleInterpolation(jointCodes, angles, times, 1);
	return 0;
}

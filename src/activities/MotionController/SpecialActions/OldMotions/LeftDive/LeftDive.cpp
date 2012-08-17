#include "LeftDive.h"

#include "hal/robot/generic_nao/kAlBroker.h"

namespace
{
	SpecialActionRegistrar<LeftDive>::Type temp("LeftDive");
}

LeftDive::LeftDive()
{
	Initialize();
	Logger::Instance().WriteMsg("SP:LeftDive", "Initialized!", Logger::ExtraInfo);

	try
	{
		motion = KAlBroker::Instance().GetBroker()->getMotionProxy();
	}
	catch (AL::ALError& e)
	{
		Logger::Instance().WriteMsg("SP:LeftDive", "Error in getting motion proxy", Logger::FatalError);
	}
}




void LeftDive::Initialize()
{
	/******************************** Left Dive ****************************/
	LeftDive_names.reserve(25);
	LeftDive_times.arraySetSize(25);
	LeftDive_keys.arraySetSize(25);
	LeftDive_names.push_back("HeadYaw");
	LeftDive_times[0].arraySetSize(3);
	LeftDive_keys[0].arraySetSize(3);
	LeftDive_times[0][0] = 0.100000;
	LeftDive_keys[0][0] = AL::ALValue::array(-0.286815, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[0][1] = 0.500000;
	LeftDive_keys[0][1] = AL::ALValue::array(-0.0843280, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[0][2] = 0.900000;
	LeftDive_keys[0][2] = AL::ALValue::array(-0.0873960, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	LeftDive_names.push_back("HeadPitch");
	LeftDive_times[1].arraySetSize(3);
	LeftDive_keys[1].arraySetSize(3);
	LeftDive_times[1][0] = 0.100000;
	LeftDive_keys[1][0] = AL::ALValue::array(0.165630, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[1][1] = 0.500000;
	LeftDive_keys[1][1] = AL::ALValue::array(-0.164180, AL::ALValue::array(2, -0.133333, 0.00153396), AL::ALValue::array(2, 0.133333, -0.00153396));
	LeftDive_times[1][2] = 0.900000;
	LeftDive_keys[1][2] = AL::ALValue::array(-0.165714, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	LeftDive_names.push_back("LShoulderPitch");
	LeftDive_times[2].arraySetSize(3);
	LeftDive_keys[2].arraySetSize(3);
	LeftDive_times[2][0] = 0.100000;
	LeftDive_keys[2][0] = AL::ALValue::array(1.74573, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[2][1] = 0.500000;
	LeftDive_keys[2][1] = AL::ALValue::array(1.19810, AL::ALValue::array(2, -0.133333, 0.547636), AL::ALValue::array(2, 0.133333, -0.547636));
	LeftDive_times[2][2] = 0.900000;
	LeftDive_keys[2][2] = AL::ALValue::array(-1.65975, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	LeftDive_names.push_back("LShoulderRoll");
	LeftDive_times[3].arraySetSize(3);
	LeftDive_keys[3].arraySetSize(3);
	LeftDive_times[3][0] = 0.100000;
	LeftDive_keys[3][0] = AL::ALValue::array(0.243948, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[3][1] = 0.500000;
	LeftDive_keys[3][1] = AL::ALValue::array(0.00870000, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[3][2] = 0.900000;
	LeftDive_keys[3][2] = AL::ALValue::array(0.0169160, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	LeftDive_names.push_back("LElbowYaw");
	LeftDive_times[4].arraySetSize(3);
	LeftDive_keys[4].arraySetSize(3);
	LeftDive_times[4][0] = 0.100000;
	LeftDive_keys[4][0] = AL::ALValue::array(-1.48180, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[4][1] = 0.500000;
	LeftDive_keys[4][1] = AL::ALValue::array(-0.806841, AL::ALValue::array(2, -0.133333, -0.184336), AL::ALValue::array(2, 0.133333, 0.184336));
	LeftDive_times[4][2] = 0.900000;
	LeftDive_keys[4][2] = AL::ALValue::array(-0.375789, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	LeftDive_names.push_back("LElbowRoll");
	LeftDive_times[5].arraySetSize(3);
	LeftDive_keys[5].arraySetSize(3);
	LeftDive_times[5][0] = 0.100000;
	LeftDive_keys[5][0] = AL::ALValue::array(-0.524670, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[5][1] = 0.500000;
	LeftDive_keys[5][1] = AL::ALValue::array(-0.955723, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[5][2] = 0.900000;
	LeftDive_keys[5][2] = AL::ALValue::array(-0.0890139, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	LeftDive_names.push_back("LWristYaw");
	LeftDive_times[6].arraySetSize(3);
	LeftDive_keys[6].arraySetSize(3);
	LeftDive_times[6][0] = 0.100000;
	LeftDive_keys[6][0] = AL::ALValue::array(-0.00000, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[6][1] = 0.500000;
	LeftDive_keys[6][1] = AL::ALValue::array(-0.00000, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[6][2] = 0.900000;
	LeftDive_keys[6][2] = AL::ALValue::array(-0.00000, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	LeftDive_names.push_back("LHand");
	LeftDive_times[7].arraySetSize(3);
	LeftDive_keys[7].arraySetSize(3);
	LeftDive_times[7][0] = 0.100000;
	LeftDive_keys[7][0] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[7][1] = 0.500000;
	LeftDive_keys[7][1] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[7][2] = 0.900000;
	LeftDive_keys[7][2] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	LeftDive_names.push_back("RShoulderPitch");
	LeftDive_times[8].arraySetSize(3);
	LeftDive_keys[8].arraySetSize(3);
	LeftDive_times[8][0] = 0.100000;
	LeftDive_keys[8][0] = AL::ALValue::array(1.73798, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[8][1] = 0.500000;
	LeftDive_keys[8][1] = AL::ALValue::array(1.30846, AL::ALValue::array(2, -0.133333, 0.429520), AL::ALValue::array(2, 0.133333, -0.429520));
	LeftDive_times[8][2] = 0.900000;
	LeftDive_keys[8][2] = AL::ALValue::array(-1.68284, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	LeftDive_names.push_back("RShoulderRoll");
	LeftDive_times[9].arraySetSize(3);
	LeftDive_keys[9].arraySetSize(3);
	LeftDive_times[9][0] = 0.100000;
	LeftDive_keys[9][0] = AL::ALValue::array(-0.265341, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[9][1] = 0.500000;
	LeftDive_keys[9][1] = AL::ALValue::array(-0.00870000, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[9][2] = 0.900000;
	LeftDive_keys[9][2] = AL::ALValue::array(-0.00870000, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	LeftDive_names.push_back("RElbowYaw");
	LeftDive_times[10].arraySetSize(3);
	LeftDive_keys[10].arraySetSize(3);
	LeftDive_times[10][0] = 0.100000;
	LeftDive_keys[10][0] = AL::ALValue::array(1.47882, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[10][1] = 0.500000;
	LeftDive_keys[10][1] = AL::ALValue::array(0.559952, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[10][2] = 0.900000;
	LeftDive_keys[10][2] = AL::ALValue::array(0.806927, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	LeftDive_names.push_back("RElbowRoll");
	LeftDive_times[11].arraySetSize(3);
	LeftDive_keys[11].arraySetSize(3);
	LeftDive_times[11][0] = 0.100000;
	LeftDive_keys[11][0] = AL::ALValue::array(0.515382, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[11][1] = 0.500000;
	LeftDive_keys[11][1] = AL::ALValue::array(0.977116, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[11][2] = 0.900000;
	LeftDive_keys[11][2] = AL::ALValue::array(0.00870000, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	LeftDive_names.push_back("RWristYaw");
	LeftDive_times[12].arraySetSize(3);
	LeftDive_keys[12].arraySetSize(3);
	LeftDive_times[12][0] = 0.100000;
	LeftDive_keys[12][0] = AL::ALValue::array(-0.00000, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[12][1] = 0.500000;
	LeftDive_keys[12][1] = AL::ALValue::array(-0.00000, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[12][2] = 0.900000;
	LeftDive_keys[12][2] = AL::ALValue::array(-0.00000, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	LeftDive_names.push_back("RHand");
	LeftDive_times[13].arraySetSize(3);
	LeftDive_keys[13].arraySetSize(3);
	LeftDive_times[13][0] = 0.100000;
	LeftDive_keys[13][0] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[13][1] = 0.500000;
	LeftDive_keys[13][1] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[13][2] = 0.900000;
	LeftDive_keys[13][2] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	LeftDive_names.push_back("LHipYawPitch");
	LeftDive_times[14].arraySetSize(4);
	LeftDive_keys[14].arraySetSize(4);
	LeftDive_times[14][0] = 0.100000;
	LeftDive_keys[14][0] = AL::ALValue::array(0.00464395, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[14][1] = 0.500000;
	LeftDive_keys[14][1] = AL::ALValue::array(-0.0659200, AL::ALValue::array(2, -0.133333, 0.0232657), AL::ALValue::array(2, 0.133333, -0.0232657));
	LeftDive_times[14][2] = 0.900000;
	LeftDive_keys[14][2] = AL::ALValue::array(-0.134950, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.166667, 0.00000));
	LeftDive_times[14][3] = 1.40000;
	LeftDive_keys[14][3] = AL::ALValue::array(-0.0337060, AL::ALValue::array(2, -0.166667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	LeftDive_names.push_back("LHipRoll");
	LeftDive_times[15].arraySetSize(4);
	LeftDive_keys[15].arraySetSize(4);
	LeftDive_times[15][0] = 0.100000;
	LeftDive_keys[15][0] = AL::ALValue::array(-0.0583340, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[15][1] = 0.500000;
	LeftDive_keys[15][1] = AL::ALValue::array(-0.0383920, AL::ALValue::array(2, -0.133333, -0.0199420), AL::ALValue::array(2, 0.133333, 0.0199420));
	LeftDive_times[15][2] = 0.900000;
	LeftDive_keys[15][2] = AL::ALValue::array(0.269941, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.166667, 0.00000));
	LeftDive_times[15][3] = 1.40000;
	LeftDive_keys[15][3] = AL::ALValue::array(-0.322183, AL::ALValue::array(2, -0.166667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	LeftDive_names.push_back("LHipPitch");
	LeftDive_times[16].arraySetSize(4);
	LeftDive_keys[16].arraySetSize(4);
	LeftDive_times[16][0] = 0.100000;
	LeftDive_keys[16][0] = AL::ALValue::array(-0.460242, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[16][1] = 0.500000;
	LeftDive_keys[16][1] = AL::ALValue::array(-0.885159, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[16][2] = 0.900000;
	LeftDive_keys[16][2] = AL::ALValue::array(-0.803859, AL::ALValue::array(2, -0.133333, -0.0813009), AL::ALValue::array(2, 0.166667, 0.101626));
	LeftDive_times[16][3] = 1.40000;
	LeftDive_keys[16][3] = AL::ALValue::array(-0.280764, AL::ALValue::array(2, -0.166667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	LeftDive_names.push_back("LKneePitch");
	LeftDive_times[17].arraySetSize(4);
	LeftDive_keys[17].arraySetSize(4);
	LeftDive_times[17][0] = 0.100000;
	LeftDive_keys[17][0] = AL::ALValue::array(0.803859, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[17][1] = 0.500000;
	LeftDive_keys[17][1] = AL::ALValue::array(2.11253, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[17][2] = 0.900000;
	LeftDive_keys[17][2] = AL::ALValue::array(2.11253, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.166667, 0.00000));
	LeftDive_times[17][3] = 1.40000;
	LeftDive_keys[17][3] = AL::ALValue::array(0.642787, AL::ALValue::array(2, -0.166667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	LeftDive_names.push_back("LAnklePitch");
	LeftDive_times[18].arraySetSize(4);
	LeftDive_keys[18].arraySetSize(4);
	LeftDive_times[18][0] = 0.100000;
	LeftDive_keys[18][0] = AL::ALValue::array(-0.409536, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[18][1] = 0.500000;
	LeftDive_keys[18][1] = AL::ALValue::array(-1.18645, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[18][2] = 0.900000;
	LeftDive_keys[18][2] = AL::ALValue::array(-1.18645, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.166667, 0.00000));
	LeftDive_times[18][3] = 1.40000;
	LeftDive_keys[18][3] = AL::ALValue::array(0.177985, AL::ALValue::array(2, -0.166667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	LeftDive_names.push_back("LAnkleRoll");
	LeftDive_times[19].arraySetSize(4);
	LeftDive_keys[19].arraySetSize(4);
	LeftDive_times[19][0] = 0.100000;
	LeftDive_keys[19][0] = AL::ALValue::array(0.0521140, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[19][1] = 0.500000;
	LeftDive_keys[19][1] = AL::ALValue::array(0.0275701, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[19][2] = 0.900000;
	LeftDive_keys[19][2] = AL::ALValue::array(0.108872, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.166667, 0.00000));
	LeftDive_times[19][3] = 1.40000;
	LeftDive_keys[19][3] = AL::ALValue::array(0.0858620, AL::ALValue::array(2, -0.166667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	LeftDive_names.push_back("RHipRoll");
	LeftDive_times[20].arraySetSize(4);
	LeftDive_keys[20].arraySetSize(4);
	LeftDive_times[20][0] = 0.100000;
	LeftDive_keys[20][0] = AL::ALValue::array(0.0152980, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[20][1] = 0.500000;
	LeftDive_keys[20][1] = AL::ALValue::array(0.138018, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[20][2] = 0.900000;
	LeftDive_keys[20][2] = AL::ALValue::array(-0.112024, AL::ALValue::array(2, -0.133333, 0.0474971), AL::ALValue::array(2, 0.166667, -0.0593714));
	LeftDive_times[20][3] = 1.40000;
	LeftDive_keys[20][3] = AL::ALValue::array(-0.182588, AL::ALValue::array(2, -0.166667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	LeftDive_names.push_back("RHipPitch");
	LeftDive_times[21].arraySetSize(4);
	LeftDive_keys[21].arraySetSize(4);
	LeftDive_times[21][0] = 0.100000;
	LeftDive_keys[21][0] = AL::ALValue::array(-0.460158, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[21][1] = 0.500000;
	LeftDive_keys[21][1] = AL::ALValue::array(-0.866668, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[21][2] = 0.900000;
	LeftDive_keys[21][2] = AL::ALValue::array(0.0598679, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.166667, 0.00000));
	LeftDive_times[21][3] = 1.40000;
	LeftDive_keys[21][3] = AL::ALValue::array(0.0568000, AL::ALValue::array(2, -0.166667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	LeftDive_names.push_back("RKneePitch");
	LeftDive_times[22].arraySetSize(4);
	LeftDive_keys[22].arraySetSize(4);
	LeftDive_times[22][0] = 0.100000;
	LeftDive_keys[22][0] = AL::ALValue::array(0.811444, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[22][1] = 0.500000;
	LeftDive_keys[22][1] = AL::ALValue::array(2.11253, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[22][2] = 0.900000;
	LeftDive_keys[22][2] = AL::ALValue::array(0.315962, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.166667, 0.00000));
	LeftDive_times[22][3] = 1.40000;
	LeftDive_keys[22][3] = AL::ALValue::array(0.331302, AL::ALValue::array(2, -0.166667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	LeftDive_names.push_back("RAnklePitch");
	LeftDive_times[23].arraySetSize(4);
	LeftDive_keys[23].arraySetSize(4);
	LeftDive_times[23][0] = 0.100000;
	LeftDive_keys[23][0] = AL::ALValue::array(-0.403483, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[23][1] = 0.500000;
	LeftDive_keys[23][1] = AL::ALValue::array(-1.18645, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[23][2] = 0.900000;
	LeftDive_keys[23][2] = AL::ALValue::array(-0.242414, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.166667, 0.00000));
	LeftDive_times[23][3] = 1.40000;
	LeftDive_keys[23][3] = AL::ALValue::array(-0.280764, AL::ALValue::array(2, -0.166667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	LeftDive_names.push_back("RAnkleRoll");
	LeftDive_times[24].arraySetSize(4);
	LeftDive_keys[24].arraySetSize(4);
	LeftDive_times[24][0] = 0.100000;
	LeftDive_keys[24][0] = AL::ALValue::array(-0.0184499, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[24][1] = 0.500000;
	LeftDive_keys[24][1] = AL::ALValue::array(-0.101286, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[24][2] = 0.900000;
	LeftDive_keys[24][2] = AL::ALValue::array(0.665714, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.166667, 0.00000));
	LeftDive_times[24][3] = 1.40000;
	LeftDive_keys[24][3] = AL::ALValue::array(-0.184122, AL::ALValue::array(2, -0.166667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
}

int LeftDive::ExecutePost()
{
	return motion->post.angleInterpolationBezier(LeftDive_names, LeftDive_times, LeftDive_keys);
}

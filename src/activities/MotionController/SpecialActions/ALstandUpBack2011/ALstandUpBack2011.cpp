#include "ALstandUpBack2011.h"

#include "hal/robot/generic_nao/kAlBroker.h"

namespace {
SpecialActionRegistrar<ALstandUpBack2011>::Type temp("ALstandUpBack2011");
}

ALstandUpBack2011::ALstandUpBack2011() {
  Initialize();
  Logger::Instance().WriteMsg("SP:ALstandUpBack2011","Initialized!",Logger::ExtraInfo);
  
  try {
    motion = KAlBroker::Instance().GetBroker()->getMotionProxy();
  } catch (AL::ALError& e) {
    Logger::Instance().WriteMsg("SP:ALstandUpBack2011","Error in getting motion proxy",Logger::FatalError);
  }
  
}




void ALstandUpBack2011::Initialize() {
  
	names.reserve(25);
	times.arraySetSize(25);
	keys.arraySetSize(25);

	names.push_back("HeadYaw");
	times[0].arraySetSize(11);
	keys[0].arraySetSize(11);

	times[0][0] = 0.300000;
	keys[0][0] = AL::ALValue::array(-0.0245859, AL::ALValue::array(3, -0.100000, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[0][1] = 0.700000;
	keys[0][1] = AL::ALValue::array(-4.76838e-07, AL::ALValue::array(3, -0.133333, -0.0109937), AL::ALValue::array(3, 0.133333, 0.0109937));
	times[0][2] = 1.10000;
	keys[0][2] = AL::ALValue::array(0.0413760, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.166667, 0.00000));
	times[0][3] = 1.60000;
	keys[0][3] = AL::ALValue::array(6.74350e-07, AL::ALValue::array(3, -0.166667, 4.21466e-07), AL::ALValue::array(3, 0.133333, -3.37173e-07));
	times[0][4] = 2.00000;
	keys[0][4] = AL::ALValue::array(3.37177e-07, AL::ALValue::array(3, -0.133333, 1.74610e-12), AL::ALValue::array(3, 0.133333, -1.74610e-12));
	times[0][5] = 2.40000;
	keys[0][5] = AL::ALValue::array(3.37175e-07, AL::ALValue::array(3, -0.133333, 1.74610e-12), AL::ALValue::array(3, 0.233333, -3.05568e-12));
	times[0][6] = 3.10000;
	keys[0][6] = AL::ALValue::array(-0.302240, AL::ALValue::array(3, -0.233333, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[0][7] = 3.50000;
	keys[0][7] = AL::ALValue::array(0.263545, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.166667, 0.00000));
	times[0][8] = 4.00000;
	keys[0][8] = AL::ALValue::array(0.120428, AL::ALValue::array(3, -0.166667, 0.0649872), AL::ALValue::array(3, 0.0666667, -0.0259949));
	times[0][9] = 4.20000;
	keys[0][9] = AL::ALValue::array(-0.00940132, AL::ALValue::array(3, -0.0666667, -0.00000), AL::ALValue::array(3, 0.300000, 0.00000));
	times[0][10] = 5.10000;
	keys[0][10] = AL::ALValue::array(0.0659200, AL::ALValue::array(3, -0.300000, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));

	names.push_back("HeadPitch");
	times[1].arraySetSize(11);
	keys[1].arraySetSize(11);

	times[1][0] = 0.300000;
	keys[1][0] = AL::ALValue::array(0.0628521, AL::ALValue::array(3, -0.100000, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[1][1] = 0.700000;
	keys[1][1] = AL::ALValue::array(0.00000, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[1][2] = 1.10000;
	keys[1][2] = AL::ALValue::array(0.514872, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.166667, 0.00000));
	times[1][3] = 1.60000;
	keys[1][3] = AL::ALValue::array(-0.671952, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[1][4] = 2.00000;
	keys[1][4] = AL::ALValue::array(0.00000, AL::ALValue::array(3, -0.133333, -0.170170), AL::ALValue::array(3, 0.133333, 0.170170));
	times[1][5] = 2.40000;
	keys[1][5] = AL::ALValue::array(0.349066, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.233333, 0.00000));
	times[1][6] = 3.10000;
	keys[1][6] = AL::ALValue::array(0.345107, AL::ALValue::array(3, -0.233333, 0.00395842), AL::ALValue::array(3, 0.133333, -0.00226196));
	times[1][7] = 3.50000;
	keys[1][7] = AL::ALValue::array(0.0104720, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.166667, 0.00000));
	times[1][8] = 4.00000;
	keys[1][8] = AL::ALValue::array(0.0296706, AL::ALValue::array(3, -0.166667, -0.0191986), AL::ALValue::array(3, 0.0666667, 0.00767945));
	times[1][9] = 4.20000;
	keys[1][9] = AL::ALValue::array(0.378859, AL::ALValue::array(3, -0.0666667, -0.00000), AL::ALValue::array(3, 0.300000, 0.00000));
	times[1][10] = 5.10000;
	keys[1][10] = AL::ALValue::array(-0.0399260, AL::ALValue::array(3, -0.300000, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));

	names.push_back("LShoulderPitch");
	times[2].arraySetSize(11);
	keys[2].arraySetSize(11);

	times[2][0] = 0.300000;
	keys[2][0] = AL::ALValue::array(2.08560, AL::ALValue::array(3, -0.100000, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[2][1] = 0.700000;
	keys[2][1] = AL::ALValue::array(2.08567, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[2][2] = 1.10000;
	keys[2][2] = AL::ALValue::array(2.03865, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.166667, 0.00000));
	times[2][3] = 1.60000;
	keys[2][3] = AL::ALValue::array(2.08560, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[2][4] = 2.00000;
	keys[2][4] = AL::ALValue::array(2.08560, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[2][5] = 2.40000;
	keys[2][5] = AL::ALValue::array(2.08560, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.233333, 0.00000));
	times[2][6] = 3.10000;
	keys[2][6] = AL::ALValue::array(1.28698, AL::ALValue::array(3, -0.233333, 0.239363), AL::ALValue::array(3, 0.133333, -0.136779));
	times[2][7] = 3.50000;
	keys[2][7] = AL::ALValue::array(0.957173, AL::ALValue::array(3, -0.133333, 0.0820408), AL::ALValue::array(3, 0.166667, -0.102551));
	times[2][8] = 4.00000;
	keys[2][8] = AL::ALValue::array(0.733209, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.0666667, 0.00000));
	times[2][9] = 4.20000;
	keys[2][9] = AL::ALValue::array(0.733209, AL::ALValue::array(3, -0.0666667, -0.00000), AL::ALValue::array(3, 0.300000, 0.00000));
	times[2][10] = 5.10000;
	keys[2][10] = AL::ALValue::array(1.59225, AL::ALValue::array(3, -0.300000, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));

	names.push_back("LShoulderRoll");
	times[3].arraySetSize(11);
	keys[3].arraySetSize(11);

	times[3][0] = 0.300000;
	keys[3][0] = AL::ALValue::array(0.980184, AL::ALValue::array(3, -0.100000, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[3][1] = 0.700000;
	keys[3][1] = AL::ALValue::array(0.366519, AL::ALValue::array(3, -0.133333, 0.0174533), AL::ALValue::array(3, 0.133333, -0.0174533));
	times[3][2] = 1.10000;
	keys[3][2] = AL::ALValue::array(0.349066, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.166667, 0.00000));
	times[3][3] = 1.60000;
	keys[3][3] = AL::ALValue::array(0.349066, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[3][4] = 2.00000;
	keys[3][4] = AL::ALValue::array(0.366519, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[3][5] = 2.40000;
	keys[3][5] = AL::ALValue::array(0.00869999, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.233333, 0.00000));
	times[3][6] = 3.10000;
	keys[3][6] = AL::ALValue::array(0.0705221, AL::ALValue::array(3, -0.233333, -0.0547644), AL::ALValue::array(3, 0.133333, 0.0312939));
	times[3][7] = 3.50000;
	keys[3][7] = AL::ALValue::array(0.266875, AL::ALValue::array(3, -0.133333, -0.0634053), AL::ALValue::array(3, 0.166667, 0.0792566));
	times[3][8] = 4.00000;
	keys[3][8] = AL::ALValue::array(0.498508, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.0666667, 0.00000));
	times[3][9] = 4.20000;
	keys[3][9] = AL::ALValue::array(0.498508, AL::ALValue::array(3, -0.0666667, -0.00000), AL::ALValue::array(3, 0.300000, 0.00000));
	times[3][10] = 5.10000;
	keys[3][10] = AL::ALValue::array(0.219320, AL::ALValue::array(3, -0.300000, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));

	names.push_back("LElbowYaw");
	times[4].arraySetSize(11);
	keys[4].arraySetSize(11);

	times[4][0] = 0.300000;
	keys[4][0] = AL::ALValue::array(-0.760906, AL::ALValue::array(3, -0.100000, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[4][1] = 0.700000;
	keys[4][1] = AL::ALValue::array(0.157080, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[4][2] = 1.10000;
	keys[4][2] = AL::ALValue::array(0.122678, AL::ALValue::array(3, -0.133333, 0.0103427), AL::ALValue::array(3, 0.166667, -0.0129284));
	times[4][3] = 1.60000;
	keys[4][3] = AL::ALValue::array(0.0872665, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[4][4] = 2.00000;
	keys[4][4] = AL::ALValue::array(0.0872665, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[4][5] = 2.40000;
	keys[4][5] = AL::ALValue::array(0.0872665, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.233333, 0.00000));
	times[4][6] = 3.10000;
	keys[4][6] = AL::ALValue::array(-0.854480, AL::ALValue::array(3, -0.233333, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[4][7] = 3.50000;
	keys[4][7] = AL::ALValue::array(-0.250085, AL::ALValue::array(3, -0.133333, -0.138897), AL::ALValue::array(3, 0.166667, 0.173622));
	times[4][8] = 4.00000;
	keys[4][8] = AL::ALValue::array(0.0830765, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.0666667, 0.00000));
	times[4][9] = 4.20000;
	keys[4][9] = AL::ALValue::array(0.0830765, AL::ALValue::array(3, -0.0666667, -0.00000), AL::ALValue::array(3, 0.300000, 0.00000));
	times[4][10] = 5.10000;
	keys[4][10] = AL::ALValue::array(-1.03089, AL::ALValue::array(3, -0.300000, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));

	names.push_back("LElbowRoll");
	times[5].arraySetSize(12);
	keys[5].arraySetSize(12);

	times[5][0] = 0.300000;
	keys[5][0] = AL::ALValue::array(-0.624296, AL::ALValue::array(3, -0.100000, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[5][1] = 0.700000;
	keys[5][1] = AL::ALValue::array(-0.0349066, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[5][2] = 1.10000;
	keys[5][2] = AL::ALValue::array(-1.54462, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.166667, 0.00000));
	times[5][3] = 1.60000;
	keys[5][3] = AL::ALValue::array(-1.54462, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[5][4] = 2.00000;
	keys[5][4] = AL::ALValue::array(-0.698132, AL::ALValue::array(3, -0.133333, -0.251618), AL::ALValue::array(3, 0.133333, 0.251618));
	times[5][5] = 2.40000;
	keys[5][5] = AL::ALValue::array(-0.0349066, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[5][6] = 2.80000;
	keys[5][6] = AL::ALValue::array(-0.191986, AL::ALValue::array(3, -0.133333, 0.157080), AL::ALValue::array(3, 0.100000, -0.117810));
	times[5][7] = 3.10000;
	keys[5][7] = AL::ALValue::array(-0.872804, AL::ALValue::array(3, -0.100000, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[5][8] = 3.50000;
	keys[5][8] = AL::ALValue::array(-0.829852, AL::ALValue::array(3, -0.133333, -0.00999259), AL::ALValue::array(3, 0.166667, 0.0124907));
	times[5][9] = 4.00000;
	keys[5][9] = AL::ALValue::array(-0.805354, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.0666667, 0.00000));
	times[5][10] = 4.20000;
	keys[5][10] = AL::ALValue::array(-0.805354, AL::ALValue::array(3, -0.0666667, -0.00000), AL::ALValue::array(3, 0.300000, 0.00000));
	times[5][11] = 5.10000;
	keys[5][11] = AL::ALValue::array(-0.681054, AL::ALValue::array(3, -0.300000, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));

	names.push_back("LWristYaw");
	times[6].arraySetSize(5);
	keys[6].arraySetSize(5);

	times[6][0] = 0.300000;
	keys[6][0] = AL::ALValue::array(-0.705682, AL::ALValue::array(3, -0.100000, -0.00000), AL::ALValue::array(3, 0.266667, 0.00000));
	times[6][1] = 1.10000;
	keys[6][1] = AL::ALValue::array(-1.78868, AL::ALValue::array(3, -0.266667, -0.00000), AL::ALValue::array(3, 0.666667, 0.00000));
	times[6][2] = 3.10000;
	keys[6][2] = AL::ALValue::array(-0.719487, AL::ALValue::array(3, -0.666667, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[6][3] = 3.50000;
	keys[6][3] = AL::ALValue::array(-0.722556, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.533333, 0.00000));
	times[6][4] = 5.10000;
	keys[6][4] = AL::ALValue::array(-0.711819, AL::ALValue::array(3, -0.533333, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));

	names.push_back("LHand");
	times[7].arraySetSize(5);
	keys[7].arraySetSize(5);

	times[7][0] = 0.300000;
	keys[7][0] = AL::ALValue::array(0.000876298, AL::ALValue::array(3, -0.100000, -0.00000), AL::ALValue::array(3, 0.266667, 0.00000));
	times[7][1] = 1.10000;
	keys[7][1] = AL::ALValue::array(0.00133325, AL::ALValue::array(3, -0.266667, -0.000172870), AL::ALValue::array(3, 0.666667, 0.000432176));
	times[7][2] = 3.10000;
	keys[7][2] = AL::ALValue::array(0.00269144, AL::ALValue::array(3, -0.666667, -0.000435451), AL::ALValue::array(3, 0.133333, 8.70902e-05));
	times[7][3] = 3.50000;
	keys[7][3] = AL::ALValue::array(0.00290088, AL::ALValue::array(3, -0.133333, -0.000121008), AL::ALValue::array(3, 0.533333, 0.000484033));
	times[7][4] = 5.10000;
	keys[7][4] = AL::ALValue::array(0.00450656, AL::ALValue::array(3, -0.533333, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));

	names.push_back("RShoulderPitch");
	times[8].arraySetSize(11);
	keys[8].arraySetSize(11);

	times[8][0] = 0.300000;
	keys[8][0] = AL::ALValue::array(2.08560, AL::ALValue::array(3, -0.100000, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[8][1] = 0.700000;
	keys[8][1] = AL::ALValue::array(2.08567, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[8][2] = 1.10000;
	keys[8][2] = AL::ALValue::array(1.95283, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.166667, 0.00000));
	times[8][3] = 1.60000;
	keys[8][3] = AL::ALValue::array(2.08560, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[8][4] = 2.00000;
	keys[8][4] = AL::ALValue::array(2.08560, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[8][5] = 2.40000;
	keys[8][5] = AL::ALValue::array(2.08560, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.233333, 0.00000));
	times[8][6] = 3.10000;
	keys[8][6] = AL::ALValue::array(2.03720, AL::ALValue::array(3, -0.233333, 0.0483980), AL::ALValue::array(3, 0.133333, -0.0276560));
	times[8][7] = 3.50000;
	keys[8][7] = AL::ALValue::array(1.44047, AL::ALValue::array(3, -0.133333, 0.169762), AL::ALValue::array(3, 0.166667, -0.212203));
	times[8][8] = 4.00000;
	keys[8][8] = AL::ALValue::array(0.891306, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.0666667, 0.00000));
	times[8][9] = 4.20000;
	keys[8][9] = AL::ALValue::array(0.891306, AL::ALValue::array(3, -0.0666667, -0.00000), AL::ALValue::array(3, 0.300000, 0.00000));
	times[8][10] = 5.10000;
	keys[8][10] = AL::ALValue::array(1.46961, AL::ALValue::array(3, -0.300000, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));

	names.push_back("RShoulderRoll");
	times[9].arraySetSize(11);
	keys[9].arraySetSize(11);

	times[9][0] = 0.300000;
	keys[9][0] = AL::ALValue::array(-1.07384, AL::ALValue::array(3, -0.100000, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[9][1] = 0.700000;
	keys[9][1] = AL::ALValue::array(-0.366519, AL::ALValue::array(3, -0.133333, -0.0174533), AL::ALValue::array(3, 0.133333, 0.0174533));
	times[9][2] = 1.10000;
	keys[9][2] = AL::ALValue::array(-0.349066, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.166667, 0.00000));
	times[9][3] = 1.60000;
	keys[9][3] = AL::ALValue::array(-0.349066, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[9][4] = 2.00000;
	keys[9][4] = AL::ALValue::array(-0.366519, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[9][5] = 2.40000;
	keys[9][5] = AL::ALValue::array(-0.00869999, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.233333, 0.00000));
	times[9][6] = 3.10000;
	keys[9][6] = AL::ALValue::array(-0.564555, AL::ALValue::array(3, -0.233333, 0.147845), AL::ALValue::array(3, 0.133333, -0.0844827));
	times[9][7] = 3.50000;
	keys[9][7] = AL::ALValue::array(-0.705682, AL::ALValue::array(3, -0.133333, 0.0456459), AL::ALValue::array(3, 0.166667, -0.0570574));
	times[9][8] = 4.00000;
	keys[9][8] = AL::ALValue::array(-0.872665, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.0666667, 0.00000));
	times[9][9] = 4.20000;
	keys[9][9] = AL::ALValue::array(-0.680678, AL::ALValue::array(3, -0.0666667, -0.0437752), AL::ALValue::array(3, 0.300000, 0.196988));
	times[9][10] = 5.10000;
	keys[9][10] = AL::ALValue::array(-0.150374, AL::ALValue::array(3, -0.300000, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));

	names.push_back("RElbowYaw");
	times[10].arraySetSize(11);
	keys[10].arraySetSize(11);

	times[10][0] = 0.300000;
	keys[10][0] = AL::ALValue::array(0.765425, AL::ALValue::array(3, -0.100000, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[10][1] = 0.700000;
	keys[10][1] = AL::ALValue::array(-0.157080, AL::ALValue::array(3, -0.133333, 0.100674), AL::ALValue::array(3, 0.133333, -0.100674));
	times[10][2] = 1.10000;
	keys[10][2] = AL::ALValue::array(-0.257754, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.166667, 0.00000));
	times[10][3] = 1.60000;
	keys[10][3] = AL::ALValue::array(-0.0872665, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[10][4] = 2.00000;
	keys[10][4] = AL::ALValue::array(-0.0872665, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[10][5] = 2.40000;
	keys[10][5] = AL::ALValue::array(-0.0872665, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.233333, 0.00000));
	times[10][6] = 3.10000;
	keys[10][6] = AL::ALValue::array(-0.0690719, AL::ALValue::array(3, -0.233333, -0.0129705), AL::ALValue::array(3, 0.133333, 0.00741170));
	times[10][7] = 3.50000;
	keys[10][7] = AL::ALValue::array(-0.0261199, AL::ALValue::array(3, -0.133333, -0.0103241), AL::ALValue::array(3, 0.166667, 0.0129051));
	times[10][8] = 4.00000;
	keys[10][8] = AL::ALValue::array(0.000615569, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.0666667, 0.00000));
	times[10][9] = 4.20000;
	keys[10][9] = AL::ALValue::array(0.000615569, AL::ALValue::array(3, -0.0666667, -0.00000), AL::ALValue::array(3, 0.300000, 0.00000));
	times[10][10] = 5.10000;
	keys[10][10] = AL::ALValue::array(0.912689, AL::ALValue::array(3, -0.300000, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));

	names.push_back("RElbowRoll");
	times[11].arraySetSize(11);
	keys[11].arraySetSize(11);

	times[11][0] = 0.300000;
	keys[11][0] = AL::ALValue::array(0.710284, AL::ALValue::array(3, -0.100000, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[11][1] = 0.700000;
	keys[11][1] = AL::ALValue::array(0.0349066, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[11][2] = 1.10000;
	keys[11][2] = AL::ALValue::array(1.54462, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.166667, 0.00000));
	times[11][3] = 1.60000;
	keys[11][3] = AL::ALValue::array(1.54462, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[11][4] = 2.00000;
	keys[11][4] = AL::ALValue::array(0.698132, AL::ALValue::array(3, -0.133333, 0.251618), AL::ALValue::array(3, 0.133333, -0.251618));
	times[11][5] = 2.40000;
	keys[11][5] = AL::ALValue::array(0.0349066, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.233333, 0.00000));
	times[11][6] = 3.10000;
	keys[11][6] = AL::ALValue::array(0.0349066, AL::ALValue::array(3, -0.233333, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[11][7] = 3.50000;
	keys[11][7] = AL::ALValue::array(0.190258, AL::ALValue::array(3, -0.133333, -0.0620562), AL::ALValue::array(3, 0.166667, 0.0775702));
	times[11][8] = 4.00000;
	keys[11][8] = AL::ALValue::array(0.453786, AL::ALValue::array(3, -0.166667, -0.0880224), AL::ALValue::array(3, 0.0666667, 0.0352089));
	times[11][9] = 4.20000;
	keys[11][9] = AL::ALValue::array(0.559952, AL::ALValue::array(3, -0.0666667, -0.00000), AL::ALValue::array(3, 0.300000, 0.00000));
	times[11][10] = 5.10000;
	keys[11][10] = AL::ALValue::array(0.380475, AL::ALValue::array(3, -0.300000, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));

	names.push_back("RWristYaw");
	times[12].arraySetSize(5);
	keys[12].arraySetSize(5);

	times[12][0] = 0.300000;
	keys[12][0] = AL::ALValue::array(1.15199, AL::ALValue::array(3, -0.100000, -0.00000), AL::ALValue::array(3, 0.266667, 0.00000));
	times[12][1] = 1.10000;
	keys[12][1] = AL::ALValue::array(1.72571, AL::ALValue::array(3, -0.266667, -0.00000), AL::ALValue::array(3, 0.666667, 0.00000));
	times[12][2] = 3.10000;
	keys[12][2] = AL::ALValue::array(1.07836, AL::ALValue::array(3, -0.666667, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[12][3] = 3.50000;
	keys[12][3] = AL::ALValue::array(1.07989, AL::ALValue::array(3, -0.133333, -0.000408991), AL::ALValue::array(3, 0.533333, 0.00163596));
	times[12][4] = 5.10000;
	keys[12][4] = AL::ALValue::array(1.08450, AL::ALValue::array(3, -0.533333, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));

	names.push_back("RHand");
	times[13].arraySetSize(7);
	keys[13].arraySetSize(7);

	times[13][0] = 0.300000;
	keys[13][0] = AL::ALValue::array(0.00000, AL::ALValue::array(3, -0.100000, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[13][1] = 0.700000;
	keys[13][1] = AL::ALValue::array(0.00000, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[13][2] = 1.10000;
	keys[13][2] = AL::ALValue::array(0.000317793, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.300000, 0.00000));
	times[13][3] = 2.00000;
	keys[13][3] = AL::ALValue::array(0.00000, AL::ALValue::array(3, -0.300000, -0.00000), AL::ALValue::array(3, 0.366667, 0.00000));
	times[13][4] = 3.10000;
	keys[13][4] = AL::ALValue::array(0.00299607, AL::ALValue::array(3, -0.366667, -0.000244350), AL::ALValue::array(3, 0.133333, 8.88547e-05));
	times[13][5] = 3.50000;
	keys[13][5] = AL::ALValue::array(0.00308492, AL::ALValue::array(3, -0.133333, -5.07728e-05), AL::ALValue::array(3, 0.533333, 0.000203091));
	times[13][6] = 5.10000;
	keys[13][6] = AL::ALValue::array(0.00375766, AL::ALValue::array(3, -0.533333, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));

	names.push_back("LHipYawPitch");
	times[14].arraySetSize(10);
	keys[14].arraySetSize(10);

	times[14][0] = 0.300000;
	keys[14][0] = AL::ALValue::array(-0.470897, AL::ALValue::array(3, -0.100000, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[14][1] = 0.700000;
	keys[14][1] = AL::ALValue::array(0.00000, AL::ALValue::array(3, -0.133333, -0.00771196), AL::ALValue::array(3, 0.133333, 0.00771196));
	times[14][2] = 1.10000;
	keys[14][2] = AL::ALValue::array(0.00771196, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.166667, 0.00000));
	times[14][3] = 1.60000;
	keys[14][3] = AL::ALValue::array(-4.76838e-07, AL::ALValue::array(3, -0.166667, 0.00771244), AL::ALValue::array(3, 0.133333, -0.00616995));
	times[14][4] = 2.00000;
	keys[14][4] = AL::ALValue::array(-0.654977, AL::ALValue::array(3, -0.133333, 0.0741898), AL::ALValue::array(3, 0.366667, -0.204022));
	times[14][5] = 3.10000;
	keys[14][5] = AL::ALValue::array(-0.858999, AL::ALValue::array(3, -0.366667, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[14][6] = 3.50000;
	keys[14][6] = AL::ALValue::array(-0.696386, AL::ALValue::array(3, -0.133333, -0.0676658), AL::ALValue::array(3, 0.166667, 0.0845822));
	times[14][7] = 4.00000;
	keys[14][7] = AL::ALValue::array(-0.402255, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.0666667, 0.00000));
	times[14][8] = 4.20000;
	keys[14][8] = AL::ALValue::array(-0.402255, AL::ALValue::array(3, -0.0666667, -0.00000), AL::ALValue::array(3, 0.300000, 0.00000));
	times[14][9] = 5.10000;
	keys[14][9] = AL::ALValue::array(-0.230059, AL::ALValue::array(3, -0.300000, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));

	names.push_back("LHipRoll");
	times[15].arraySetSize(11);
	keys[15].arraySetSize(11);

	times[15][0] = 0.300000;
	keys[15][0] = AL::ALValue::array(0.0445279, AL::ALValue::array(3, -0.100000, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[15][1] = 0.700000;
	keys[15][1] = AL::ALValue::array(1.56923e-07, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[15][2] = 1.10000;
	keys[15][2] = AL::ALValue::array(0.0583340, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.166667, 0.00000));
	times[15][3] = 1.60000;
	keys[15][3] = AL::ALValue::array(1.56923e-07, AL::ALValue::array(3, -0.166667, 0.0227993), AL::ALValue::array(3, 0.133333, -0.0182394));
	times[15][4] = 2.00000;
	keys[15][4] = AL::ALValue::array(-0.0647821, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[15][5] = 2.40000;
	keys[15][5] = AL::ALValue::array(0.541052, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.233333, 0.00000));
	times[15][6] = 3.10000;
	keys[15][6] = AL::ALValue::array(-0.174835, AL::ALValue::array(3, -0.233333, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[15][7] = 3.50000;
	keys[15][7] = AL::ALValue::array(0.00924597, AL::ALValue::array(3, -0.133333, -0.0543439), AL::ALValue::array(3, 0.166667, 0.0679298));
	times[15][8] = 4.00000;
	keys[15][8] = AL::ALValue::array(0.191986, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.0666667, 0.00000));
	times[15][9] = 4.20000;
	keys[15][9] = AL::ALValue::array(0.174533, AL::ALValue::array(3, -0.0666667, 0.00242899), AL::ALValue::array(3, 0.300000, -0.0109305));
	times[15][10] = 5.10000;
	keys[15][10] = AL::ALValue::array(0.151908, AL::ALValue::array(3, -0.300000, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));

	names.push_back("LHipPitch");
	times[16].arraySetSize(11);
	keys[16].arraySetSize(11);

	times[16][0] = 0.300000;
	keys[16][0] = AL::ALValue::array(0.250085, AL::ALValue::array(3, -0.100000, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[16][1] = 0.700000;
	keys[16][1] = AL::ALValue::array(-0.174533, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[16][2] = 1.10000;
	keys[16][2] = AL::ALValue::array(0.374338, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.166667, 0.00000));
	times[16][3] = 1.60000;
	keys[16][3] = AL::ALValue::array(-0.174533, AL::ALValue::array(3, -0.166667, 0.287485), AL::ALValue::array(3, 0.133333, -0.229988));
	times[16][4] = 2.00000;
	keys[16][4] = AL::ALValue::array(-1.17808, AL::ALValue::array(3, -0.133333, 0.232711), AL::ALValue::array(3, 0.133333, -0.232711));
	times[16][5] = 2.40000;
	keys[16][5] = AL::ALValue::array(-1.57080, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.233333, 0.00000));
	times[16][6] = 3.10000;
	keys[16][6] = AL::ALValue::array(-0.0567160, AL::ALValue::array(3, -0.233333, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[16][7] = 3.50000;
	keys[16][7] = AL::ALValue::array(-0.455555, AL::ALValue::array(3, -0.133333, 0.118296), AL::ALValue::array(3, 0.166667, -0.147870));
	times[16][8] = 4.00000;
	keys[16][8] = AL::ALValue::array(-0.855211, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.0666667, 0.00000));
	times[16][9] = 4.20000;
	keys[16][9] = AL::ALValue::array(-0.835988, AL::ALValue::array(3, -0.0666667, -0.0192230), AL::ALValue::array(3, 0.300000, 0.0865036));
	times[16][10] = 5.10000;
	keys[16][10] = AL::ALValue::array(0.213269, AL::ALValue::array(3, -0.300000, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));

	names.push_back("LKneePitch");
	times[17].arraySetSize(11);
	keys[17].arraySetSize(11);

	times[17][0] = 0.300000;
	keys[17][0] = AL::ALValue::array(0.0919980, AL::ALValue::array(3, -0.100000, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[17][1] = 0.700000;
	keys[17][1] = AL::ALValue::array(1.67552, AL::ALValue::array(3, -0.133333, -0.321373), AL::ALValue::array(3, 0.133333, 0.321373));
	times[17][2] = 1.10000;
	keys[17][2] = AL::ALValue::array(2.02024, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.166667, 0.00000));
	times[17][3] = 1.60000;
	keys[17][3] = AL::ALValue::array(1.67552, AL::ALValue::array(3, -0.166667, 0.174531), AL::ALValue::array(3, 0.133333, -0.139625));
	times[17][4] = 2.00000;
	keys[17][4] = AL::ALValue::array(1.07777, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[17][5] = 2.40000;
	keys[17][5] = AL::ALValue::array(1.67552, AL::ALValue::array(3, -0.133333, -0.125394), AL::ALValue::array(3, 0.233333, 0.219439));
	times[17][6] = 3.10000;
	keys[17][6] = AL::ALValue::array(2.11227, AL::ALValue::array(3, -0.233333, -0.000458130), AL::ALValue::array(3, 0.133333, 0.000261789));
	times[17][7] = 3.50000;
	keys[17][7] = AL::ALValue::array(2.11253, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.166667, 0.00000));
	times[17][8] = 4.00000;
	keys[17][8] = AL::ALValue::array(2.11253, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.0666667, 0.00000));
	times[17][9] = 4.20000;
	keys[17][9] = AL::ALValue::array(2.11253, AL::ALValue::array(3, -0.0666667, -0.00000), AL::ALValue::array(3, 0.300000, 0.00000));
	times[17][10] = 5.10000;
	keys[17][10] = AL::ALValue::array(-0.0890139, AL::ALValue::array(3, -0.300000, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));

	names.push_back("LAnklePitch");
	times[18].arraySetSize(11);
	keys[18].arraySetSize(11);

	times[18][0] = 0.300000;
	keys[18][0] = AL::ALValue::array(0.825249, AL::ALValue::array(3, -0.100000, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[18][1] = 0.700000;
	keys[18][1] = AL::ALValue::array(0.244346, AL::ALValue::array(3, -0.133333, 0.213226), AL::ALValue::array(3, 0.133333, -0.213226));
	times[18][2] = 1.10000;
	keys[18][2] = AL::ALValue::array(-0.454107, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.166667, 0.00000));
	times[18][3] = 1.60000;
	keys[18][3] = AL::ALValue::array(0.244346, AL::ALValue::array(3, -0.166667, -0.209552), AL::ALValue::array(3, 0.133333, 0.167642));
	times[18][4] = 2.00000;
	keys[18][4] = AL::ALValue::array(0.677476, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[18][5] = 2.40000;
	keys[18][5] = AL::ALValue::array(0.663225, AL::ALValue::array(3, -0.133333, 0.0142506), AL::ALValue::array(3, 0.233333, -0.0249386));
	times[18][6] = 3.10000;
	keys[18][6] = AL::ALValue::array(-1.10145, AL::ALValue::array(3, -0.233333, 0.153978), AL::ALValue::array(3, 0.133333, -0.0879873));
	times[18][7] = 3.50000;
	keys[18][7] = AL::ALValue::array(-1.18944, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.166667, 0.00000));
	times[18][8] = 4.00000;
	keys[18][8] = AL::ALValue::array(-1.18944, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.0666667, 0.00000));
	times[18][9] = 4.20000;
	keys[18][9] = AL::ALValue::array(-1.18944, AL::ALValue::array(3, -0.0666667, -0.00000), AL::ALValue::array(3, 0.300000, 0.00000));
	times[18][10] = 5.10000;
	keys[18][10] = AL::ALValue::array(0.0812601, AL::ALValue::array(3, -0.300000, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));

	names.push_back("LAnkleRoll");
	times[19].arraySetSize(11);
	keys[19].arraySetSize(11);

	times[19][0] = 0.300000;
	keys[19][0] = AL::ALValue::array(-0.0337060, AL::ALValue::array(3, -0.100000, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[19][1] = 0.700000;
	keys[19][1] = AL::ALValue::array(0.00000, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[19][2] = 1.10000;
	keys[19][2] = AL::ALValue::array(-0.0337060, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.166667, 0.00000));
	times[19][3] = 1.60000;
	keys[19][3] = AL::ALValue::array(0.00000, AL::ALValue::array(3, -0.166667, -0.0337060), AL::ALValue::array(3, 0.133333, 0.0269648));
	times[19][4] = 2.00000;
	keys[19][4] = AL::ALValue::array(0.171085, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[19][5] = 2.40000;
	keys[19][5] = AL::ALValue::array(-0.104720, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.233333, 0.00000));
	times[19][6] = 3.10000;
	keys[19][6] = AL::ALValue::array(-0.0996681, AL::ALValue::array(3, -0.233333, -0.00505168), AL::ALValue::array(3, 0.133333, 0.00288668));
	times[19][7] = 3.50000;
	keys[19][7] = AL::ALValue::array(0.0429939, AL::ALValue::array(3, -0.133333, -0.0322627), AL::ALValue::array(3, 0.166667, 0.0403283));
	times[19][8] = 4.00000;
	keys[19][8] = AL::ALValue::array(0.118105, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.0666667, 0.00000));
	times[19][9] = 4.20000;
	keys[19][9] = AL::ALValue::array(0.0872665, AL::ALValue::array(3, -0.0666667, 0.0154296), AL::ALValue::array(3, 0.300000, -0.0694333));
	times[19][10] = 5.10000;
	keys[19][10] = AL::ALValue::array(-0.136484, AL::ALValue::array(3, -0.300000, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));

	names.push_back("RHipRoll");
	times[20].arraySetSize(11);
	keys[20].arraySetSize(11);

	times[20][0] = 0.300000;
	keys[20][0] = AL::ALValue::array(-0.148756, AL::ALValue::array(3, -0.100000, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[20][1] = 0.700000;
	keys[20][1] = AL::ALValue::array(0.00000, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[20][2] = 1.10000;
	keys[20][2] = AL::ALValue::array(-0.0229680, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.166667, 0.00000));
	times[20][3] = 1.60000;
	keys[20][3] = AL::ALValue::array(0.00000, AL::ALValue::array(3, -0.166667, -0.0157155), AL::ALValue::array(3, 0.133333, 0.0125724));
	times[20][4] = 2.00000;
	keys[20][4] = AL::ALValue::array(0.0618955, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[20][5] = 2.40000;
	keys[20][5] = AL::ALValue::array(-0.541052, AL::ALValue::array(3, -0.133333, 0.0396788), AL::ALValue::array(3, 0.233333, -0.0694379));
	times[20][6] = 3.10000;
	keys[20][6] = AL::ALValue::array(-0.610490, AL::ALValue::array(3, -0.233333, 0.0181409), AL::ALValue::array(3, 0.133333, -0.0103662));
	times[20][7] = 3.50000;
	keys[20][7] = AL::ALValue::array(-0.626573, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.166667, 0.00000));
	times[20][8] = 4.00000;
	keys[20][8] = AL::ALValue::array(-0.296706, AL::ALValue::array(3, -0.166667, -0.145029), AL::ALValue::array(3, 0.0666667, 0.0580114));
	times[20][9] = 4.20000;
	keys[20][9] = AL::ALValue::array(-0.0174533, AL::ALValue::array(3, -0.0666667, -0.00000), AL::ALValue::array(3, 0.300000, 0.00000));
	times[20][10] = 5.10000;
	keys[20][10] = AL::ALValue::array(-0.0628521, AL::ALValue::array(3, -0.300000, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));

	names.push_back("RHipPitch");
	times[21].arraySetSize(11);
	keys[21].arraySetSize(11);

	times[21][0] = 0.300000;
	keys[21][0] = AL::ALValue::array(0.185572, AL::ALValue::array(3, -0.100000, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[21][1] = 0.700000;
	keys[21][1] = AL::ALValue::array(-0.174533, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[21][2] = 1.10000;
	keys[21][2] = AL::ALValue::array(0.282215, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.166667, 0.00000));
	times[21][3] = 1.60000;
	keys[21][3] = AL::ALValue::array(-0.174533, AL::ALValue::array(3, -0.166667, 0.267921), AL::ALValue::array(3, 0.133333, -0.214337));
	times[21][4] = 2.00000;
	keys[21][4] = AL::ALValue::array(-1.16456, AL::ALValue::array(3, -0.133333, 0.232711), AL::ALValue::array(3, 0.133333, -0.232711));
	times[21][5] = 2.40000;
	keys[21][5] = AL::ALValue::array(-1.57080, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.233333, 0.00000));
	times[21][6] = 3.10000;
	keys[21][6] = AL::ALValue::array(-1.56012, AL::ALValue::array(3, -0.233333, -0.0106762), AL::ALValue::array(3, 0.133333, 0.00610069));
	times[21][7] = 3.50000;
	keys[21][7] = AL::ALValue::array(-1.02974, AL::ALValue::array(3, -0.133333, -0.0969325), AL::ALValue::array(3, 0.166667, 0.121166));
	times[21][8] = 4.00000;
	keys[21][8] = AL::ALValue::array(-0.905826, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.0666667, 0.00000));
	times[21][9] = 4.20000;
	keys[21][9] = AL::ALValue::array(-0.905826, AL::ALValue::array(3, -0.0666667, -0.00000), AL::ALValue::array(3, 0.300000, 0.00000));
	times[21][10] = 5.10000;
	keys[21][10] = AL::ALValue::array(0.208583, AL::ALValue::array(3, -0.300000, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));

	names.push_back("RKneePitch");
	times[22].arraySetSize(11);
	keys[22].arraySetSize(11);

	times[22][0] = 0.300000;
	keys[22][0] = AL::ALValue::array(0.191792, AL::ALValue::array(3, -0.100000, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[22][1] = 0.700000;
	keys[22][1] = AL::ALValue::array(1.67552, AL::ALValue::array(3, -0.133333, -0.305776), AL::ALValue::array(3, 0.133333, 0.305776));
	times[22][2] = 1.10000;
	keys[22][2] = AL::ALValue::array(2.02645, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.166667, 0.00000));
	times[22][3] = 1.60000;
	keys[22][3] = AL::ALValue::array(1.67552, AL::ALValue::array(3, -0.166667, 0.172702), AL::ALValue::array(3, 0.133333, -0.138162));
	times[22][4] = 2.00000;
	keys[22][4] = AL::ALValue::array(1.09386, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[22][5] = 2.40000;
	keys[22][5] = AL::ALValue::array(1.67552, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.233333, 0.00000));
	times[22][6] = 3.10000;
	keys[22][6] = AL::ALValue::array(1.09992, AL::ALValue::array(3, -0.233333, 0.0281243), AL::ALValue::array(3, 0.133333, -0.0160710));
	times[22][7] = 3.50000;
	keys[22][7] = AL::ALValue::array(1.08385, AL::ALValue::array(3, -0.133333, 0.0160710), AL::ALValue::array(3, 0.166667, -0.0200888));
	times[22][8] = 4.00000;
	keys[22][8] = AL::ALValue::array(0.876155, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.0666667, 0.00000));
	times[22][9] = 4.20000;
	keys[22][9] = AL::ALValue::array(1.76278, AL::ALValue::array(3, -0.0666667, -0.00000), AL::ALValue::array(3, 0.300000, 0.00000));
	times[22][10] = 5.10000;
	keys[22][10] = AL::ALValue::array(-0.0766580, AL::ALValue::array(3, -0.300000, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));

	names.push_back("RAnklePitch");
	times[23].arraySetSize(11);
	keys[23].arraySetSize(11);

	times[23][0] = 0.300000;
	keys[23][0] = AL::ALValue::array(0.466378, AL::ALValue::array(3, -0.100000, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[23][1] = 0.700000;
	keys[23][1] = AL::ALValue::array(0.244346, AL::ALValue::array(3, -0.133333, 0.140105), AL::ALValue::array(3, 0.133333, -0.140105));
	times[23][2] = 1.10000;
	keys[23][2] = AL::ALValue::array(-0.374254, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.166667, 0.00000));
	times[23][3] = 1.60000;
	keys[23][3] = AL::ALValue::array(0.244346, AL::ALValue::array(3, -0.166667, -0.202152), AL::ALValue::array(3, 0.133333, 0.161721));
	times[23][4] = 2.00000;
	keys[23][4] = AL::ALValue::array(0.717365, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[23][5] = 2.40000;
	keys[23][5] = AL::ALValue::array(0.663225, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.233333, 0.00000));
	times[23][6] = 3.10000;
	keys[23][6] = AL::ALValue::array(0.783916, AL::ALValue::array(3, -0.233333, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[23][7] = 3.50000;
	keys[23][7] = AL::ALValue::array(0.441568, AL::ALValue::array(3, -0.133333, 0.0307178), AL::ALValue::array(3, 0.166667, -0.0383972));
	times[23][8] = 4.00000;
	keys[23][8] = AL::ALValue::array(0.403171, AL::ALValue::array(3, -0.166667, 0.0383972), AL::ALValue::array(3, 0.0666667, -0.0153589));
	times[23][9] = 4.20000;
	keys[23][9] = AL::ALValue::array(-0.579449, AL::ALValue::array(3, -0.0666667, -0.00000), AL::ALValue::array(3, 0.300000, 0.00000));
	times[23][10] = 5.10000;
	keys[23][10] = AL::ALValue::array(0.0813440, AL::ALValue::array(3, -0.300000, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));

	names.push_back("RAnkleRoll");
	times[24].arraySetSize(11);
	keys[24].arraySetSize(11);

	times[24][0] = 0.300000;
	keys[24][0] = AL::ALValue::array(0.167248, AL::ALValue::array(3, -0.100000, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[24][1] = 0.700000;
	keys[24][1] = AL::ALValue::array(8.63852e-08, AL::ALValue::array(3, -0.133333, 0.0345150), AL::ALValue::array(3, 0.133333, -0.0345150));
	times[24][2] = 1.10000;
	keys[24][2] = AL::ALValue::array(-0.0398420, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.166667, 0.00000));
	times[24][3] = 1.60000;
	keys[24][3] = AL::ALValue::array(8.63852e-08, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[24][4] = 2.00000;
	keys[24][4] = AL::ALValue::array(-0.160838, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[24][5] = 2.40000;
	keys[24][5] = AL::ALValue::array(0.104720, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.233333, 0.00000));
	times[24][6] = 3.10000;
	keys[24][6] = AL::ALValue::array(-0.0981341, AL::ALValue::array(3, -0.233333, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[24][7] = 3.50000;
	keys[24][7] = AL::ALValue::array(0.443314, AL::ALValue::array(3, -0.133333, -0.115220), AL::ALValue::array(3, 0.166667, 0.144025));
	times[24][8] = 4.00000;
	keys[24][8] = AL::ALValue::array(0.679603, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.0666667, 0.00000));
	times[24][9] = 4.20000;
	keys[24][9] = AL::ALValue::array(0.277507, AL::ALValue::array(3, -0.0666667, 0.0373738), AL::ALValue::array(3, 0.300000, -0.168182));
	times[24][10] = 5.10000;
	keys[24][10] = AL::ALValue::array(0.0629359, AL::ALValue::array(3, -0.300000, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));
  
}

int ALstandUpBack2011::ExecutePost() {
  return  motion->post.angleInterpolationBezier(names, times, keys); 
} 
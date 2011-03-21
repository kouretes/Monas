#include "ALstandUpFront2011.h"

#include "hal/robot/generic_nao/kAlBroker.h"

namespace {
SpecialActionRegistrar<ALstandUpFront2011>::Type temp("ALstandUpFront2011");
}

ALstandUpFront2011::ALstandUpFront2011() {
  Initialize();
  Logger::Instance().WriteMsg("SP:ALstandUpFront2011","Initialized!",Logger::ExtraInfo);
  
  try {
    motion = KAlBroker::Instance().GetBroker()->getMotionProxy();
  } catch (AL::ALError& e) {
    Logger::Instance().WriteMsg("SP:ALstandUpFront2011","Error in getting motion proxy",Logger::FatalError);
  }
  
}




void ALstandUpFront2011::Initialize() {
  
	names.reserve(25);
	times.arraySetSize(25);
	keys.arraySetSize(25);

	names.push_back("HeadYaw");
	times[0].arraySetSize(12);
	keys[0].arraySetSize(12);

	times[0][0] = 1.00000;
	keys[0][0] = AL::ALValue::array(-0.0245859, AL::ALValue::array(3, -0.333333, -0.00000), AL::ALValue::array(3, 0.333333, 0.00000));
	times[0][1] = 2.00000;
	keys[0][1] = AL::ALValue::array(-4.76838e-07, AL::ALValue::array(3, -0.333333, -0.0122152), AL::ALValue::array(3, 0.266667, 0.00977214));
	times[0][2] = 2.80000;
	keys[0][2] = AL::ALValue::array(0.0413760, AL::ALValue::array(3, -0.266667, -0.00000), AL::ALValue::array(3, 0.266667, 0.00000));
	times[0][3] = 3.60000;
	keys[0][3] = AL::ALValue::array(6.74350e-07, AL::ALValue::array(3, -0.266667, 3.85341e-07), AL::ALValue::array(3, 0.233333, -3.37173e-07));
	times[0][4] = 4.30000;
	keys[0][4] = AL::ALValue::array(3.37177e-07, AL::ALValue::array(3, -0.233333, 2.44455e-12), AL::ALValue::array(3, 0.166667, -1.74610e-12));
	times[0][5] = 4.80000;
	keys[0][5] = AL::ALValue::array(3.37175e-07, AL::ALValue::array(3, -0.166667, 1.74610e-12), AL::ALValue::array(3, 0.266667, -2.79377e-12));
	times[0][6] = 5.60000;
	keys[0][6] = AL::ALValue::array(-0.513931, AL::ALValue::array(3, -0.266667, -0.00000), AL::ALValue::array(3, 0.300000, 0.00000));
	times[0][7] = 6.50000;
	keys[0][7] = AL::ALValue::array(-0.302240, AL::ALValue::array(3, -0.300000, -0.129579), AL::ALValue::array(3, 0.300000, 0.129579));
	times[0][8] = 7.40000;
	keys[0][8] = AL::ALValue::array(0.263545, AL::ALValue::array(3, -0.300000, -0.00000), AL::ALValue::array(3, 0.200000, 0.00000));
	times[0][9] = 8.00000;
	keys[0][9] = AL::ALValue::array(0.120428, AL::ALValue::array(3, -0.200000, 0.0454910), AL::ALValue::array(3, 0.200000, -0.0454910));
	times[0][10] = 8.60000;
	keys[0][10] = AL::ALValue::array(-0.00940132, AL::ALValue::array(3, -0.200000, -0.00000), AL::ALValue::array(3, 0.633333, 0.00000));
	times[0][11] = 10.5000;
	keys[0][11] = AL::ALValue::array(0.0659200, AL::ALValue::array(3, -0.633333, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));

	names.push_back("HeadPitch");
	times[1].arraySetSize(12);
	keys[1].arraySetSize(12);

	times[1][0] = 1.00000;
	keys[1][0] = AL::ALValue::array(0.0628521, AL::ALValue::array(3, -0.333333, -0.00000), AL::ALValue::array(3, 0.333333, 0.00000));
	times[1][1] = 2.00000;
	keys[1][1] = AL::ALValue::array(0.00000, AL::ALValue::array(3, -0.333333, -0.00000), AL::ALValue::array(3, 0.266667, 0.00000));
	times[1][2] = 2.80000;
	keys[1][2] = AL::ALValue::array(0.514900, AL::ALValue::array(3, -0.266667, -0.00000), AL::ALValue::array(3, 0.266667, 0.00000));
	times[1][3] = 3.60000;
	keys[1][3] = AL::ALValue::array(-0.672001, AL::ALValue::array(3, -0.266667, -0.00000), AL::ALValue::array(3, 0.233333, 0.00000));
	times[1][4] = 4.30000;
	keys[1][4] = AL::ALValue::array(0.00000, AL::ALValue::array(3, -0.233333, -0.198541), AL::ALValue::array(3, 0.166667, 0.141815));
	times[1][5] = 4.80000;
	keys[1][5] = AL::ALValue::array(0.349066, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.266667, 0.00000));
	times[1][6] = 5.60000;
	keys[1][6] = AL::ALValue::array(0.171766, AL::ALValue::array(3, -0.266667, -0.00000), AL::ALValue::array(3, 0.300000, 0.00000));
	times[1][7] = 6.50000;
	keys[1][7] = AL::ALValue::array(0.345107, AL::ALValue::array(3, -0.300000, -0.00000), AL::ALValue::array(3, 0.300000, 0.00000));
	times[1][8] = 7.40000;
	keys[1][8] = AL::ALValue::array(0.0104720, AL::ALValue::array(3, -0.300000, -0.00000), AL::ALValue::array(3, 0.200000, 0.00000));
	times[1][9] = 8.00000;
	keys[1][9] = AL::ALValue::array(0.0296706, AL::ALValue::array(3, -0.200000, -0.0191986), AL::ALValue::array(3, 0.200000, 0.0191986));
	times[1][10] = 8.60000;
	keys[1][10] = AL::ALValue::array(0.378859, AL::ALValue::array(3, -0.200000, -0.00000), AL::ALValue::array(3, 0.633333, 0.00000));
	times[1][11] = 10.5000;
	keys[1][11] = AL::ALValue::array(-0.0399260, AL::ALValue::array(3, -0.633333, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));

	names.push_back("LShoulderPitch");
	times[2].arraySetSize(12);
	keys[2].arraySetSize(12);

	times[2][0] = 1.00000;
	keys[2][0] = AL::ALValue::array(2.08560, AL::ALValue::array(3, -0.333333, -0.00000), AL::ALValue::array(3, 0.333333, 0.00000));
	times[2][1] = 2.00000;
	keys[2][1] = AL::ALValue::array(2.09440, AL::ALValue::array(3, -0.333333, -0.00000), AL::ALValue::array(3, 0.266667, 0.00000));
	times[2][2] = 2.80000;
	keys[2][2] = AL::ALValue::array(2.03865, AL::ALValue::array(3, -0.266667, -0.00000), AL::ALValue::array(3, 0.266667, 0.00000));
	times[2][3] = 3.60000;
	keys[2][3] = AL::ALValue::array(2.08560, AL::ALValue::array(3, -0.266667, -0.00000), AL::ALValue::array(3, 0.233333, 0.00000));
	times[2][4] = 4.30000;
	keys[2][4] = AL::ALValue::array(2.08560, AL::ALValue::array(3, -0.233333, -0.00000), AL::ALValue::array(3, 0.166667, 0.00000));
	times[2][5] = 4.80000;
	keys[2][5] = AL::ALValue::array(2.08560, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.266667, 0.00000));
	times[2][6] = 5.60000;
	keys[2][6] = AL::ALValue::array(2.03251, AL::ALValue::array(3, -0.266667, 0.0530929), AL::ALValue::array(3, 0.300000, -0.0597295));
	times[2][7] = 6.50000;
	keys[2][7] = AL::ALValue::array(1.28698, AL::ALValue::array(3, -0.300000, 0.179222), AL::ALValue::array(3, 0.300000, -0.179222));
	times[2][8] = 7.40000;
	keys[2][8] = AL::ALValue::array(0.957173, AL::ALValue::array(3, -0.300000, 0.110755), AL::ALValue::array(3, 0.200000, -0.0738367));
	times[2][9] = 8.00000;
	keys[2][9] = AL::ALValue::array(0.733209, AL::ALValue::array(3, -0.200000, -0.00000), AL::ALValue::array(3, 0.200000, 0.00000));
	times[2][10] = 8.60000;
	keys[2][10] = AL::ALValue::array(0.733209, AL::ALValue::array(3, -0.200000, -0.00000), AL::ALValue::array(3, 0.633333, 0.00000));
	times[2][11] = 10.5000;
	keys[2][11] = AL::ALValue::array(1.59225, AL::ALValue::array(3, -0.633333, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));

	names.push_back("LShoulderRoll");
	times[3].arraySetSize(12);
	keys[3].arraySetSize(12);

	times[3][0] = 1.00000;
	keys[3][0] = AL::ALValue::array(0.980184, AL::ALValue::array(3, -0.333333, -0.00000), AL::ALValue::array(3, 0.333333, 0.00000));
	times[3][1] = 2.00000;
	keys[3][1] = AL::ALValue::array(0.366519, AL::ALValue::array(3, -0.333333, 0.0218166), AL::ALValue::array(3, 0.266667, -0.0174533));
	times[3][2] = 2.80000;
	keys[3][2] = AL::ALValue::array(0.349066, AL::ALValue::array(3, -0.266667, -0.00000), AL::ALValue::array(3, 0.266667, 0.00000));
	times[3][3] = 3.60000;
	keys[3][3] = AL::ALValue::array(0.349066, AL::ALValue::array(3, -0.266667, -0.00000), AL::ALValue::array(3, 0.233333, 0.00000));
	times[3][4] = 4.30000;
	keys[3][4] = AL::ALValue::array(0.366519, AL::ALValue::array(3, -0.233333, -0.00000), AL::ALValue::array(3, 0.166667, 0.00000));
	times[3][5] = 4.80000;
	keys[3][5] = AL::ALValue::array(0.00869999, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.266667, 0.00000));
	times[3][6] = 5.60000;
	keys[3][6] = AL::ALValue::array(0.299088, AL::ALValue::array(3, -0.266667, -0.00000), AL::ALValue::array(3, 0.300000, 0.00000));
	times[3][7] = 6.50000;
	keys[3][7] = AL::ALValue::array(0.0705221, AL::ALValue::array(3, -0.300000, -0.00000), AL::ALValue::array(3, 0.300000, 0.00000));
	times[3][8] = 7.40000;
	keys[3][8] = AL::ALValue::array(0.266875, AL::ALValue::array(3, -0.300000, -0.0855972), AL::ALValue::array(3, 0.200000, 0.0570648));
	times[3][9] = 8.00000;
	keys[3][9] = AL::ALValue::array(0.498508, AL::ALValue::array(3, -0.200000, -0.00000), AL::ALValue::array(3, 0.200000, 0.00000));
	times[3][10] = 8.60000;
	keys[3][10] = AL::ALValue::array(0.498508, AL::ALValue::array(3, -0.200000, -0.00000), AL::ALValue::array(3, 0.633333, 0.00000));
	times[3][11] = 10.5000;
	keys[3][11] = AL::ALValue::array(0.219320, AL::ALValue::array(3, -0.633333, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));

	names.push_back("LElbowYaw");
	times[4].arraySetSize(12);
	keys[4].arraySetSize(12);

	times[4][0] = 1.00000;
	keys[4][0] = AL::ALValue::array(-0.760906, AL::ALValue::array(3, -0.333333, -0.00000), AL::ALValue::array(3, 0.333333, 0.00000));
	times[4][1] = 2.00000;
	keys[4][1] = AL::ALValue::array(0.157080, AL::ALValue::array(3, -0.333333, -0.00000), AL::ALValue::array(3, 0.266667, 0.00000));
	times[4][2] = 2.80000;
	keys[4][2] = AL::ALValue::array(0.122678, AL::ALValue::array(3, -0.266667, 0.0116355), AL::ALValue::array(3, 0.266667, -0.0116355));
	times[4][3] = 3.60000;
	keys[4][3] = AL::ALValue::array(0.0872665, AL::ALValue::array(3, -0.266667, -0.00000), AL::ALValue::array(3, 0.233333, 0.00000));
	times[4][4] = 4.30000;
	keys[4][4] = AL::ALValue::array(0.0872665, AL::ALValue::array(3, -0.233333, -0.00000), AL::ALValue::array(3, 0.166667, 0.00000));
	times[4][5] = 4.80000;
	keys[4][5] = AL::ALValue::array(0.0872665, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.266667, 0.00000));
	times[4][6] = 5.60000;
	keys[4][6] = AL::ALValue::array(-1.96049, AL::ALValue::array(3, -0.266667, -0.00000), AL::ALValue::array(3, 0.300000, 0.00000));
	times[4][7] = 6.50000;
	keys[4][7] = AL::ALValue::array(-0.854480, AL::ALValue::array(3, -0.300000, -0.285068), AL::ALValue::array(3, 0.300000, 0.285068));
	times[4][8] = 7.40000;
	keys[4][8] = AL::ALValue::array(-0.250085, AL::ALValue::array(3, -0.300000, -0.187511), AL::ALValue::array(3, 0.200000, 0.125008));
	times[4][9] = 8.00000;
	keys[4][9] = AL::ALValue::array(0.0830765, AL::ALValue::array(3, -0.200000, -0.00000), AL::ALValue::array(3, 0.200000, 0.00000));
	times[4][10] = 8.60000;
	keys[4][10] = AL::ALValue::array(0.0830765, AL::ALValue::array(3, -0.200000, -0.00000), AL::ALValue::array(3, 0.633333, 0.00000));
	times[4][11] = 10.5000;
	keys[4][11] = AL::ALValue::array(-1.03089, AL::ALValue::array(3, -0.633333, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));

	names.push_back("LElbowRoll");
	times[5].arraySetSize(13);
	keys[5].arraySetSize(13);

	times[5][0] = 1.00000;
	keys[5][0] = AL::ALValue::array(-0.624296, AL::ALValue::array(3, -0.333333, -0.00000), AL::ALValue::array(3, 0.333333, 0.00000));
	times[5][1] = 2.00000;
	keys[5][1] = AL::ALValue::array(0.00000, AL::ALValue::array(3, -0.333333, -0.00000), AL::ALValue::array(3, 0.266667, 0.00000));
	times[5][2] = 2.80000;
	keys[5][2] = AL::ALValue::array(-1.56210, AL::ALValue::array(3, -0.266667, -0.00000), AL::ALValue::array(3, 0.266667, 0.00000));
	times[5][3] = 3.60000;
	keys[5][3] = AL::ALValue::array(-1.56210, AL::ALValue::array(3, -0.266667, -0.00000), AL::ALValue::array(3, 0.233333, 0.00000));
	times[5][4] = 4.30000;
	keys[5][4] = AL::ALValue::array(-0.698132, AL::ALValue::array(3, -0.233333, -0.302050), AL::ALValue::array(3, 0.166667, 0.215750));
	times[5][5] = 4.80000;
	keys[5][5] = AL::ALValue::array(-0.00869999, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.133333, 0.00000));
	times[5][6] = 5.20000;
	keys[5][6] = AL::ALValue::array(-0.191986, AL::ALValue::array(3, -0.133333, 0.172908), AL::ALValue::array(3, 0.133333, -0.172908));
	times[5][7] = 5.60000;
	keys[5][7] = AL::ALValue::array(-1.04615, AL::ALValue::array(3, -0.133333, -0.00000), AL::ALValue::array(3, 0.300000, 0.00000));
	times[5][8] = 6.50000;
	keys[5][8] = AL::ALValue::array(-0.872804, AL::ALValue::array(3, -0.300000, -0.0360492), AL::ALValue::array(3, 0.300000, 0.0360492));
	times[5][9] = 7.40000;
	keys[5][9] = AL::ALValue::array(-0.829852, AL::ALValue::array(3, -0.300000, -0.0134900), AL::ALValue::array(3, 0.200000, 0.00899333));
	times[5][10] = 8.00000;
	keys[5][10] = AL::ALValue::array(-0.805354, AL::ALValue::array(3, -0.200000, -0.00000), AL::ALValue::array(3, 0.200000, 0.00000));
	times[5][11] = 8.60000;
	keys[5][11] = AL::ALValue::array(-0.805354, AL::ALValue::array(3, -0.200000, -0.00000), AL::ALValue::array(3, 0.633333, 0.00000));
	times[5][12] = 10.5000;
	keys[5][12] = AL::ALValue::array(-0.681054, AL::ALValue::array(3, -0.633333, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));

	names.push_back("LWristYaw");
	times[6].arraySetSize(6);
	keys[6].arraySetSize(6);

	times[6][0] = 1.00000;
	keys[6][0] = AL::ALValue::array(-0.705682, AL::ALValue::array(3, -0.333333, -0.00000), AL::ALValue::array(3, 0.600000, 0.00000));
	times[6][1] = 2.80000;
	keys[6][1] = AL::ALValue::array(-1.78868, AL::ALValue::array(3, -0.600000, -0.00000), AL::ALValue::array(3, 0.933333, 0.00000));
	times[6][2] = 5.60000;
	keys[6][2] = AL::ALValue::array(-0.725624, AL::ALValue::array(3, -0.933333, -0.0190916), AL::ALValue::array(3, 0.300000, 0.00613659));
	times[6][3] = 6.50000;
	keys[6][3] = AL::ALValue::array(-0.719487, AL::ALValue::array(3, -0.300000, -0.00000), AL::ALValue::array(3, 0.300000, 0.00000));
	times[6][4] = 7.40000;
	keys[6][4] = AL::ALValue::array(-0.722556, AL::ALValue::array(3, -0.300000, -0.00000), AL::ALValue::array(3, 1.03333, 0.00000));
	times[6][5] = 10.5000;
	keys[6][5] = AL::ALValue::array(-0.711819, AL::ALValue::array(3, -1.03333, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));

	names.push_back("LHand");
	times[7].arraySetSize(6);
	keys[7].arraySetSize(6);

	times[7][0] = 1.00000;
	keys[7][0] = AL::ALValue::array(0.000876298, AL::ALValue::array(3, -0.333333, -0.00000), AL::ALValue::array(3, 0.600000, 0.00000));
	times[7][1] = 2.80000;
	keys[7][1] = AL::ALValue::array(0.00133325, AL::ALValue::array(3, -0.600000, -0.000171360), AL::ALValue::array(3, 0.933333, 0.000266560));
	times[7][2] = 5.60000;
	keys[7][2] = AL::ALValue::array(0.00219006, AL::ALValue::array(3, -0.933333, -0.000342605), AL::ALValue::array(3, 0.300000, 0.000110123));
	times[7][3] = 6.50000;
	keys[7][3] = AL::ALValue::array(0.00269144, AL::ALValue::array(3, -0.300000, -0.000118470), AL::ALValue::array(3, 0.300000, 0.000118470));
	times[7][4] = 7.40000;
	keys[7][4] = AL::ALValue::array(0.00290088, AL::ALValue::array(3, -0.300000, -0.000136134), AL::ALValue::array(3, 1.03333, 0.000468907));
	times[7][5] = 10.5000;
	keys[7][5] = AL::ALValue::array(0.00450656, AL::ALValue::array(3, -1.03333, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));

	names.push_back("RShoulderPitch");
	times[8].arraySetSize(12);
	keys[8].arraySetSize(12);

	times[8][0] = 1.00000;
	keys[8][0] = AL::ALValue::array(2.08560, AL::ALValue::array(3, -0.333333, -0.00000), AL::ALValue::array(3, 0.333333, 0.00000));
	times[8][1] = 2.00000;
	keys[8][1] = AL::ALValue::array(2.09440, AL::ALValue::array(3, -0.333333, -0.00000), AL::ALValue::array(3, 0.266667, 0.00000));
	times[8][2] = 2.80000;
	keys[8][2] = AL::ALValue::array(1.95283, AL::ALValue::array(3, -0.266667, -0.00000), AL::ALValue::array(3, 0.266667, 0.00000));
	times[8][3] = 3.60000;
	keys[8][3] = AL::ALValue::array(2.08560, AL::ALValue::array(3, -0.266667, -0.00000), AL::ALValue::array(3, 0.233333, 0.00000));
	times[8][4] = 4.30000;
	keys[8][4] = AL::ALValue::array(2.08560, AL::ALValue::array(3, -0.233333, -0.00000), AL::ALValue::array(3, 0.166667, 0.00000));
	times[8][5] = 4.80000;
	keys[8][5] = AL::ALValue::array(2.08560, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.266667, 0.00000));
	times[8][6] = 5.60000;
	keys[8][6] = AL::ALValue::array(2.07708, AL::ALValue::array(3, -0.266667, 0.00759185), AL::ALValue::array(3, 0.300000, -0.00854083));
	times[8][7] = 6.50000;
	keys[8][7] = AL::ALValue::array(2.03720, AL::ALValue::array(3, -0.300000, 0.0398808), AL::ALValue::array(3, 0.300000, -0.0398808));
	times[8][8] = 7.40000;
	keys[8][8] = AL::ALValue::array(1.44047, AL::ALValue::array(3, -0.300000, 0.229179), AL::ALValue::array(3, 0.200000, -0.152786));
	times[8][9] = 8.00000;
	keys[8][9] = AL::ALValue::array(0.891306, AL::ALValue::array(3, -0.200000, -0.00000), AL::ALValue::array(3, 0.200000, 0.00000));
	times[8][10] = 8.60000;
	keys[8][10] = AL::ALValue::array(0.891306, AL::ALValue::array(3, -0.200000, -0.00000), AL::ALValue::array(3, 0.633333, 0.00000));
	times[8][11] = 10.5000;
	keys[8][11] = AL::ALValue::array(1.46961, AL::ALValue::array(3, -0.633333, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));

	names.push_back("RShoulderRoll");
	times[9].arraySetSize(12);
	keys[9].arraySetSize(12);

	times[9][0] = 1.00000;
	keys[9][0] = AL::ALValue::array(-1.07384, AL::ALValue::array(3, -0.333333, -0.00000), AL::ALValue::array(3, 0.333333, 0.00000));
	times[9][1] = 2.00000;
	keys[9][1] = AL::ALValue::array(-0.366519, AL::ALValue::array(3, -0.333333, -0.0218166), AL::ALValue::array(3, 0.266667, 0.0174533));
	times[9][2] = 2.80000;
	keys[9][2] = AL::ALValue::array(-0.349066, AL::ALValue::array(3, -0.266667, -0.00000), AL::ALValue::array(3, 0.266667, 0.00000));
	times[9][3] = 3.60000;
	keys[9][3] = AL::ALValue::array(-0.349066, AL::ALValue::array(3, -0.266667, -0.00000), AL::ALValue::array(3, 0.233333, 0.00000));
	times[9][4] = 4.30000;
	keys[9][4] = AL::ALValue::array(-0.366519, AL::ALValue::array(3, -0.233333, -0.00000), AL::ALValue::array(3, 0.166667, 0.00000));
	times[9][5] = 4.80000;
	keys[9][5] = AL::ALValue::array(-0.00869999, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.266667, 0.00000));
	times[9][6] = 5.60000;
	keys[9][6] = AL::ALValue::array(-0.457173, AL::ALValue::array(3, -0.266667, 0.0871929), AL::ALValue::array(3, 0.300000, -0.0980920));
	times[9][7] = 6.50000;
	keys[9][7] = AL::ALValue::array(-0.564555, AL::ALValue::array(3, -0.300000, 0.0414181), AL::ALValue::array(3, 0.300000, -0.0414181));
	times[9][8] = 7.40000;
	keys[9][8] = AL::ALValue::array(-0.705682, AL::ALValue::array(3, -0.300000, 0.0616220), AL::ALValue::array(3, 0.200000, -0.0410813));
	times[9][9] = 8.00000;
	keys[9][9] = AL::ALValue::array(-0.872665, AL::ALValue::array(3, -0.200000, -0.00000), AL::ALValue::array(3, 0.200000, 0.00000));
	times[9][10] = 8.60000;
	keys[9][10] = AL::ALValue::array(-0.680678, AL::ALValue::array(3, -0.200000, -0.0577833), AL::ALValue::array(3, 0.633333, 0.182980));
	times[9][11] = 10.5000;
	keys[9][11] = AL::ALValue::array(-0.150374, AL::ALValue::array(3, -0.633333, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));

	names.push_back("RElbowYaw");
	times[10].arraySetSize(12);
	keys[10].arraySetSize(12);

	times[10][0] = 1.00000;
	keys[10][0] = AL::ALValue::array(0.765425, AL::ALValue::array(3, -0.333333, -0.00000), AL::ALValue::array(3, 0.333333, 0.00000));
	times[10][1] = 2.00000;
	keys[10][1] = AL::ALValue::array(-0.157080, AL::ALValue::array(3, -0.333333, 0.125843), AL::ALValue::array(3, 0.266667, -0.100674));
	times[10][2] = 2.80000;
	keys[10][2] = AL::ALValue::array(-0.257754, AL::ALValue::array(3, -0.266667, -0.00000), AL::ALValue::array(3, 0.266667, 0.00000));
	times[10][3] = 3.60000;
	keys[10][3] = AL::ALValue::array(-0.0872665, AL::ALValue::array(3, -0.266667, -0.00000), AL::ALValue::array(3, 0.233333, 0.00000));
	times[10][4] = 4.30000;
	keys[10][4] = AL::ALValue::array(-0.0872665, AL::ALValue::array(3, -0.233333, -0.00000), AL::ALValue::array(3, 0.166667, 0.00000));
	times[10][5] = 4.80000;
	keys[10][5] = AL::ALValue::array(-0.0872665, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.266667, 0.00000));
	times[10][6] = 5.60000;
	keys[10][6] = AL::ALValue::array(-0.0813440, AL::ALValue::array(3, -0.266667, -0.00285404), AL::ALValue::array(3, 0.300000, 0.00321080));
	times[10][7] = 6.50000;
	keys[10][7] = AL::ALValue::array(-0.0690719, AL::ALValue::array(3, -0.300000, -0.00920402), AL::ALValue::array(3, 0.300000, 0.00920402));
	times[10][8] = 7.40000;
	keys[10][8] = AL::ALValue::array(-0.0261199, AL::ALValue::array(3, -0.300000, -0.0139375), AL::ALValue::array(3, 0.200000, 0.00929167));
	times[10][9] = 8.00000;
	keys[10][9] = AL::ALValue::array(0.000615569, AL::ALValue::array(3, -0.200000, -0.00000), AL::ALValue::array(3, 0.200000, 0.00000));
	times[10][10] = 8.60000;
	keys[10][10] = AL::ALValue::array(0.000615569, AL::ALValue::array(3, -0.200000, -0.00000), AL::ALValue::array(3, 0.633333, 0.00000));
	times[10][11] = 10.5000;
	keys[10][11] = AL::ALValue::array(0.912689, AL::ALValue::array(3, -0.633333, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));

	names.push_back("RElbowRoll");
	times[11].arraySetSize(12);
	keys[11].arraySetSize(12);

	times[11][0] = 1.00000;
	keys[11][0] = AL::ALValue::array(0.710284, AL::ALValue::array(3, -0.333333, -0.00000), AL::ALValue::array(3, 0.333333, 0.00000));
	times[11][1] = 2.00000;
	keys[11][1] = AL::ALValue::array(0.00000, AL::ALValue::array(3, -0.333333, -0.00000), AL::ALValue::array(3, 0.266667, 0.00000));
	times[11][2] = 2.80000;
	keys[11][2] = AL::ALValue::array(1.56210, AL::ALValue::array(3, -0.266667, -0.00000), AL::ALValue::array(3, 0.266667, 0.00000));
	times[11][3] = 3.60000;
	keys[11][3] = AL::ALValue::array(1.56210, AL::ALValue::array(3, -0.266667, -0.00000), AL::ALValue::array(3, 0.233333, 0.00000));
	times[11][4] = 4.30000;
	keys[11][4] = AL::ALValue::array(0.698132, AL::ALValue::array(3, -0.233333, 0.302050), AL::ALValue::array(3, 0.166667, -0.215750));
	times[11][5] = 4.80000;
	keys[11][5] = AL::ALValue::array(0.00869999, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.266667, 0.00000));
	times[11][6] = 5.60000;
	keys[11][6] = AL::ALValue::array(0.0966839, AL::ALValue::array(3, -0.266667, -0.00000), AL::ALValue::array(3, 0.300000, 0.00000));
	times[11][7] = 6.50000;
	keys[11][7] = AL::ALValue::array(0.0215180, AL::ALValue::array(3, -0.300000, -0.00000), AL::ALValue::array(3, 0.300000, 0.00000));
	times[11][8] = 7.40000;
	keys[11][8] = AL::ALValue::array(0.190258, AL::ALValue::array(3, -0.300000, -0.0864535), AL::ALValue::array(3, 0.200000, 0.0576357));
	times[11][9] = 8.00000;
	keys[11][9] = AL::ALValue::array(0.453786, AL::ALValue::array(3, -0.200000, -0.0616156), AL::ALValue::array(3, 0.200000, 0.0616156));
	times[11][10] = 8.60000;
	keys[11][10] = AL::ALValue::array(0.559952, AL::ALValue::array(3, -0.200000, -0.00000), AL::ALValue::array(3, 0.633333, 0.00000));
	times[11][11] = 10.5000;
	keys[11][11] = AL::ALValue::array(0.380475, AL::ALValue::array(3, -0.633333, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));

	names.push_back("RWristYaw");
	times[12].arraySetSize(6);
	keys[12].arraySetSize(6);

	times[12][0] = 1.00000;
	keys[12][0] = AL::ALValue::array(1.15199, AL::ALValue::array(3, -0.333333, -0.00000), AL::ALValue::array(3, 0.600000, 0.00000));
	times[12][1] = 2.80000;
	keys[12][1] = AL::ALValue::array(1.72571, AL::ALValue::array(3, -0.600000, -0.00000), AL::ALValue::array(3, 0.933333, 0.00000));
	times[12][2] = 5.60000;
	keys[12][2] = AL::ALValue::array(1.07989, AL::ALValue::array(3, -0.933333, 0.00477301), AL::ALValue::array(3, 0.300000, -0.00153418));
	times[12][3] = 6.50000;
	keys[12][3] = AL::ALValue::array(1.07836, AL::ALValue::array(3, -0.300000, -0.00000), AL::ALValue::array(3, 0.300000, 0.00000));
	times[12][4] = 7.40000;
	keys[12][4] = AL::ALValue::array(1.07989, AL::ALValue::array(3, -0.300000, -0.000460114), AL::ALValue::array(3, 1.03333, 0.00158484));
	times[12][5] = 10.5000;
	keys[12][5] = AL::ALValue::array(1.08450, AL::ALValue::array(3, -1.03333, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));

	names.push_back("RHand");
	times[13].arraySetSize(8);
	keys[13].arraySetSize(8);

	times[13][0] = 1.00000;
	keys[13][0] = AL::ALValue::array(0.00000, AL::ALValue::array(3, -0.333333, -0.00000), AL::ALValue::array(3, 0.333333, 0.00000));
	times[13][1] = 2.00000;
	keys[13][1] = AL::ALValue::array(0.00000, AL::ALValue::array(3, -0.333333, -0.00000), AL::ALValue::array(3, 0.266667, 0.00000));
	times[13][2] = 2.80000;
	keys[13][2] = AL::ALValue::array(0.000317793, AL::ALValue::array(3, -0.266667, -0.00000), AL::ALValue::array(3, 0.500000, 0.00000));
	times[13][3] = 4.30000;
	keys[13][3] = AL::ALValue::array(0.00000, AL::ALValue::array(3, -0.500000, -0.00000), AL::ALValue::array(3, 0.433333, 0.00000));
	times[13][4] = 5.60000;
	keys[13][4] = AL::ALValue::array(0.00286915, AL::ALValue::array(3, -0.433333, -0.000183330), AL::ALValue::array(3, 0.300000, 0.000126920));
	times[13][5] = 6.50000;
	keys[13][5] = AL::ALValue::array(0.00299607, AL::ALValue::array(3, -0.300000, -3.59625e-05), AL::ALValue::array(3, 0.300000, 3.59625e-05));
	times[13][6] = 7.40000;
	keys[13][6] = AL::ALValue::array(0.00308492, AL::ALValue::array(3, -0.300000, -5.71194e-05), AL::ALValue::array(3, 1.03333, 0.000196745));
	times[13][7] = 10.5000;
	keys[13][7] = AL::ALValue::array(0.00375766, AL::ALValue::array(3, -1.03333, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));

	names.push_back("LHipYawPitch");
	times[14].arraySetSize(11);
	keys[14].arraySetSize(11);

	times[14][0] = 1.00000;
	keys[14][0] = AL::ALValue::array(-0.470897, AL::ALValue::array(3, -0.333333, -0.00000), AL::ALValue::array(3, 0.333333, 0.00000));
	times[14][1] = 2.00000;
	keys[14][1] = AL::ALValue::array(0.00000, AL::ALValue::array(3, -0.333333, -0.00963996), AL::ALValue::array(3, 0.266667, 0.00771196));
	times[14][2] = 2.80000;
	keys[14][2] = AL::ALValue::array(0.00771196, AL::ALValue::array(3, -0.266667, -0.00000), AL::ALValue::array(3, 0.266667, 0.00000));
	times[14][3] = 3.60000;
	keys[14][3] = AL::ALValue::array(-4.76838e-07, AL::ALValue::array(3, -0.266667, 0.00771244), AL::ALValue::array(3, 0.233333, -0.00674839));
	times[14][4] = 4.30000;
	keys[14][4] = AL::ALValue::array(-0.654977, AL::ALValue::array(3, -0.233333, -0.00000), AL::ALValue::array(3, 0.433333, 0.00000));
	times[14][5] = 5.60000;
	keys[14][5] = AL::ALValue::array(-0.498508, AL::ALValue::array(3, -0.433333, -0.00000), AL::ALValue::array(3, 0.300000, 0.00000));
	times[14][6] = 6.50000;
	keys[14][6] = AL::ALValue::array(-0.858999, AL::ALValue::array(3, -0.300000, -0.00000), AL::ALValue::array(3, 0.300000, 0.00000));
	times[14][7] = 7.40000;
	keys[14][7] = AL::ALValue::array(-0.696386, AL::ALValue::array(3, -0.300000, -0.0913488), AL::ALValue::array(3, 0.200000, 0.0608992));
	times[14][8] = 8.00000;
	keys[14][8] = AL::ALValue::array(-0.402255, AL::ALValue::array(3, -0.200000, -0.00000), AL::ALValue::array(3, 0.200000, 0.00000));
	times[14][9] = 8.60000;
	keys[14][9] = AL::ALValue::array(-0.402255, AL::ALValue::array(3, -0.200000, -0.00000), AL::ALValue::array(3, 0.633333, 0.00000));
	times[14][10] = 10.5000;
	keys[14][10] = AL::ALValue::array(-0.230059, AL::ALValue::array(3, -0.633333, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));

	names.push_back("LHipRoll");
	times[15].arraySetSize(12);
	keys[15].arraySetSize(12);

	times[15][0] = 1.00000;
	keys[15][0] = AL::ALValue::array(0.0445279, AL::ALValue::array(3, -0.333333, -0.00000), AL::ALValue::array(3, 0.333333, 0.00000));
	times[15][1] = 2.00000;
	keys[15][1] = AL::ALValue::array(1.56923e-07, AL::ALValue::array(3, -0.333333, -0.00000), AL::ALValue::array(3, 0.266667, 0.00000));
	times[15][2] = 2.80000;
	keys[15][2] = AL::ALValue::array(0.0583340, AL::ALValue::array(3, -0.266667, -0.00000), AL::ALValue::array(3, 0.266667, 0.00000));
	times[15][3] = 3.60000;
	keys[15][3] = AL::ALValue::array(1.56923e-07, AL::ALValue::array(3, -0.266667, 0.0218873), AL::ALValue::array(3, 0.233333, -0.0191514));
	times[15][4] = 4.30000;
	keys[15][4] = AL::ALValue::array(-0.0647821, AL::ALValue::array(3, -0.233333, -0.00000), AL::ALValue::array(3, 0.166667, 0.00000));
	times[15][5] = 4.80000;
	keys[15][5] = AL::ALValue::array(0.541052, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.266667, 0.00000));
	times[15][6] = 5.60000;
	keys[15][6] = AL::ALValue::array(0.154976, AL::ALValue::array(3, -0.266667, 0.112296), AL::ALValue::array(3, 0.300000, -0.126333));
	times[15][7] = 6.50000;
	keys[15][7] = AL::ALValue::array(-0.174835, AL::ALValue::array(3, -0.300000, -0.00000), AL::ALValue::array(3, 0.300000, 0.00000));
	times[15][8] = 7.40000;
	keys[15][8] = AL::ALValue::array(0.00924597, AL::ALValue::array(3, -0.300000, -0.0733642), AL::ALValue::array(3, 0.200000, 0.0489095));
	times[15][9] = 8.00000;
	keys[15][9] = AL::ALValue::array(0.191986, AL::ALValue::array(3, -0.200000, -0.00000), AL::ALValue::array(3, 0.200000, 0.00000));
	times[15][10] = 8.60000;
	keys[15][10] = AL::ALValue::array(0.174533, AL::ALValue::array(3, -0.200000, 0.00320627), AL::ALValue::array(3, 0.633333, -0.0101532));
	times[15][11] = 10.5000;
	keys[15][11] = AL::ALValue::array(0.151908, AL::ALValue::array(3, -0.633333, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));

	names.push_back("LHipPitch");
	times[16].arraySetSize(12);
	keys[16].arraySetSize(12);

	times[16][0] = 1.00000;
	keys[16][0] = AL::ALValue::array(0.250085, AL::ALValue::array(3, -0.333333, -0.00000), AL::ALValue::array(3, 0.333333, 0.00000));
	times[16][1] = 2.00000;
	keys[16][1] = AL::ALValue::array(-0.174533, AL::ALValue::array(3, -0.333333, -0.00000), AL::ALValue::array(3, 0.266667, 0.00000));
	times[16][2] = 2.80000;
	keys[16][2] = AL::ALValue::array(0.374338, AL::ALValue::array(3, -0.266667, -0.00000), AL::ALValue::array(3, 0.266667, 0.00000));
	times[16][3] = 3.60000;
	keys[16][3] = AL::ALValue::array(-0.174533, AL::ALValue::array(3, -0.266667, 0.275986), AL::ALValue::array(3, 0.233333, -0.241488));
	times[16][4] = 4.30000;
	keys[16][4] = AL::ALValue::array(-1.17808, AL::ALValue::array(3, -0.233333, 0.271496), AL::ALValue::array(3, 0.166667, -0.193925));
	times[16][5] = 4.80000;
	keys[16][5] = AL::ALValue::array(-1.57080, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.266667, 0.00000));
	times[16][6] = 5.60000;
	keys[16][6] = AL::ALValue::array(-0.857465, AL::ALValue::array(3, -0.266667, -0.237503), AL::ALValue::array(3, 0.300000, 0.267191));
	times[16][7] = 6.50000;
	keys[16][7] = AL::ALValue::array(-0.0567160, AL::ALValue::array(3, -0.300000, -0.00000), AL::ALValue::array(3, 0.300000, 0.00000));
	times[16][8] = 7.40000;
	keys[16][8] = AL::ALValue::array(-0.455555, AL::ALValue::array(3, -0.300000, 0.159699), AL::ALValue::array(3, 0.200000, -0.106466));
	times[16][9] = 8.00000;
	keys[16][9] = AL::ALValue::array(-0.855211, AL::ALValue::array(3, -0.200000, -0.00000), AL::ALValue::array(3, 0.200000, 0.00000));
	times[16][10] = 8.60000;
	keys[16][10] = AL::ALValue::array(-0.835988, AL::ALValue::array(3, -0.200000, -0.0192230), AL::ALValue::array(3, 0.633333, 0.0608729));
	times[16][11] = 10.5000;
	keys[16][11] = AL::ALValue::array(0.213269, AL::ALValue::array(3, -0.633333, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));

	names.push_back("LKneePitch");
	times[17].arraySetSize(12);
	keys[17].arraySetSize(12);

	times[17][0] = 1.00000;
	keys[17][0] = AL::ALValue::array(0.0919980, AL::ALValue::array(3, -0.333333, -0.00000), AL::ALValue::array(3, 0.333333, 0.00000));
	times[17][1] = 2.00000;
	keys[17][1] = AL::ALValue::array(1.67552, AL::ALValue::array(3, -0.333333, -0.357081), AL::ALValue::array(3, 0.266667, 0.285665));
	times[17][2] = 2.80000;
	keys[17][2] = AL::ALValue::array(2.02024, AL::ALValue::array(3, -0.266667, -0.00000), AL::ALValue::array(3, 0.266667, 0.00000));
	times[17][3] = 3.60000;
	keys[17][3] = AL::ALValue::array(1.67552, AL::ALValue::array(3, -0.266667, 0.167550), AL::ALValue::array(3, 0.233333, -0.146606));
	times[17][4] = 4.30000;
	keys[17][4] = AL::ALValue::array(1.07777, AL::ALValue::array(3, -0.233333, -0.00000), AL::ALValue::array(3, 0.166667, 0.00000));
	times[17][5] = 4.80000;
	keys[17][5] = AL::ALValue::array(1.67552, AL::ALValue::array(3, -0.166667, -0.132434), AL::ALValue::array(3, 0.266667, 0.211894));
	times[17][6] = 5.60000;
	keys[17][6] = AL::ALValue::array(2.11075, AL::ALValue::array(3, -0.266667, -0.00134981), AL::ALValue::array(3, 0.300000, 0.00151853));
	times[17][7] = 6.50000;
	keys[17][7] = AL::ALValue::array(2.11227, AL::ALValue::array(3, -0.300000, -0.000261789), AL::ALValue::array(3, 0.300000, 0.000261789));
	times[17][8] = 7.40000;
	keys[17][8] = AL::ALValue::array(2.11253, AL::ALValue::array(3, -0.300000, -0.00000), AL::ALValue::array(3, 0.200000, 0.00000));
	times[17][9] = 8.00000;
	keys[17][9] = AL::ALValue::array(2.11253, AL::ALValue::array(3, -0.200000, -0.00000), AL::ALValue::array(3, 0.200000, 0.00000));
	times[17][10] = 8.60000;
	keys[17][10] = AL::ALValue::array(2.11253, AL::ALValue::array(3, -0.200000, -0.00000), AL::ALValue::array(3, 0.633333, 0.00000));
	times[17][11] = 10.5000;
	keys[17][11] = AL::ALValue::array(-0.0890139, AL::ALValue::array(3, -0.633333, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));

	names.push_back("LAnklePitch");
	times[18].arraySetSize(12);
	keys[18].arraySetSize(12);

	times[18][0] = 1.00000;
	keys[18][0] = AL::ALValue::array(0.825249, AL::ALValue::array(3, -0.333333, -0.00000), AL::ALValue::array(3, 0.333333, 0.00000));
	times[18][1] = 2.00000;
	keys[18][1] = AL::ALValue::array(0.244346, AL::ALValue::array(3, -0.333333, 0.236918), AL::ALValue::array(3, 0.266667, -0.189534));
	times[18][2] = 2.80000;
	keys[18][2] = AL::ALValue::array(-0.454107, AL::ALValue::array(3, -0.266667, -0.00000), AL::ALValue::array(3, 0.266667, 0.00000));
	times[18][3] = 3.60000;
	keys[18][3] = AL::ALValue::array(0.244346, AL::ALValue::array(3, -0.266667, -0.201170), AL::ALValue::array(3, 0.233333, 0.176024));
	times[18][4] = 4.30000;
	keys[18][4] = AL::ALValue::array(0.677476, AL::ALValue::array(3, -0.233333, -0.00000), AL::ALValue::array(3, 0.166667, 0.00000));
	times[18][5] = 4.80000;
	keys[18][5] = AL::ALValue::array(0.663225, AL::ALValue::array(3, -0.166667, 0.0142506), AL::ALValue::array(3, 0.266667, -0.0228010));
	times[18][6] = 5.60000;
	keys[18][6] = AL::ALValue::array(-0.455639, AL::ALValue::array(3, -0.266667, 0.276813), AL::ALValue::array(3, 0.300000, -0.311414));
	times[18][7] = 6.50000;
	keys[18][7] = AL::ALValue::array(-1.10145, AL::ALValue::array(3, -0.300000, 0.0880605), AL::ALValue::array(3, 0.300000, -0.0880605));
	times[18][8] = 7.40000;
	keys[18][8] = AL::ALValue::array(-1.18952, AL::ALValue::array(3, -0.300000, -0.00000), AL::ALValue::array(3, 0.200000, 0.00000));
	times[18][9] = 8.00000;
	keys[18][9] = AL::ALValue::array(-1.18952, AL::ALValue::array(3, -0.200000, -0.00000), AL::ALValue::array(3, 0.200000, 0.00000));
	times[18][10] = 8.60000;
	keys[18][10] = AL::ALValue::array(-1.18952, AL::ALValue::array(3, -0.200000, -0.00000), AL::ALValue::array(3, 0.633333, 0.00000));
	times[18][11] = 10.5000;
	keys[18][11] = AL::ALValue::array(0.0812601, AL::ALValue::array(3, -0.633333, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));

	names.push_back("LAnkleRoll");
	times[19].arraySetSize(12);
	keys[19].arraySetSize(12);

	times[19][0] = 1.00000;
	keys[19][0] = AL::ALValue::array(-0.0337060, AL::ALValue::array(3, -0.333333, -0.00000), AL::ALValue::array(3, 0.333333, 0.00000));
	times[19][1] = 2.00000;
	keys[19][1] = AL::ALValue::array(0.00000, AL::ALValue::array(3, -0.333333, -0.00000), AL::ALValue::array(3, 0.266667, 0.00000));
	times[19][2] = 2.80000;
	keys[19][2] = AL::ALValue::array(-0.0337060, AL::ALValue::array(3, -0.266667, -0.00000), AL::ALValue::array(3, 0.266667, 0.00000));
	times[19][3] = 3.60000;
	keys[19][3] = AL::ALValue::array(0.00000, AL::ALValue::array(3, -0.266667, -0.0337060), AL::ALValue::array(3, 0.233333, 0.0294927));
	times[19][4] = 4.30000;
	keys[19][4] = AL::ALValue::array(0.171085, AL::ALValue::array(3, -0.233333, -0.00000), AL::ALValue::array(3, 0.166667, 0.00000));
	times[19][5] = 4.80000;
	keys[19][5] = AL::ALValue::array(-0.104720, AL::ALValue::array(3, -0.166667, 0.0726687), AL::ALValue::array(3, 0.266667, -0.116270));
	times[19][6] = 5.60000;
	keys[19][6] = AL::ALValue::array(-0.395731, AL::ALValue::array(3, -0.266667, -0.00000), AL::ALValue::array(3, 0.300000, 0.00000));
	times[19][7] = 6.50000;
	keys[19][7] = AL::ALValue::array(-0.0996681, AL::ALValue::array(3, -0.300000, -0.0731208), AL::ALValue::array(3, 0.300000, 0.0731208));
	times[19][8] = 7.40000;
	keys[19][8] = AL::ALValue::array(0.0429939, AL::ALValue::array(3, -0.300000, -0.0435546), AL::ALValue::array(3, 0.200000, 0.0290364));
	times[19][9] = 8.00000;
	keys[19][9] = AL::ALValue::array(0.118105, AL::ALValue::array(3, -0.200000, -0.00000), AL::ALValue::array(3, 0.200000, 0.00000));
	times[19][10] = 8.60000;
	keys[19][10] = AL::ALValue::array(0.0872665, AL::ALValue::array(3, -0.200000, 0.0203671), AL::ALValue::array(3, 0.633333, -0.0644959));
	times[19][11] = 10.5000;
	keys[19][11] = AL::ALValue::array(-0.136484, AL::ALValue::array(3, -0.633333, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));

	names.push_back("RHipRoll");
	times[20].arraySetSize(12);
	keys[20].arraySetSize(12);

	times[20][0] = 1.00000;
	keys[20][0] = AL::ALValue::array(-0.148756, AL::ALValue::array(3, -0.333333, -0.00000), AL::ALValue::array(3, 0.333333, 0.00000));
	times[20][1] = 2.00000;
	keys[20][1] = AL::ALValue::array(0.00000, AL::ALValue::array(3, -0.333333, -0.00000), AL::ALValue::array(3, 0.266667, 0.00000));
	times[20][2] = 2.80000;
	keys[20][2] = AL::ALValue::array(-0.0229680, AL::ALValue::array(3, -0.266667, -0.00000), AL::ALValue::array(3, 0.266667, 0.00000));
	times[20][3] = 3.60000;
	keys[20][3] = AL::ALValue::array(0.00000, AL::ALValue::array(3, -0.266667, -0.0150868), AL::ALValue::array(3, 0.233333, 0.0132010));
	times[20][4] = 4.30000;
	keys[20][4] = AL::ALValue::array(0.0618955, AL::ALValue::array(3, -0.233333, -0.00000), AL::ALValue::array(3, 0.166667, 0.00000));
	times[20][5] = 4.80000;
	keys[20][5] = AL::ALValue::array(-0.541052, AL::ALValue::array(3, -0.166667, 0.0108014), AL::ALValue::array(3, 0.266667, -0.0172823));
	times[20][6] = 5.60000;
	keys[20][6] = AL::ALValue::array(-0.558334, AL::ALValue::array(3, -0.266667, 0.0108922), AL::ALValue::array(3, 0.300000, -0.0122538));
	times[20][7] = 6.50000;
	keys[20][7] = AL::ALValue::array(-0.610490, AL::ALValue::array(3, -0.300000, 0.0113732), AL::ALValue::array(3, 0.300000, -0.0113732));
	times[20][8] = 7.40000;
	keys[20][8] = AL::ALValue::array(-0.626573, AL::ALValue::array(3, -0.300000, -0.00000), AL::ALValue::array(3, 0.200000, 0.00000));
	times[20][9] = 8.00000;
	keys[20][9] = AL::ALValue::array(-0.296706, AL::ALValue::array(3, -0.200000, -0.101520), AL::ALValue::array(3, 0.200000, 0.101520));
	times[20][10] = 8.60000;
	keys[20][10] = AL::ALValue::array(-0.0174533, AL::ALValue::array(3, -0.200000, -0.00000), AL::ALValue::array(3, 0.633333, 0.00000));
	times[20][11] = 10.5000;
	keys[20][11] = AL::ALValue::array(-0.0628521, AL::ALValue::array(3, -0.633333, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));

	names.push_back("RHipPitch");
	times[21].arraySetSize(12);
	keys[21].arraySetSize(12);

	times[21][0] = 1.00000;
	keys[21][0] = AL::ALValue::array(0.185572, AL::ALValue::array(3, -0.333333, -0.00000), AL::ALValue::array(3, 0.333333, 0.00000));
	times[21][1] = 2.00000;
	keys[21][1] = AL::ALValue::array(-0.174533, AL::ALValue::array(3, -0.333333, -0.00000), AL::ALValue::array(3, 0.266667, 0.00000));
	times[21][2] = 2.80000;
	keys[21][2] = AL::ALValue::array(0.282215, AL::ALValue::array(3, -0.266667, -0.00000), AL::ALValue::array(3, 0.266667, 0.00000));
	times[21][3] = 3.60000;
	keys[21][3] = AL::ALValue::array(-0.174533, AL::ALValue::array(3, -0.266667, 0.257204), AL::ALValue::array(3, 0.233333, -0.225054));
	times[21][4] = 4.30000;
	keys[21][4] = AL::ALValue::array(-1.16456, AL::ALValue::array(3, -0.233333, 0.271496), AL::ALValue::array(3, 0.166667, -0.193925));
	times[21][5] = 4.80000;
	keys[21][5] = AL::ALValue::array(-1.57080, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.266667, 0.00000));
	times[21][6] = 5.60000;
	keys[21][6] = AL::ALValue::array(-1.52484, AL::ALValue::array(3, -0.266667, -0.00000), AL::ALValue::array(3, 0.300000, 0.00000));
	times[21][7] = 6.50000;
	keys[21][7] = AL::ALValue::array(-1.56012, AL::ALValue::array(3, -0.300000, -0.00000), AL::ALValue::array(3, 0.300000, 0.00000));
	times[21][8] = 7.40000;
	keys[21][8] = AL::ALValue::array(-1.02974, AL::ALValue::array(3, -0.300000, -0.130859), AL::ALValue::array(3, 0.200000, 0.0872392));
	times[21][9] = 8.00000;
	keys[21][9] = AL::ALValue::array(-0.905826, AL::ALValue::array(3, -0.200000, -0.00000), AL::ALValue::array(3, 0.200000, 0.00000));
	times[21][10] = 8.60000;
	keys[21][10] = AL::ALValue::array(-0.905826, AL::ALValue::array(3, -0.200000, -0.00000), AL::ALValue::array(3, 0.633333, 0.00000));
	times[21][11] = 10.5000;
	keys[21][11] = AL::ALValue::array(0.208583, AL::ALValue::array(3, -0.633333, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));

	names.push_back("RKneePitch");
	times[22].arraySetSize(12);
	keys[22].arraySetSize(12);

	times[22][0] = 1.00000;
	keys[22][0] = AL::ALValue::array(0.191792, AL::ALValue::array(3, -0.333333, -0.00000), AL::ALValue::array(3, 0.333333, 0.00000));
	times[22][1] = 2.00000;
	keys[22][1] = AL::ALValue::array(1.67552, AL::ALValue::array(3, -0.333333, -0.339751), AL::ALValue::array(3, 0.266667, 0.271801));
	times[22][2] = 2.80000;
	keys[22][2] = AL::ALValue::array(2.02645, AL::ALValue::array(3, -0.266667, -0.00000), AL::ALValue::array(3, 0.266667, 0.00000));
	times[22][3] = 3.60000;
	keys[22][3] = AL::ALValue::array(1.67552, AL::ALValue::array(3, -0.266667, 0.165794), AL::ALValue::array(3, 0.233333, -0.145070));
	times[22][4] = 4.30000;
	keys[22][4] = AL::ALValue::array(1.09386, AL::ALValue::array(3, -0.233333, -0.00000), AL::ALValue::array(3, 0.166667, 0.00000));
	times[22][5] = 4.80000;
	keys[22][5] = AL::ALValue::array(1.67552, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.266667, 0.00000));
	times[22][6] = 5.60000;
	keys[22][6] = AL::ALValue::array(1.22111, AL::ALValue::array(3, -0.266667, 0.0902895), AL::ALValue::array(3, 0.300000, -0.101576));
	times[22][7] = 6.50000;
	keys[22][7] = AL::ALValue::array(1.09992, AL::ALValue::array(3, -0.300000, 0.0160710), AL::ALValue::array(3, 0.300000, -0.0160710));
	times[22][8] = 7.40000;
	keys[22][8] = AL::ALValue::array(1.08385, AL::ALValue::array(3, -0.300000, 0.0160710), AL::ALValue::array(3, 0.200000, -0.0107140));
	times[22][9] = 8.00000;
	keys[22][9] = AL::ALValue::array(0.876155, AL::ALValue::array(3, -0.200000, -0.00000), AL::ALValue::array(3, 0.200000, 0.00000));
	times[22][10] = 8.60000;
	keys[22][10] = AL::ALValue::array(1.76278, AL::ALValue::array(3, -0.200000, -0.00000), AL::ALValue::array(3, 0.633333, 0.00000));
	times[22][11] = 10.5000;
	keys[22][11] = AL::ALValue::array(-0.0766580, AL::ALValue::array(3, -0.633333, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));

	names.push_back("RAnklePitch");
	times[23].arraySetSize(12);
	keys[23].arraySetSize(12);

	times[23][0] = 1.00000;
	keys[23][0] = AL::ALValue::array(0.466378, AL::ALValue::array(3, -0.333333, -0.00000), AL::ALValue::array(3, 0.333333, 0.00000));
	times[23][1] = 2.00000;
	keys[23][1] = AL::ALValue::array(0.244346, AL::ALValue::array(3, -0.333333, 0.155673), AL::ALValue::array(3, 0.266667, -0.124538));
	times[23][2] = 2.80000;
	keys[23][2] = AL::ALValue::array(-0.374254, AL::ALValue::array(3, -0.266667, -0.00000), AL::ALValue::array(3, 0.266667, 0.00000));
	times[23][3] = 3.60000;
	keys[23][3] = AL::ALValue::array(0.244346, AL::ALValue::array(3, -0.266667, -0.194066), AL::ALValue::array(3, 0.233333, 0.169808));
	times[23][4] = 4.30000;
	keys[23][4] = AL::ALValue::array(0.717365, AL::ALValue::array(3, -0.233333, -0.00000), AL::ALValue::array(3, 0.166667, 0.00000));
	times[23][5] = 4.80000;
	keys[23][5] = AL::ALValue::array(0.663225, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.266667, 0.00000));
	times[23][6] = 5.60000;
	keys[23][6] = AL::ALValue::array(0.785451, AL::ALValue::array(3, -0.266667, -0.00000), AL::ALValue::array(3, 0.300000, 0.00000));
	times[23][7] = 6.50000;
	keys[23][7] = AL::ALValue::array(0.783916, AL::ALValue::array(3, -0.300000, 0.00153411), AL::ALValue::array(3, 0.300000, -0.00153411));
	times[23][8] = 7.40000;
	keys[23][8] = AL::ALValue::array(0.441568, AL::ALValue::array(3, -0.300000, 0.0575958), AL::ALValue::array(3, 0.200000, -0.0383972));
	times[23][9] = 8.00000;
	keys[23][9] = AL::ALValue::array(0.403171, AL::ALValue::array(3, -0.200000, 0.0383972), AL::ALValue::array(3, 0.200000, -0.0383972));
	times[23][10] = 8.60000;
	keys[23][10] = AL::ALValue::array(-0.579449, AL::ALValue::array(3, -0.200000, -0.00000), AL::ALValue::array(3, 0.633333, 0.00000));
	times[23][11] = 10.5000;
	keys[23][11] = AL::ALValue::array(0.0813440, AL::ALValue::array(3, -0.633333, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));

	names.push_back("RAnkleRoll");
	times[24].arraySetSize(12);
	keys[24].arraySetSize(12);

	times[24][0] = 1.00000;
	keys[24][0] = AL::ALValue::array(0.167248, AL::ALValue::array(3, -0.333333, -0.00000), AL::ALValue::array(3, 0.333333, 0.00000));
	times[24][1] = 2.00000;
	keys[24][1] = AL::ALValue::array(8.63852e-08, AL::ALValue::array(3, -0.333333, 0.0383500), AL::ALValue::array(3, 0.266667, -0.0306800));
	times[24][2] = 2.80000;
	keys[24][2] = AL::ALValue::array(-0.0398420, AL::ALValue::array(3, -0.266667, -0.00000), AL::ALValue::array(3, 0.266667, 0.00000));
	times[24][3] = 3.60000;
	keys[24][3] = AL::ALValue::array(8.63852e-08, AL::ALValue::array(3, -0.266667, -0.00000), AL::ALValue::array(3, 0.233333, 0.00000));
	times[24][4] = 4.30000;
	keys[24][4] = AL::ALValue::array(-0.160838, AL::ALValue::array(3, -0.233333, -0.00000), AL::ALValue::array(3, 0.166667, 0.00000));
	times[24][5] = 4.80000;
	keys[24][5] = AL::ALValue::array(0.104720, AL::ALValue::array(3, -0.166667, -0.00000), AL::ALValue::array(3, 0.266667, 0.00000));
	times[24][6] = 5.60000;
	keys[24][6] = AL::ALValue::array(0.00924597, AL::ALValue::array(3, -0.266667, 0.0318202), AL::ALValue::array(3, 0.300000, -0.0357977));
	times[24][7] = 6.50000;
	keys[24][7] = AL::ALValue::array(-0.0981341, AL::ALValue::array(3, -0.300000, -0.00000), AL::ALValue::array(3, 0.300000, 0.00000));
	times[24][8] = 7.40000;
	keys[24][8] = AL::ALValue::array(0.443314, AL::ALValue::array(3, -0.300000, -0.155547), AL::ALValue::array(3, 0.200000, 0.103698));
	times[24][9] = 8.00000;
	keys[24][9] = AL::ALValue::array(0.679603, AL::ALValue::array(3, -0.200000, -0.00000), AL::ALValue::array(3, 0.200000, 0.00000));
	times[24][10] = 8.60000;
	keys[24][10] = AL::ALValue::array(0.277507, AL::ALValue::array(3, -0.200000, 0.0493334), AL::ALValue::array(3, 0.633333, -0.156222));
	times[24][11] = 10.5000;
	keys[24][11] = AL::ALValue::array(0.0629359, AL::ALValue::array(3, -0.633333, -0.00000), AL::ALValue::array(3, 0.00000, 0.00000));
  
}

int ALstandUpFront2011::ExecutePost() {
  return  motion->post.angleInterpolationBezier(names, times, keys); 
} 

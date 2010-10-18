#include "ALstandUpBack2010.h"

#include "hal/robot/generic_nao/kAlBroker.h"

namespace {
  SpecialActionRegistrar<ALstandUpBack2010>::Type temp("ALstandUpBack2010");
}

ALstandUpBack2010::ALstandUpBack2010() {
  Initialize();
  Logger::Instance().WriteMsg("SP:ALstandUpBack2010","Initialized!",Logger::ExtraInfo);
  
  try {
    motion = KAlBroker::Instance().GetBroker()->getMotionProxy();
  } catch (AL::ALError& e) {
    Logger::Instance().WriteMsg("SP:ALstandUpBack2010","Error in getting motion proxy",Logger::FatalError);
  }
  
}




void ALstandUpBack2010::Initialize() {
  
  names.arraySetSize(25);
  times.arraySetSize(25);
  keys.arraySetSize(25);
  
  names[0] = "HeadYaw";
  times[0].arraySetSize(10);
  keys[0].arraySetSize(10);
  
  times[0][0] = 1;
  keys[0][0] = AL::ALValue::array(-0.0245859, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[0][1] = 2;
  keys[0][1] = AL::ALValue::array(-4.76838e-07, AL::ALValue::array(2, -0.333333, -1.43898e-06), AL::ALValue::array(2, 0.266667, 1.15119e-06));
  times[0][2] = 2.8;
  keys[0][2] = AL::ALValue::array(6.7435e-07, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[0][3] = 3.5;
  keys[0][3] = AL::ALValue::array(3.37177e-07, AL::ALValue::array(2, -0.233333, 2.44455e-12), AL::ALValue::array(2, 0.166667, -1.7461e-12));
  times[0][4] = 4;
  keys[0][4] = AL::ALValue::array(3.37175e-07, AL::ALValue::array(2, -0.166667, 1.7461e-12), AL::ALValue::array(2, 0.266667, -2.79377e-12));
  times[0][5] = 4.8;
  keys[0][5] = AL::ALValue::array(-0.513931, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.3, 0.0));
  times[0][6] = 5.7;
  keys[0][6] = AL::ALValue::array(-0.30224, AL::ALValue::array(2, -0.3, -0.0796626), AL::ALValue::array(2, 0.333333, 0.0885141));
  times[0][7] = 6.7;
  keys[0][7] = AL::ALValue::array(-0.00940132, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.2, 0.0));
  times[0][8] = 7.3;
  keys[0][8] = AL::ALValue::array(-0.00940132, AL::ALValue::array(2, -0.2, -0.0), AL::ALValue::array(2, 0.7, 0.0));
  times[0][9] = 9.4;
  keys[0][9] = AL::ALValue::array(0.06592, AL::ALValue::array(2, -0.7, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  names[1] = "HeadPitch";
  times[1].arraySetSize(10);
  keys[1].arraySetSize(10);
  
  times[1][0] = 1;
  keys[1][0] = AL::ALValue::array(0.0628521, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[1][1] = 2;
  keys[1][1] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, 0.0628521), AL::ALValue::array(2, 0.266667, -0.0502817));
  times[1][2] = 2.8;
  keys[1][2] = AL::ALValue::array(-0.785398, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[1][3] = 3.5;
  keys[1][3] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.233333, -0.22059), AL::ALValue::array(2, 0.166667, 0.157564));
  times[1][4] = 4;
  keys[1][4] = AL::ALValue::array(0.349066, AL::ALValue::array(2, -0.166667, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
  times[1][5] = 4.8;
  keys[1][5] = AL::ALValue::array(0.171766, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.3, 0.0));
  times[1][6] = 5.7;
  keys[1][6] = AL::ALValue::array(0.345107, AL::ALValue::array(2, -0.3, -0.0303761), AL::ALValue::array(2, 0.333333, 0.0337512));
  times[1][7] = 6.7;
  keys[1][7] = AL::ALValue::array(0.378859, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.2, 0.0));
  times[1][8] = 7.3;
  keys[1][8] = AL::ALValue::array(0.378859, AL::ALValue::array(2, -0.2, -0.0), AL::ALValue::array(2, 0.7, 0.0));
  times[1][9] = 9.4;
  keys[1][9] = AL::ALValue::array(-0.039926, AL::ALValue::array(2, -0.7, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  names[2] = "LShoulderPitch";
  times[2].arraySetSize(10);
  keys[2].arraySetSize(10);
  
  times[2][0] = 1;
  keys[2][0] = AL::ALValue::array(2.0856, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[2][1] = 2;
  keys[2][1] = AL::ALValue::array(2.0944, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
  times[2][2] = 2.8;
  keys[2][2] = AL::ALValue::array(2.0944, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[2][3] = 3.5;
  keys[2][3] = AL::ALValue::array(2.0944, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.166667, 0.0));
  times[2][4] = 4;
  keys[2][4] = AL::ALValue::array(2.0944, AL::ALValue::array(2, -0.166667, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
  times[2][5] = 4.8;
  keys[2][5] = AL::ALValue::array(2.03251, AL::ALValue::array(2, -0.266667, 0.0618893), AL::ALValue::array(2, 0.3, -0.0696255));
  times[2][6] = 5.7;
  keys[2][6] = AL::ALValue::array(1.28698, AL::ALValue::array(2, -0.3, 0.205152), AL::ALValue::array(2, 0.333333, -0.227947));
  times[2][7] = 6.7;
  keys[2][7] = AL::ALValue::array(0.733209, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.2, 0.0));
  times[2][8] = 7.3;
  keys[2][8] = AL::ALValue::array(0.733209, AL::ALValue::array(2, -0.2, -0.0), AL::ALValue::array(2, 0.7, 0.0));
  times[2][9] = 9.4;
  keys[2][9] = AL::ALValue::array(1.59225, AL::ALValue::array(2, -0.7, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  names[3] = "LShoulderRoll";
  times[3].arraySetSize(10);
  keys[3].arraySetSize(10);
  
  times[3][0] = 1;
  keys[3][0] = AL::ALValue::array(0.980184, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[3][1] = 2;
  keys[3][1] = AL::ALValue::array(0.366519, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
  times[3][2] = 2.8;
  keys[3][2] = AL::ALValue::array(0.523599, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[3][3] = 3.5;
  keys[3][3] = AL::ALValue::array(0.366519, AL::ALValue::array(2, -0.233333, 0.101811), AL::ALValue::array(2, 0.166667, -0.0727221));
  times[3][4] = 4;
  keys[3][4] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.166667, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
  times[3][5] = 4.8;
  keys[3][5] = AL::ALValue::array(0.299088, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.3, 0.0));
  times[3][6] = 5.7;
  keys[3][6] = AL::ALValue::array(0.0705221, AL::ALValue::array(2, -0.3, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[3][7] = 6.7;
  keys[3][7] = AL::ALValue::array(0.498508, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.2, 0.0));
  times[3][8] = 7.3;
  keys[3][8] = AL::ALValue::array(0.498508, AL::ALValue::array(2, -0.2, -0.0), AL::ALValue::array(2, 0.7, 0.0));
  times[3][9] = 9.4;
  keys[3][9] = AL::ALValue::array(0.21932, AL::ALValue::array(2, -0.7, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  names[4] = "LElbowYaw";
  times[4].arraySetSize(10);
  keys[4].arraySetSize(10);
  
  times[4][0] = 1;
  keys[4][0] = AL::ALValue::array(-0.760906, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[4][1] = 2;
  keys[4][1] = AL::ALValue::array(0.15708, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
  times[4][2] = 2.8;
  keys[4][2] = AL::ALValue::array(0.0872665, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[4][3] = 3.5;
  keys[4][3] = AL::ALValue::array(0.0872665, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.166667, 0.0));
  times[4][4] = 4;
  keys[4][4] = AL::ALValue::array(0.0872665, AL::ALValue::array(2, -0.166667, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
  times[4][5] = 4.8;
  keys[4][5] = AL::ALValue::array(-1.96049, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.3, 0.0));
  times[4][6] = 5.7;
  keys[4][6] = AL::ALValue::array(-0.85448, AL::ALValue::array(2, -0.3, -0.322669), AL::ALValue::array(2, 0.333333, 0.358521));
  times[4][7] = 6.7;
  keys[4][7] = AL::ALValue::array(0.0830765, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.2, 0.0));
  times[4][8] = 7.3;
  keys[4][8] = AL::ALValue::array(0.0830765, AL::ALValue::array(2, -0.2, -0.0), AL::ALValue::array(2, 0.7, 0.0));
  times[4][9] = 9.4;
  keys[4][9] = AL::ALValue::array(-1.03089, AL::ALValue::array(2, -0.7, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  names[5] = "LElbowRoll";
  times[5].arraySetSize(11);
  keys[5].arraySetSize(11);
  
  times[5][0] = 1;
  keys[5][0] = AL::ALValue::array(-0.624296, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[5][1] = 2;
  keys[5][1] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
  times[5][2] = 2.8;
  keys[5][2] = AL::ALValue::array(-1.65806, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[5][3] = 3.5;
  keys[5][3] = AL::ALValue::array(-0.698132, AL::ALValue::array(2, -0.233333, -0.322401), AL::ALValue::array(2, 0.166667, 0.230287));
  times[5][4] = 4;
  keys[5][4] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.166667, -0.0), AL::ALValue::array(2, 0.133333, 0.0));
  times[5][5] = 4.4;
  keys[5][5] = AL::ALValue::array(-0.191986, AL::ALValue::array(2, -0.133333, 0.174358), AL::ALValue::array(2, 0.133333, -0.174358));
  times[5][6] = 4.8;
  keys[5][6] = AL::ALValue::array(-1.04615, AL::ALValue::array(2, -0.133333, -0.0), AL::ALValue::array(2, 0.3, 0.0));
  times[5][7] = 5.7;
  keys[5][7] = AL::ALValue::array(-0.872804, AL::ALValue::array(2, -0.3, -0.0380199), AL::ALValue::array(2, 0.333333, 0.0422443));
  times[5][8] = 6.7;
  keys[5][8] = AL::ALValue::array(-0.805354, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.2, 0.0));
  times[5][9] = 7.3;
  keys[5][9] = AL::ALValue::array(-0.805354, AL::ALValue::array(2, -0.2, -0.0), AL::ALValue::array(2, 0.7, 0.0));
  times[5][10] = 9.4;
  keys[5][10] = AL::ALValue::array(-0.681054, AL::ALValue::array(2, -0.7, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  names[6] = "LWristYaw";
  times[6].arraySetSize(4);
  keys[6].arraySetSize(4);
  
  times[6][0] = 1;
  keys[6][0] = AL::ALValue::array(-0.705682, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 1.26667, 0.0));
  times[6][1] = 4.8;
  keys[6][1] = AL::ALValue::array(-0.725624, AL::ALValue::array(2, -1.26667, -0.0), AL::ALValue::array(2, 0.3, 0.0));
  times[6][2] = 5.7;
  keys[6][2] = AL::ALValue::array(-0.719487, AL::ALValue::array(2, -0.3, -0.000900363), AL::ALValue::array(2, 1.23333, 0.00370149));
  times[6][3] = 9.4;
  keys[6][3] = AL::ALValue::array(-0.711819, AL::ALValue::array(2, -1.23333, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  names[7] = "LHand";
  times[7].arraySetSize(4);
  keys[7].arraySetSize(4);
  
  times[7][0] = 1;
  keys[7][0] = AL::ALValue::array(0.0481964, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 1.26667, 0.0));
  times[7][1] = 4.8;
  keys[7][1] = AL::ALValue::array(0.120453, AL::ALValue::array(2, -1.26667, -0.0269052), AL::ALValue::array(2, 0.3, 0.00637229));
  times[7][2] = 5.7;
  keys[7][2] = AL::ALValue::array(0.148029, AL::ALValue::array(2, -0.3, -0.00830924), AL::ALValue::array(2, 1.23333, 0.0341602));
  times[7][3] = 9.4;
  keys[7][3] = AL::ALValue::array(0.247861, AL::ALValue::array(2, -1.23333, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  names[8] = "RShoulderPitch";
  times[8].arraySetSize(10);
  keys[8].arraySetSize(10);
  
  times[8][0] = 1;
  keys[8][0] = AL::ALValue::array(2.0856, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[8][1] = 2;
  keys[8][1] = AL::ALValue::array(2.0944, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
  times[8][2] = 2.8;
  keys[8][2] = AL::ALValue::array(2.0944, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[8][3] = 3.5;
  keys[8][3] = AL::ALValue::array(2.0944, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.166667, 0.0));
  times[8][4] = 4;
  keys[8][4] = AL::ALValue::array(2.0944, AL::ALValue::array(2, -0.166667, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
  times[8][5] = 4.8;
  keys[8][5] = AL::ALValue::array(2.07708, AL::ALValue::array(2, -0.266667, 0.00897168), AL::ALValue::array(2, 0.3, -0.0100931));
  times[8][6] = 5.7;
  keys[8][6] = AL::ALValue::array(2.0372, AL::ALValue::array(2, -0.3, 0.0398808), AL::ALValue::array(2, 0.333333, -0.044312));
  times[8][7] = 6.7;
  keys[8][7] = AL::ALValue::array(0.891306, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.2, 0.0));
  times[8][8] = 7.3;
  keys[8][8] = AL::ALValue::array(0.891306, AL::ALValue::array(2, -0.2, -0.0), AL::ALValue::array(2, 0.7, 0.0));
  times[8][9] = 9.4;
  keys[8][9] = AL::ALValue::array(1.46961, AL::ALValue::array(2, -0.7, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  names[9] = "RShoulderRoll";
  times[9].arraySetSize(10);
  keys[9].arraySetSize(10);
  
  times[9][0] = 1;
  keys[9][0] = AL::ALValue::array(-1.07384, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[9][1] = 2;
  keys[9][1] = AL::ALValue::array(-0.366519, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
  times[9][2] = 2.8;
  keys[9][2] = AL::ALValue::array(-0.523599, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[9][3] = 3.5;
  keys[9][3] = AL::ALValue::array(-0.366519, AL::ALValue::array(2, -0.233333, -0.101811), AL::ALValue::array(2, 0.166667, 0.0727221));
  times[9][4] = 4;
  keys[9][4] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.166667, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
  times[9][5] = 4.8;
  keys[9][5] = AL::ALValue::array(-0.457173, AL::ALValue::array(2, -0.266667, 0.0885576), AL::ALValue::array(2, 0.3, -0.0996273));
  times[9][6] = 5.7;
  keys[9][6] = AL::ALValue::array(-0.564555, AL::ALValue::array(2, -0.3, 0.0656039), AL::ALValue::array(2, 0.333333, -0.0728932));
  times[9][7] = 6.7;
  keys[9][7] = AL::ALValue::array(-0.872665, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.2, 0.0));
  times[9][8] = 7.3;
  keys[9][8] = AL::ALValue::array(-0.680678, AL::ALValue::array(2, -0.2, -0.053503), AL::ALValue::array(2, 0.7, 0.187261));
  times[9][9] = 9.4;
  keys[9][9] = AL::ALValue::array(-0.150374, AL::ALValue::array(2, -0.7, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  names[10] = "RElbowYaw";
  times[10].arraySetSize(10);
  keys[10].arraySetSize(10);
  
  times[10][0] = 1;
  keys[10][0] = AL::ALValue::array(0.765425, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[10][1] = 2;
  keys[10][1] = AL::ALValue::array(-0.15708, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
  times[10][2] = 2.8;
  keys[10][2] = AL::ALValue::array(-0.0872665, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[10][3] = 3.5;
  keys[10][3] = AL::ALValue::array(-0.0872665, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.166667, 0.0));
  times[10][4] = 4;
  keys[10][4] = AL::ALValue::array(-0.0872665, AL::ALValue::array(2, -0.166667, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
  times[10][5] = 4.8;
  keys[10][5] = AL::ALValue::array(-0.081344, AL::ALValue::array(2, -0.266667, -0.00285404), AL::ALValue::array(2, 0.3, 0.0032108));
  times[10][6] = 5.7;
  keys[10][6] = AL::ALValue::array(-0.0690719, AL::ALValue::array(2, -0.3, -0.0122721), AL::ALValue::array(2, 0.333333, 0.0136357));
  times[10][7] = 6.7;
  keys[10][7] = AL::ALValue::array(0.000615569, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.2, 0.0));
  times[10][8] = 7.3;
  keys[10][8] = AL::ALValue::array(0.000615569, AL::ALValue::array(2, -0.2, -0.0), AL::ALValue::array(2, 0.7, 0.0));
  times[10][9] = 9.4;
  keys[10][9] = AL::ALValue::array(0.912689, AL::ALValue::array(2, -0.7, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  names[11] = "RElbowRoll";
  times[11].arraySetSize(10);
  keys[11].arraySetSize(10);
  
  times[11][0] = 1;
  keys[11][0] = AL::ALValue::array(0.710284, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[11][1] = 2;
  keys[11][1] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
  times[11][2] = 2.8;
  keys[11][2] = AL::ALValue::array(1.65806, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[11][3] = 3.5;
  keys[11][3] = AL::ALValue::array(0.698132, AL::ALValue::array(2, -0.233333, 0.322401), AL::ALValue::array(2, 0.166667, -0.230287));
  times[11][4] = 4;
  keys[11][4] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.166667, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
  times[11][5] = 4.8;
  keys[11][5] = AL::ALValue::array(0.0966839, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.3, 0.0));
  times[11][6] = 5.7;
  keys[11][6] = AL::ALValue::array(0.021518, AL::ALValue::array(2, -0.3, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[11][7] = 6.7;
  keys[11][7] = AL::ALValue::array(0.453786, AL::ALValue::array(2, -0.333333, -0.112174), AL::ALValue::array(2, 0.2, 0.0673043));
  times[11][8] = 7.3;
  keys[11][8] = AL::ALValue::array(0.559952, AL::ALValue::array(2, -0.2, -0.0), AL::ALValue::array(2, 0.7, 0.0));
  times[11][9] = 9.4;
  keys[11][9] = AL::ALValue::array(0.380475, AL::ALValue::array(2, -0.7, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  names[12] = "RWristYaw";
  times[12].arraySetSize(4);
  keys[12].arraySetSize(4);
  
  times[12][0] = 1;
  keys[12][0] = AL::ALValue::array(1.15199, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 1.26667, 0.0));
  times[12][1] = 4.8;
  keys[12][1] = AL::ALValue::array(1.07989, AL::ALValue::array(2, -1.26667, 0.00647765), AL::ALValue::array(2, 0.3, -0.00153418));
  times[12][2] = 5.7;
  keys[12][2] = AL::ALValue::array(1.07836, AL::ALValue::array(2, -0.3, -0.0), AL::ALValue::array(2, 1.23333, 0.0));
  times[12][3] = 9.4;
  keys[12][3] = AL::ALValue::array(1.0845, AL::ALValue::array(2, -1.23333, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  names[13] = "RHand";
  times[13].arraySetSize(6);
  keys[13].arraySetSize(6);
  
  times[13][0] = 1;
  keys[13][0] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[13][1] = 2;
  keys[13][1] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.5, 0.0));
  times[13][2] = 3.5;
  keys[13][2] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.5, -0.0), AL::ALValue::array(2, 0.433333, 0.0));
  times[13][3] = 4.8;
  keys[13][3] = AL::ALValue::array(0.157803, AL::ALValue::array(2, -0.433333, -0.0100841), AL::ALValue::array(2, 0.3, 0.00698131));
  times[13][4] = 5.7;
  keys[13][4] = AL::ALValue::array(0.164784, AL::ALValue::array(2, -0.3, -0.00318709), AL::ALValue::array(2, 1.23333, 0.0131025));
  times[13][5] = 9.4;
  keys[13][5] = AL::ALValue::array(0.206671, AL::ALValue::array(2, -1.23333, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  names[14] = "LHipYawPitch";
  times[14].arraySetSize(9);
  keys[14].arraySetSize(9);
  
  times[14][0] = 1;
  keys[14][0] = AL::ALValue::array(-0.470897, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[14][1] = 2;
  keys[14][1] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
  times[14][2] = 2.8;
  keys[14][2] = AL::ALValue::array(-4.76838e-07, AL::ALValue::array(2, -0.266667, 4.76838e-07), AL::ALValue::array(2, 0.233333, -4.17233e-07));
  times[14][3] = 3.5;
  keys[14][3] = AL::ALValue::array(-0.654977, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.433333, 0.0));
  times[14][4] = 4.8;
  keys[14][4] = AL::ALValue::array(-0.498508, AL::ALValue::array(2, -0.433333, -0.0), AL::ALValue::array(2, 0.3, 0.0));
  times[14][5] = 5.7;
  keys[14][5] = AL::ALValue::array(-0.858999, AL::ALValue::array(2, -0.3, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[14][6] = 6.7;
  keys[14][6] = AL::ALValue::array(-0.402255, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.2, 0.0));
  times[14][7] = 7.3;
  keys[14][7] = AL::ALValue::array(-0.402255, AL::ALValue::array(2, -0.2, -0.0), AL::ALValue::array(2, 0.7, 0.0));
  times[14][8] = 9.4;
  keys[14][8] = AL::ALValue::array(-0.230059, AL::ALValue::array(2, -0.7, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  names[15] = "LHipRoll";
  times[15].arraySetSize(10);
  keys[15].arraySetSize(10);
  
  times[15][0] = 1;
  keys[15][0] = AL::ALValue::array(0.0445279, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[15][1] = 2;
  keys[15][1] = AL::ALValue::array(1.56923e-07, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
  times[15][2] = 2.8;
  keys[15][2] = AL::ALValue::array(1.56923e-07, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[15][3] = 3.5;
  keys[15][3] = AL::ALValue::array(-0.0647821, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.166667, 0.0));
  times[15][4] = 4;
  keys[15][4] = AL::ALValue::array(0.541052, AL::ALValue::array(2, -0.166667, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
  times[15][5] = 4.8;
  keys[15][5] = AL::ALValue::array(0.154976, AL::ALValue::array(2, -0.266667, 0.112296), AL::ALValue::array(2, 0.3, -0.126333));
  times[15][6] = 5.7;
  keys[15][6] = AL::ALValue::array(-0.174835, AL::ALValue::array(2, -0.3, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[15][7] = 6.7;
  keys[15][7] = AL::ALValue::array(0.191986, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.2, 0.0));
  times[15][8] = 7.3;
  keys[15][8] = AL::ALValue::array(0.174533, AL::ALValue::array(2, -0.2, 0.00296877), AL::ALValue::array(2, 0.7, -0.0103907));
  times[15][9] = 9.4;
  keys[15][9] = AL::ALValue::array(0.151908, AL::ALValue::array(2, -0.7, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  names[16] = "LHipPitch";
  times[16].arraySetSize(10);
  keys[16].arraySetSize(10);
  
  times[16][0] = 1;
  keys[16][0] = AL::ALValue::array(0.250085, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[16][1] = 2;
  keys[16][1] = AL::ALValue::array(-0.174533, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
  times[16][2] = 2.8;
  keys[16][2] = AL::ALValue::array(-0.174533, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[16][3] = 3.5;
  keys[16][3] = AL::ALValue::array(-1.17808, AL::ALValue::array(2, -0.233333, 0.271496), AL::ALValue::array(2, 0.166667, -0.193925));
  times[16][4] = 4;
  keys[16][4] = AL::ALValue::array(-1.5708, AL::ALValue::array(2, -0.166667, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
  times[16][5] = 4.8;
  keys[16][5] = AL::ALValue::array(-0.857465, AL::ALValue::array(2, -0.266667, -0.237503), AL::ALValue::array(2, 0.3, 0.267191));
  times[16][6] = 5.7;
  keys[16][6] = AL::ALValue::array(-0.056716, AL::ALValue::array(2, -0.3, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[16][7] = 6.7;
  keys[16][7] = AL::ALValue::array(-0.855211, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.2, 0.0));
  times[16][8] = 7.3;
  keys[16][8] = AL::ALValue::array(-0.835988, AL::ALValue::array(2, -0.2, -0.019223), AL::ALValue::array(2, 0.7, 0.0672806));
  times[16][9] = 9.4;
  keys[16][9] = AL::ALValue::array(0.213269, AL::ALValue::array(2, -0.7, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  names[17] = "LKneePitch";
  times[17].arraySetSize(10);
  keys[17].arraySetSize(10);
  
  times[17][0] = 1;
  keys[17][0] = AL::ALValue::array(0.091998, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[17][1] = 2;
  keys[17][1] = AL::ALValue::array(1.67552, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
  times[17][2] = 2.8;
  keys[17][2] = AL::ALValue::array(1.67552, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[17][3] = 3.5;
  keys[17][3] = AL::ALValue::array(1.07777, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.166667, 0.0));
  times[17][4] = 4;
  keys[17][4] = AL::ALValue::array(1.67552, AL::ALValue::array(2, -0.166667, -0.132434), AL::ALValue::array(2, 0.266667, 0.211894));
  times[17][5] = 4.8;
  keys[17][5] = AL::ALValue::array(2.11075, AL::ALValue::array(2, -0.266667, -0.00134981), AL::ALValue::array(2, 0.3, 0.00151853));
  times[17][6] = 5.7;
  keys[17][6] = AL::ALValue::array(2.11227, AL::ALValue::array(2, -0.3, -0.00151853), AL::ALValue::array(2, 0.333333, 0.00168726));
  times[17][7] = 6.7;
  keys[17][7] = AL::ALValue::array(2.20585, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.2, 0.0));
  times[17][8] = 7.3;
  keys[17][8] = AL::ALValue::array(2.20585, AL::ALValue::array(2, -0.2, -0.0), AL::ALValue::array(2, 0.7, 0.0));
  times[17][9] = 9.4;
  keys[17][9] = AL::ALValue::array(-0.0890139, AL::ALValue::array(2, -0.7, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  names[18] = "LAnklePitch";
  times[18].arraySetSize(10);
  keys[18].arraySetSize(10);
  
  times[18][0] = 1;
  keys[18][0] = AL::ALValue::array(0.825249, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[18][1] = 2;
  keys[18][1] = AL::ALValue::array(0.244346, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
  times[18][2] = 2.8;
  keys[18][2] = AL::ALValue::array(0.244346, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[18][3] = 3.5;
  keys[18][3] = AL::ALValue::array(0.677476, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.166667, 0.0));
  times[18][4] = 4;
  keys[18][4] = AL::ALValue::array(0.663225, AL::ALValue::array(2, -0.166667, 0.0142506), AL::ALValue::array(2, 0.266667, -0.022801));
  times[18][5] = 4.8;
  keys[18][5] = AL::ALValue::array(-0.455639, AL::ALValue::array(2, -0.266667, 0.276813), AL::ALValue::array(2, 0.3, -0.311414));
  times[18][6] = 5.7;
  keys[18][6] = AL::ALValue::array(-1.10145, AL::ALValue::array(2, -0.3, 0.108248), AL::ALValue::array(2, 0.333333, -0.120276));
  times[18][7] = 6.7;
  keys[18][7] = AL::ALValue::array(-1.22173, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.2, 0.0));
  times[18][8] = 7.3;
  keys[18][8] = AL::ALValue::array(-1.22173, AL::ALValue::array(2, -0.2, -0.0), AL::ALValue::array(2, 0.7, 0.0));
  times[18][9] = 9.4;
  keys[18][9] = AL::ALValue::array(0.0812601, AL::ALValue::array(2, -0.7, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  names[19] = "LAnkleRoll";
  times[19].arraySetSize(10);
  keys[19].arraySetSize(10);
  
  times[19][0] = 1;
  keys[19][0] = AL::ALValue::array(-0.033706, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[19][1] = 2;
  keys[19][1] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
  times[19][2] = 2.8;
  keys[19][2] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[19][3] = 3.5;
  keys[19][3] = AL::ALValue::array(0.171085, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.166667, 0.0));
  times[19][4] = 4;
  keys[19][4] = AL::ALValue::array(-0.10472, AL::ALValue::array(2, -0.166667, 0.0726687), AL::ALValue::array(2, 0.266667, -0.11627));
  times[19][5] = 4.8;
  keys[19][5] = AL::ALValue::array(-0.395731, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.3, 0.0));
  times[19][6] = 5.7;
  keys[19][6] = AL::ALValue::array(-0.0996681, AL::ALValue::array(2, -0.3, -0.0811319), AL::ALValue::array(2, 0.333333, 0.0901466));
  times[19][7] = 6.7;
  keys[19][7] = AL::ALValue::array(0.118105, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.2, 0.0));
  times[19][8] = 7.3;
  keys[19][8] = AL::ALValue::array(0.0872665, AL::ALValue::array(2, -0.2, 0.0188584), AL::ALValue::array(2, 0.7, -0.0660045));
  times[19][9] = 9.4;
  keys[19][9] = AL::ALValue::array(-0.136484, AL::ALValue::array(2, -0.7, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  names[20] = "RHipRoll";
  times[20].arraySetSize(10);
  keys[20].arraySetSize(10);
  
  times[20][0] = 1;
  keys[20][0] = AL::ALValue::array(-0.148756, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[20][1] = 2;
  keys[20][1] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
  times[20][2] = 2.8;
  keys[20][2] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[20][3] = 3.5;
  keys[20][3] = AL::ALValue::array(0.0618955, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.166667, 0.0));
  times[20][4] = 4;
  keys[20][4] = AL::ALValue::array(-0.541052, AL::ALValue::array(2, -0.166667, 0.0108014), AL::ALValue::array(2, 0.266667, -0.0172822));
  times[20][5] = 4.8;
  keys[20][5] = AL::ALValue::array(-0.558334, AL::ALValue::array(2, -0.266667, 0.0108922), AL::ALValue::array(2, 0.3, -0.0122538));
  times[20][6] = 5.7;
  keys[20][6] = AL::ALValue::array(-0.61049, AL::ALValue::array(2, -0.3, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[20][7] = 6.7;
  keys[20][7] = AL::ALValue::array(-0.296706, AL::ALValue::array(2, -0.333333, -0.123549), AL::ALValue::array(2, 0.2, 0.0741296));
  times[20][8] = 7.3;
  keys[20][8] = AL::ALValue::array(-0.0174533, AL::ALValue::array(2, -0.2, -0.0), AL::ALValue::array(2, 0.7, 0.0));
  times[20][9] = 9.4;
  keys[20][9] = AL::ALValue::array(-0.0628521, AL::ALValue::array(2, -0.7, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  names[21] = "RHipPitch";
  times[21].arraySetSize(10);
  keys[21].arraySetSize(10);
  
  times[21][0] = 1;
  keys[21][0] = AL::ALValue::array(0.185572, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[21][1] = 2;
  keys[21][1] = AL::ALValue::array(-0.174533, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
  times[21][2] = 2.8;
  keys[21][2] = AL::ALValue::array(-0.174533, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[21][3] = 3.5;
  keys[21][3] = AL::ALValue::array(-1.16456, AL::ALValue::array(2, -0.233333, 0.271496), AL::ALValue::array(2, 0.166667, -0.193925));
  times[21][4] = 4;
  keys[21][4] = AL::ALValue::array(-1.5708, AL::ALValue::array(2, -0.166667, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
  times[21][5] = 4.8;
  keys[21][5] = AL::ALValue::array(-1.52484, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.3, 0.0));
  times[21][6] = 5.7;
  keys[21][6] = AL::ALValue::array(-1.56012, AL::ALValue::array(2, -0.3, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[21][7] = 6.7;
  keys[21][7] = AL::ALValue::array(-0.905826, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.2, 0.0));
  times[21][8] = 7.3;
  keys[21][8] = AL::ALValue::array(-0.905826, AL::ALValue::array(2, -0.2, -0.0), AL::ALValue::array(2, 0.7, 0.0));
  times[21][9] = 9.4;
  keys[21][9] = AL::ALValue::array(0.208583, AL::ALValue::array(2, -0.7, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  names[22] = "RKneePitch";
  times[22].arraySetSize(10);
  keys[22].arraySetSize(10);
  
  times[22][0] = 1;
  keys[22][0] = AL::ALValue::array(0.191792, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[22][1] = 2;
  keys[22][1] = AL::ALValue::array(1.67552, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
  times[22][2] = 2.8;
  keys[22][2] = AL::ALValue::array(1.67552, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[22][3] = 3.5;
  keys[22][3] = AL::ALValue::array(1.09386, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.166667, 0.0));
  times[22][4] = 4;
  keys[22][4] = AL::ALValue::array(1.67552, AL::ALValue::array(2, -0.166667, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
  times[22][5] = 4.8;
  keys[22][5] = AL::ALValue::array(1.22111, AL::ALValue::array(2, -0.266667, 0.0902895), AL::ALValue::array(2, 0.3, -0.101576));
  times[22][6] = 5.7;
  keys[22][6] = AL::ALValue::array(1.09992, AL::ALValue::array(2, -0.3, 0.0544659), AL::ALValue::array(2, 0.333333, -0.0605176));
  times[22][7] = 6.7;
  keys[22][7] = AL::ALValue::array(0.876155, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.2, 0.0));
  times[22][8] = 7.3;
  keys[22][8] = AL::ALValue::array(1.76278, AL::ALValue::array(2, -0.2, -0.0), AL::ALValue::array(2, 0.7, 0.0));
  times[22][9] = 9.4;
  keys[22][9] = AL::ALValue::array(-0.076658, AL::ALValue::array(2, -0.7, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  names[23] = "RAnklePitch";
  times[23].arraySetSize(10);
  keys[23].arraySetSize(10);
  
  times[23][0] = 1;
  keys[23][0] = AL::ALValue::array(0.466378, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[23][1] = 2;
  keys[23][1] = AL::ALValue::array(0.244346, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
  times[23][2] = 2.8;
  keys[23][2] = AL::ALValue::array(0.244346, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[23][3] = 3.5;
  keys[23][3] = AL::ALValue::array(0.717365, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.166667, 0.0));
  times[23][4] = 4;
  keys[23][4] = AL::ALValue::array(0.663225, AL::ALValue::array(2, -0.166667, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
  times[23][5] = 4.8;
  keys[23][5] = AL::ALValue::array(0.785451, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.3, 0.0));
  times[23][6] = 5.7;
  keys[23][6] = AL::ALValue::array(0.783916, AL::ALValue::array(2, -0.3, 0.00153411), AL::ALValue::array(2, 0.333333, -0.00170457));
  times[23][7] = 6.7;
  keys[23][7] = AL::ALValue::array(0.403171, AL::ALValue::array(2, -0.333333, 0.284035), AL::ALValue::array(2, 0.2, -0.170421));
  times[23][8] = 7.3;
  keys[23][8] = AL::ALValue::array(-0.579449, AL::ALValue::array(2, -0.2, -0.0), AL::ALValue::array(2, 0.7, 0.0));
  times[23][9] = 9.4;
  keys[23][9] = AL::ALValue::array(0.081344, AL::ALValue::array(2, -0.7, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  names[24] = "RAnkleRoll";
  times[24].arraySetSize(10);
  keys[24].arraySetSize(10);
  
  times[24][0] = 1;
  keys[24][0] = AL::ALValue::array(0.167248, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[24][1] = 2;
  keys[24][1] = AL::ALValue::array(8.63852e-08, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
  times[24][2] = 2.8;
  keys[24][2] = AL::ALValue::array(8.63852e-08, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[24][3] = 3.5;
  keys[24][3] = AL::ALValue::array(-0.160838, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.166667, 0.0));
  times[24][4] = 4;
  keys[24][4] = AL::ALValue::array(0.10472, AL::ALValue::array(2, -0.166667, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
  times[24][5] = 4.8;
  keys[24][5] = AL::ALValue::array(0.00924597, AL::ALValue::array(2, -0.266667, 0.0318202), AL::ALValue::array(2, 0.3, -0.0357977));
  times[24][6] = 5.7;
  keys[24][6] = AL::ALValue::array(-0.0981341, AL::ALValue::array(2, -0.3, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[24][7] = 6.7;
  keys[24][7] = AL::ALValue::array(0.679603, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.2, 0.0));
  times[24][8] = 7.3;
  keys[24][8] = AL::ALValue::array(0.277507, AL::ALValue::array(2, -0.2, 0.0456791), AL::ALValue::array(2, 0.7, -0.159877));
  times[24][9] = 9.4;
  keys[24][9] = AL::ALValue::array(0.0629359, AL::ALValue::array(2, -0.7, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
}

int ALstandUpBack2010::ExecutePost() {
  return motion->post.angleInterpolationBezier(names, times, keys); 
} 

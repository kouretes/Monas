#include "ALstandUpFront2010.h"

#include "hal/robot/generic_nao/kAlBroker.h"

namespace {
  SpecialActionRegistrar<ALstandUpFront2010>::Type temp("ALstandUpFront2010");
}

ALstandUpFront2010::ALstandUpFront2010() {
  Initialize();
  Logger::Instance().WriteMsg("SP:ALstandUpFront2010","Initialized!",Logger::ExtraInfo);
  
  try {
    motion = KAlBroker::Instance().GetBroker()->getMotionProxy();
  } catch (AL::ALError& e) {
    Logger::Instance().WriteMsg("SP:ALstandUpFront2010","Error in getting motion proxy",Logger::FatalError);
  }
  
}




void ALstandUpFront2010::Initialize() {
  
  names.arraySetSize(25);
  times.arraySetSize(25);
  keys.arraySetSize(25);
  
  names[0] = "HeadYaw";
  times[0].arraySetSize(4);
  keys[0].arraySetSize(4);
  
  times[0][0] = 1;
  keys[0][0] = AL::ALValue::array(-0.058334, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[0][1] = 1.7;
  keys[0][1] = AL::ALValue::array(-0.058334, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[0][2] = 2.4;
  keys[0][2] = AL::ALValue::array(-0.058334, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 2.33333, 0.0));
  times[0][3] = 9.4;
  keys[0][3] = AL::ALValue::array(0.067454, AL::ALValue::array(2, -2.33333, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  names[1] = "HeadPitch";
  times[1].arraySetSize(8);
  keys[1].arraySetSize(8);
  
  times[1][0] = 1;
  keys[1][0] = AL::ALValue::array(-0.576826, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[1][1] = 1.7;
  keys[1][1] = AL::ALValue::array(-0.592166, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[1][2] = 2.4;
  keys[1][2] = AL::ALValue::array(-0.547679, AL::ALValue::array(2, -0.233333, -0.0444867), AL::ALValue::array(2, 0.333333, 0.0635524));
  times[1][3] = 3.4;
  keys[1][3] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.433333, 0.0));
  times[1][4] = 4.7;
  keys[1][4] = AL::ALValue::array(-0.349066, AL::ALValue::array(2, -0.433333, 0.0756309), AL::ALValue::array(2, 0.5, -0.0872665));
  times[1][5] = 6.2;
  keys[1][5] = AL::ALValue::array(-0.488692, AL::ALValue::array(2, -0.5, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[1][6] = 7.2;
  keys[1][6] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.733333, 0.0));
  times[1][7] = 9.4;
  keys[1][7] = AL::ALValue::array(-0.0414599, AL::ALValue::array(2, -0.733333, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  names[2] = "LShoulderPitch";
  times[2].arraySetSize(10);
  keys[2].arraySetSize(10);
  
  times[2][0] = 1;
  keys[2][0] = AL::ALValue::array(0.084328, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[2][1] = 1.7;
  keys[2][1] = AL::ALValue::array(-0.37127, AL::ALValue::array(2, -0.233333, 0.216805), AL::ALValue::array(2, 0.233333, -0.216805));
  times[2][2] = 2.4;
  keys[2][2] = AL::ALValue::array(-1.2165, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[2][3] = 3.4;
  keys[2][3] = AL::ALValue::array(-0.872665, AL::ALValue::array(2, -0.333333, -0.15101), AL::ALValue::array(2, 0.433333, 0.196313));
  times[2][4] = 4.7;
  keys[2][4] = AL::ALValue::array(-0.174533, AL::ALValue::array(2, -0.433333, -0.189077), AL::ALValue::array(2, 0.233333, 0.101811));
  times[2][5] = 5.4;
  keys[2][5] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.233333, -0.122173), AL::ALValue::array(2, 0.266667, 0.139626));
  times[2][6] = 6.2;
  keys[2][6] = AL::ALValue::array(0.610865, AL::ALValue::array(2, -0.266667, -0.165483), AL::ALValue::array(2, 0.333333, 0.206854));
  times[2][7] = 7.2;
  keys[2][7] = AL::ALValue::array(1.11701, AL::ALValue::array(2, -0.333333, -0.153377), AL::ALValue::array(2, 0.4, 0.184053));
  times[2][8] = 8.4;
  keys[2][8] = AL::ALValue::array(1.62316, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[2][9] = 9.4;
  keys[2][9] = AL::ALValue::array(1.59378, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  names[3] = "LShoulderRoll";
  times[3].arraySetSize(10);
  keys[3].arraySetSize(10);
  
  times[3][0] = 1;
  keys[3][0] = AL::ALValue::array(1.5539, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[3][1] = 1.7;
  keys[3][1] = AL::ALValue::array(1.03541, AL::ALValue::array(2, -0.233333, 0.256178), AL::ALValue::array(2, 0.233333, -0.256178));
  times[3][2] = 2.4;
  keys[3][2] = AL::ALValue::array(0.016832, AL::ALValue::array(2, -0.233333, 0.00569243), AL::ALValue::array(2, 0.333333, -0.00813205));
  times[3][3] = 3.4;
  keys[3][3] = AL::ALValue::array(0.00869999, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.433333, 0.0));
  times[3][4] = 4.7;
  keys[3][4] = AL::ALValue::array(0.00869999, AL::ALValue::array(2, -0.433333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[3][5] = 5.4;
  keys[3][5] = AL::ALValue::array(0.00869999, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
  times[3][6] = 6.2;
  keys[3][6] = AL::ALValue::array(0.0349066, AL::ALValue::array(2, -0.266667, -0.0181037), AL::ALValue::array(2, 0.333333, 0.0226296));
  times[3][7] = 7.2;
  keys[3][7] = AL::ALValue::array(0.1309, AL::ALValue::array(2, -0.333333, -0.0211555), AL::ALValue::array(2, 0.4, 0.0253866));
  times[3][8] = 8.4;
  keys[3][8] = AL::ALValue::array(0.174533, AL::ALValue::array(2, -0.4, -0.0157975), AL::ALValue::array(2, 0.333333, 0.0131645));
  times[3][9] = 9.4;
  keys[3][9] = AL::ALValue::array(0.217786, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  names[4] = "LElbowYaw";
  times[4].arraySetSize(10);
  keys[4].arraySetSize(10);
  
  times[4][0] = 1;
  keys[4][0] = AL::ALValue::array(-0.0353239, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[4][1] = 1.7;
  keys[4][1] = AL::ALValue::array(-1.70892, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[4][2] = 2.4;
  keys[4][2] = AL::ALValue::array(-1.58006, AL::ALValue::array(2, -0.233333, -0.00648616), AL::ALValue::array(2, 0.333333, 0.00926594));
  times[4][3] = 3.4;
  keys[4][3] = AL::ALValue::array(-1.5708, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.433333, 0.0));
  times[4][4] = 4.7;
  keys[4][4] = AL::ALValue::array(-1.5708, AL::ALValue::array(2, -0.433333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[4][5] = 5.4;
  keys[4][5] = AL::ALValue::array(-1.5708, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
  times[4][6] = 6.2;
  keys[4][6] = AL::ALValue::array(-0.244346, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[4][7] = 7.2;
  keys[4][7] = AL::ALValue::array(-0.925025, AL::ALValue::array(2, -0.333333, 0.200977), AL::ALValue::array(2, 0.4, -0.241173));
  times[4][8] = 8.4;
  keys[4][8] = AL::ALValue::array(-1.5708, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[4][9] = 9.4;
  keys[4][9] = AL::ALValue::array(-1.01862, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  names[5] = "LElbowRoll";
  times[5].arraySetSize(10);
  keys[5].arraySetSize(10);
  
  times[5][0] = 1;
  keys[5][0] = AL::ALValue::array(-0.0367741, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[5][1] = 1.7;
  keys[5][1] = AL::ALValue::array(-0.84826, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[5][2] = 2.4;
  keys[5][2] = AL::ALValue::array(-0.352778, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[5][3] = 3.4;
  keys[5][3] = AL::ALValue::array(-0.610865, AL::ALValue::array(2, -0.333333, 0.175264), AL::ALValue::array(2, 0.433333, -0.227843));
  times[5][4] = 4.7;
  keys[5][4] = AL::ALValue::array(-1.5621, AL::ALValue::array(2, -0.433333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[5][5] = 5.4;
  keys[5][5] = AL::ALValue::array(-0.139626, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
  times[5][6] = 6.2;
  keys[5][6] = AL::ALValue::array(-0.715585, AL::ALValue::array(2, -0.266667, 0.170654), AL::ALValue::array(2, 0.333333, -0.213318));
  times[5][7] = 7.2;
  keys[5][7] = AL::ALValue::array(-1.29154, AL::ALValue::array(2, -0.333333, 0.0872665), AL::ALValue::array(2, 0.4, -0.10472));
  times[5][8] = 8.4;
  keys[5][8] = AL::ALValue::array(-1.39626, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[5][9] = 9.4;
  keys[5][9] = AL::ALValue::array(-0.681054, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  names[6] = "LWristYaw";
  times[6].arraySetSize(4);
  keys[6].arraySetSize(4);
  
  times[6][0] = 1;
  keys[6][0] = AL::ALValue::array(-1.55092, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[6][1] = 1.7;
  keys[6][1] = AL::ALValue::array(-1.15054, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[6][2] = 2.4;
  keys[6][2] = AL::ALValue::array(-1.55092, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 2.33333, 0.0));
  times[6][3] = 9.4;
  keys[6][3] = AL::ALValue::array(-0.710284, AL::ALValue::array(2, -2.33333, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  names[7] = "LHand";
  times[7].arraySetSize(4);
  keys[7].arraySetSize(4);
  
  times[7][0] = 1;
  keys[7][0] = AL::ALValue::array(0.0562249, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[7][1] = 1.7;
  keys[7][1] = AL::ALValue::array(0.0551777, AL::ALValue::array(2, -0.233333, 0.0010472), AL::ALValue::array(2, 0.233333, -0.0010472));
  times[7][2] = 2.4;
  keys[7][2] = AL::ALValue::array(0.0188749, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 2.33333, 0.0));
  times[7][3] = 9.4;
  keys[7][3] = AL::ALValue::array(0.250305, AL::ALValue::array(2, -2.33333, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  names[8] = "RShoulderPitch";
  times[8].arraySetSize(10);
  keys[8].arraySetSize(10);
  
  times[8][0] = 1;
  keys[8][0] = AL::ALValue::array(-0.0275701, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[8][1] = 1.7;
  keys[8][1] = AL::ALValue::array(-0.622761, AL::ALValue::array(2, -0.233333, 0.188426), AL::ALValue::array(2, 0.233333, -0.188426));
  times[8][2] = 2.4;
  keys[8][2] = AL::ALValue::array(-1.15813, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[8][3] = 3.4;
  keys[8][3] = AL::ALValue::array(-0.872665, AL::ALValue::array(2, -0.333333, -0.14255), AL::ALValue::array(2, 0.433333, 0.185315));
  times[8][4] = 4.7;
  keys[8][4] = AL::ALValue::array(-0.174533, AL::ALValue::array(2, -0.433333, -0.189077), AL::ALValue::array(2, 0.233333, 0.101811));
  times[8][5] = 5.4;
  keys[8][5] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.233333, -0.122173), AL::ALValue::array(2, 0.266667, 0.139626));
  times[8][6] = 6.2;
  keys[8][6] = AL::ALValue::array(0.610865, AL::ALValue::array(2, -0.266667, -0.165483), AL::ALValue::array(2, 0.333333, 0.206854));
  times[8][7] = 7.2;
  keys[8][7] = AL::ALValue::array(1.11701, AL::ALValue::array(2, -0.333333, -0.153377), AL::ALValue::array(2, 0.4, 0.184053));
  times[8][8] = 8.4;
  keys[8][8] = AL::ALValue::array(1.62316, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[8][9] = 9.4;
  keys[8][9] = AL::ALValue::array(1.47882, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  names[9] = "RShoulderRoll";
  times[9].arraySetSize(10);
  keys[9].arraySetSize(10);
  
  times[9][0] = 1;
  keys[9][0] = AL::ALValue::array(-1.53558, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[9][1] = 1.7;
  keys[9][1] = AL::ALValue::array(-1.07538, AL::ALValue::array(2, -0.233333, -0.251832), AL::ALValue::array(2, 0.233333, 0.251832));
  times[9][2] = 2.4;
  keys[9][2] = AL::ALValue::array(-0.0245859, AL::ALValue::array(2, -0.233333, -0.0111202), AL::ALValue::array(2, 0.333333, 0.0158859));
  times[9][3] = 3.4;
  keys[9][3] = AL::ALValue::array(-0.00869999, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.433333, 0.0));
  times[9][4] = 4.7;
  keys[9][4] = AL::ALValue::array(-0.00869999, AL::ALValue::array(2, -0.433333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[9][5] = 5.4;
  keys[9][5] = AL::ALValue::array(-0.00869999, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
  times[9][6] = 6.2;
  keys[9][6] = AL::ALValue::array(-0.0349066, AL::ALValue::array(2, -0.266667, 0.0181037), AL::ALValue::array(2, 0.333333, -0.0226296));
  times[9][7] = 7.2;
  keys[9][7] = AL::ALValue::array(-0.1309, AL::ALValue::array(2, -0.333333, 0.0211555), AL::ALValue::array(2, 0.4, -0.0253866));
  times[9][8] = 8.4;
  keys[9][8] = AL::ALValue::array(-0.174533, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[9][9] = 9.4;
  keys[9][9] = AL::ALValue::array(-0.14884, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  names[10] = "RElbowYaw";
  times[10].arraySetSize(10);
  keys[10].arraySetSize(10);
  
  times[10][0] = 1;
  keys[10][0] = AL::ALValue::array(-0.01078, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[10][1] = 1.7;
  keys[10][1] = AL::ALValue::array(1.36215, AL::ALValue::array(2, -0.233333, -0.199421), AL::ALValue::array(2, 0.233333, 0.199421));
  times[10][2] = 2.4;
  keys[10][2] = AL::ALValue::array(1.56157, AL::ALValue::array(2, -0.233333, -0.0064581), AL::ALValue::array(2, 0.333333, 0.00922586));
  times[10][3] = 3.4;
  keys[10][3] = AL::ALValue::array(1.5708, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.433333, 0.0));
  times[10][4] = 4.7;
  keys[10][4] = AL::ALValue::array(1.5708, AL::ALValue::array(2, -0.433333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[10][5] = 5.4;
  keys[10][5] = AL::ALValue::array(1.5708, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
  times[10][6] = 6.2;
  keys[10][6] = AL::ALValue::array(0.244346, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[10][7] = 7.2;
  keys[10][7] = AL::ALValue::array(0.925025, AL::ALValue::array(2, -0.333333, -0.200977), AL::ALValue::array(2, 0.4, 0.241173));
  times[10][8] = 8.4;
  keys[10][8] = AL::ALValue::array(1.5708, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[10][9] = 9.4;
  keys[10][9] = AL::ALValue::array(0.90962, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  names[11] = "RElbowRoll";
  times[11].arraySetSize(10);
  keys[11].arraySetSize(10);
  
  times[11][0] = 1;
  keys[11][0] = AL::ALValue::array(0.0353239, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[11][1] = 1.7;
  keys[11][1] = AL::ALValue::array(0.805393, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[11][2] = 2.4;
  keys[11][2] = AL::ALValue::array(0.382007, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[11][3] = 3.4;
  keys[11][3] = AL::ALValue::array(0.610865, AL::ALValue::array(2, -0.333333, -0.171028), AL::ALValue::array(2, 0.433333, 0.222336));
  times[11][4] = 4.7;
  keys[11][4] = AL::ALValue::array(1.5621, AL::ALValue::array(2, -0.433333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[11][5] = 5.4;
  keys[11][5] = AL::ALValue::array(0.139626, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
  times[11][6] = 6.2;
  keys[11][6] = AL::ALValue::array(0.715585, AL::ALValue::array(2, -0.266667, -0.170654), AL::ALValue::array(2, 0.333333, 0.213318));
  times[11][7] = 7.2;
  keys[11][7] = AL::ALValue::array(1.29154, AL::ALValue::array(2, -0.333333, -0.0872665), AL::ALValue::array(2, 0.4, 0.10472));
  times[11][8] = 8.4;
  keys[11][8] = AL::ALValue::array(1.39626, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[11][9] = 9.4;
  keys[11][9] = AL::ALValue::array(0.392746, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  names[12] = "RWristYaw";
  times[12].arraySetSize(4);
  keys[12].arraySetSize(4);
  
  times[12][0] = 1;
  keys[12][0] = AL::ALValue::array(1.50941, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[12][1] = 1.7;
  keys[12][1] = AL::ALValue::array(1.50941, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[12][2] = 2.4;
  keys[12][2] = AL::ALValue::array(1.50941, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 2.33333, 0.0));
  times[12][3] = 9.4;
  keys[12][3] = AL::ALValue::array(1.08603, AL::ALValue::array(2, -2.33333, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  names[13] = "RHand";
  times[13].arraySetSize(4);
  keys[13].arraySetSize(4);
  
  times[13][0] = 1;
  keys[13][0] = AL::ALValue::array(0.0178277, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[13][1] = 1.7;
  keys[13][1] = AL::ALValue::array(0.0174786, AL::ALValue::array(2, -0.233333, 0.000349068), AL::ALValue::array(2, 0.233333, -0.000349068));
  times[13][2] = 2.4;
  keys[13][2] = AL::ALValue::array(0.0122427, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 2.33333, 0.0));
  times[13][3] = 9.4;
  keys[13][3] = AL::ALValue::array(0.209115, AL::ALValue::array(2, -2.33333, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  names[14] = "LHipYawPitch";
  times[14].arraySetSize(10);
  keys[14].arraySetSize(10);
  
  times[14][0] = 1;
  keys[14][0] = AL::ALValue::array(-0.033706, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[14][1] = 1.7;
  keys[14][1] = AL::ALValue::array(-0.033706, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[14][2] = 2.4;
  keys[14][2] = AL::ALValue::array(4.19617e-05, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[14][3] = 3.4;
  keys[14][3] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, 4.19617e-05), AL::ALValue::array(2, 0.433333, -5.45502e-05));
  times[14][4] = 4.7;
  keys[14][4] = AL::ALValue::array(-0.872665, AL::ALValue::array(2, -0.433333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[14][5] = 5.4;
  keys[14][5] = AL::ALValue::array(-0.872665, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
  times[14][6] = 6.2;
  keys[14][6] = AL::ALValue::array(-0.965167, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[14][7] = 7.2;
  keys[14][7] = AL::ALValue::array(-0.785398, AL::ALValue::array(2, -0.333333, -0.146237), AL::ALValue::array(2, 0.4, 0.175485));
  times[14][8] = 8.4;
  keys[14][8] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[14][9] = 9.4;
  keys[14][9] = AL::ALValue::array(-0.236194, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  names[15] = "LHipRoll";
  times[15].arraySetSize(10);
  keys[15].arraySetSize(10);
  
  times[15][0] = 1;
  keys[15][0] = AL::ALValue::array(0.0629359, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[15][1] = 1.7;
  keys[15][1] = AL::ALValue::array(0.0614019, AL::ALValue::array(2, -0.233333, 0.00153397), AL::ALValue::array(2, 0.233333, -0.00153397));
  times[15][2] = 2.4;
  keys[15][2] = AL::ALValue::array(4.19617e-05, AL::ALValue::array(2, -0.233333, 2.92634e-05), AL::ALValue::array(2, 0.333333, -4.18048e-05));
  times[15][3] = 3.4;
  keys[15][3] = AL::ALValue::array(1.56923e-07, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.433333, 0.0));
  times[15][4] = 4.7;
  keys[15][4] = AL::ALValue::array(1.56923e-07, AL::ALValue::array(2, -0.433333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[15][5] = 5.4;
  keys[15][5] = AL::ALValue::array(1.56923e-07, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
  times[15][6] = 6.2;
  keys[15][6] = AL::ALValue::array(0.0872665, AL::ALValue::array(2, -0.266667, -0.0139626), AL::ALValue::array(2, 0.333333, 0.0174533));
  times[15][7] = 7.2;
  keys[15][7] = AL::ALValue::array(0.10472, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.4, 0.0));
  times[15][8] = 8.4;
  keys[15][8] = AL::ALValue::array(-0.010472, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[15][9] = 9.4;
  keys[15][9] = AL::ALValue::array(0.14884, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  names[16] = "LHipPitch";
  times[16].arraySetSize(10);
  keys[16].arraySetSize(10);
  
  times[16][0] = 1;
  keys[16][0] = AL::ALValue::array(0.0614019, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[16][1] = 1.7;
  keys[16][1] = AL::ALValue::array(0.0598679, AL::ALValue::array(2, -0.233333, 0.00153397), AL::ALValue::array(2, 0.233333, -0.00153397));
  times[16][2] = 2.4;
  keys[16][2] = AL::ALValue::array(4.19617e-05, AL::ALValue::array(2, -0.233333, 0.059826), AL::ALValue::array(2, 0.333333, -0.0854657));
  times[16][3] = 3.4;
  keys[16][3] = AL::ALValue::array(-1.5708, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.433333, 0.0));
  times[16][4] = 4.7;
  keys[16][4] = AL::ALValue::array(-1.5708, AL::ALValue::array(2, -0.433333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[16][5] = 5.4;
  keys[16][5] = AL::ALValue::array(-1.5708, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
  times[16][6] = 6.2;
  keys[16][6] = AL::ALValue::array(-1.5708, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[16][7] = 7.2;
  keys[16][7] = AL::ALValue::array(-1.06989, AL::ALValue::array(2, -0.333333, -0.0189077), AL::ALValue::array(2, 0.4, 0.0226893));
  times[16][8] = 8.4;
  keys[16][8] = AL::ALValue::array(-1.0472, AL::ALValue::array(2, -0.4, -0.0226893), AL::ALValue::array(2, 0.333333, 0.0189077));
  times[16][9] = 9.4;
  keys[16][9] = AL::ALValue::array(0.213269, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  names[17] = "LKneePitch";
  times[17].arraySetSize(10);
  keys[17].arraySetSize(10);
  
  times[17][0] = 1;
  keys[17][0] = AL::ALValue::array(-0.0706059, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[17][1] = 1.7;
  keys[17][1] = AL::ALValue::array(-0.067538, AL::ALValue::array(2, -0.233333, -0.00306794), AL::ALValue::array(2, 0.233333, 0.00306794));
  times[17][2] = 2.4;
  keys[17][2] = AL::ALValue::array(2.09234, AL::ALValue::array(2, -0.233333, -0.0014416), AL::ALValue::array(2, 0.333333, 0.00205942));
  times[17][3] = 3.4;
  keys[17][3] = AL::ALValue::array(2.0944, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.433333, 0.0));
  times[17][4] = 4.7;
  keys[17][4] = AL::ALValue::array(1.0472, AL::ALValue::array(2, -0.433333, 0.0648265), AL::ALValue::array(2, 0.233333, -0.0349066));
  times[17][5] = 5.4;
  keys[17][5] = AL::ALValue::array(1.01229, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
  times[17][6] = 6.2;
  keys[17][6] = AL::ALValue::array(2.11253, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[17][7] = 7.2;
  keys[17][7] = AL::ALValue::array(2.11253, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.4, 0.0));
  times[17][8] = 8.4;
  keys[17][8] = AL::ALValue::array(2.0944, AL::ALValue::array(2, -0.4, 0.018134), AL::ALValue::array(2, 0.333333, -0.0151117));
  times[17][9] = 9.4;
  keys[17][9] = AL::ALValue::array(-0.090548, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  names[18] = "LAnklePitch";
  times[18].arraySetSize(10);
  keys[18].arraySetSize(10);
  
  times[18][0] = 1;
  keys[18][0] = AL::ALValue::array(0.921892, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[18][1] = 1.7;
  keys[18][1] = AL::ALValue::array(0.922747, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[18][2] = 2.4;
  keys[18][2] = AL::ALValue::array(-1.1352, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[18][3] = 3.4;
  keys[18][3] = AL::ALValue::array(-1.13446, AL::ALValue::array(2, -0.333333, -0.000738228), AL::ALValue::array(2, 0.433333, 0.000959697));
  times[18][4] = 4.7;
  keys[18][4] = AL::ALValue::array(-0.783653, AL::ALValue::array(2, -0.433333, -0.264708), AL::ALValue::array(2, 0.233333, 0.142535));
  times[18][5] = 5.4;
  keys[18][5] = AL::ALValue::array(0.0872665, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
  times[18][6] = 6.2;
  keys[18][6] = AL::ALValue::array(-0.312414, AL::ALValue::array(2, -0.266667, 0.118941), AL::ALValue::array(2, 0.333333, -0.148676));
  times[18][7] = 7.2;
  keys[18][7] = AL::ALValue::array(-0.715585, AL::ALValue::array(2, -0.333333, 0.111331), AL::ALValue::array(2, 0.4, -0.133597));
  times[18][8] = 8.4;
  keys[18][8] = AL::ALValue::array(-1.0472, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[18][9] = 9.4;
  keys[18][9] = AL::ALValue::array(0.076658, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  names[19] = "LAnkleRoll";
  times[19].arraySetSize(10);
  keys[19].arraySetSize(10);
  
  times[19][0] = 1;
  keys[19][0] = AL::ALValue::array(-0.00149204, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[19][1] = 1.7;
  keys[19][1] = AL::ALValue::array(-0.00149204, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[19][2] = 2.4;
  keys[19][2] = AL::ALValue::array(4.19617e-05, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[19][3] = 3.4;
  keys[19][3] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, 4.19617e-05), AL::ALValue::array(2, 0.433333, -5.45502e-05));
  times[19][4] = 4.7;
  keys[19][4] = AL::ALValue::array(-0.680678, AL::ALValue::array(2, -0.433333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[19][5] = 5.4;
  keys[19][5] = AL::ALValue::array(-0.555015, AL::ALValue::array(2, -0.233333, -0.059729), AL::ALValue::array(2, 0.266667, 0.0682618));
  times[19][6] = 6.2;
  keys[19][6] = AL::ALValue::array(-0.296706, AL::ALValue::array(2, -0.266667, -0.0667104), AL::ALValue::array(2, 0.333333, 0.083388));
  times[19][7] = 7.2;
  keys[19][7] = AL::ALValue::array(-0.10472, AL::ALValue::array(2, -0.333333, -0.0449554), AL::ALValue::array(2, 0.4, 0.0539465));
  times[19][8] = 8.4;
  keys[19][8] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[19][9] = 9.4;
  keys[19][9] = AL::ALValue::array(-0.13495, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  names[20] = "RHipRoll";
  times[20].arraySetSize(10);
  keys[20].arraySetSize(10);
  
  times[20][0] = 1;
  keys[20][0] = AL::ALValue::array(-0.084328, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[20][1] = 1.7;
  keys[20][1] = AL::ALValue::array(-0.085862, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[20][2] = 2.4;
  keys[20][2] = AL::ALValue::array(4.19617e-05, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[20][3] = 3.4;
  keys[20][3] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.433333, 0.0));
  times[20][4] = 4.7;
  keys[20][4] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.433333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[20][5] = 5.4;
  keys[20][5] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
  times[20][6] = 6.2;
  keys[20][6] = AL::ALValue::array(-0.0872665, AL::ALValue::array(2, -0.266667, 0.0139626), AL::ALValue::array(2, 0.333333, -0.0174533));
  times[20][7] = 7.2;
  keys[20][7] = AL::ALValue::array(-0.10472, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.4, 0.0));
  times[20][8] = 8.4;
  keys[20][8] = AL::ALValue::array(0.010472, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[20][9] = 9.4;
  keys[20][9] = AL::ALValue::array(-0.059784, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  names[21] = "RHipPitch";
  times[21].arraySetSize(10);
  keys[21].arraySetSize(10);
  
  times[21][0] = 1;
  keys[21][0] = AL::ALValue::array(0.076658, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[21][1] = 1.7;
  keys[21][1] = AL::ALValue::array(0.0812601, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[21][2] = 2.4;
  keys[21][2] = AL::ALValue::array(-4.19617e-05, AL::ALValue::array(2, -0.233333, 0.081302), AL::ALValue::array(2, 0.333333, -0.116146));
  times[21][3] = 3.4;
  keys[21][3] = AL::ALValue::array(-1.5708, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.433333, 0.0));
  times[21][4] = 4.7;
  keys[21][4] = AL::ALValue::array(-1.5708, AL::ALValue::array(2, -0.433333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[21][5] = 5.4;
  keys[21][5] = AL::ALValue::array(-1.5708, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
  times[21][6] = 6.2;
  keys[21][6] = AL::ALValue::array(-1.5708, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[21][7] = 7.2;
  keys[21][7] = AL::ALValue::array(-1.06989, AL::ALValue::array(2, -0.333333, -0.0189077), AL::ALValue::array(2, 0.4, 0.0226893));
  times[21][8] = 8.4;
  keys[21][8] = AL::ALValue::array(-1.0472, AL::ALValue::array(2, -0.4, -0.0226893), AL::ALValue::array(2, 0.333333, 0.0189077));
  times[21][9] = 9.4;
  keys[21][9] = AL::ALValue::array(0.210117, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  names[22] = "RKneePitch";
  times[22].arraySetSize(10);
  keys[22].arraySetSize(10);
  
  times[22][0] = 1;
  keys[22][0] = AL::ALValue::array(-0.078192, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[22][1] = 1.7;
  keys[22][1] = AL::ALValue::array(-0.076658, AL::ALValue::array(2, -0.233333, -0.00153397), AL::ALValue::array(2, 0.233333, 0.00153397));
  times[22][2] = 2.4;
  keys[22][2] = AL::ALValue::array(2.09548, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[22][3] = 3.4;
  keys[22][3] = AL::ALValue::array(2.0944, AL::ALValue::array(2, -0.333333, 0.00108204), AL::ALValue::array(2, 0.433333, -0.00140666));
  times[22][4] = 4.7;
  keys[22][4] = AL::ALValue::array(1.0472, AL::ALValue::array(2, -0.433333, 0.0648265), AL::ALValue::array(2, 0.233333, -0.0349066));
  times[22][5] = 5.4;
  keys[22][5] = AL::ALValue::array(1.01229, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
  times[22][6] = 6.2;
  keys[22][6] = AL::ALValue::array(2.12019, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[22][7] = 7.2;
  keys[22][7] = AL::ALValue::array(2.12019, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.4, 0.0));
  times[22][8] = 8.4;
  keys[22][8] = AL::ALValue::array(2.0944, AL::ALValue::array(2, -0.4, 0.0257959), AL::ALValue::array(2, 0.333333, -0.0214966));
  times[22][9] = 9.4;
  keys[22][9] = AL::ALValue::array(-0.076658, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  names[23] = "RAnklePitch";
  times[23].arraySetSize(10);
  keys[23].arraySetSize(10);
  
  times[23][0] = 1;
  keys[23][0] = AL::ALValue::array(0.929646, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[23][1] = 1.7;
  keys[23][1] = AL::ALValue::array(0.93118, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[23][2] = 2.4;
  keys[23][2] = AL::ALValue::array(-1.13512, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[23][3] = 3.4;
  keys[23][3] = AL::ALValue::array(-1.13446, AL::ALValue::array(2, -0.333333, -0.000654472), AL::ALValue::array(2, 0.433333, 0.000850813));
  times[23][4] = 4.7;
  keys[23][4] = AL::ALValue::array(-0.783653, AL::ALValue::array(2, -0.433333, -0.264708), AL::ALValue::array(2, 0.233333, 0.142535));
  times[23][5] = 5.4;
  keys[23][5] = AL::ALValue::array(0.0872665, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
  times[23][6] = 6.2;
  keys[23][6] = AL::ALValue::array(-0.312414, AL::ALValue::array(2, -0.266667, 0.118941), AL::ALValue::array(2, 0.333333, -0.148676));
  times[23][7] = 7.2;
  keys[23][7] = AL::ALValue::array(-0.715585, AL::ALValue::array(2, -0.333333, 0.111331), AL::ALValue::array(2, 0.4, -0.133597));
  times[23][8] = 8.4;
  keys[23][8] = AL::ALValue::array(-1.0472, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[23][9] = 9.4;
  keys[23][9] = AL::ALValue::array(0.0859459, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  names[24] = "RAnkleRoll";
  times[24].arraySetSize(10);
  keys[24].arraySetSize(10);
  
  times[24][0] = 1;
  keys[24][0] = AL::ALValue::array(0.00157596, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[24][1] = 1.7;
  keys[24][1] = AL::ALValue::array(0.00157596, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[24][2] = 2.4;
  keys[24][2] = AL::ALValue::array(4.19617e-05, AL::ALValue::array(2, -0.233333, 2.93127e-05), AL::ALValue::array(2, 0.333333, -4.18753e-05));
  times[24][3] = 3.4;
  keys[24][3] = AL::ALValue::array(8.63852e-08, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.433333, 0.0));
  times[24][4] = 4.7;
  keys[24][4] = AL::ALValue::array(0.680678, AL::ALValue::array(2, -0.433333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
  times[24][5] = 5.4;
  keys[24][5] = AL::ALValue::array(0.555015, AL::ALValue::array(2, -0.233333, 0.059729), AL::ALValue::array(2, 0.266667, -0.0682618));
  times[24][6] = 6.2;
  keys[24][6] = AL::ALValue::array(0.296706, AL::ALValue::array(2, -0.266667, 0.0667104), AL::ALValue::array(2, 0.333333, -0.083388));
  times[24][7] = 7.2;
  keys[24][7] = AL::ALValue::array(0.10472, AL::ALValue::array(2, -0.333333, 0.0449554), AL::ALValue::array(2, 0.4, -0.0539465));
  times[24][8] = 8.4;
  keys[24][8] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  times[24][9] = 9.4;
  keys[24][9] = AL::ALValue::array(0.0614019, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
}

int ALstandUpFront2010::ExecutePost() {
  return motion->post.angleInterpolationBezier(names, times, keys);
} 

#include "LieDown.h"

#include "hal/robot/generic_nao/kAlBroker.h"

namespace {
  SpecialActionRegistrar<LieDown>::Type temp("LieDown");
}

LieDown::LieDown() {
  Initialize();
  Logger::Instance().WriteMsg("SP:LieDown","Initialized!",Logger::ExtraInfo);
  
  try {
    motion = KAlBroker::Instance().GetBroker()->getMotionProxy();
  } catch (AL::ALError& e) {
    Logger::Instance().WriteMsg("SP:LieDown","Error in getting motion proxy",Logger::FatalError);
  }
  
}




void LieDown::Initialize() {
  
  /************************* Lie Down *********************************/
  
  LieDown_names.arraySetSize(21);
  LieDown_times.arraySetSize(21);
  LieDown_keys.arraySetSize(21);
  
  LieDown_names[0] = "HeadYaw";
  LieDown_times[0].arraySetSize(5);
  LieDown_keys[0].arraySetSize(5);
  
  LieDown_times[0][0] = 2;
  LieDown_keys[0][0] = AL::ALValue::array(-0.680678, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[0][1] = 3;
  LieDown_keys[0][1] = AL::ALValue::array(0.750492, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[0][2] = 4;
  LieDown_keys[0][2] = AL::ALValue::array(-0.523599, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[0][3] = 5;
  LieDown_keys[0][3] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.666667, 0.0));
  LieDown_times[0][4] = 7;
  LieDown_keys[0][4] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  LieDown_names[1] = "HeadPitch";
  LieDown_times[1].arraySetSize(6);
  LieDown_keys[1].arraySetSize(6);
  
  LieDown_times[1][0] = 1;
  LieDown_keys[1][0] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[1][1] = 2;
  LieDown_keys[1][1] = AL::ALValue::array(-0.0191986, AL::ALValue::array(2, -0.333333, 0.0191986), AL::ALValue::array(2, 0.333333, -0.0191986));
  LieDown_times[1][2] = 3;
  LieDown_keys[1][2] = AL::ALValue::array(-0.15708, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[1][3] = 4;
  LieDown_keys[1][3] = AL::ALValue::array(-0.15708, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[1][4] = 5;
  LieDown_keys[1][4] = AL::ALValue::array(-0.785398, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.666667, 0.0));
  LieDown_times[1][5] = 7;
  LieDown_keys[1][5] = AL::ALValue::array(-0.785398, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  LieDown_names[2] = "LShoulderPitch";
  LieDown_times[2].arraySetSize(6);
  LieDown_keys[2].arraySetSize(6);
  
  LieDown_times[2][0] = 1;
  LieDown_keys[2][0] = AL::ALValue::array(1.62316, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[2][1] = 2;
  LieDown_keys[2][1] = AL::ALValue::array(1.62316, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[2][2] = 3;
  LieDown_keys[2][2] = AL::ALValue::array(0.715585, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[2][3] = 4;
  LieDown_keys[2][3] = AL::ALValue::array(1.27409, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[2][4] = 5;
  LieDown_keys[2][4] = AL::ALValue::array(-0.453786, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.666667, 0.0));
  LieDown_times[2][5] = 7;
  LieDown_keys[2][5] = AL::ALValue::array(-0.453786, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  LieDown_names[3] = "LShoulderRoll";
  LieDown_times[3].arraySetSize(6);
  LieDown_keys[3].arraySetSize(6);
  
  LieDown_times[3][0] = 1;
  LieDown_keys[3][0] = AL::ALValue::array(0.314159, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[3][1] = 2;
  LieDown_keys[3][1] = AL::ALValue::array(0.314159, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[3][2] = 3;
  LieDown_keys[3][2] = AL::ALValue::array(0.645772, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[3][3] = 4;
  LieDown_keys[3][3] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[3][4] = 5;
  LieDown_keys[3][4] = AL::ALValue::array(0.907571, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.666667, 0.0));
  LieDown_times[3][5] = 7;
  LieDown_keys[3][5] = AL::ALValue::array(0.907571, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  LieDown_names[4] = "LElbowYaw";
  LieDown_times[4].arraySetSize(6);
  LieDown_keys[4].arraySetSize(6);
  
  LieDown_times[4][0] = 1;
  LieDown_keys[4][0] = AL::ALValue::array(-1.5708, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[4][1] = 2;
  LieDown_keys[4][1] = AL::ALValue::array(-1.5708, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[4][2] = 3;
  LieDown_keys[4][2] = AL::ALValue::array(-1.5708, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[4][3] = 4;
  LieDown_keys[4][3] = AL::ALValue::array(-1.5708, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[4][4] = 5;
  LieDown_keys[4][4] = AL::ALValue::array(-1.5708, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.666667, 0.0));
  LieDown_times[4][5] = 7;
  LieDown_keys[4][5] = AL::ALValue::array(-1.5708, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  LieDown_names[5] = "LElbowRoll";
  LieDown_times[5].arraySetSize(6);
  LieDown_keys[5].arraySetSize(6);
  
  LieDown_times[5][0] = 1;
  LieDown_keys[5][0] = AL::ALValue::array(-0.139626, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[5][1] = 2;
  LieDown_keys[5][1] = AL::ALValue::array(-0.139626, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[5][2] = 3;
  LieDown_keys[5][2] = AL::ALValue::array(-1.22173, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[5][3] = 4;
  LieDown_keys[5][3] = AL::ALValue::array(-1.22173, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[5][4] = 5;
  LieDown_keys[5][4] = AL::ALValue::array(-1.01229, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.666667, 0.0));
  LieDown_times[5][5] = 7;
  LieDown_keys[5][5] = AL::ALValue::array(-1.01229, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  LieDown_names[6] = "RShoulderPitch";
  LieDown_times[6].arraySetSize(6);
  LieDown_keys[6].arraySetSize(6);
  
  LieDown_times[6][0] = 1;
  LieDown_keys[6][0] = AL::ALValue::array(1.62316, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[6][1] = 2;
  LieDown_keys[6][1] = AL::ALValue::array(0.715585, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[6][2] = 3;
  LieDown_keys[6][2] = AL::ALValue::array(0.715585, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[6][3] = 4;
  LieDown_keys[6][3] = AL::ALValue::array(1.27409, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[6][4] = 5;
  LieDown_keys[6][4] = AL::ALValue::array(-0.453786, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.666667, 0.0));
  LieDown_times[6][5] = 7;
  LieDown_keys[6][5] = AL::ALValue::array(-0.453786, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  LieDown_names[7] = "RShoulderRoll";
  LieDown_times[7].arraySetSize(6);
  LieDown_keys[7].arraySetSize(6);
  
  LieDown_times[7][0] = 1;
  LieDown_keys[7][0] = AL::ALValue::array(-0.314159, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[7][1] = 2;
  LieDown_keys[7][1] = AL::ALValue::array(-0.645772, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[7][2] = 3;
  LieDown_keys[7][2] = AL::ALValue::array(-0.645772, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[7][3] = 4;
  LieDown_keys[7][3] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[7][4] = 5;
  LieDown_keys[7][4] = AL::ALValue::array(-0.907571, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.666667, 0.0));
  LieDown_times[7][5] = 7;
  LieDown_keys[7][5] = AL::ALValue::array(-0.907571, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  LieDown_names[8] = "RElbowYaw";
  LieDown_times[8].arraySetSize(6);
  LieDown_keys[8].arraySetSize(6);
  
  LieDown_times[8][0] = 1;
  LieDown_keys[8][0] = AL::ALValue::array(1.5708, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[8][1] = 2;
  LieDown_keys[8][1] = AL::ALValue::array(1.5708, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[8][2] = 3;
  LieDown_keys[8][2] = AL::ALValue::array(1.5708, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[8][3] = 4;
  LieDown_keys[8][3] = AL::ALValue::array(1.5708, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[8][4] = 5;
  LieDown_keys[8][4] = AL::ALValue::array(1.5708, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.666667, 0.0));
  LieDown_times[8][5] = 7;
  LieDown_keys[8][5] = AL::ALValue::array(1.5708, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  LieDown_names[9] = "RElbowRoll";
  LieDown_times[9].arraySetSize(6);
  LieDown_keys[9].arraySetSize(6);
  
  LieDown_times[9][0] = 1;
  LieDown_keys[9][0] = AL::ALValue::array(0.139626, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[9][1] = 2;
  LieDown_keys[9][1] = AL::ALValue::array(1.22173, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[9][2] = 3;
  LieDown_keys[9][2] = AL::ALValue::array(1.22173, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[9][3] = 4;
  LieDown_keys[9][3] = AL::ALValue::array(1.22173, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[9][4] = 5;
  LieDown_keys[9][4] = AL::ALValue::array(1.01229, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.666667, 0.0));
  LieDown_times[9][5] = 7;
  LieDown_keys[9][5] = AL::ALValue::array(1.01229, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  LieDown_names[10] = "LHipYawPitch";
  LieDown_times[10].arraySetSize(6);
  LieDown_keys[10].arraySetSize(6);
  
  LieDown_times[10][0] = 1;
  LieDown_keys[10][0] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[10][1] = 2;
  LieDown_keys[10][1] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[10][2] = 3;
  LieDown_keys[10][2] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[10][3] = 4;
  LieDown_keys[10][3] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[10][4] = 5;
  LieDown_keys[10][4] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.666667, 0.0));
  LieDown_times[10][5] = 7;
  LieDown_keys[10][5] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  LieDown_names[11] = "LHipRoll";
  LieDown_times[11].arraySetSize(6);
  LieDown_keys[11].arraySetSize(6);
  
  LieDown_times[11][0] = 1;
  LieDown_keys[11][0] = AL::ALValue::array(-0.010472, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[11][1] = 2;
  LieDown_keys[11][1] = AL::ALValue::array(-0.010472, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[11][2] = 3;
  LieDown_keys[11][2] = AL::ALValue::array(-0.010472, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[11][3] = 4;
  LieDown_keys[11][3] = AL::ALValue::array(-0.010472, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[11][4] = 5;
  LieDown_keys[11][4] = AL::ALValue::array(-0.010472, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.666667, 0.0));
  LieDown_times[11][5] = 7;
  LieDown_keys[11][5] = AL::ALValue::array(-0.010472, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  LieDown_names[12] = "LHipPitch";
  LieDown_times[12].arraySetSize(6);
  LieDown_keys[12].arraySetSize(6);
  
  LieDown_times[12][0] = 1;
  LieDown_keys[12][0] = AL::ALValue::array(-0.523599, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[12][1] = 2;
  LieDown_keys[12][1] = AL::ALValue::array(-0.698132, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[12][2] = 3;
  LieDown_keys[12][2] = AL::ALValue::array(-0.610865, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[12][3] = 4;
  LieDown_keys[12][3] = AL::ALValue::array(-0.959931, AL::ALValue::array(2, -0.333333, 0.159989), AL::ALValue::array(2, 0.333333, -0.159989));
  LieDown_times[12][4] = 5;
  LieDown_keys[12][4] = AL::ALValue::array(-1.5708, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.666667, 0.0));
  LieDown_times[12][5] = 7;
  LieDown_keys[12][5] = AL::ALValue::array(-1.5708, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  LieDown_names[13] = "LKneePitch";
  LieDown_times[13].arraySetSize(6);
  LieDown_keys[13].arraySetSize(6);
  
  LieDown_times[13][0] = 1;
  LieDown_keys[13][0] = AL::ALValue::array(1.0472, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[13][1] = 2;
  LieDown_keys[13][1] = AL::ALValue::array(1.39626, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[13][2] = 3;
  LieDown_keys[13][2] = AL::ALValue::array(1.39626, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[13][3] = 4;
  LieDown_keys[13][3] = AL::ALValue::array(1.74533, AL::ALValue::array(2, -0.333333, -0.0756309), AL::ALValue::array(2, 0.333333, 0.0756309));
  LieDown_times[13][4] = 5;
  LieDown_keys[13][4] = AL::ALValue::array(1.85005, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.666667, 0.0));
  LieDown_times[13][5] = 7;
  LieDown_keys[13][5] = AL::ALValue::array(1.85005, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  LieDown_names[14] = "LAnklePitch";
  LieDown_times[14].arraySetSize(6);
  LieDown_keys[14].arraySetSize(6);
  
  LieDown_times[14][0] = 1;
  LieDown_keys[14][0] = AL::ALValue::array(-0.523599, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[14][1] = 2;
  LieDown_keys[14][1] = AL::ALValue::array(-0.698132, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[14][2] = 3;
  LieDown_keys[14][2] = AL::ALValue::array(-0.698132, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[14][3] = 4;
  LieDown_keys[14][3] = AL::ALValue::array(-0.872665, AL::ALValue::array(2, -0.333333, 0.0581776), AL::ALValue::array(2, 0.333333, -0.0581776));
  LieDown_times[14][4] = 5;
  LieDown_keys[14][4] = AL::ALValue::array(-1.0472, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.666667, 0.0));
  LieDown_times[14][5] = 7;
  LieDown_keys[14][5] = AL::ALValue::array(-1.0472, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  LieDown_names[15] = "LAnkleRoll";
  LieDown_times[15].arraySetSize(6);
  LieDown_keys[15].arraySetSize(6);
  
  LieDown_times[15][0] = 1;
  LieDown_keys[15][0] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[15][1] = 2;
  LieDown_keys[15][1] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[15][2] = 3;
  LieDown_keys[15][2] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[15][3] = 4;
  LieDown_keys[15][3] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[15][4] = 5;
  LieDown_keys[15][4] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.666667, 0.0));
  LieDown_times[15][5] = 7;
  LieDown_keys[15][5] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  LieDown_names[16] = "RHipRoll";
  LieDown_times[16].arraySetSize(6);
  LieDown_keys[16].arraySetSize(6);
  
  LieDown_times[16][0] = 1;
  LieDown_keys[16][0] = AL::ALValue::array(0.010472, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[16][1] = 2;
  LieDown_keys[16][1] = AL::ALValue::array(0.010472, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[16][2] = 3;
  LieDown_keys[16][2] = AL::ALValue::array(0.010472, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[16][3] = 4;
  LieDown_keys[16][3] = AL::ALValue::array(0.010472, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[16][4] = 5;
  LieDown_keys[16][4] = AL::ALValue::array(0.010472, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.666667, 0.0));
  LieDown_times[16][5] = 7;
  LieDown_keys[16][5] = AL::ALValue::array(0.010472, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  LieDown_names[17] = "RHipPitch";
  LieDown_times[17].arraySetSize(6);
  LieDown_keys[17].arraySetSize(6);
  
  LieDown_times[17][0] = 1;
  LieDown_keys[17][0] = AL::ALValue::array(-0.523599, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[17][1] = 2;
  LieDown_keys[17][1] = AL::ALValue::array(-0.698132, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[17][2] = 3;
  LieDown_keys[17][2] = AL::ALValue::array(-0.610865, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[17][3] = 4;
  LieDown_keys[17][3] = AL::ALValue::array(-0.959931, AL::ALValue::array(2, -0.333333, 0.159989), AL::ALValue::array(2, 0.333333, -0.159989));
  LieDown_times[17][4] = 5;
  LieDown_keys[17][4] = AL::ALValue::array(-1.5708, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.666667, 0.0));
  LieDown_times[17][5] = 7;
  LieDown_keys[17][5] = AL::ALValue::array(-1.5708, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  LieDown_names[18] = "RKneePitch";
  LieDown_times[18].arraySetSize(6);
  LieDown_keys[18].arraySetSize(6);
  
  LieDown_times[18][0] = 1;
  LieDown_keys[18][0] = AL::ALValue::array(1.0472, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[18][1] = 2;
  LieDown_keys[18][1] = AL::ALValue::array(1.39626, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[18][2] = 3;
  LieDown_keys[18][2] = AL::ALValue::array(1.39626, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[18][3] = 4;
  LieDown_keys[18][3] = AL::ALValue::array(1.74533, AL::ALValue::array(2, -0.333333, -0.0756309), AL::ALValue::array(2, 0.333333, 0.0756309));
  LieDown_times[18][4] = 5;
  LieDown_keys[18][4] = AL::ALValue::array(1.85005, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.666667, 0.0));
  LieDown_times[18][5] = 7;
  LieDown_keys[18][5] = AL::ALValue::array(1.85005, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  LieDown_names[19] = "RAnklePitch";
  LieDown_times[19].arraySetSize(6);
  LieDown_keys[19].arraySetSize(6);
  
  LieDown_times[19][0] = 1;
  LieDown_keys[19][0] = AL::ALValue::array(-0.523599, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[19][1] = 2;
  LieDown_keys[19][1] = AL::ALValue::array(-0.698132, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[19][2] = 3;
  LieDown_keys[19][2] = AL::ALValue::array(-0.698132, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[19][3] = 4;
  LieDown_keys[19][3] = AL::ALValue::array(-0.872665, AL::ALValue::array(2, -0.333333, 0.0581776), AL::ALValue::array(2, 0.333333, -0.0581776));
  LieDown_times[19][4] = 5;
  LieDown_keys[19][4] = AL::ALValue::array(-1.0472, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.666667, 0.0));
  LieDown_times[19][5] = 7;
  LieDown_keys[19][5] = AL::ALValue::array(-1.0472, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  LieDown_names[20] = "RAnkleRoll";
  LieDown_times[20].arraySetSize(6);
  LieDown_keys[20].arraySetSize(6);
  
  LieDown_times[20][0] = 1;
  LieDown_keys[20][0] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[20][1] = 2;
  LieDown_keys[20][1] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[20][2] = 3;
  LieDown_keys[20][2] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[20][3] = 4;
  LieDown_keys[20][3] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
  LieDown_times[20][4] = 5;
  LieDown_keys[20][4] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.666667, 0.0));
  LieDown_times[20][5] = 7;
  LieDown_keys[20][5] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0)); 
  
  
}

int LieDown::ExecutePost() {
  return motion->post.angleInterpolationBezier(LieDown_names, LieDown_times, LieDown_keys); 
} 

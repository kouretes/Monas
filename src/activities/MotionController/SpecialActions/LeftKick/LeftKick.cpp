#include "LeftKick.h"

#include "hal/robot/generic_nao/kAlBroker.h"

namespace {
  SpecialActionRegistrar<LeftKick>::Type temp("LeftKick");
}

LeftKick::LeftKick() {
  Initialize();
  Logger::Instance().WriteMsg("SP:LeftKick","Initialized!",Logger::ExtraInfo);
  
  try {
    motion = KAlBroker::Instance().GetBroker()->getMotionProxy();
  } catch (AL::ALError& e) {
    Logger::Instance().WriteMsg("SP:LeftKick","Error in getting motion proxy",Logger::FatalError);
  }
  
}




void LeftKick::Initialize() {
  
  /************************* Left Kick *********************************/
  
  LeftKick_names.arraySetSize(23);
  LeftKick_times.arraySetSize(23);
  LeftKick_keys.arraySetSize(23);
  
  /*
  LeftKick_names[0] = "HeadYaw";
  LeftKick_times[0].arraySetSize(6);
  LeftKick_keys[0].arraySetSize(6);
  
  LeftKick_times[0][0] = 1.13333;
  LeftKick_keys[0][0] = AL::ALValue::array(0.021518, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.688889, 0.0));
  LeftKick_times[0][1] = 3.2;
  LeftKick_keys[0][1] = AL::ALValue::array(0.030638, AL::ALValue::array(2, -0.688889, -0.0), AL::ALValue::array(2, 0.288889, 0.0));
  LeftKick_times[0][2] = 4.06667;
  LeftKick_keys[0][2] = AL::ALValue::array(0.030722, AL::ALValue::array(2, -0.288889, -0.0), AL::ALValue::array(2, 0.111111, 0.0));
  LeftKick_times[0][3] = 4.4;
  LeftKick_keys[0][3] = AL::ALValue::array(0.0261199, AL::ALValue::array(2, -0.111111, -0.0), AL::ALValue::array(2, 0.377778, 0.0));
  LeftKick_times[0][4] = 5.53333;
  LeftKick_keys[0][4] = AL::ALValue::array(0.032172, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.4, 0.0));
  LeftKick_times[0][5] = 6.73333;
  LeftKick_keys[0][5] = AL::ALValue::array(0.010696, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  LeftKick_names[1] = "HeadPitch";
  LeftKick_times[1].arraySetSize(6);
  LeftKick_keys[1].arraySetSize(6);
  
  LeftKick_times[1][0] = 1.13333;
  LeftKick_keys[1][0] = AL::ALValue::array(-0.016916, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.688889, 0.0));
  LeftKick_times[1][1] = 3.2;
  LeftKick_keys[1][1] = AL::ALValue::array(-0.0337899, AL::ALValue::array(2, -0.688889, -0.0), AL::ALValue::array(2, 0.288889, 0.0));
  LeftKick_times[1][2] = 4.06667;
  LeftKick_keys[1][2] = AL::ALValue::array(-0.012314, AL::ALValue::array(2, -0.288889, -0.0), AL::ALValue::array(2, 0.111111, 0.0));
  LeftKick_times[1][3] = 4.4;
  LeftKick_keys[1][3] = AL::ALValue::array(-0.023052, AL::ALValue::array(2, -0.111111, -0.0), AL::ALValue::array(2, 0.377778, 0.0));
  LeftKick_times[1][4] = 5.53333;
  LeftKick_keys[1][4] = AL::ALValue::array(-0.030722, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.4, 0.0));
  LeftKick_times[1][5] = 6.73333;
  LeftKick_keys[1][5] = AL::ALValue::array(0.00762803, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  */
  
  LeftKick_names[2] = "LShoulderPitch";
  LeftKick_times[2].arraySetSize(6);
  LeftKick_keys[2].arraySetSize(6);
  
  LeftKick_times[2][0] = 1.13333;
  LeftKick_keys[2][0] = AL::ALValue::array(1.43126, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.688889, 0.0));
  LeftKick_times[2][1] = 3.2;
  LeftKick_keys[2][1] = AL::ALValue::array(1.52015, AL::ALValue::array(2, -0.688889, -0.0), AL::ALValue::array(2, 0.288889, 0.0));
  LeftKick_times[2][2] = 4.06667;
  LeftKick_keys[2][2] = AL::ALValue::array(1.76414, AL::ALValue::array(2, -0.288889, -0.0), AL::ALValue::array(2, 0.111111, 0.0));
  LeftKick_times[2][3] = 4.4;
  LeftKick_keys[2][3] = AL::ALValue::array(2.06635, AL::ALValue::array(2, -0.111111, -0.0), AL::ALValue::array(2, 0.377778, 0.0));
  LeftKick_times[2][4] = 5.53333;
  LeftKick_keys[2][4] = AL::ALValue::array(1.5539, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.4, 0.0));
  LeftKick_times[2][5] = 6.73333;
  LeftKick_keys[2][5] = AL::ALValue::array(1.41584, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  LeftKick_names[3] = "LShoulderRoll";
  LeftKick_times[3].arraySetSize(6);
  LeftKick_keys[3].arraySetSize(6);
  
  LeftKick_times[3][0] = 1.13333;
  LeftKick_keys[3][0] = AL::ALValue::array(0.358999, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.688889, 0.0));
  LeftKick_times[3][1] = 3.2;
  LeftKick_keys[3][1] = AL::ALValue::array(0.762356, AL::ALValue::array(2, -0.688889, -0.0), AL::ALValue::array(2, 0.288889, 0.0));
  LeftKick_times[3][2] = 4.06667;
  LeftKick_keys[3][2] = AL::ALValue::array(1.10606, AL::ALValue::array(2, -0.288889, -0.0), AL::ALValue::array(2, 0.111111, 0.0));
  LeftKick_times[3][3] = 4.4;
  LeftKick_keys[3][3] = AL::ALValue::array(0.403483, AL::ALValue::array(2, -0.111111, -0.0), AL::ALValue::array(2, 0.377778, 0.0));
  LeftKick_times[3][4] = 5.53333;
  LeftKick_keys[3][4] = AL::ALValue::array(0.728609, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.4, 0.0));
  LeftKick_times[3][5] = 6.73333;
  LeftKick_keys[3][5] = AL::ALValue::array(0.352778, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  LeftKick_names[4] = "LElbowYaw";
  LeftKick_times[4].arraySetSize(6);
  LeftKick_keys[4].arraySetSize(6);
  
  LeftKick_times[4][0] = 1.13333;
  LeftKick_keys[4][0] = AL::ALValue::array(-1.37749, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.688889, 0.0));
  LeftKick_times[4][1] = 3.2;
  LeftKick_keys[4][1] = AL::ALValue::array(-1.39138, AL::ALValue::array(2, -0.688889, -0.0), AL::ALValue::array(2, 0.288889, 0.0));
  LeftKick_times[4][2] = 4.06667;
  LeftKick_keys[4][2] = AL::ALValue::array(-1.88984, AL::ALValue::array(2, -0.288889, -0.0), AL::ALValue::array(2, 0.111111, 0.0));
  LeftKick_times[4][3] = 4.4;
  LeftKick_keys[4][3] = AL::ALValue::array(-1.76098, AL::ALValue::array(2, -0.111111, -0.0), AL::ALValue::array(2, 0.377778, 0.0));
  LeftKick_times[4][4] = 5.53333;
  LeftKick_keys[4][4] = AL::ALValue::array(-1.39752, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.4, 0.0));
  LeftKick_times[4][5] = 6.73333;
  LeftKick_keys[4][5] = AL::ALValue::array(-1.39445, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  LeftKick_names[5] = "LElbowRoll";
  LeftKick_times[5].arraySetSize(6);
  LeftKick_keys[5].arraySetSize(6);
  
  LeftKick_times[5][0] = 1.13333;
  LeftKick_keys[5][0] = AL::ALValue::array(-1.0631, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.688889, 0.0));
  LeftKick_times[5][1] = 3.2;
  LeftKick_keys[5][1] = AL::ALValue::array(-1.01393, AL::ALValue::array(2, -0.688889, -0.0), AL::ALValue::array(2, 0.288889, 0.0));
  LeftKick_times[5][2] = 4.06667;
  LeftKick_keys[5][2] = AL::ALValue::array(-1.02782, AL::ALValue::array(2, -0.288889, -0.0), AL::ALValue::array(2, 0.111111, 0.0));
  LeftKick_times[5][3] = 4.4;
  LeftKick_keys[5][3] = AL::ALValue::array(-0.112024, AL::ALValue::array(2, -0.111111, -0.0), AL::ALValue::array(2, 0.377778, 0.0));
  LeftKick_times[5][4] = 5.53333;
  LeftKick_keys[5][4] = AL::ALValue::array(-0.998592, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.4, 0.0));
  LeftKick_times[5][5] = 6.73333;
  LeftKick_keys[5][5] = AL::ALValue::array(-1.02927, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  LeftKick_names[6] = "LWristYaw";
  LeftKick_times[6].arraySetSize(6);
  LeftKick_keys[6].arraySetSize(6);
  
  LeftKick_times[6][0] = 1.13333;
  LeftKick_keys[6][0] = AL::ALValue::array(0, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.688889, 0.0));
  LeftKick_times[6][1] = 3.2;
  LeftKick_keys[6][1] = AL::ALValue::array(0, AL::ALValue::array(2, -0.688889, -0.0), AL::ALValue::array(2, 0.288889, 0.0));
  LeftKick_times[6][2] = 4.06667;
  LeftKick_keys[6][2] = AL::ALValue::array(0, AL::ALValue::array(2, -0.288889, -0.0), AL::ALValue::array(2, 0.111111, 0.0));
  LeftKick_times[6][3] = 4.4;
  LeftKick_keys[6][3] = AL::ALValue::array(0, AL::ALValue::array(2, -0.111111, -0.0), AL::ALValue::array(2, 0.377778, 0.0));
  LeftKick_times[6][4] = 5.53333;
  LeftKick_keys[6][4] = AL::ALValue::array(0, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.4, 0.0));
  LeftKick_times[6][5] = 6.73333;
  LeftKick_keys[6][5] = AL::ALValue::array(0, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  LeftKick_names[7] = "LHand";
  LeftKick_times[7].arraySetSize(6);
  LeftKick_keys[7].arraySetSize(6);
  
  LeftKick_times[7][0] = 1.13333;
  LeftKick_keys[7][0] = AL::ALValue::array(0, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.688889, 0.0));
  LeftKick_times[7][1] = 3.2;
  LeftKick_keys[7][1] = AL::ALValue::array(0, AL::ALValue::array(2, -0.688889, -0.0), AL::ALValue::array(2, 0.288889, 0.0));
  LeftKick_times[7][2] = 4.06667;
  LeftKick_keys[7][2] = AL::ALValue::array(0, AL::ALValue::array(2, -0.288889, -0.0), AL::ALValue::array(2, 0.111111, 0.0));
  LeftKick_times[7][3] = 4.4;
  LeftKick_keys[7][3] = AL::ALValue::array(0, AL::ALValue::array(2, -0.111111, -0.0), AL::ALValue::array(2, 0.377778, 0.0));
  LeftKick_times[7][4] = 5.53333;
  LeftKick_keys[7][4] = AL::ALValue::array(0, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.4, 0.0));
  LeftKick_times[7][5] = 6.73333;
  LeftKick_keys[7][5] = AL::ALValue::array(0, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  LeftKick_names[8] = "RShoulderPitch";
  LeftKick_times[8].arraySetSize(6);
  LeftKick_keys[8].arraySetSize(6);
  
  LeftKick_times[8][0] = 1.13333;
  LeftKick_keys[8][0] = AL::ALValue::array(1.43732, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.688889, 0.0));
  LeftKick_times[8][1] = 3.2;
  LeftKick_keys[8][1] = AL::ALValue::array(1.4374, AL::ALValue::array(2, -0.688889, -0.0), AL::ALValue::array(2, 0.288889, 0.0));
  LeftKick_times[8][2] = 4.06667;
  LeftKick_keys[8][2] = AL::ALValue::array(1.71957, AL::ALValue::array(2, -0.288889, -0.0), AL::ALValue::array(2, 0.111111, 0.0));
  LeftKick_times[8][3] = 4.4;
  LeftKick_keys[8][3] = AL::ALValue::array(1.69196, AL::ALValue::array(2, -0.111111, -0.0), AL::ALValue::array(2, 0.377778, 0.0));
  LeftKick_times[8][4] = 5.53333;
  LeftKick_keys[8][4] = AL::ALValue::array(1.47115, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.4, 0.0));
  LeftKick_times[8][5] = 6.73333;
  LeftKick_keys[8][5] = AL::ALValue::array(1.42973, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  LeftKick_names[9] = "RShoulderRoll";
  LeftKick_times[9].arraySetSize(6);
  LeftKick_keys[9].arraySetSize(6);
  
  LeftKick_times[9][0] = 1.13333;
  LeftKick_keys[9][0] = AL::ALValue::array(-0.361981, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.688889, 0.0));
  LeftKick_times[9][1] = 3.2;
  LeftKick_keys[9][1] = AL::ALValue::array(-0.426494, AL::ALValue::array(2, -0.688889, -0.0), AL::ALValue::array(2, 0.288889, 0.0));
  LeftKick_times[9][2] = 4.06667;
  LeftKick_keys[9][2] = AL::ALValue::array(-0.371186, AL::ALValue::array(2, -0.288889, -0.0), AL::ALValue::array(2, 0.111111, 0.0));
  LeftKick_times[9][3] = 4.4;
  LeftKick_keys[9][3] = AL::ALValue::array(-0.386526, AL::ALValue::array(2, -0.111111, -0.0), AL::ALValue::array(2, 0.377778, 0.0));
  LeftKick_times[9][4] = 5.53333;
  LeftKick_keys[9][4] = AL::ALValue::array(-0.408086, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.4, 0.0));
  LeftKick_times[9][5] = 6.73333;
  LeftKick_keys[9][5] = AL::ALValue::array(-0.342125, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  LeftKick_names[10] = "RElbowYaw";
  LeftKick_times[10].arraySetSize(6);
  LeftKick_keys[10].arraySetSize(6);
  
  LeftKick_times[10][0] = 1.13333;
  LeftKick_keys[10][0] = AL::ALValue::array(1.37757, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.688889, 0.0));
  LeftKick_times[10][1] = 3.2;
  LeftKick_keys[10][1] = AL::ALValue::array(1.05995, AL::ALValue::array(2, -0.688889, -0.0), AL::ALValue::array(2, 0.288889, 0.0));
  LeftKick_times[10][2] = 4.06667;
  LeftKick_keys[10][2] = AL::ALValue::array(1.24718, AL::ALValue::array(2, -0.288889, -0.0), AL::ALValue::array(2, 0.111111, 0.0));
  LeftKick_times[10][3] = 4.4;
  LeftKick_keys[10][3] = AL::ALValue::array(1.23491, AL::ALValue::array(2, -0.111111, -0.0), AL::ALValue::array(2, 0.377778, 0.0));
  LeftKick_times[10][4] = 5.53333;
  LeftKick_keys[10][4] = AL::ALValue::array(1.07069, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.4, 0.0));
  LeftKick_times[10][5] = 6.73333;
  LeftKick_keys[10][5] = AL::ALValue::array(1.38363, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  LeftKick_names[11] = "RElbowRoll";
  LeftKick_times[11].arraySetSize(6);
  LeftKick_keys[11].arraySetSize(6);
  
  LeftKick_times[11][0] = 1.13333;
  LeftKick_keys[11][0] = AL::ALValue::array(1.02314, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.688889, 0.0));
  LeftKick_times[11][1] = 3.2;
  LeftKick_keys[11][1] = AL::ALValue::array(0.753235, AL::ALValue::array(2, -0.688889, -0.0), AL::ALValue::array(2, 0.288889, 0.0));
  LeftKick_times[11][2] = 4.06667;
  LeftKick_keys[11][2] = AL::ALValue::array(0.575208, AL::ALValue::array(2, -0.288889, -0.0), AL::ALValue::array(2, 0.111111, 0.0));
  LeftKick_times[11][3] = 4.4;
  LeftKick_keys[11][3] = AL::ALValue::array(0.58748, AL::ALValue::array(2, -0.111111, -0.0), AL::ALValue::array(2, 0.377778, 0.0));
  LeftKick_times[11][4] = 5.53333;
  LeftKick_keys[11][4] = AL::ALValue::array(0.722556, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.4, 0.0));
  LeftKick_times[11][5] = 6.73333;
  LeftKick_keys[11][5] = AL::ALValue::array(1.02475, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  LeftKick_names[12] = "RWristYaw";
  LeftKick_times[12].arraySetSize(6);
  LeftKick_keys[12].arraySetSize(6);
  
  LeftKick_times[12][0] = 1.13333;
  LeftKick_keys[12][0] = AL::ALValue::array(0, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.688889, 0.0));
  LeftKick_times[12][1] = 3.2;
  LeftKick_keys[12][1] = AL::ALValue::array(0, AL::ALValue::array(2, -0.688889, -0.0), AL::ALValue::array(2, 0.288889, 0.0));
  LeftKick_times[12][2] = 4.06667;
  LeftKick_keys[12][2] = AL::ALValue::array(0, AL::ALValue::array(2, -0.288889, -0.0), AL::ALValue::array(2, 0.111111, 0.0));
  LeftKick_times[12][3] = 4.4;
  LeftKick_keys[12][3] = AL::ALValue::array(0, AL::ALValue::array(2, -0.111111, -0.0), AL::ALValue::array(2, 0.377778, 0.0));
  LeftKick_times[12][4] = 5.53333;
  LeftKick_keys[12][4] = AL::ALValue::array(0, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.4, 0.0));
  LeftKick_times[12][5] = 6.73333;
  LeftKick_keys[12][5] = AL::ALValue::array(0, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  LeftKick_names[13] = "RHand";
  LeftKick_times[13].arraySetSize(6);
  LeftKick_keys[13].arraySetSize(6);
  
  LeftKick_times[13][0] = 1.13333;
  LeftKick_keys[13][0] = AL::ALValue::array(0, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.688889, 0.0));
  LeftKick_times[13][1] = 3.2;
  LeftKick_keys[13][1] = AL::ALValue::array(0, AL::ALValue::array(2, -0.688889, -0.0), AL::ALValue::array(2, 0.288889, 0.0));
  LeftKick_times[13][2] = 4.06667;
  LeftKick_keys[13][2] = AL::ALValue::array(0, AL::ALValue::array(2, -0.288889, -0.0), AL::ALValue::array(2, 0.111111, 0.0));
  LeftKick_times[13][3] = 4.4;
  LeftKick_keys[13][3] = AL::ALValue::array(0, AL::ALValue::array(2, -0.111111, -0.0), AL::ALValue::array(2, 0.377778, 0.0));
  LeftKick_times[13][4] = 5.53333;
  LeftKick_keys[13][4] = AL::ALValue::array(0, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.4, 0.0));
  LeftKick_times[13][5] = 6.73333;
  LeftKick_keys[13][5] = AL::ALValue::array(0, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  LeftKick_names[14] = "LHipYawPitch";
  LeftKick_times[14].arraySetSize(6);
  LeftKick_keys[14].arraySetSize(6);
  
  LeftKick_times[14][0] = 1.13333;
  LeftKick_keys[14][0] = AL::ALValue::array(-0.00456004, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.688889, 0.0));
  LeftKick_times[14][1] = 3.2;
  LeftKick_keys[14][1] = AL::ALValue::array(-0.167164, AL::ALValue::array(2, -0.688889, -0.0), AL::ALValue::array(2, 0.288889, 0.0));
  LeftKick_times[14][2] = 4.06667;
  LeftKick_keys[14][2] = AL::ALValue::array(-0.101202, AL::ALValue::array(2, -0.288889, -0.0), AL::ALValue::array(2, 0.111111, 0.0));
  LeftKick_times[14][3] = 4.4;
  LeftKick_keys[14][3] = AL::ALValue::array(-0.0475121, AL::ALValue::array(2, -0.111111, -0.0), AL::ALValue::array(2, 0.377778, 0.0));
  LeftKick_times[14][4] = 5.53333;
  LeftKick_keys[14][4] = AL::ALValue::array(-0.174835, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.4, 0.0));
  LeftKick_times[14][5] = 6.73333;
  LeftKick_keys[14][5] = AL::ALValue::array(-0.00609404, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  LeftKick_names[15] = "LHipRoll";
  LeftKick_times[15].arraySetSize(6);
  LeftKick_keys[15].arraySetSize(6);
  
  LeftKick_times[15][0] = 1.13333;
  LeftKick_keys[15][0] = AL::ALValue::array(0.00609404, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.688889, 0.0));
  LeftKick_times[15][1] = 3.2;
  LeftKick_keys[15][1] = AL::ALValue::array(-0.151824, AL::ALValue::array(2, -0.688889, -0.0), AL::ALValue::array(2, 0.288889, 0.0));
  LeftKick_times[15][2] = 4.06667;
  LeftKick_keys[15][2] = AL::ALValue::array(-0.12728, AL::ALValue::array(2, -0.288889, -0.0), AL::ALValue::array(2, 0.111111, 0.0));
  LeftKick_times[15][3] = 4.4;
  LeftKick_keys[15][3] = AL::ALValue::array(-0.128814, AL::ALValue::array(2, -0.111111, -0.0), AL::ALValue::array(2, 0.377778, 0.0));
  LeftKick_times[15][4] = 5.53333;
  LeftKick_keys[15][4] = AL::ALValue::array(-0.144154, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.4, 0.0));
  LeftKick_times[15][5] = 6.73333;
  LeftKick_keys[15][5] = AL::ALValue::array(-0.00609404, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  LeftKick_names[16] = "LHipPitch";
  LeftKick_times[16].arraySetSize(6);
  LeftKick_keys[16].arraySetSize(6);
  
  LeftKick_times[16][0] = 1.13333;
  LeftKick_keys[16][0] = AL::ALValue::array(-0.44797, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.688889, 0.0));
  LeftKick_times[16][1] = 3.2;
  LeftKick_keys[16][1] = AL::ALValue::array(-0.420624, AL::ALValue::array(2, -0.688889, -0.0), AL::ALValue::array(2, 0.288889, 0.0));
  LeftKick_times[16][2] = 4.06667;
  LeftKick_keys[16][2] = AL::ALValue::array(0.357464, AL::ALValue::array(2, -0.288889, -0.0), AL::ALValue::array(2, 0.111111, 0.0));
  LeftKick_times[16][3] = 4.4;
  LeftKick_keys[16][3] = AL::ALValue::array(-1.15046, AL::ALValue::array(2, -0.111111, -0.0), AL::ALValue::array(2, 0.377778, 0.0));
  LeftKick_times[16][4] = 5.53333;
  LeftKick_keys[16][4] = AL::ALValue::array(-0.421808, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.4, 0.0));
  LeftKick_times[16][5] = 6.73333;
  LeftKick_keys[16][5] = AL::ALValue::array(-0.443284, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  LeftKick_names[17] = "LKneePitch";
  LeftKick_times[17].arraySetSize(6);
  LeftKick_keys[17].arraySetSize(6);
  
  LeftKick_times[17][0] = 1.13333;
  LeftKick_keys[17][0] = AL::ALValue::array(0.699545, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.688889, 0.0));
  LeftKick_times[17][1] = 3.2;
  LeftKick_keys[17][1] = AL::ALValue::array(1.21335, AL::ALValue::array(2, -0.688889, -0.0), AL::ALValue::array(2, 0.288889, 0.0));
  LeftKick_times[17][2] = 4.06667;
  LeftKick_keys[17][2] = AL::ALValue::array(0.624296, AL::ALValue::array(2, -0.288889, -0.0), AL::ALValue::array(2, 0.111111, 0.0));
  LeftKick_times[17][3] = 4.4;
  LeftKick_keys[17][3] = AL::ALValue::array(1.22869, AL::ALValue::array(2, -0.111111, -0.0), AL::ALValue::array(2, 0.377778, 0.0));
  LeftKick_times[17][4] = 5.53333;
  LeftKick_keys[17][4] = AL::ALValue::array(1.20108, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.4, 0.0));
  LeftKick_times[17][5] = 6.73333;
  LeftKick_keys[17][5] = AL::ALValue::array(0.708667, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  LeftKick_names[18] = "LAnklePitch";
  LeftKick_times[18].arraySetSize(6);
  LeftKick_keys[18].arraySetSize(6);
  
  LeftKick_times[18][0] = 1.13333;
  LeftKick_keys[18][0] = AL::ALValue::array(-0.360449, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.688889, 0.0));
  LeftKick_times[18][1] = 3.2;
  LeftKick_keys[18][1] = AL::ALValue::array(-0.618244, AL::ALValue::array(2, -0.688889, -0.0), AL::ALValue::array(2, 0.288889, 0.0));
  LeftKick_times[18][2] = 4.06667;
  LeftKick_keys[18][2] = AL::ALValue::array(-1.15054, AL::ALValue::array(2, -0.288889, -0.0), AL::ALValue::array(2, 0.111111, 0.0));
  LeftKick_times[18][3] = 4.4;
  LeftKick_keys[18][3] = AL::ALValue::array(-0.150374, AL::ALValue::array(2, -0.111111, -0.0), AL::ALValue::array(2, 0.377778, 0.0));
  LeftKick_times[18][4] = 5.53333;
  LeftKick_keys[18][4] = AL::ALValue::array(-0.619779, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.4, 0.0));
  LeftKick_times[18][5] = 6.73333;
  LeftKick_keys[18][5] = AL::ALValue::array(-0.358999, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  LeftKick_names[19] = "LAnkleRoll";
  LeftKick_times[19].arraySetSize(6);
  LeftKick_keys[19].arraySetSize(6);
  
  LeftKick_times[19][0] = 1.13333;
  LeftKick_keys[19][0] = AL::ALValue::array(-0.00771196, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.688889, 0.0));
  LeftKick_times[19][1] = 3.2;
  LeftKick_keys[19][1] = AL::ALValue::array(-0.141086, AL::ALValue::array(2, -0.688889, -0.0), AL::ALValue::array(2, 0.288889, 0.0));
  LeftKick_times[19][2] = 4.06667;
  LeftKick_keys[19][2] = AL::ALValue::array(-0.0383081, AL::ALValue::array(2, -0.288889, -0.0), AL::ALValue::array(2, 0.111111, 0.0));
  LeftKick_times[19][3] = 4.4;
  LeftKick_keys[19][3] = AL::ALValue::array(-0.12728, AL::ALValue::array(2, -0.111111, -0.0), AL::ALValue::array(2, 0.377778, 0.0));
  LeftKick_times[19][4] = 5.53333;
  LeftKick_keys[19][4] = AL::ALValue::array(-0.138018, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.4, 0.0));
  LeftKick_times[19][5] = 6.73333;
  LeftKick_keys[19][5] = AL::ALValue::array(4.19617e-05, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  LeftKick_names[20] = "RHipRoll";
  LeftKick_times[20].arraySetSize(6);
  LeftKick_keys[20].arraySetSize(6);
  
  LeftKick_times[20][0] = 1.13333;
  LeftKick_keys[20][0] = AL::ALValue::array(-0.00617796, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.688889, 0.0));
  LeftKick_times[20][1] = 3.2;
  LeftKick_keys[20][1] = AL::ALValue::array(0.00310997, AL::ALValue::array(2, -0.688889, -0.0), AL::ALValue::array(2, 0.288889, 0.0));
  LeftKick_times[20][2] = 4.06667;
  LeftKick_keys[20][2] = AL::ALValue::array(0.00464395, AL::ALValue::array(2, -0.288889, -0.0), AL::ALValue::array(2, 0.111111, 0.0));
  LeftKick_times[20][3] = 4.4;
  LeftKick_keys[20][3] = AL::ALValue::array(0.0184499, AL::ALValue::array(2, -0.111111, -0.0), AL::ALValue::array(2, 0.377778, 0.0));
  LeftKick_times[20][4] = 5.53333;
  LeftKick_keys[20][4] = AL::ALValue::array(0.00924597, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.4, 0.0));
  LeftKick_times[20][5] = 6.73333;
  LeftKick_keys[20][5] = AL::ALValue::array(-0.00609404, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  LeftKick_names[21] = "RHipPitch";
  LeftKick_times[21].arraySetSize(6);
  LeftKick_keys[21].arraySetSize(6);
  
  LeftKick_times[21][0] = 1.13333;
  LeftKick_keys[21][0] = AL::ALValue::array(-0.31765, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.688889, 0.0));
  LeftKick_times[21][1] = 3.2;
  LeftKick_keys[21][1] = AL::ALValue::array(-0.792379, AL::ALValue::array(2, -0.688889, -0.0), AL::ALValue::array(2, 0.288889, 0.0));
  LeftKick_times[21][2] = 4.06667;
  LeftKick_keys[21][2] = AL::ALValue::array(-0.911238, AL::ALValue::array(2, -0.288889, -0.0), AL::ALValue::array(2, 0.111111, 0.0));
  LeftKick_times[21][3] = 4.4;
  LeftKick_keys[21][3] = AL::ALValue::array(-0.18719, AL::ALValue::array(2, -0.111111, -0.0), AL::ALValue::array(2, 0.377778, 0.0));
  LeftKick_times[21][4] = 5.53333;
  LeftKick_keys[21][4] = AL::ALValue::array(-0.80079, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.4, 0.0));
  LeftKick_times[21][5] = 6.73333;
  LeftKick_keys[21][5] = AL::ALValue::array(-0.44797, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  LeftKick_names[22] = "RKneePitch";
  LeftKick_times[22].arraySetSize(6);
  LeftKick_keys[22].arraySetSize(6);
  
  LeftKick_times[22][0] = 1.13333;
  LeftKick_keys[22][0] = AL::ALValue::array(0.690259, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.688889, 0.0));
  LeftKick_times[22][1] = 3.2;
  LeftKick_keys[22][1] = AL::ALValue::array(1.35917, AL::ALValue::array(2, -0.688889, -0.0), AL::ALValue::array(2, 0.288889, 0.0));
  LeftKick_times[22][2] = 4.06667;
  LeftKick_keys[22][2] = AL::ALValue::array(1.25025, AL::ALValue::array(2, -0.288889, -0.0), AL::ALValue::array(2, 0.111111, 0.0));
  LeftKick_times[22][3] = 4.4;
  LeftKick_keys[22][3] = AL::ALValue::array(1.09072, AL::ALValue::array(2, -0.111111, -0.0), AL::ALValue::array(2, 0.377778, 0.0));
  LeftKick_times[22][4] = 5.53333;
  LeftKick_keys[22][4] = AL::ALValue::array(1.37911, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.4, 0.0));
  LeftKick_times[22][5] = 6.73333;
  LeftKick_keys[22][5] = AL::ALValue::array(0.70108, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  LeftKick_names[0] = "RAnklePitch";
  LeftKick_times[0].arraySetSize(6);
  LeftKick_keys[0].arraySetSize(6);
  
  LeftKick_times[0][0] = 1.13333;
  LeftKick_keys[0][0] = AL::ALValue::array(-0.352862, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.688889, 0.0));
  LeftKick_times[0][1] = 3.2;
  LeftKick_keys[0][1] = AL::ALValue::array(-0.566003, AL::ALValue::array(2, -0.688889, -0.0), AL::ALValue::array(2, 0.288889, 0.0));
  LeftKick_times[0][2] = 4.06667;
  LeftKick_keys[0][2] = AL::ALValue::array(-0.546063, AL::ALValue::array(2, -0.288889, -0.0), AL::ALValue::array(2, 0.111111, 0.0));
  LeftKick_times[0][3] = 4.4;
  LeftKick_keys[0][3] = AL::ALValue::array(-0.67952, AL::ALValue::array(2, -0.111111, -0.0), AL::ALValue::array(2, 0.377778, 0.0));
  LeftKick_times[0][4] = 5.53333;
  LeftKick_keys[0][4] = AL::ALValue::array(-0.553732, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.4, 0.0));
  LeftKick_times[0][5] = 6.73333;
  LeftKick_keys[0][5] = AL::ALValue::array(-0.358915, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.0, 0.0));
  
  LeftKick_names[1] = "RAnkleRoll";
  LeftKick_times[1].arraySetSize(6);
  LeftKick_keys[1].arraySetSize(6);
  
  LeftKick_times[1][0] = 1.13333;
  LeftKick_keys[1][0] = AL::ALValue::array(0.00762803, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.688889, 0.0));
  LeftKick_times[1][1] = 3.2;
  LeftKick_keys[1][1] = AL::ALValue::array(-0.314428, AL::ALValue::array(2, -0.688889, -0.0), AL::ALValue::array(2, 0.288889, 0.0));
  LeftKick_times[1][2] = 4.06667;
  LeftKick_keys[1][2] = AL::ALValue::array(-0.286815, AL::ALValue::array(2, -0.288889, -0.0), AL::ALValue::array(2, 0.111111, 0.0));
  LeftKick_times[1][3] = 4.4;
  LeftKick_keys[1][3] = AL::ALValue::array(-0.263807, AL::ALValue::array(2, -0.111111, -0.0), AL::ALValue::array(2, 0.377778, 0.0));
  LeftKick_times[1][4] = 5.53333;
  LeftKick_keys[1][4] = AL::ALValue::array(-0.315962, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.4, 0.0));
  LeftKick_times[1][5] = 6.73333;
  LeftKick_keys[1][5] = AL::ALValue::array(0.00771196, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.0, 0.0)); 
  
  
}

int LeftKick::ExecutePost() {
  return motion->post.angleInterpolationBezier(LeftKick_names, LeftKick_times, LeftKick_keys); 
} 

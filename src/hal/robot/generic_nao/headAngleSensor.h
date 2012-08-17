#ifndef _headAngleSensor_h_
#define _headAngleSensor_h_ 1

#include "hal/robot/robotSensor.h"
#include "hal/robot/generic_nao/kAlBroker.h"

class HeadAngleSensor : public RobotSensor
{

public:

	HeadAngleSensor();

	float GetHeadAngle();
};

#endif  // _headAngleSensor_h_

#include "hal/robot/generic_nao/headAngleSensor.h"


#include <alproxy.h>

#include <vector>
#include <string>

HeadAngleSensor::HeadAngleSensor() 
{
        ;
}

float HeadAngleSensor::GetHeadAngle() {
    std::vector<std::string>  name;
    name.push_back("HeadYaw");
    bool ssf=true;
    return 
        KAlBroker::Instance()->GetBroker()->getProxy("ALMotion")->call<std::vector<float> >("getAngles",name,ssf)[0];
}


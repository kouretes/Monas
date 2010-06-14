#include "KCameraTransformation.h"
#include "hal/robot/generic_nao/kAlBroker.h"
#include "tools/logger.h"
#include "tools/toString.h"

#include <vector>

using namespace std;

KCameraTranformation::KCameraTranformation(){}
void KCameraTranformation::Init()
{
    try {
		motion = KAlBroker::Instance().GetBroker()->getMotionProxy();
	} catch (AL::ALError& e) {
		Logger::Instance().WriteMsg("KCameraTransformation","Error in getting motion proxy",Logger::FatalError);
	}

}
float  KCameraTranformation::cot(float theta)
{
    //float  t;
    //if(theta<KMat::transformations::PI/4.0)
    return 1.0/tan((double)theta);
    //else
        //t=1.0/tan((double)theta);
    //return t;
};

std::vector<float>  KCameraTranformation::getKinematics(const std::string toAsk)
{
    //Name, space, useSensors
    //TORSO=0
    //WORLD=1
    //NAO=2
   std::vector<float> values= motion->getPosition(toAsk, 2, true);
   return values;


}

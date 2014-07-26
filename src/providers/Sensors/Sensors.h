#ifndef SENSORS_H
#define SENSORS_H


#include "hal/robot/nao/generic_nao/aldebaran-sensors.h"
#include "hal/robot/nao/generic_nao/robot_consts.h"
#include "messages/SensorsMessage.pb.h"
#include <vector>
#include <map>
#include <string>

#include "core/include/IProvider.hpp"

#define DEBOUNCE_MILLISEC 20
#define MCLICKDISTANCE_MILLISEC   300




PROVIDER_START
class Sensors: public IProvider
{
public:
	PROVIDER_CONSTRUCTOR(Sensors);
	int PROVIDER_VISIBLE IEX_DIRECTIVE_HOT Execute();

	void PROVIDER_VISIBLE UserInit();
	const std::string PROVIDER_VISIBLE GetName() const
	{
		return "Sensors";
	}
	PROVIDER_VISIBLE ~Sensors()
	{
	}
	ButtonMessage		BM;

private:

	AL::DCMProxy *dcm;
	boost::shared_ptr<AL::ALMotionProxy> motion;
	AL::ALValue commands;//,stiffnessCommand;
	//	boost::shared_ptr<AL::ALMemoryFastAccess> MemoryFastAccess;

	void initialization();
	bool updateButtons();



#ifndef KROBOT_IS_REMOTE
	void synchronisedDCMcallback();
	boost::shared_ptr<AL::ALMemoryProxy> memory;
	std::vector<float*> buttonValues;
	size_t  buttoneventstopicid;
#else

	AL::ALMemoryFastAccess jointaccess, sensoraccess, buttonaccess;
	std::vector<float> jointValues, sensorValues, buttonValues;
#endif /* KROBOT_IS_REMOTE_OFF*/

	struct buttonstate
	{
		KSystem::Time::TimeAbsolute last_pressed;
		unsigned char last_val, count;
	};
	std::vector<struct buttonstate> buttonevnts;





	RtTime rtm;
	unsigned int timediff;

	bool firstrun, anglefilterreset;

	float smoothness; //sensordata = smoothness*value + (1-smoothness)*oldvalue

};
PROVIDER_END
#endif


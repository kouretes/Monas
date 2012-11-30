#ifndef SENSORS_H
#define SENSORS_H


#include "hal/robot/generic_nao/aldebaran-sensors.h"
#include "messages/SensorsMessage.pb.h"
#include <vector>
#include <map>
#include <string>

#include "architecture/executables/IProvider.h"
#include "tools/stat/cumulative.h"
#include "hal/robot/generic_nao/robot_consts.h"
#include "tools/stat/kalman.h"

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
		AL::ALValue commands;
		commands.arraySetSize(3);
		commands[0] = std::string("Device/SubDeviceList/US/Actuator/Value");
		commands[1] = std::string("Merge");
		commands[2].arraySetSize(1);
		commands[2][0].arraySetSize(2);
		commands[2][0][0] = 0.0;
		commands[2][0][1] = dcm->getTime(1000);
		dcm->set(commands);
	}
	RobotPositionMessage RPM;
	ButtonMessage		BM;
	AllSensorValuesMessage ASM;

private:

	AL::DCMProxy *dcm;
	boost::shared_ptr<AL::ALMotionProxy> motion;
	KMath::Kalman1D<float> angle[2];
	AL::ALValue commands;//,stiffnessCommand;
	//	boost::shared_ptr<AL::ALMemoryFastAccess> MemoryFastAccess;

	void initialization();

	sample_counter sc;
	cumulative_central_moments<float> gyravg[KDeviceLists::GYR_SIZE];
	cumulative_central_moments<float> accnorm;

	void fillComputedData(unsigned int timediff);
	void fetchValues();
	bool updateButtons();



#ifndef KROBOT_IS_REMOTE
	void synchronisedDCMcallback();
	boost::shared_ptr<AL::ALMemoryProxy> memory;
	std::vector<float*> jointValues, sensorValues, buttonValues;
	size_t sensorstopicid, buttoneventstopicid;
#else

	AL::ALMemoryFastAccess jointaccess, sensoraccess, buttonaccess;
	std::vector<float> jointValues, sensorValues, buttonValues;
#endif /* KROBOT_IS_REMOTE_OFF*/

	struct buttonstate
	{
		boost::posix_time::ptime last_pressed;
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


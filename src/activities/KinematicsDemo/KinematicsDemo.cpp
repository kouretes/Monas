#include "KinematicsDemo.h"
#include "messages/SensorsMessage.pb.h"
#include "hal/robot/generic_nao/robot_consts.h"

#include "hal/robot/generic_nao/kAlBroker.h"

#include "core/elements/math/Common.hpp"
#include "hal/robot/generic_nao/aldebaran-sensors.h"
#include <boost/date_time/posix_time/posix_time.hpp>

#include "core/include/Logger.hpp"
#include "tools/toString.h"
#include "hal/robot/generic_nao/KinematicsDefines.h"

#define LHipRange (LHipPitchHigh - LHipPitchLow)
#define RHipRange (RHipPitchHigh - RHipPitchLow)

#define W_LShoulderPitchLow  0.7
#define W_RShoulderPitchLow  0.7

#define LShoulderPitchRange (LShoulderPitchHigh - W_LShoulderPitchLow)
#define RShoulderPitchRange (LShoulderPitchHigh - W_RShoulderPitchLow)

#define CalcLShoulderPitch(RHiPitch_v)  ((RHiPitch_v -  RHipPitchLow)/RHipRange)*1.2*LShoulderPitchRange + W_LShoulderPitchLow + 0.1
#define CalcRShoulderPitch(LHiPitch_v)  ((LHiPitch_v -  LHipPitchLow)/LHipRange)*1.2*RShoulderPitchRange + W_RShoulderPitchLow + 0.1


//using boost::posix_time::milliseconds;
ACTIVITY_REGISTER(KinematicsDemo)
;

void KinematicsDemo::UserInit()
{
	/**
	 Initializing instances need by the Walk Engine
	 **/


	/**
	 Initializing DCM
	 **/
	initialise_devices();


	/**
	 Set Body Stiffness
	 **/
	setStiffness(0.5f);



	Reset();
}

void KinematicsDemo::Reset()
{

	setStiffness(0.75f);
	std::cout << "Walk Engine Reseted" << std::endl;
	sleep(1);
}

int KinematicsDemo::Execute()
{
	static bool firstrun = true;

	if (firstrun) //Initializer atPreProcess call back so the DCMcallback function will be executed every 10 ms
	{
		KAlBroker::Instance().GetBroker()->getProxy("DCM")->getModule()->atPostProcess(KALBIND(&KinematicsDemo::DCMcallback, this));
		firstrun = false;
	}
	return 0;
}



int KinematicsDemo::DCMcallback()
{

	/** Read Values of joints **/
	for (int j = 0, i = 0; i < KDeviceLists::NUMOFJOINTS; i++, j++)
		alljoints[j] = *jointPtr[i];

	alljoints[KDeviceLists::R_LEG+KDeviceLists::HIP_YAW_PITCH]=alljoints[KDeviceLists::L_LEG+KDeviceLists::HIP_YAW_PITCH];

	nkin.setJoints(alljoints); //Feed to kinematics
	NAOKinematics::kmatTable Tl,Tr;
	float dz=-alljoints[KDeviceLists::HEAD+KDeviceLists::PITCH]/HeadPitchHigh;
	float dy=alljoints[KDeviceLists::HEAD+KDeviceLists::YAW]/HeadYawHigh;
	dz*=30;
	dy*=20;

#ifdef ANALYTICAL
	std::vector<float> joints_action,l,r;
	Tl.identity();
	Tr.identity();
	Tl(1,4)=dy+25;
	Tl(2,4)=dz-260;
	Tr(1,4)=dy-25;
	Tr(2,4)=dz-260;
	l=nkin.inverseLeftLeg(Tl);
	r=nkin.inverseLeftLeg(Tr);
	joints_action.push_back(l);
	joints_action.push_back(r);
	if (joints_action.size() != 12)
	{
		//std::cerr << "Not enough joint values" << std::endl;

			int *test;
			test=0;
			*test=5;
		return 0;
	}
	int p;
	for (p = 0; p < KDeviceLists::LEG_SIZE * 2; p++)
		commands[5][(p)][0] = (float) joints_action[p];
	//Left Shoulder use right hip value

	//Send command
	try
	{
		/// Get time in 0 ms
		int DCMtime = dcm->getTime(0);
		commands[4][0] = DCMtime;
		dcm->setAlias(commands);
	} catch (const AL::ALError &e)
	{
		throw ALERROR("KWalk", "execute_action", "Error when sending command to DCM : " + e.toString());
	}





#endif // ANALYTICAL

	return 0;
}

void KinematicsDemo::initialise_devices()
{

	try
	{
		dcm = new AL::DCMProxy(KAlBroker::Instance().GetBroker());
	} catch (AL::ALError& e)
	{
		//Logger::Instance().WriteMsg("MotionController", "Error in getting dcm proxy", Logger::FatalError);
		LogEntry(LogLevel::FatalError, GetName()) << "Error in getting dcm proxy" << e.getDescription();
	}
	try
	{
		memory = KAlBroker::Instance().GetBroker()->getMemoryProxy();
	} catch (AL::ALError& e)
	{
		//Logger::Instance().WriteMsg("AutoBalance", "Error in getting ALmemory proxy", Logger::FatalError);
		LogEntry(LogLevel::FatalError, GetName()) << "Error in getting ALmemory proxy" << e.getDescription();
	}
	try
	{
		motion = new AL::ALMotionProxy(boost::shared_ptr<AL::ALBroker>(KAlBroker::Instance().GetBroker()));
	} catch (AL::ALError& e)
	{
		LogEntry(LogLevel::FatalError, GetName()) << "Error in getting motion proxy" << e.getDescription();
	}
	//Initialise ptr
	std::vector<std::string> jointKeys = KDeviceLists::getJointKeys();
	std::vector<std::string> sensorKeys = KDeviceLists::getSensorKeys();

	KMath::KMat::GenMatrix<double,3,4> fsrposl,fsrposr;
	fsrposl(0,0)=memory->getData("Device/SubDeviceList/LFoot/FSR/FrontLeft/Sensor/XPosition");
	fsrposl(0,1)=memory->getData("Device/SubDeviceList/LFoot/FSR/FrontRight/Sensor/XPosition");
	fsrposl(0,2)=memory->getData("Device/SubDeviceList/LFoot/FSR/RearLeft/Sensor/XPosition");
	fsrposl(0,3)=memory->getData("Device/SubDeviceList/LFoot/FSR/RearRight/Sensor/XPosition");

	fsrposr(0,0)=memory->getData("Device/SubDeviceList/RFoot/FSR/FrontLeft/Sensor/XPosition");
	fsrposr(0,1)=memory->getData("Device/SubDeviceList/RFoot/FSR/FrontRight/Sensor/XPosition");
	fsrposr(0,2)=memory->getData("Device/SubDeviceList/RFoot/FSR/RearLeft/Sensor/XPosition");
	fsrposr(0,3)=memory->getData("Device/SubDeviceList/RFoot/FSR/RearRight/Sensor/XPosition");


	fsrposl(1,0)=memory->getData("Device/SubDeviceList/LFoot/FSR/FrontLeft/Sensor/YPosition");
	fsrposl(1,1)=memory->getData("Device/SubDeviceList/LFoot/FSR/FrontRight/Sensor/YPosition");
	fsrposl(1,2)=memory->getData("Device/SubDeviceList/LFoot/FSR/RearLeft/Sensor/YPosition");
	fsrposl(1,3)=memory->getData("Device/SubDeviceList/LFoot/FSR/RearRight/Sensor/YPosition");

	fsrposr(1,0)=memory->getData("Device/SubDeviceList/RFoot/FSR/FrontLeft/Sensor/YPosition");
	fsrposr(1,1)=memory->getData("Device/SubDeviceList/RFoot/FSR/FrontRight/Sensor/YPosition");
	fsrposr(1,2)=memory->getData("Device/SubDeviceList/RFoot/FSR/RearLeft/Sensor/YPosition");
	fsrposr(1,3)=memory->getData("Device/SubDeviceList/RFoot/FSR/RearRight/Sensor/YPosition");


	sensorPtr.resize(KDeviceLists::NUMOFSENSORS);
	for (int i = 0; i < KDeviceLists::NUMOFSENSORS; i++)
	{
		sensorPtr[i] = (float *) memory->getDataPtr(sensorKeys[i]);
	}

	jointPtr.resize(KDeviceLists::NUMOFJOINTS);
	for (int i = 0; i < KDeviceLists::NUMOFJOINTS; i++)
	{

		jointPtr[i] = (float *) memory->getDataPtr(jointKeys[i]);
		std::cout << jointKeys[i] << i << " " << jointPtr[i] << std::endl;
		//sleep(1);

	}

	std::cout << " Number of position joints " << jointPtr.size() << std::endl;
	std::cout << " Number of sensor values " << sensorPtr.size() << std::endl;
	createJointsPositionActuatorAlias();
	prepareJointsPositionActuatorCommand();

	sleep(1);

	alljoints.resize(KDeviceLists::NUMOFJOINTS, 0);

	LogEntry(LogLevel::Info, GetName()) << "Devices initialized!";

}

void KinematicsDemo::prepareJointsPositionActuatorCommand()
{
	commands.arraySetSize(6);
	commands[0] = std::string("jointActuator");
	commands[1] = std::string("ClearAll"); /// Erase all previous commands
	commands[2] = std::string("time-separate");
	commands[3] = 0;

	commands[4].arraySetSize(1);
//commands[4][0]  Will be the new time
// to control

	commands[5].arraySetSize(KDeviceLists::LEG_SIZE * 2 + 2); // For joints //2legs + 2 hip_pitch

	for (int i = 0; i < KDeviceLists::LEG_SIZE * 2 ; i++)
		commands[5][i].arraySetSize(1);
//commands[5][i][0] will be the new angle

}

void KinematicsDemo::createJointsPositionActuatorAlias()
{
	AL::ALValue jointAliasses;

	std::vector<std::string> jointActuatorKeys = KDeviceLists::getPositionActuatorKeys();

	jointAliasses.arraySetSize(2);
	jointAliasses[0] = std::string("jointActuator"); // Alias for all joint actuators

	jointAliasses[1].arraySetSize(KDeviceLists::LEG_SIZE * 2 ); //

	//int idx = 0;
	// Joints actuator list
	std::string actuatorname;
	int l = 0;
	for (int j = KDeviceLists::HIP_YAW_PITCH; j < KDeviceLists::LEG_SIZE; j++, l++)
	{
		actuatorname = jointActuatorKeys[KDeviceLists::L_LEG + j];
		jointAliasses[1][l] = actuatorname;
		std::cout << " Joint Name " << actuatorname << " " << std::endl;
	}

	for (int j = KDeviceLists::HIP_YAW_PITCH; j < KDeviceLists::LEG_SIZE; j++, l++)
	{
		actuatorname = jointActuatorKeys[KDeviceLists::R_LEG + j];
		jointAliasses[1][l] = actuatorname;
		std::cout << " Joint Name " << actuatorname << " " << std::endl;
	}
///*Create Joint Alias*
	try
	{
		dcm->createAlias(jointAliasses);
	} catch (const AL::ALError &e)
	{
		//throw ALERROR("mainModule", "createPositionActuatorAlias()", "Error when creating Alias : " + e.toString());
		LogEntry(LogLevel::FatalError, GetName()) << "Error when creating Alias : " << e.getDescription();
	}

	std::cout << " Ankles PositionActuatorAlias created " << std::endl;
}

void KinematicsDemo::setStiffness(const float& stiffnessValue)
{
	motion->setStiffnesses("Body", stiffnessValue);
}


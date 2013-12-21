#include "KinematicsDemo2.h"
#include "messages/SensorsMessage.pb.h"
#include "hal/robot/generic_nao/robot_consts.h"

#include "hal/robot/generic_nao/kAlBroker.h"

#include "core/elements/math/Common.hpp"
#include "hal/robot/generic_nao/aldebaran-sensors.h"
#include <boost/date_time/posix_time/posix_time.hpp>

#include "core/include/Logger.hpp"
#include "tools/toString.h"
#include "hal/robot/generic_nao/KinematicsDefines.h"

#define ANALYTICAL


//using boost::posix_time::milliseconds;
ACTIVITY_REGISTER(KinematicsDemo2)
;

void KinematicsDemo2::UserInit()
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

	_blk.updateSubscription("sensors", msgentry::SUBSCRIBE_ON_TOPIC);
	angX=angY=-1000.0;
	Reset();
}

void KinematicsDemo2::Reset()
{

	setStiffness(0.5);
	std::cout << "Walk Engine Reseted" << std::endl;
	sleep(1);
}

int KinematicsDemo2::Execute()
{
	static bool firstrun = true;


		if (firstrun) //Initializer atPreProcess call back so the DCMcallback function will be executed every 10 ms
		{
			KAlBroker::Instance().GetBroker()->getProxy("DCM")->getModule()->atPostProcess(KALBIND(&KinematicsDemo2::DCMcallback, this));
			firstrun = false;
		}
	
	return 0;
}



int KinematicsDemo2::DCMcallback()
{
	
	allsm = _blk.readData<AllSensorValuesMessage> ("sensors");
	if(allsm != 0){
		angX = allsm->computeddata(ANGLE + AXIS_X).sensorvalue();
		angY = allsm->computeddata(ANGLE + AXIS_Y).sensorvalue();
	}else{
		return 0;
	}
	std::vector<float> oldall;
	NAOKinematics::kmatTable Tl,Tr;
	NAOKinematics::kmatTable rotSpace;
	std::vector<float> joints_action;
	NAOKinematics::AngleContainer l,r;
	
	oldall=alljoints;
	/** Read Values of joints **/
	for (int j = 0, i = 0; i < KDeviceLists::NUMOFJOINTS; i++, j++)
		alljoints[j] = *jointPtr[i];

	alljoints[KDeviceLists::R_LEG+KDeviceLists::HIP_YAW_PITCH]=alljoints[KDeviceLists::L_LEG+KDeviceLists::HIP_YAW_PITCH];

	nkin.setJoints(alljoints); //Feed to kinematics

	//attack here
	
	KVecDouble3 com = nkin.calculateCenterOfMass();
	KMath::KMat::transformations::makeRotationZYX(rotSpace,0.0,angY,angX);
	KVecDouble3 comPoint;

	//comPoint = rotSpace.transform(com);
	comPoint=com;
	NAOKinematics::kmatTable rotTrans;
	rotTrans = rotSpace;
	rotTrans(0,3) = comPoint(0);
	rotTrans(1,3) = comPoint(1);//+50;
	rotTrans(2,3) = -300;//-260;
	rotSpace.fast_invert();
	rotSpace*=rotTrans;

	l=nkin.inverseLeftLeg(rotSpace);
	std::cout << l.size() << " " << angX << " " << angY << std::endl;
	if(l.size()>0)
	{
		joints_action.insert(joints_action.end(),l[0].begin(),l[0].end());
		joints_action.insert(joints_action.end(),l[0].begin(),l[0].end());
	}
	if (joints_action.size() != 12)
	{
		//std::cerr << "Not enough joint values" << std::endl;

		return 0;
	}
	int p;
	for (p = 0; p < KDeviceLists::LEG_SIZE*2 ; p++)
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

	return 0;
}

void KinematicsDemo2::initialise_devices()
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

void KinematicsDemo2::prepareJointsPositionActuatorCommand()
{
	commands.arraySetSize(6);
	commands[0] = std::string("jointActuator");
	commands[1] = std::string("ClearAll"); /// Erase all previous commands
	commands[2] = std::string("time-separate");
	commands[3] = 0;

	commands[4].arraySetSize(1);
//commands[4][0]  Will be the new time
// to control

	commands[5].arraySetSize(KDeviceLists::LEG_SIZE * 2); // For joints //2legs + 2 hip_pitch

	for (int i = 0; i < KDeviceLists::LEG_SIZE * 2 ; i++)
		commands[5][i].arraySetSize(1);
//commands[5][i][0] will be the new angle

}

void KinematicsDemo2::createJointsPositionActuatorAlias()
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
	for (int j =0; j < KDeviceLists::LEG_SIZE; j++, l++)
	{
		actuatorname = jointActuatorKeys[KDeviceLists::L_LEG + j];
		jointAliasses[1][l] = actuatorname;
		std::cout << " Joint Name " << actuatorname << " " << std::endl;
	}

	for (int j = 0; j < KDeviceLists::LEG_SIZE; j++, l++)
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

void KinematicsDemo2::setStiffness(const float& stiffnessValue)
{
	motion->setFallManagerEnabled(false);
	motion->setStiffnesses("Body", 0.0);
		motion->setStiffnesses("LLeg", stiffnessValue);

}


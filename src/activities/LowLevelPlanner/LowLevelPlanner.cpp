#include "LowLevelPlanner.h"
#include "messages/SensorsMessage.pb.h"
#include "hal/robot/nao/generic_nao/robot_consts.h"

#include "hal/robot/nao/generic_nao/kAlBroker.h"

#include "core/elements/math/Common.hpp"
#include "hal/robot/nao/generic_nao/aldebaran-sensors.h"
#include <boost/date_time/posix_time/posix_time.hpp>

#include "core/include/Logger.hpp"
#include "tools/toString.h"
#include "hal/robot/nao/generic_nao/KinematicsDefines.h"

//#define LHipRange (LHipPitchHigh - LHipPitchLow)
//#define RHipRange (RHipPitchHigh - RHipPitchLow)

//#define W_LShoulderPitchLow  0.7
//#define W_RShoulderPitchLow  0.7

#define ARMROLL (10*3.14f/180)

//#define LShoulderPitchRange (LShoulderPitchHigh - W_LShoulderPitchLow)
//#define RShoulderPitchRange (LShoulderPitchHigh - W_RShoulderPitchLow)

//#define CalcLShoulderPitch(RHiPitch_v)  (((RHiPitch_v) -  RHipPitchLow)/RHipRange)*1.2*LShoulderPitchRange + W_LShoulderPitchLow + 0.1
//#define CalcRShoulderPitch(LHiPitch_v)  (((LHiPitch_v) -  LHipPitchLow)/LHipRange)*1.2*RShoulderPitchRange + W_RShoulderPitchLow + 0.1


//using boost::posix_time::milliseconds;
ACTIVITY_REGISTER(LowLevelPlanner)
;

void LowLevelPlanner::UserInit()
{
	/**
	 Initializing instances need by the Walk Engine
	 **/


	engine =new WalkEngine(NaoRobot);
	/**
	 Initializing DCM
	 **/
	initialise_devices();



	std::cout << "Walk Engine Initialized" << std::endl;
	dcm_counter = 0; //dcm_counter = -1;
	/**
	 Set Body Stiffness
	 **/
	setStiffness(0.5f);
	state = DO_NOTHING;
	dcm_state = DCM_STOP;

    _blk.updateSubscription("buttonevents", Messaging::MessageEntry::SUBSCRIBE_ON_TOPIC);

	Reset();
}

void LowLevelPlanner::Reset()
{

	setStiffness(1.0f);
	std::cout << "Walk Engine Reseted" << std::endl;
	sleep(1);
}

int LowLevelPlanner::Execute()
{
	static bool firstrun = true;

	if (firstrun) //Initializer atPreProcess call back so the DCMcallback function will be executed every 10 ms
	{
		KAlBroker::Instance().GetBroker()->getProxy("DCM")->getModule()->atPostProcess(KALBIND(&LowLevelPlanner::DCMcallback, this));
		firstrun = false;
	}



    if (state == DO_NOTHING)
    {
        dcm_counter = 0;
        state = INIT_WALK;
        std::cout << "Message arrived should initialize walk" << std::endl;
    }


	speed.push_back(0);
	speed.push_back(0);
	speed.push_back(0);



    boost::shared_ptr<const ButtonMessage> bm = _blk.readSignal<ButtonMessage>("buttonevents");
    int chest=0;
    if(bm)
        chest=bm->data(KDeviceLists::CHEST_BUTTON);
    if(chest==1)
        state=IDLE;
    if(chest>1)
    {
        //speed[2]=0.05;
        if(chest==3)
            speed[0]=1;
        else
            speed[0]=0;
        state=DO_STEPS;

    }

	switch (state)
	{
		case DO_NOTHING:
			dcm_state = DCM_STOP;
			return 0;
			break;


		case INIT_WALK:
			dcm_state = INIT_WALK;
			engine->addInit();
			NaoPlanner.initialize(NaoRobot);

			while(NaoPlanner.inst.size()>0)
			{
					engine->walkbuffer.add(NaoPlanner.inst.front());
					NaoPlanner.inst.pop();
			}
			//NaoPlanner.oneStep(std::vector<float>(3,0));
			//NaoPlanner.oneStep(std::vector<float>(3,0));
			//NaoPlanner.oneStep(std::vector<float>(3,0));
			//NaoPlanner.oneStep(std::vector<float>(3,0));
			//while(NaoPlanner.inst.size()>0)
			//{
			//		engine->walkbuffer.add(NaoPlanner.inst.front());
			//		NaoPlanner.inst.pop();
			//}

			state=IDLE;
			break;
		case DO_STEPS:

            if(engine->walkbuffer.size()<2)
			{
				NaoPlanner.oneStep(speed);
				while(NaoPlanner.inst.size()>0)
				{
						engine->walkbuffer.add(NaoPlanner.inst.front());
						NaoPlanner.inst.pop();
				}
			}
			break;

		case FINAL_STEP:
			/**
			 Plan a step and make another FINAL so
			 both Feet come parallel
			 **/
			state = WAIT_TO_FINISH;
			break;
		case IDLE:

		    break;
		case WAIT_TO_FINISH:
			return 0;
			break; // nothing to do

		default:
			break;
	}



	return 0;
}



int LowLevelPlanner::DCMcallback()
{




	/** Read Values of joints **/
	for (int j = 0, i = 0; i < KDeviceLists::NUMOFJOINTS; i++, j++)
		alljoints[j] = *jointPtr[i];
	alljoints[KDeviceLists::R_LEG+KDeviceLists::HIP_YAW_PITCH]=alljoints[KDeviceLists::L_LEG+KDeviceLists::HIP_YAW_PITCH];

	fsrl.zero();
	fsrr.zero();
	for(int i=0;i<4;i++)
	{
		fsrl(i)=*sensorPtr[KDeviceLists::L_FSR+i];
		fsrr(i)=*sensorPtr[KDeviceLists::R_FSR+i];
	}

	engine->setFSR(fsrl,fsrr);

	engine->nkin.setJoints(alljoints); //Feed to kinematics
	if(dcm_state==INIT_WALK)
	{
		engine->Reset();
		dcm_state=DCM_RUN;
	}
	if (dcm_state == DCM_STOP) //Nothing to execute
		return 0;

    std::vector<float>joints_action=engine->runStep();

	if (joints_action.size() != 12)
	{
		std::cerr << "Not enough joint values" << std::endl;

			int *test;
			test=0;
			*test=5;
		return 0;
	}

    joints_action[0]=0;
    joints_action[KDeviceLists::LEG_SIZE]=0;
	int p;
	for (p = 0; p < KDeviceLists::LEG_SIZE * 2; p++)

		commands[5][(p)][0] = (float) joints_action[p];
	//Left Shoulder use right hip value
	try{

	commands[5][p+KDeviceLists::SHOULDER_PITCH][0] =engine->armangles(0);
	commands[5][p+KDeviceLists::SHOULDER_ROLL][0] =engine->armangles(1);
	commands[5][p+KDeviceLists::ELBOW_YAW][0] = 0.0f;
	commands[5][p+KDeviceLists::ELBOW_ROLL][0] =-engine->armangles(1);
	p+=KDeviceLists::ARM_SIZE;

	commands[5][p+KDeviceLists::SHOULDER_PITCH][0] = engine->armangles(2);
	commands[5][p+KDeviceLists::SHOULDER_ROLL][0] =-engine->armangles(3);
	commands[5][p+KDeviceLists::ELBOW_YAW][0] = 0.0f;
	commands[5][p+KDeviceLists::ELBOW_ROLL][0] =engine->armangles(3);
	}
	catch (const AL::ALError &e)
	{
		std::cout<<e.toString()<<std::endl;
	}
	//Right Shoulder use left hip value


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

void LowLevelPlanner::initialise_devices()
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
	/*try
	{
		motion = new AL::ALMotionProxy(boost::shared_ptr<AL::ALBroker>(KAlBroker::Instance().GetBroker()));
		motion->setFallManagerEnabled(false);
	} catch (AL::ALError& e)
	{
		LogEntry(LogLevel::FatalError, GetName()) << "Error in getting motion proxy" << e.getDescription();
	}*/
	//Initialise ptr
	std::vector<std::string> jointKeys = KDeviceLists::getJointKeys();
	std::vector<std::string> sensorKeys = KDeviceLists::getSensorKeys();

	fsrposl.zero();
	fsrposr.zero();

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

	engine->initFSR(fsrposl,fsrposr);

	sensorPtr.resize(KDeviceLists::NUMOFSENSORS);
	for (int i = 0; i < KDeviceLists::NUMOFSENSORS; i++)
	{
		sensorPtr[i] = (float *) memory->getDataPtr(sensorKeys[i]);
	}

	jointPtr.resize(KDeviceLists::NUMOFJOINTS);
	for (int i = 0; i < KDeviceLists::NUMOFJOINTS; i++)
	{

		jointPtr[i] = (float *) memory->getDataPtr(jointKeys[i]);


	}

	createHardnessActuatorAlias();
	createJointsPositionActuatorAlias();
	prepareJointsPositionActuatorCommand();

	sleep(1);

	alljoints.resize(KDeviceLists::NUMOFJOINTS, 0);

	LogEntry(LogLevel::Info, GetName()) << "Devices initialized!";

}

void LowLevelPlanner::prepareJointsPositionActuatorCommand()
{
	commands.arraySetSize(6);
	commands[0] = std::string("jointActuator");
	commands[1] = std::string("ClearAll"); /// Erase all previous commands
	commands[2] = std::string("time-separate");
	commands[3] = 0;

	commands[4].arraySetSize(1);


	commands[5].arraySetSize(KDeviceLists::LEG_SIZE * 2 +KDeviceLists::ARM_SIZE*2 ); // For joints //2legs + 2 hip_pitch

	for (int i = 0; i < KDeviceLists::LEG_SIZE * 2  +KDeviceLists::ARM_SIZE*2; i++)
		commands[5][i].arraySetSize(1);

}

void LowLevelPlanner::createJointsPositionActuatorAlias()
{
	AL::ALValue jointAliasses;

	std::vector<std::string> jointActuatorKeys = KDeviceLists::getPositionActuatorKeys();

	jointAliasses.arraySetSize(2);
	jointAliasses[0] = std::string("jointActuator"); // Alias for all joint actuators

	jointAliasses[1].arraySetSize(KDeviceLists::LEG_SIZE * 2 + KDeviceLists::ARM_SIZE*2); //

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

	for (int j = 0; j <  KDeviceLists::ARM_SIZE; j++, l++)
	{
			actuatorname = jointActuatorKeys[KDeviceLists::L_ARM + j];
			jointAliasses[1][l] = actuatorname;
			std::cout << " Joint Name " << actuatorname << " " << std::endl;
		}

	for (int j = 0; j <  KDeviceLists::ARM_SIZE; j++, l++)
	{
			actuatorname = jointActuatorKeys[KDeviceLists::R_ARM + j];
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

void LowLevelPlanner::setStiffness(const float& stiffnessValue)
{
	setStiffnessDCM(stiffnessValue);
	//motion->setStiffnesses("Body", stiffnessValue);
}

void LowLevelPlanner::createHardnessActuatorAlias()
{
	AL::ALValue jointAliasses;
	// Alias for all joint stiffness
	jointAliasses.clear();

	jointAliasses.arraySetSize(2);
	jointAliasses[0] = std::string("jointStiffness"); // Alias for all 25 actuators
	jointAliasses[1].arraySetSize(KDeviceLists::NUMOFJOINTS);
	std::cout <<"size " <<  KDeviceLists::NUMOFJOINTS << std::endl;
	// stiffness list
	std::vector<std::string> HardnessActuatorStrings = KDeviceLists::getHardnessActuatorKeys();
	// Joints actuator list
	for (int i = 0; i < KDeviceLists::NUMOFJOINTS; i++)
	{
		jointAliasses[1][i] = HardnessActuatorStrings[i];
	}
	// Create alias
	try
	{
		dcm->createAlias(jointAliasses);
	} catch (const AL::ALError &e)
	{
		LogEntry(LogLevel::FatalError, GetName()) << "Error when creating Hardness Alias : " << e.getDescription();
		//throw ALERROR(GetName(), "createHardnessActuatorAlias()", "Error when creating Alias : " + e.toString());
	}
}

void LowLevelPlanner::setStiffnessDCM(const float &stiffnessValue)
{
	int DCMtime;
	/** Read Values of joints **/
	for (int j = 0, i = 0; i < KDeviceLists::NUMOFJOINTS; i++, j++)
		alljoints[j] = *jointPtr[i];

	int l=0;
	for (int j = KDeviceLists::HIP_YAW_PITCH; j < KDeviceLists::LEG_SIZE; j++, l++)
		commands[5][(l)][0] = alljoints[KDeviceLists::L_LEG + j];
	for (int j = KDeviceLists::HIP_YAW_PITCH; j < KDeviceLists::LEG_SIZE; j++, l++)
		commands[5][(l)][0] = alljoints[KDeviceLists::R_LEG + j];
	for (int j = 0; j <  KDeviceLists::ARM_SIZE; j++, l++)
		commands[5][(l)][0] = alljoints[KDeviceLists::L_ARM + j];
	for (int j = 0; j <  KDeviceLists::ARM_SIZE; j++, l++)
		commands[5][(l)][0] = alljoints[KDeviceLists::R_ARM + j];

	try
	{
		/// Get time in 0 ms
		DCMtime = dcm->getTime(10);
		commands[4][0] = DCMtime;
		dcm->setAlias(commands);
	} catch (const AL::ALError &e)
	{
		throw ALERROR("DcmStiffness", "execute_action", "Error when sending command to DCM : " + e.toString());
	}
	
	
	AL::ALValue stiffnessCommands;

	// increase stiffness with the "jointStiffness" Alias created at initialisation
	try
	{
		// Get time : return the time in 1 seconde
		DCMtime = dcm->getTime(1000);
	} catch (const AL::ALError &e)
	{
		throw ALERROR(GetName(), "setStiffness()", "Error on DCM getTime : " + e.toString());
	}

	// Prepare one dcm command:
	// it will linearly "Merge" all joint stiffness
	// from last value to "stiffnessValue" in 1 seconde
	stiffnessCommands.arraySetSize(3);
	stiffnessCommands[0] = std::string("jointStiffness");
	stiffnessCommands[1] = std::string("ClearAll");
	stiffnessCommands[2].arraySetSize(1);
	stiffnessCommands[2][0].arraySetSize(2);
	stiffnessCommands[2][0][0] = stiffnessValue;
	stiffnessCommands[2][0][1] = DCMtime;
	try
	{
		dcm->set(stiffnessCommands);
	} catch (const AL::ALError &e)
	{
		throw ALERROR(GetName(), "setStiffness()", "Error when sending stiffness to DCM : " + e.toString());
	}
}

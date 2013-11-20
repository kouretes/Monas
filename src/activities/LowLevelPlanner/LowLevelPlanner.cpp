#include "LowLevelPlanner.h"
#include "messages/SensorsMessage.pb.h"
#include "hal/robot/generic_nao/robot_consts.h"

#include "hal/robot/generic_nao/kAlBroker.h"

#include "core/elements/math/Common.hpp"
#include "hal/robot/generic_nao/aldebaran-sensors.h"
#include <boost/date_time/posix_time/posix_time.hpp>

#include "core/include/Logger.hpp"
#include "tools/toString.h"

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



	Reset();
}

void LowLevelPlanner::Reset()
{

	setStiffness(0.75f);
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

	static int counter = 0; //just for testing
	if ((counter % 2 == 0))
	{
		MotionWalkMessage* wmot = new MotionWalkMessage();
		float x = rand() / ((float) RAND_MAX);
		x = (x - 0.5);
		float y = rand() / ((float) RAND_MAX);
		y = (y - 0.5);
		float z = rand() / ((float) RAND_MAX);
		z = (z - 0.5);
		float s = rand() / ((float) RAND_MAX);

		x = 0.00;
		y = 0.0000;
		z = 0.000;
		s = 1;
		wmot->set_command("setWalkTargetVelocity");
		wmot->add_parameter(x);
		wmot->add_parameter(y);
		wmot->add_parameter(z);
		wmot->add_parameter(s);

		_blk.publishSignal(*wmot, "motion");
		delete wmot;
	}
	counter++;

	/**
	 Message GetSpeed from high level

	 **/

	/**
	 Check if after Stepping the Target is reached
	 **/
	wm = _blk.readSignal<MotionWalkMessage>("motion");
	if (wm != NULL)
	{
		if (wm->command() == "setWalkTargetVelocity")
		{
			if (state == DO_NOTHING)
			{
				dcm_counter = 0;
				state = INIT_WALK;
				std::cout << "Message arrived should initialize walk" << std::endl;
			}
		} else
		{
			return 0;
		}
	} else
		return 0;

	speed.push_back(wm->parameter(0));
	speed.push_back(wm->parameter(1));
	speed.push_back(wm->parameter(2));

	/*if (dcm_length[next_2B_inserted] != 0)
	{
		std::cout << "Buffers are full" << std::endl;
		return 0;
	}*/

	if (speed.size() < 3)
		std::cerr << "Not Enought Speed Values" << std::endl;

	/*if (speed[0] == 0 && speed[1] == 0 && speed[2] == 0)
		state = FINAL_STEP;*/

	//std::cout << " State " << state <<  " next_2B_inserted " << next_2B_inserted << std::endl;

	switch (state)
	{
		case DO_NOTHING:
			dcm_state = DCM_STOP;
			return 0;
			break;


		case INIT_WALK:
			dcm_state = INIT_WALK;
			//engine->Reset();
			engine->addInit();
			NaoPlanner.initialize(NaoRobot);
			//NaoPlanner.initialize(NaoRobot);
			//NaoPlanner.initialize(NaoRobot);
			while(NaoPlanner.inst.size()>0)
			{
					engine->walkbuffer.add(NaoPlanner.inst.front());
					NaoPlanner.inst.pop();
			}


			state=DO_STEPS;
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
			/*Plan two consecutive Steps with the Predefined Speed
			 **/
			//NaoPlanner.oneStep(speed);
			//NaoPlanner.oneStep(speed);


			//Calculate_Tragectories();

			//dcm_state = DCM_RUN;
			break;

		case FINAL_STEP:
			/**
			 Plan a step and make another FINAL so
			 both Feet come parallel
			 **/
			//NaoPlanner.oneStep(speed);
			//NaoPlanner.finalStep();
			//Calculate_Tragectories();
			state = WAIT_TO_FINISH;
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
	KMath::KMat::GenMatrix<double,4,1> fsrl,fsrr;
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
	std::vector<float> joints_action=engine->runStep();



	if (joints_action.size() != 12)
	{
		//std::cerr << "Not enough joint values" << std::endl;

			int *test;
			test=0;
			*test=5;
		return 0;
	}

	for (int p = 0; p < KDeviceLists::LEG_SIZE * 2; p++)
		commands[5][(p)][0] = (float) joints_action[p];

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

void LowLevelPlanner::prepareJointsPositionActuatorCommand()
{
	commands.arraySetSize(6);
	commands[0] = std::string("jointActuator");
	commands[1] = std::string("ClearAll"); /// Erase all previous commands
	commands[2] = std::string("time-separate");
	commands[3] = 0;

	commands[4].arraySetSize(1);
//commands[4][0]  Will be the new time
// to control

	commands[5].arraySetSize(KDeviceLists::LEG_SIZE * 2); // For joints

	for (int i = 0; i < KDeviceLists::LEG_SIZE * 2; i++)
		commands[5][i].arraySetSize(1);
//commands[5][i][0] will be the new angle

}

void LowLevelPlanner::createJointsPositionActuatorAlias()
{
	AL::ALValue jointAliasses;

	std::vector<std::string> jointActuatorKeys = KDeviceLists::getPositionActuatorKeys();

	jointAliasses.arraySetSize(2);
	jointAliasses[0] = std::string("jointActuator"); // Alias for all joint actuators

	jointAliasses[1].arraySetSize(KDeviceLists::LEG_SIZE * 2); //

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

void LowLevelPlanner::setStiffness(const float& stiffnessValue)
{
	motion->setStiffnesses("Body", stiffnessValue);
}


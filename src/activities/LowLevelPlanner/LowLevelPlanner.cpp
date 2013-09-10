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

	NaoPlanner.initialize(NaoRobot);
	NaoZmpTrajectoryPlanner.ZMPTrajectoryInitialize(&NaoPlanner, NaoRobot, KWalkMath);
	NaoFootTrajectoryPlanner.FootTrajectoryInitialize(&NaoPlanner, NaoRobot, KWalkMath);
	NaoLIPMx = new LIPMPreviewController(NaoRobot);
	NaoLIPMy = new LIPMPreviewController(NaoRobot);

    /**
    Initializing DCM
    **/
	initialise_devices();

	std::cout << "Walk Engine Initialized" << std::endl;
	dcm_counter=0;//dcm_counter = -1;
	//Reset(); ??????
}

void LowLevelPlanner::Reset()
{
	NaoPlanner.clear();
	NaoFootTrajectoryPlanner.FootTrajectoryClear();
	NaoZmpTrajectoryPlanner.ZMPTrajectoryClear();
	std::cout << "Walk Engine Reseted" << std::endl;
	dcm_counter = 0;
}

int LowLevelPlanner::Execute()
{


	static int counter = 0;

	if ((counter % 1 == 0))
	{
		MotionWalkMessage* wmot = new MotionWalkMessage();
		float x = rand() / ((float) RAND_MAX);
		x = (x - 0.5);
		float y = rand() / ((float) RAND_MAX);
		y = (y - 0.5);
		float z = rand() / ((float) RAND_MAX);
		z = (z - 0.5);
		float s = rand() / ((float) RAND_MAX);

		x = 4.2;//3.4;
		y = 0;
		z = 0;
		s = 0;
		wmot->set_command("setWalkTargetVelocity");
		wmot->add_parameter(x);
		wmot->add_parameter(y);
		wmot->add_parameter(z);
		wmot->add_parameter(s);

		_blk.publishSignal(*wmot, "motion");
		//Logger::Instance().WriteMsg("MotionController", "Sending Command: setWalkTargetVelocity ", Logger::ExtraInfo);
		//_blk.publishSignal(*wmot, "motion");
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
			;
		} else
		{
			return 0;
		}
	} else
		return 0;

	finalStep = false;
	speed.push_back(wm->parameter(0));
	speed.push_back(wm->parameter(1));
	speed.push_back(wm->parameter(2));

	if (speed.size() < 3)
        std::cerr<<"Not Enought Speed Values"<<std::endl;
		return 0;
	if (finalStep)
	{

		/**
         Plan a step and make another FINAL so
		 both Feet come parallel
		 **/
		NaoPlanner.oneStep(speed);
		NaoPlanner.finalStep();
	} else
	{   /**
	     Plan two concecutive Steps with the Predifined Speed
	    **/
		NaoPlanner.oneStep(speed);
		NaoPlanner.oneStep(speed);
		//NaoPlanner.afterStep();
	}
	/**
	 Update the Foot Trajectory Planner
	 instance with the new planned points
	 **/
	NaoFootTrajectoryPlanner.FootTrajectoryUpdate(&NaoPlanner);
	/**
	 Update the ZMP Trajectory Planner
	 instance with the new planned points
	 **/
	NaoZmpTrajectoryPlanner.ZMPTrajectoryUpdate(&NaoPlanner);

	for (unsigned int i = 0; i < 2; i++)
	{
		/**
		 Calculate the Next Feet Points to Interpolate
		 **/

		NaoFootTrajectoryPlanner.NextTrajectory();

		/**
		 Feet Trajectory computation
		 **/

		NaoFootTrajectoryPlanner.FootTrajectoryInterpolate();

		/**
		 Calculate the Next ZMP Points to Interpolate
		 **/

		NaoZmpTrajectoryPlanner.NextTrajectory();

		/**
		 ZMP Trajectory computation
		 **/

		NaoZmpTrajectoryPlanner.ZMPTrajectoryInterpolate();
	}

	std::cout << "Walk Engine is Executed Walk should start" << std::endl;

	/**
	Set Body Stiffness
	**/
	setStiffness(0.75f);
	std::cout << "Pray !" << std::endl;

	static bool firstrun = true;
	if (firstrun)
	{
		KAlBroker::Instance().GetBroker()->getProxy("DCM")->getModule()->atPreProcess(KALBIND(&LowLevelPlanner::DCMcallback, this));
		//		KAlBroker::Instance().GetBroker()->getProxy("DCM")->getModule()->atPostProcess(KALBIND(&Zmp::DCMcallback , this));

		firstrun = false;
	}

	return 0;
}

void LowLevelPlanner::Calculate_Desired_COM()
{

	NaoLIPMx->LIPMComPredictor(NaoZmpTrajectoryPlanner.ZMPX);
	NaoLIPMy->LIPMComPredictor(NaoZmpTrajectoryPlanner.ZMPY);

}

int LowLevelPlanner::DCMcallback()
{
	if (dcm_counter < 0) return 0;

	Calculate_Desired_COM();
	std::vector<float> joints_action = Calculate_IK();
	if (joints_action.size() != 12)
	{
		std::cerr << "Not enough joint values" <<std::endl;
		return 0;
	}

	for (int p = 0; p < KDeviceLists::LEG_SIZE * 2; p++)
		commands[5][(p)][0] = (float) joints_action[p];

	//commands[5][(KDeviceLists::ANKLE_ROLL)][0] = -joints_action[KDeviceLists::ANKLE_ROLL];
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

	int leg = LEG::LEFT_LEG;

	std::string device_string;

	int idx = 0;
	//Initialise ptr
	std::vector<std::string> jointKeys = KDeviceLists::getJointKeys();
	std::vector<std::string> sensorKeys = KDeviceLists::getSensorKeys();

	for (leg = 0; leg < 2; leg++) //both legs
	{
		if (leg == LEG::LEFT_LEG)
			idx = KDeviceLists::L_FSR;
		else if (leg == LEG::RIGHT_LEG)
			idx = KDeviceLists::R_FSR;
		else
			std::cerr << "The robot has 2 legs only leg :" << leg << " unrecognized.!! " <<std::endl;

		std::string supportleg_prefix = (leg == LEG::LEFT_LEG) ? "L" : "R";
		for (int i = 0; i < 2; i++)
		{
			std::string axis = (i == 0) ? "Y" : "X";
			fsr_position[0][i][leg] = ((i == 0) ? -1 : 1) * (float) memory->getData("Device/SubDeviceList/" + supportleg_prefix + "Foot/FSR/FrontLeft/Sensor/" + axis + "Position");
			fsr_position[1][i][leg] = ((i == 0) ? -1 : 1)
					* (float) memory->getData("Device/SubDeviceList/" + supportleg_prefix + "Foot/FSR/FrontRight/Sensor/" + axis + "Position");
			fsr_position[2][i][leg] = ((i == 0) ? -1 : 1) * (float) memory->getData("Device/SubDeviceList/" + supportleg_prefix + "Foot/FSR/RearLeft/Sensor/" + axis + "Position");
			fsr_position[3][i][leg] = ((i == 0) ? -1 : 1) * (float) memory->getData("Device/SubDeviceList/" + supportleg_prefix + "Foot/FSR/RearRight/Sensor/" + axis + "Position");

			for (int j = 0; j < 4; j++)
				std::cout << "FSR pos axis " << axis << " " << j << " " << (float) fsr_position[j][i][leg] <<std::endl;
		}

		for (int i = idx; i < (idx + 4/* KDeviceLists::FSR_SIZE*/); i++)
		{
			device_string = sensorKeys[i];
			sensorPtr.push_back((float *) memory->getDataPtr(device_string));
			std::cout << "sensorKeys[" << i - idx << "]:  " << device_string <<std::endl;

		}
	}

	for (int i = 0; i < KDeviceLists::GYR_SIZE; i++)
	{
		sensorPtr.push_back((float *) memory->getDataPtr(sensorKeys[KDeviceLists::GYR + i]));
		std::cout << "sensorKeys[end]:  " << sensorKeys[KDeviceLists::GYR + i] <<std::endl;
		//cout << "Value[" << i << "]: " << *(sensorPtr.end()) << endl;
		std::cout << "sensorPtr[" << sensorPtr.size() - 1 << "]: " << sensorKeys[KDeviceLists::GYR + i] <<std::endl;

	}
	std::cout << "sensorPtr.size" << sensorPtr.size();
	for (int i = 0; i < KDeviceLists::ACC_SIZE; i++)
	{
		sensorPtr.push_back((float *) memory->getDataPtr(sensorKeys[KDeviceLists::ACC + i]));

		std::cout << "sensorPtr[" << sensorPtr.size() - 1 << "]: " << sensorKeys[KDeviceLists::ACC + i] <<std::endl;

	}

	jointPtr.resize(KDeviceLists::NUMOFJOINTS);
	for (int i = 0; i < KDeviceLists::NUMOFJOINTS; i++)
	{

		jointPtr[i] = (float *) memory->getDataPtr(jointKeys[i]);
		std::cout << jointKeys[i] << i << " " << jointPtr[i] <<std::endl;
		//sleep(1);

	}

	std::cout << " Number of position joints " << jointPtr.size() <<std::endl;
	createJointsPositionActuatorAlias();
	prepareJointsPositionActuatorCommand();

	sleep(1);

	alljoints.resize(KDeviceLists::NUMOFJOINTS, 0);

	nkin = new NAOKinematics();

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
		std::cout << " Joint Name " << actuatorname << " " <<std::endl;
	}

	for (int j = KDeviceLists::HIP_YAW_PITCH; j < KDeviceLists::LEG_SIZE; j++, l++)
	{
		actuatorname = jointActuatorKeys[KDeviceLists::R_LEG + j];
		jointAliasses[1][l] = actuatorname;
		std::cout << " Joint Name " << actuatorname << " " <<std::endl;
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

	std::cout << " Ankles PositionActuatorAlias created " <<std::endl;
}

void LowLevelPlanner::setStiffness(const float& stiffnessValue)
{

	motion->setStiffnesses("Body", stiffnessValue);

}

std::vector<float> LowLevelPlanner::Calculate_IK()
{
	float NewL[3], NewR[3];
	float yawL,yawR;
	std::vector<float> ret;
	if (NaoFootTrajectoryPlanner.Xl.size() < dcm_counter)
	{
		std::cout << " Empty plan !" <<std::endl;
		return ret;
	}

	/** Read Values of joints **/
	for (int j = 0, i = 0; i < KDeviceLists::NUMOFJOINTS; i++, j++)
		alljoints[j] = *jointPtr[i];

	/** Calculate COM
	//NAOKinematics::FKvars calc_com = nkin->calculateCenterOfMass(alljoints);
    **/

	NewL[0] = 1000 * (NaoFootTrajectoryPlanner.Xl[dcm_counter] - NaoLIPMx->COM[dcm_counter])+ 18.18 -30 ;
	NewL[1] = 1000 * (NaoFootTrajectoryPlanner.Yl[dcm_counter] - NaoLIPMy->COM[dcm_counter])- 0.0726;
	NewL[2] = 1000 * (NaoFootTrajectoryPlanner.Zl[dcm_counter] - NaoRobot.getRobotParameter("ComZ") )- 46.76;

    NewR[0] = 1000 * (NaoFootTrajectoryPlanner.Xr[dcm_counter] - NaoLIPMx->COM[dcm_counter])+ 18.18 -30; //+ calc_com.pointX;
	NewR[1] = 1000 * (NaoFootTrajectoryPlanner.Yr[dcm_counter] - NaoLIPMy->COM[dcm_counter])- 0.0726;// + calc_com.pointY;
	NewR[2] = 1000 * (NaoFootTrajectoryPlanner.Zr[dcm_counter] - NaoRobot.getRobotParameter("ComZ")- 46.76e-3);// + calc_com.pointZ;

	dcm_counter++;

	//std::cout << "Left(end+1,:) = [\t" << NewL[0] << " " << NewL[1] << " " << NewL[2] << "]; \tRight(end+1,:)= [ " << NewR[0] << " " << NewR[1] << " " << NewR[2] << "];" <<std::endl;

	yawL=0;
	yawR=0;
	/*if(dcm_counter>0){
	   yawL=NaoFootTrajectoryPlanner.Thl[dcm_counter]-NaoFootTrajectoryPlanner.Thl[dcm_counter-1];
	   yawR=NaoFootTrajectoryPlanner.Thr[dcm_counter]-NaoFootTrajectoryPlanner.Thr[dcm_counter-1];
        }
	else
	{
	   yawL=NaoFootTrajectoryPlanner.Thl[dcm_counter];
	   yawR=NaoFootTrajectoryPlanner.Thr[dcm_counter];
	}*/
	std::vector<std::vector<float> > resultR, resultL;
	/**std::vector<float> empty, controlled_leg_l, controlled_leg_r;

	controlled_leg_l.resize(6);
	controlled_leg_r.resize(6);

	for (int i = KDeviceLists::HIP_YAW_PITCH, j = 0; i < KDeviceLists::LEG_SIZE; i++, j++)
	{
		controlled_leg_l[j] = *jointPtr[KDeviceLists::L_LEG + i];
		controlled_leg_r[j] = *jointPtr[KDeviceLists::R_LEG + i];
	}
    **/
	resultL = nkin->inverseLeftLeg(NewL[0], NewL[1], NewL[2], 0, 0, yawL);
	resultR = nkin->inverseRightLeg(NewR[0], NewR[1], NewR[2], 0, 0,yawR);

	if (!resultL.empty())
	{
		ret = resultL.at(0);
		if (!resultR.empty())
			ret.insert(ret.end(), resultR.at(0).begin(), resultR.at(0).end());
		else
			std::cerr << "Right Leg EMPTY VECTOR " <<std::endl;
	} else
		std::cerr << "Left Leg EMPTY VECTOR " <<std::endl;

	//std::cout << " Number of joint values : " << ret.size() <<std::endl;

	return ret;
}

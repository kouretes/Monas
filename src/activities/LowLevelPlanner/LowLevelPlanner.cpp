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
	ZmpBuffer[X] = new CircularBuffer<float>(51);
	ZmpBuffer[Y] = new CircularBuffer<float>(51);

	next_2B_inserted = 0;

	std::cout << "Walk Engine Initialized" << std::endl;
	dcm_counter = 0; //dcm_counter = -1;
	/**
	 Set Body Stiffness
	 **/
	setStiffness(0.5f);
	state = DO_NOTHING;
	dcm_state = DCM_STOP;

	supportleg=KDeviceLists::SUPPORT_LEG_NONE;

	Tilerror.identity();
	Tirerror.identity();


	Reset();
}

void LowLevelPlanner::Reset()
{
	memset(FeetTrajectory, 0, sizeof(float) * (2 * 4 * 2 * MAX_TRAJECTORY_LENGTH));
	memset(ZmpTrajectory, 0, sizeof(float) * (2 * 2 * MAX_TRAJECTORY_LENGTH));

	dcm_counter = 0;
	dcm_length[0] = 0;
	dcm_length[1] = 0;

	current_buffer = 0;
	setStiffness(0.9f);
	std::cout << "Walk Engine Reseted" << std::endl;
	sleep(1);
}

int LowLevelPlanner::Execute()
{
	static bool firstrun = true;

	if (firstrun) //Initializer atPreProcess call back so the DCMcallback function will be executed every 10 ms
	{
		Tis.identity();
		supportleg=KDeviceLists::SUPPORT_LEG_NONE;
		KAlBroker::Instance().GetBroker()->getProxy("DCM")->getModule()->atPreProcess(KALBIND(&LowLevelPlanner::DCMcallback, this));
		firstrun = false;
	}

	static int counter = 0; //just for testing

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

		x =0.00;
		y = 0.001;
		z = 0.0;
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

	finalStep = false;
	speed.push_back(wm->parameter(0));
	speed.push_back(wm->parameter(1));
	speed.push_back(wm->parameter(2));

	if (dcm_length[next_2B_inserted] != 0)
	{
		std::cout << "Buffers are full" << std::endl;
		return 0;
	}

	if (speed.size() < 3)
		std::cerr << "Not Enought Speed Values" << std::endl;

	/*if (speed[0] == 0 && speed[1] == 0 && speed[2] == 0)
		state = FINAL_STEP;*/

	//std::cout << " State " << state <<  " next_2B_inserted " << next_2B_inserted << std::endl;

	switch (state)
	{
		case DO_NOTHING:
			return 0;
			break;


		case INIT_WALK:
			//initialize
			int zmplength;
			zmplength = NaoZmpTrajectoryPlanner.ZMPTrajectoryInitStep(ZmpTrajectory[next_2B_inserted]);

			dcm_length[next_2B_inserted] = NaoFootTrajectoryPlanner.FootTrajectoryInitStep(FeetTrajectory[next_2B_inserted]);

			//std::cout << " Zmplength " << zmplength << " foot len: " << dcm_length[next_2B_inserted] << std::endl;

			for (int i = 0; i < dcm_length[next_2B_inserted] && i < 51; i++)
			{
				ZmpBuffer[X]->cbPush(ZmpTrajectory[next_2B_inserted][X][i]);
				ZmpBuffer[Y]->cbPush(ZmpTrajectory[next_2B_inserted][Y][i]);
			}
			state = DO_STEPS;
			dcm_counter = 0;

			std::cout << "Walk Engine is Executed Walk should start" << std::endl;

			break;

		case DO_STEPS:
			/*Plan two consecutive Steps with the Predefined Speed
			 **/
			NaoPlanner.oneStep(speed);
			NaoPlanner.oneStep(speed);

			Calculate_Tragectories();

			dcm_state = DCM_RUN;
			break;

		case FINAL_STEP:
			/**
			 Plan a step and make another FINAL so
			 both Feet come parallel
			 **/
			NaoPlanner.oneStep(speed);
			NaoPlanner.finalStep();
			Calculate_Tragectories();
			state = WAIT_TO_FINISH;
			break;
		case WAIT_TO_FINISH:
			return 0;
			break; // nothing to do

		default:
			break;
	}

	last = next_2B_inserted;
	next_2B_inserted++;
	next_2B_inserted %= 2;

	return 0;
}

void LowLevelPlanner::Calculate_Tragectories()
{
	int X=0, ready_size;
	for (unsigned int i = 0; i < 2; i++)
	{
		/**
		 Calculate the Next Feet Points to Interpolate
		 **/

		NaoFootTrajectoryPlanner.NextTrajectory();

		/**
		 Feet Trajectory computation
		 **/

		X = NaoFootTrajectoryPlanner.FootTrajectoryInterpolate(FeetTrajectory[next_2B_inserted], i);
		/**
		 Calculate the Next ZMP Points to Interpolate
		 **/

		NaoZmpTrajectoryPlanner.NextTrajectory();

		/**
		 ZMP Trajectory computation
		 **/

		ready_size = NaoZmpTrajectoryPlanner.ZMPTrajectoryInterpolate(ZmpTrajectory[next_2B_inserted], i);
	}

	dcm_length[next_2B_inserted] = X ;
}

void LowLevelPlanner::Calculate_Desired_COM()
{



	KVecFloat2 copi=getCoP();
	NAOKinematics::kmatTable t=nkin->getForwardEffector((NAOKinematics::Effectors)chainsupport);
	t.fast_invert();
	KVecDouble3 CoMm =(Tis*t).transform(nkin->calculateCenterOfMass());// (Tis*t).transform();
	CoMm.scalar_mult(1.0/1000.0);

	//CoMm.prettyPrint();

	NaoLIPMx->LIPMComPredictor(*ZmpBuffer[X],CoMm(0),copi(0));

	NaoLIPMy->LIPMComPredictor(*ZmpBuffer[Y],CoMm(1),copi(1));

	//std::cout<<NaoLIPMx->Com<<" "<<NaoLIPMy->Com<<std::endl;

	if (dcm_length[current_buffer] > (dcm_counter + PreviewWindow))
	{
		ZmpBuffer[X]->cbPush(ZmpTrajectory[current_buffer][X][dcm_counter + PreviewWindow]);
		ZmpBuffer[Y]->cbPush(ZmpTrajectory[current_buffer][Y][dcm_counter + PreviewWindow]);
	} else
	{
		//buffer at the end must load from the other buffer
		int other_buffer = (current_buffer + 1) % 2;
		if (dcm_length[other_buffer] > 0)
		{ //the other buffer is ready to read

			if (!ZmpBuffer[X]->cbIsFull()) /// must fill now
			{
				int p;

				int pos = dcm_counter + PreviewWindow - dcm_length[current_buffer]; //we need more
				//std::cout <<" Need to add " << pos << std::endl;
				for (p = 0; p <pos ; p++)
				{
					ZmpBuffer[X]->cbPush(ZmpTrajectory[other_buffer][X][p]);
					ZmpBuffer[Y]->cbPush(ZmpTrajectory[other_buffer][Y][p]);
				}
			} else
			{ //just load the next from the other ready buffer

				int pos = dcm_counter + PreviewWindow - dcm_length[current_buffer];
				ZmpBuffer[X]->cbPush(ZmpTrajectory[other_buffer][X][pos]);
				ZmpBuffer[Y]->cbPush(ZmpTrajectory[other_buffer][Y][pos]);
			}
		} else
		{
			//oups the other buffer is not ready, emptying
			ZmpBuffer[X]->pop();
			ZmpBuffer[Y]->pop();
		}
	}
	//if(ZmpBuffer[X]->size()  < 51)
	//	std::cout << " ZmpBuffer size " << ZmpBuffer[X]->size() << std::endl;
}

KVecFloat2 LowLevelPlanner::getCoP()
{

	KVecFloat2 res;
	KMath::KMat::GenMatrix<double,4,1> fsrl,fsrr;
	KVecDouble3 copl,copr,copi,cops,copsprime;
	float weightl,weightr,weights, weightsprime;

	for(int i=0;i<4;i++)
	{
		fsrl(i)=*sensorPtr[KDeviceLists::L_FSR+i];
		fsrr(i)=*sensorPtr[KDeviceLists::R_FSR+i];
	}
	/*fsrl.prettyPrint();
	fsrr.prettyPrint();*/
	/*fsrposl.prettyPrint();
	fsrposr.prettyPrint();*/
	//weightl=sqrt(fsrl.norm2());
	//weightr=sqrt(fsrr.norm2());

	weightl=fsrl(0)+fsrl(1)+fsrl(2)+fsrl(3);
	weightr=fsrr(0)+fsrr(1)+fsrr(2)+fsrr(3);
	//fsrposr=fsrposl;
	copl=fsrposl*fsrl;
	copr=fsrposr*fsrr;

	copl.scalar_mult(1./(weightl));
	copr.scalar_mult(1./(weightr));

	if(weightl<0.05)
	{
		copl.zero();
		weightl=0;
	}

	if(weightr<0.05)
	{
		copr.zero();
		weightr=0;
	}
#define Margin 0.002

	if(weightl==0 || weightr==0)
	{
		if(copl(1)>=fsrposl(0,1)-Margin || copl(1)<= fsrposl(1,1)+Margin || copl(0)>=fsrposl(0,0)-Margin ||copl(0)<=fsrposl(2,0)+Margin)
		{
			weightl=0;
			copl.zero();
		}


		if(copr(1)>=fsrposr(0,1) -Margin|| copr(1)<= fsrposr(1,1)+Margin || copr(0)>=fsrposr(0,0)-Margin ||copr(0)<=fsrposr(2,0)+Margin)
		{
			weightr=0;
			copr.zero();
		}

	}

	copl.scalar_mult(1000);
	copr.scalar_mult(1000);

	/*copl.prettyPrint();
	copr.prettyPrint();*/

	/*copr.prettyPrint();
	copl.prettyPrint();*/
	if(chainsupport==KDeviceLists::CHAIN_R_LEG)
	{
		cops=copr;
		copsprime=copl;
		weights=weightr;
		weightsprime=weightl;
	}
	else
	{
		cops=copl;
		copsprime=copr;
		weights=weightl;
		weightsprime=weightr;
	}



	//std::cout<<"-----------"<<std::endl;
	/*cops.prettyPrint();
	copsprime.prettyPrint();
	std::cout<<weights<<" "<<weightsprime<<std::endl; */
    //copi=(Tis.transform(cops)).scalar_mult(weights)+ ((Tis*Tssprime).transform(copsprime)).scalar_mult(weightsprime);
    copi=(Tis.transform(cops)).scalar_mult(weights)+ ((Tis*Tssprime).transform(copsprime)).scalar_mult(weightsprime);
    copi.scalar_mult(1./(weights+weightsprime));


    copi.scalar_mult(1.0/1000.0);
    //std::cout<<"COP"<<std::endl;
    //copi.prettyPrint();
    if((weights+weightsprime)<1)
		copi.zero();

	res(0)=copi(0);
	res(1)=copi(1);
	//res.prettyPrint();

	return res;
}

int LowLevelPlanner::DCMcallback()
{

	if (dcm_state == DCM_STOP) //Nothing to execute
		return 0;

	if (dcm_counter >= dcm_length[current_buffer]) //buffer end;
	{
		std::cout << "Switch Buffer" << std::endl;
		int next_buffer = (current_buffer + 1) % 2;
		if (dcm_length[next_buffer] > 0)
		{ //ready to switch buffers
			dcm_length[current_buffer] = 0;
			current_buffer = next_buffer;
			dcm_counter = 0;

			KMath::KMat::transformations::makeTransformation(Tis,
									(double)FeetTrajectory[current_buffer][X][LEFT][dcm_counter]*1000,
									(double)FeetTrajectory[current_buffer][Y][LEFT][dcm_counter]*1000,
									(double)FeetTrajectory[current_buffer][Z][LEFT][dcm_counter]*1000,
									0.0,
									0.0,
									(double)FeetTrajectory[current_buffer][Theta][LEFT][dcm_counter]);
		} else
		{
			//provlima i telos mallon telos
			std::cout << "Just stopping " << std::endl;
			state = DO_NOTHING;
			dcm_state = DCM_STOP;
			dcm_counter = 0;
			return 0;
		}
	}

	/** Read Values of joints **/
	for (int j = 0, i = 0; i < KDeviceLists::NUMOFJOINTS; i++, j++)
	alljoints[j] = *jointPtr[i];

	nkin->setJoints(alljoints); //Feed to kinematics

	KDeviceLists::SupportLeg oldsupportleg=supportleg;

	float pe=(NaoRobot.getWalkParameter(Tds)/2)/NaoRobot.getWalkParameter(Tstep);
	bool rightsupport=dcm_counter>(dcm_length[current_buffer]/2)*pe &&
					   dcm_counter<(dcm_length[current_buffer]/2)*(1+pe);

    double_support=(dcm_counter<(dcm_length[current_buffer]/2)*pe*2) ||
					(dcm_counter>dcm_length[current_buffer]/2 &&
					 dcm_counter<(dcm_length[current_buffer]/2)*(1+pe*2));
	//rightsupport=true;
	//progress of double support
	if(dcm_counter<dcm_length[current_buffer]/2.0)
	{
		double_support_progress=(dcm_counter*1.0)/(dcm_length[current_buffer]/2.0);
	}
	else
	{
		double_support_progress=(dcm_counter*1.0-dcm_length[current_buffer]/2.0)/(dcm_length[current_buffer]/2);
	}
	double_support_progress/=(NaoRobot.getWalkParameter(Tds)/NaoRobot.getWalkParameter(Tstep));

	if(double_support==false)
			double_support_progress=0;
	std::cout<<"double_support_progress:"<<double_support_progress<<std::endl;
	if(rightsupport==true)
		supportleg=KDeviceLists::SUPPORT_LEG_RIGHT;
	else
		supportleg=KDeviceLists::SUPPORT_LEG_LEFT;

	//float percentage_ss=NaoRobot.getWalkParameter(Tss)/NaoRobot.getWalkParameter(Tstep);
	//double_support=dcm_counter>dcm_length[current_buffer]*percentage_ss/2;


	//nextleg=KDeviceLists::SUPPORT_LEG_RIGHT;
	chainsupport= (rightsupport==true)?KDeviceLists::CHAIN_R_LEG:KDeviceLists::CHAIN_L_LEG;

	KDeviceLists::ChainsNames otherleg= (rightsupport==true)?
										KDeviceLists::CHAIN_L_LEG:KDeviceLists::CHAIN_R_LEG;
	NAOKinematics::kmatTable merger=nkin->getForwardFromTo(
															(NAOKinematics::Effectors)chainsupport,
															(NAOKinematics::Effectors)otherleg
														  );

	NAOKinematics::FKvars t;
	t.p=merger.getTranslation();
	t.a=merger.getEulerAngles();

	t.p(2)/=10000.0;
	t.a(0)/=10000.0;
	t.a(1)/=10000.0;

	Tssprime=NAOKinematics::getTransformation(t);



	if(oldsupportleg==KDeviceLists::SUPPORT_LEG_NONE) //Initialize to support leg
	{
		std::cout<<"RESET ODOMETRY----"<<std::endl;
		Tis=nkin->getForwardEffector((NAOKinematics::Effectors)chainsupport);

		NAOKinematics::FKvars t;
		t.p=Tis.getTranslation();
		t.a=Tis.getEulerAngles();
		t.p(2)=0;
		t.a(0)=0;
		t.a(1)=0;




		Tis=NAOKinematics::getTransformation(t);
		//Tis.fast_invert();
	}

	else if(oldsupportleg!=supportleg )
	{
		NAOKinematics::kmatTable t=Tssprime;
		t.fast_invert();
		std::cout<<"SWITCH LEG----"<<std::endl;
		Tis*=t;
	}



	//std::cout << dcm_counter << " " << current_buffer << " " << dcm_length[current_buffer] << std::endl;
	Calculate_Desired_COM();




	//std::cout<<"Sup:"<<whichleg<<","<<nextleg<<std::endl;

	std::vector<float> joints_action = Calculate_IK();
	dcm_counter++;
	if (joints_action.size() != 12)
	{
		std::cerr << "Not enough joint values" << std::endl;

			/*int *test;
			test=0;
			*test=5;*/
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


	/*for(int i=0;i<4;i++)
	{
		fsrposl(1,i)=-fsrposl(1,i);
		fsrposr(1,i)=-fsrposr(1,i);
	}*/

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

std::vector<float> LowLevelPlanner::Calculate_IK()
{

		//Get Tps
	NAOKinematics::kmatTable Tsp=nkin->getForwardEffector((NAOKinematics::Effectors)chainsupport);
	Tsp.fast_invert();//Tps->Tsp


	NAOKinematics::kmatTable Tip,Tpi,Til,Tir;
	Tip=Tis*Tsp;

	Tpi=Tip;
	Tpi.fast_invert();//Get Inverse transform

	float corr=double_support_progress;
	//if(double_support)
		//corr=1;
	if(corr>0.5) corr=corr-0.5;
	corr*=2;
	if(corr<0.7) corr=0.7;





	if(dcm_counter>0)
	{

		NAOKinematics::kmatTable l=nkin->getForwardEffector((NAOKinematics::Effectors)KDeviceLists::CHAIN_L_LEG);
		NAOKinematics::kmatTable r=nkin->getForwardEffector((NAOKinematics::Effectors)KDeviceLists::CHAIN_R_LEG);
		NAOKinematics::kmatTable Tilold,Tirold;

		KMath::KMat::transformations::makeTransformation(Tilold,
				(double)FeetTrajectory[current_buffer][X][LEFT][dcm_counter-1]*1000,
				(double)FeetTrajectory[current_buffer][Y][LEFT][dcm_counter-1]*1000,
				(double)FeetTrajectory[current_buffer][Z][LEFT][dcm_counter-1]*1000,
				0.0,
				0.0,
				(double)FeetTrajectory[current_buffer][Theta][LEFT][dcm_counter-1]
				);

		KMath::KMat::transformations::makeTransformation(Tirold,
				(double)FeetTrajectory[current_buffer][X][RIGHT][dcm_counter-1]*1000,
				(double)FeetTrajectory[current_buffer][Y][RIGHT][dcm_counter-1]*1000,
				(double)FeetTrajectory[current_buffer][Z][RIGHT][dcm_counter-1]*1000,
				0.0,
				0.0,
				(double)FeetTrajectory[current_buffer][Theta][RIGHT][dcm_counter-1]
				);
		l=Tip*l;
		l.fast_invert();
		r=Tip*r;
		r.fast_invert();
		Tilerror=l*Tilold;
		Tirerror=r*Tirold;

		NAOKinematics::FKvars t;
		t.p=Tilerror.getTranslation();
		t.a=Tilerror.getEulerAngles();

		t.p.scalar_mult(corr/100);
		t.a.scalar_mult(corr/100);

		Tilerror=NAOKinematics::getTransformation(t);

		t.p=Tirerror.getTranslation();
		t.a=Tirerror.getEulerAngles();

		t.p.scalar_mult(corr/100);
		t.a.scalar_mult(corr/100);

		Tirerror=NAOKinematics::getTransformation(t);






	}



	/*std::cout<<"Tip:"<<std::endl;
	Tip.prettyPrint();
	Tpi.prettyPrint();*/

	//Construct Ti{l,r}
	KMath::KMat::transformations::makeTransformation(Til,
				(double)FeetTrajectory[current_buffer][X][LEFT][dcm_counter]*1000,
				(double)FeetTrajectory[current_buffer][Y][LEFT][dcm_counter]*1000,
				(double)FeetTrajectory[current_buffer][Z][LEFT][dcm_counter]*1000,
				0.0,
				0.0,
				(double)FeetTrajectory[current_buffer][Theta][LEFT][dcm_counter]
				);
	KMath::KMat::transformations::makeTransformation(Tir,
				(double)FeetTrajectory[current_buffer][X][RIGHT][dcm_counter]*1000,
				(double)FeetTrajectory[current_buffer][Y][RIGHT][dcm_counter]*1000,
				(double)FeetTrajectory[current_buffer][Z][RIGHT][dcm_counter]*1000,
				0.0,
				0.0,
				(double)FeetTrajectory[current_buffer][Theta][RIGHT][dcm_counter]
				);
	NAOKinematics::kmatTable Tpprimei,Tipprime; //Transformation of next pelvis p' pprime :)


	KVecDouble3 com_error,desired;//All in inertial frame;
	//std::cout<<NaoRobot.getWalkParameter(ComZ)<<std::endl;
	desired=KVecDouble3( NaoLIPMx->Com,NaoLIPMy->Com,NaoRobot.getWalkParameter(ComZ)).scalar_mult(1000);
	/*desired(1)/=2;
	desired(2)/=2;*/
	//Til.getTranslation().prettyPrint();
	//.prettyPrint();
	//Tis.getTranslation().prettyPrint();
	//desired.prettyPrint();
	std::vector<float> ret;
	Tipprime=Tip;


	for(unsigned iter=0;iter<2;iter++)
	{

		KVecDouble3 measured = nkin->calculateCenterOfMass();
		//measured(2)=-40;
		com_error=desired;
		com_error-=Tipprime.transform(measured);


		com_error(2)*=corr;


		//if(double_support==false)
			//com_error(2)*=0.99;
		//com_error(2)*=0.1;

		//com_error.scalar_mult(0.001);
		//com_error(2)/=2;
		//com_error.scalar_mult(-1);
		//com_error.scalar_mult(0.5);
		//com_error(2)*=2;
		//com_error.prettyPrint();
		//com_error.zero();
		//com_error(0)*=0.6;
		//com_error.scalar_mult(0.999);
		//com_error.scalar_mult(1.0/100);
		//Fix rotation first, using yawpitchroll coupling
		//First generate Tipprime and then invert
		KMath::KMat::transformations::makeRotationXYZ(Tpprimei,
													0.0,
													0.0,
													(double)
													(FeetTrajectory[current_buffer][Theta][LEFT][dcm_counter]+
													 FeetTrajectory[current_buffer][Theta][RIGHT][dcm_counter])/2
													);

		//Tpprimei.identity();
		Tpprimei.setTranslation(com_error+Tipprime.getTranslation());
		//Tpprimei(2,3)=270;
		Tipprime=Tpprimei;


		Tpprimei.fast_invert(); // Tip'->Tp'i
		//Generate inverse kin targets as Tp'{l,r}
		//Tpprimei.prettyPrint();

		NAOKinematics::kmatTable Tpprimel,Tpprimer;

		Tpprimel=Tpprimei*Til*Tilerror;
		Tpprimer=Tpprimei*Tir*Tirerror;

		/*Tpprimel.prettyPrint();
		Tpprimer.prettyPrint();*/


		/*Tpprimel(0,3)-=30;
		Tpprimer(0,3)-=30;*/





		//(com_error).prettyPrint();


		std::vector<std::vector<float> > resultR, resultL;
		/*

		KVecDouble3 l=KVecDouble3(
				(double)FeetTrajectory[current_buffer][X][LEFT][dcm_counter]*1000,
				(double)FeetTrajectory[current_buffer][Y][LEFT][dcm_counter]*1000,
				(double)FeetTrajectory[current_buffer][Z][LEFT][dcm_counter]*1000
				);
		KVecDouble3 r=KVecDouble3(
				(double)FeetTrajectory[current_buffer][X][RIGHT][dcm_counter]*1000,
				(double)FeetTrajectory[current_buffer][Y][RIGHT][dcm_counter]*1000,
				(double)FeetTrajectory[current_buffer][Z][RIGHT][dcm_counter]*1000
				);
		KVecDouble3 temptemp;
		temptemp=desired;
		temptemp-=measured;
		temptemp.scalar_mult(-1);
		l=l+temptemp;
		r=r+temptemp;

		Tpprimel.identity();
		Tpprimer.identity();
		Tpprimel.setTranslation(l);
		Tpprimer.setTranslation(r);
		*/

		resultL = nkin->inverseLeftLeg(Tpprimel);
		resultR = nkin->inverseRightLeg(Tpprimer);


		if (!resultL.empty())
		{
			ret = resultL.at(0);
			if (!resultR.empty())
			{
				ret.insert(ret.end(), resultR.at(0).begin(), resultR.at(0).end());
				nkin->setChain(KDeviceLists::CHAIN_L_LEG,resultL[0]);
				nkin->setChain(KDeviceLists::CHAIN_R_LEG,resultR[0]);
			}

			else
				std::cerr << "Right Leg EMPTY VECTOR " << std::endl;
		} else
			std::cerr << "Left Leg EMPTY VECTOR " << std::endl;


	}

	//com_error.prettyPrint();

	//Store Com for feedback


	//std::cout << " Number of joint values : " << ret.size() <<std::endl;






	//ret.clear();
	return ret;
}


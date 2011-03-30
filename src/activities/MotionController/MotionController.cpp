#include "MotionController.h"

#include "hal/robot/generic_nao/kAlBroker.h"
#include "hal/robot/generic_nao/robot_consts.h"
#include "hal/robot/generic_nao/aldebaran-sensors.h"

#include "tools/logger.h"
#include "tools/toString.h"

#include "ISpecialAction.h"
#include "KmeManager.h"
#include "KmeAction.h"
#include "XarManager.h"
namespace
{
	ActivityRegistrar<MotionController>::Type temp("MotionController");
}
using namespace std;
using namespace KDeviceLists;
MotionController::MotionController()
{
	waitfor = microsec_clock::universal_time() - hours(1);
}

void MotionController::UserInit()
{

	//try {
	//tts = KAlBroker::Instance().GetBroker()->getProxy("ALTextToSpeech");
	//} catch (AL::ALError& e) {
	//Logger::Instance().WriteMsg("MotionController","Error in getting TextToSpeech proxy",Logger::FatalError);
	//}

	try
	{
		dcm = KAlBroker::Instance().GetBroker()->getDcmProxy();
	} catch (AL::ALError& e)
	{
		Logger::Instance().WriteMsg("AutoBalance", "Error in getting dcm proxy", Logger::FatalError);
	}

	try
	{
		motion = KAlBroker::Instance().GetBroker()->getMotionProxy();
	} catch (AL::ALError& e)
	{
		Logger::Instance().WriteMsg("MotionController", "Error in getting motion proxy", Logger::FatalError);
	}
	try
	{
		pbroker = AL::ALPtr<AL::ALBroker>(KAlBroker::Instance().GetBroker());
		framemanager = AL::ALPtr<AL::ALFrameManagerProxy>(new AL::ALFrameManagerProxy(pbroker));
	} catch (AL::ALError& e)
	{
		Logger::Instance().WriteMsg("MotionController", "Error in getting frameManager proxy" + e.getDescription(), Logger::FatalError);
	}

	Logger::Instance().WriteMsg("MotionController", "Loading special actions!", Logger::Info);

	{
		std::vector<std::string> registeredSpecialActions = SpecialActionFactory::Instance()->GetRegisteredProducts();
		std::vector<std::string>::const_iterator it;
		for (it = registeredSpecialActions.begin(); it < registeredSpecialActions.end(); ++it)
		{
			SpActions.insert(SpAsoocElement(*it, boost::shared_ptr<ISpecialAction>(SpecialActionFactory::Instance()->CreateObject(*it))));
		}
	}

	{
		std::vector<ISpecialAction*> kmeActions = KmeManager::LoadActionsKME();
		std::vector<ISpecialAction*>::const_iterator it;
		for (it = kmeActions.begin(); it < kmeActions.end(); ++it)
		{
			SpActions.insert(SpAsoocElement((*it)->GetName(), boost::shared_ptr<ISpecialAction>(*it)));
		}
	}

	{
		std::vector<ISpecialAction*> xarActions = XarManager::LoadActionsXAR(framemanager);
		std::vector<ISpecialAction*>::const_iterator it;
		for (it = xarActions.begin(); it < xarActions.end(); ++it)
		{
			SpActions.insert(SpAsoocElement((*it)->GetName(), boost::shared_ptr<ISpecialAction>(*it)));
		}
	}

	motion->setStiffnesses("Body", 0.7);
	motion->setStiffnesses("Head", 0.95);
	createDCMAlias();

	motion->setWalkArmsEnable(true, true);

	//TODO motion->setMotionConfig([["ENABLE_STIFFNESS_PROTECTION",true]]);
	AL::ALValue config;
	config.arraySetSize(15);
	for (int i = 0; i < 15; ++i)
		config[i].arraySetSize(2);
	config[0][0] = "WALK_MAX_TRAPEZOID";
	config[0][1] = 3.5; // 4.5
	config[1][0] = "WALK_MIN_TRAPEZOID";
	config[1][1] = 4.4; // 3.5
	config[2][0] = "WALK_STEP_MAX_PERIOD";
	config[2][1] = 30; // 30
	config[3][0] = "WALK_STEP_MIN_PERIOD";
	config[3][1] = 17; // 21
	config[4][0] = "WALK_MAX_STEP_X";
	config[4][1] = 0.05; // 0.04
	config[5][0] = "WALK_MAX_STEP_Y";
	config[5][1] = 0.05; // 0.04
	config[6][0] = "WALK_MAX_STEP_THETA";
	config[6][1] = 24; // 20
	config[7][0] = "WALK_STEP_HEIGHT";
	config[7][1] = 0.017; // 0.015
	config[8][0] = "WALK_FOOT_SEPARATION";
	config[8][1] = 0.105; // 0.095
	config[9][0] = "WALK_FOOT_ORIENTATION";
	config[9][1] = 0;// 0
	config[10][0] = "WALK_TORSO_HEIGHT";
	config[10][1] = 0.29;
	config[11][0] = "WALK_TORSO_ORIENTATION_X";
	config[11][1] = 0.0; // 0
	config[12][0] = "WALK_TORSO_ORIENTATION_Y";
	config[12][1] = 0.0; // 0
	config[13][0] = "ENABLE_FOOT_CONTACT_PROTECTION";
	config[13][1] = true;
	config[14][0] = "ENABLE_FALL_MANAGEMENT_PROTECTION";
	config[14][1] = false;
	motion->setMotionConfig(config);

	Logger::Instance().WriteMsg("MotionController", "Subcribing to topics", Logger::Info);
	_blk->subscribeTo("motion", 0);
	_blk->subscribeTo("sensors", 0);

	AccZvalue = 0.0;
	AccXvalue = 0.0;
	AccYvalue = 0.0;

	robotDown = false;
	robotUp = true;

	walkPID = 0;
	headPID = 0;
	actionPID = 0;

	counter = 0;

	pam = new MotionActionMessage();
	pam->set_command("NULL");

	for (int i = 0; i < 6; i++)
	{
		walkPrevAng[i] = 0;
	}

	for (int i = 0; i < 20; i++)
	{
		comp[i] = 0;
	}

	RKickAng1[0] = -20.1131;
	RKickAng1[1] = -0.90073;
	RKickAng1[2] = -15.9987;
	RKickAng1[3] = -1.40386;
	RKickAng1[4] = 53.2648;
	RKickAng1[5] = 50.7873;

	RKickAng2[0] = -26.453;
	RKickAng2[1] = -0.612838;
	RKickAng2[2] = -9.67049;
	RKickAng2[3] = -1.84332;
	RKickAng2[4] = 48.5186;
	RKickAng2[5] = 55.1057;

	RKickAng3[0] = -29.4413;
	RKickAng3[1] = -0.349163;
	RKickAng3[2] = -13.0983;
	RKickAng3[3] = 1.23289;
	RKickAng3[4] = 50.4523;
	RKickAng3[5] = 59.0608;

	RKickAng4[0] = -35.7695;
	RKickAng4[1] = -0.0854875;
	RKickAng4[2] = -19.2507;
	RKickAng4[3] = 6.33061;
	RKickAng4[4] = 55.9894;
	RKickAng4[5] = 67.2348;

	RKickAng5[0] = -49.4376;
	RKickAng5[1] = -0.173379;
	RKickAng5[2] = -33.9511;
	RKickAng5[3] = 8.4732;
	RKickAng5[4] = 79.9857;
	RKickAng5[5] = 80.1333;

	//	RKickAng5 [0] = -49.4376;
	//	RKickAng5 [1] = -0.173379;
	//	RKickAng5 [2] = -32.1511;
	//	RKickAng5 [3] = 8.4732;
	//	RKickAng5 [4] = 76.9857;
	//	RKickAng5 [5] = 80.1333;

	LKickAng1[0] = -15.9987;
	LKickAng1[1] = 1.40386;
	LKickAng1[2] = -20.1131;
	LKickAng1[3] = 0.70073;
	LKickAng1[4] = 50.7873;
	LKickAng1[5] = 53.2648;

	LKickAng2[0] = -9.67049;
	LKickAng2[1] = 1.84332;
	LKickAng2[2] = -26.453;
	LKickAng2[3] = 0.612838;
	LKickAng2[4] = 55.1057;
	LKickAng2[5] = 48.5186;

	LKickAng3[0] = -13.0983;
	LKickAng3[1] = -1.23289;
	LKickAng3[2] = -29.4413;
	LKickAng3[3] = 0.3491639;
	LKickAng3[4] = 59.0608;
	LKickAng3[5] = 50.4523;

	LKickAng4[0] = -19.2507;
	LKickAng4[1] = -6.33061;
	LKickAng4[2] = -35.7695;
	LKickAng4[3] = 0.0854875;
	LKickAng4[4] = 67.2348;
	LKickAng4[5] = 55.9894;

	LKickAng5[0] = -33.9511;
	LKickAng5[1] = -8.4732;
	LKickAng5[2] = -49.4376;
	LKickAng5[3] = 0.173379;
	LKickAng5[4] = 80.1333;
	LKickAng5[5] = 79.9857;

	//	LKickAng5 [0] = -22.1511;
	//	LKickAng5 [1] = -7.4732;
	//	LKickAng5 [2] = -45.4376;
	//	LKickAng5 [3] = 0.173379;
	//	LKickAng5 [4] = 78.1333;
	//	LKickAng5 [5] = 71.9857;

	for (int i = 0; i < 6; i++)
	{

		if (RKickAng2[i] < RKickAng1[i])
			diffRKick1[i] = -1;
		else
			diffRKick1[i] = 1;

		if (RKickAng3[i] < RKickAng2[i])
			diffRKick2[i] = -1;
		else
			diffRKick2[i] = 1;

		if (RKickAng4[i] < RKickAng3[i])
			diffRKick3[i] = -1;
		else
			diffRKick3[i] = 1;

		if (RKickAng5[i] < RKickAng4[i])
			diffRKick4[i] = -1;
		else
			diffRKick4[i] = 1;

		if (LKickAng2[i] < LKickAng1[i])
			diffLKick1[i] = -1;
		else
			diffLKick1[i] = 1;

		if (LKickAng3[i] < LKickAng2[i])
			diffLKick2[i] = -1;
		else
			diffLKick2[i] = 1;

		if (LKickAng4[i] < LKickAng3[i])
			diffLKick3[i] = -1;
		else
			diffLKick3[i] = 1;

		if (LKickAng5[i] < LKickAng4[i])
			diffLKick4[i] = -1;
		else
			diffLKick4[i] = 1;
	}

	walkingWithVelocity = false;
	setStiffnessDCM(1);
	Logger::Instance().WriteMsg("MotionController", "Initialization Completed", Logger::Info);
}

int MotionController::Execute()
{
	counter++;
	//Logger::Instance().WriteMsg("MotionController","MotionController BEGIN execution "+_toString(counter),Logger::Info);
	//testcommands();
	read_messages();
	mglrun();
	//Logger::Instance().WriteMsg("MotionController","MotionController END   execution "+_toString(counter),Logger::Info);
	return 0;
}

void MotionController::read_messages()
{

	/* Messages for Walk, Head, Action */
	hm = _blk->readSignal<MotionHeadMessage> ("motion");
	if (hm != NULL)
	{
		cout << "Pira minima gia kefali " << endl;
	}
	wm = _blk->readSignal<MotionWalkMessage> ("motion");
	am = _blk->readSignal<MotionActionMessage> ("motion");

	/* Messages for Intertial Readings */
	//im = _blk->readData<InertialSensorsMessage>("sensors");
	//	im = NULL;
	allsm = _blk->readData<AllSensorValuesMessage> ("sensors");
	if (allsm == NULL)
	{
		cout << "DEN PAIRNEI MINIMATA OLE " << endl;
	}
	//	if (allsm) {
	//		if (allsm->has_ism())
	//			im = allsm->mutable_ism();
	//	}
	//Logger::Instance().WriteMsg("MotionController", "read_messages ", Logger::ExtraExtraInfo);
}

void MotionController::mglrun()
{

	if (allsm != NULL && allsm->sensordata_size() >= L_FSR)//Has Accelerometers
	{

		AccZvalue = allsm->sensordata(ACC + AXIS_Z).sensorvalue();
		AccXvalue = allsm->sensordata(ACC + AXIS_X).sensorvalue();
		AccYvalue = allsm->sensordata(ACC + AXIS_Y).sensorvalue();
		gyrX = allsm->sensordata(GYR + AXIS_X).sensorvalue();
		gyrY = allsm->sensordata(GYR + AXIS_Y).sensorvalue();
		accnorm = sqrt(AccZvalue * AccZvalue + AccYvalue * AccYvalue + AccXvalue * AccXvalue);
		angX = atan(AccYvalue / AccZvalue) + gyrX * 0.01744 * 0.05;//TO rad * 0.5 (integration for half a second  * 1/10 because its in 0.1 deg resolution)
		angY = atan(-AccXvalue / AccZvalue) + gyrY * 0.01744 * 0.05;

		/* Check if the robot is falling and remove stiffness, kill all motions */
		//Logger::Instance().WriteMsg("MotionController", "Accnorm:"+_toString(accnorm), Logger::ExtraInfo);
		//Logger::Instance().WriteMsg("MotionController", "angX:"+_toString(atan(-AccXvalue/AccZvalue))+ "angY:"+_toString(atan(AccYvalue/AccZvalue)), Logger::ExtraInfo);
		//Logger::Instance().WriteMsg("MotionController", "GyrX:"+_toString(gyrX)+ "GyrY:"+_toString(gyrY), Logger::ExtraInfo);
		float normdist = (accnorm - KDeviceLists::Interpret::GRAVITY_PULL) / KDeviceLists::Interpret::GRAVITY_PULL;
		if ((normdist < -0.35 || normdist > 0.75 || fabs(gyrX) > 600.0 || fabs(gyrY) > 600.0) && (fabs(angX) > 0.75 || fabs(angY) > 0.75))
		{
			Logger::Instance().WriteMsg("MotionController", "Robot falling: Stiffness off", Logger::ExtraInfo);

			robotUp = false;
			robotDown = false;
			killCommands();
			//		tts->pCall<AL::ALValue>(std::string("say"), std::string("Ouch!"));
			//motion->setStiffnesses("Body", 0.0);
			setStiffnessDCM(0);
			waitfor = microsec_clock::universal_time() + boost::posix_time::milliseconds(350);
			//ALstandUpCross();

			return;
		}
	}

	if (waitfor > microsec_clock::universal_time())
		return;
	/* Check if an Action command has been completed */
	if ((actionPID != 0) && !motion->isRunning(actionPID) && !framemanager->isRunning(actionPID) /*isRunning(actionPID)*/)
	{
		actionPID = 0;
		if (robotDown)
		{
			robotDown = false;
			robotUp = true;
			if((fabs(angX) > 0.75 || fabs(angY) > 0.75)) robotUp=false;
		}
		if (!robotDown && !robotUp)
		{
			robotDown = true;
			robotUp = false;
		}
		Logger::Instance().WriteMsg("MotionController", "Action completed! Motion executed " + _toString(counter) + " times.", Logger::ExtraInfo);
	}
	if ((actionPID == 0) && !robotDown && !robotUp)
	{
		//Now execute an alstandupcross
		//motion->setStiffnesses("Body", 0.5);
		setStiffnessDCM(0.7);
		//usleep(300000);
		ALstandUpCross();
		return;
	}

	/* Check if the robot is down and stand up */
	if ((actionPID == 0) && robotDown)
	{
		Logger::Instance().WriteMsg("MotionController", "Will stand up now ...", Logger::ExtraInfo);
		//motion->setStiffnesses("Body", 1.0);
		//motion->setStiffnesses("Head", 0.95);
		setStiffnessDCM(1);
		robotDown = true;
		robotUp = false;
		ALstandUp();
		Logger::Instance().WriteMsg("MotionController", "StandUp ID: " + _toString(actionPID), Logger::ExtraInfo);
		//uŔ(7000000);
		return;
	}

	/* The robot is up and ready to execute motions */
	if (robotUp)
	{

		/* Check if a Walk command has been completed */
		if ((walkPID != 0) && !motion->isRunning(walkPID) && !motion->walkIsActive())
		{
			walkPID = 0;
			Logger::Instance().WriteMsg("MotionController", "Walk completed! Motion executed " + _toString(counter) + " times.", Logger::ExtraInfo);
		}

		/* Check if a Head command has been completed */
		if ((headPID != 0) && !motion->isRunning(headPID))
		{
			headPID = 0;
			Logger::Instance().WriteMsg("MotionController", "Head completed! Motion executed " + _toString(counter) + " times.", Logger::ExtraInfo);
		}

		/* Check if there is a command to execute */

		if ((wm != NULL) && (actionPID == 0))
		{
			if (wm->command() == "walkTo")
			{
				walkParam1 = wm->parameter(0);
				walkParam2 = wm->parameter(1);
				walkParam3 = wm->parameter(2);
				Logger::Instance().WriteMsg("MotionController", wm->command() + " with parameters " + _toString(walkParam1) + " " + _toString(walkParam2) + " "
						+ _toString(walkParam3), Logger::ExtraInfo);
				walkPID = motion->post.walkTo(walkParam1, walkParam2, walkParam3);
				Logger::Instance().WriteMsg("MotionController", "Walk ID: " + _toString(walkPID), Logger::ExtraInfo);
			} else if (wm->command() == "setWalkTargetVelocity")
			{
				walkParam1 = wm->parameter(0);
				walkParam2 = wm->parameter(1);
				walkParam3 = wm->parameter(2);
				walkParam4 = wm->parameter(3);
				Logger::Instance().WriteMsg("MotionController", wm->command() + " with parameters " + _toString(walkParam1) + " " + _toString(walkParam2) + " "
						+ _toString(walkParam3) + " " + _toString(walkParam4), Logger::ExtraInfo);
				walkPID = motion->post.setWalkTargetVelocity(walkParam1, walkParam2, walkParam3, walkParam4);
				walkingWithVelocity = true;
				Logger::Instance().WriteMsg("MotionController", "Walk ID: " + _toString(walkPID), Logger::ExtraInfo);
			} else
				Logger::Instance().WriteMsg("MotionController", "Invalid Walk Command: " + wm->command(), Logger::ExtraInfo);
		}

		if (hm != NULL)
		{
			killHeadCommand();
			if (hm->command() == "NoDCMsetHead")
			{
				headParam1 = hm->parameter(0);
				headParam2 = hm->parameter(1);
				Logger::Instance().WriteMsg("MotionController", hm->command() + " with parameters " + _toString(headParam1) + " " + _toString(headParam2), Logger::ExtraInfo);
				names.arraySetSize(2);
				values.arraySetSize(2);
				names[0] = "HeadYaw";
				values[0] = headParam1;
				names[1] = "HeadPitch";
				values[1] = headParam2;
				float fractionMaxSpeed = 0.98;
				headPID = motion->post.setAngles(names, values, fractionMaxSpeed);
				Logger::Instance().WriteMsg("MotionController", " Head ID: " + _toString(headPID), Logger::ExtraInfo);
			} else if (hm->command() == "changeHead")
			{
				headParam1 = hm->parameter(0);
				headParam2 = hm->parameter(1);
				Logger::Instance().WriteMsg("MotionController", hm->command() + " with parameters " + _toString(headParam1) + " " + _toString(headParam2), Logger::ExtraInfo);
				names.arraySetSize(2);
				values.arraySetSize(2);
				names[0] = "HeadYaw";
				values[0] = headParam1;
				names[1] = "HeadPitch";
				values[1] = headParam2;
				float fractionMaxSpeed = 0.98;
				headPID = motion->post.changeAngles(names, values, fractionMaxSpeed);
				Logger::Instance().WriteMsg("MotionController", " Head ID: " + _toString(headPID), Logger::ExtraInfo);
			} else if (hm->command() == "setHead")
			{
				//				headParam1 = hm->parameter(0);
				//				headParam2 = hm->parameter(1);
				//Logger::Instance().WriteMsg("MotionController", hm->command() + " with parameters " + _toString(headParam1) + " " + _toString(headParam2), Logger::ExtraInfo);
				//				names.arraySetSize(2);
				//				values.arraySetSize(2);
				//				names[0] = "HeadYaw";
				//				values[0] = headParam1;
				//				names[1] = "HeadPitch";
				//				values[1] = headParam2;
				//				float fractionMaxSpeed = 0.98;
				//				headPID = motion->post.changeAngles(names, values, fractionMaxSpeed);
				//				Logger::Instance().WriteMsg("MotionController", " Head ID: " + _toString(headPID), Logger::ExtraInfo);

				for (int p = 0; p < HEAD_SIZE; p++)
					commands[5][(p)][0] = hm->parameter(p);

				int DCMtime;

				try
				{ // Get time in 0 ms
					DCMtime = dcm->getTime(0);
				} catch (const AL::ALError &e)
				{
					throw ALERROR("mainModule", "execute_action()", "Error on DCM getTime : " + e.toString());
				}

				commands[4][0] = DCMtime;
				//Send command
				try
				{
					dcm->setAlias(commands);
				} catch (const AL::ALError &e)
				{
					throw ALERROR("mainModule", "execute_action", "Error when sending command to DCM : " + e.toString());
				}

			} else
				Logger::Instance().WriteMsg("MotionController", "Invalid Head Command: " + hm->command(), Logger::ExtraInfo);
		}

		if ((actionPID == 0) && ((pam->command() != "NULL" && am == NULL) || (am != NULL && am->command() == "MyLeftKick") || (am != NULL && am->command() == "MyRightKick")))
		{

			int readyToKick = 0;

			if (am != NULL)
			{
				Logger::Instance().WriteMsg("MotionController", "AM: " + am->command(), Logger::ExtraInfo);
				pam->set_command(am->command());
			}
			Logger::Instance().WriteMsg("MotionController", "PAM: " + pam->command(), Logger::ExtraInfo);

			if (pam->command() == "MyRightKick")
			{
				AngleCompare();

				Logger::Instance().WriteMsg("MotionController", _toString(comp[0]) + _toString(comp[1]) + _toString(comp[2]) + _toString(comp[3]), Logger::ExtraInfo);
				Logger::Instance().WriteMsg("MotionController", _toString(comp[8]) + _toString(comp[9]) + _toString(comp[10]) + _toString(comp[11]) + _toString(comp[16])
						+ _toString(comp[17]), Logger::ExtraInfo);

				if (comp[3] >= 3 && comp[11] >= 3 && comp[16] == 1 && comp[17] == 1)
				{
					Logger::Instance().WriteMsg("MotionController", "CASE1 ", Logger::ExtraInfo);

					pam->set_command("rightKickD.xar");
					readyToKick = 1;
				} else if (comp[2] >= 3 && comp[10] >= 3 && comp[16] == 1 && comp[17] == 1)
				{
					Logger::Instance().WriteMsg("MotionController", "CASE2 ", Logger::ExtraInfo);

					pam->set_command("rightKickC.xar");
					readyToKick = 1;
				} else if (comp[1] >= 2 && comp[9] >= 3 && comp[16] == 1)
				{
					Logger::Instance().WriteMsg("MotionController", "CASE3 ", Logger::ExtraInfo);

					pam->set_command("rightKickB.xar");
					readyToKick = 1;
				} else if (comp[0] >= 2 && comp[8] >= 3 && comp[16] == 1)
				{
					Logger::Instance().WriteMsg("MotionController", "CASE4 ", Logger::ExtraInfo);

					pam->set_command("rightKickA.xar");
					readyToKick = 1;
				}
				if (readyToKick == 1)
				{
					SpAssocCont::iterator it = SpActions.find(pam->command());
					if (it == SpActions.end())
						Logger::Instance().WriteMsg("MotionController", "SpAction " + pam->command() + " not found!", Logger::Error);
					else
					{
						killWalkCommand();
						actionPID = it->second->ExecutePost();
					}
					pam->set_command("NULL");
					readyToKick = 0;
				}
			} else if (pam->command() == "MyLeftKick")
			{
				AngleCompare();

				Logger::Instance().WriteMsg("MotionController", _toString(comp[4]) + _toString(comp[5]) + _toString(comp[6]) + _toString(comp[7]), Logger::ExtraInfo);
				Logger::Instance().WriteMsg("MotionController", _toString(comp[12]) + _toString(comp[13]) + _toString(comp[14]) + _toString(comp[15]) + _toString(comp[18])
						+ _toString(comp[19]), Logger::ExtraInfo);

				if (comp[7] >= 3 && comp[15] >= 3 && comp[18] == 1 && comp[19] == 1)
				{
					Logger::Instance().WriteMsg("MotionController", "CASE5 ", Logger::ExtraInfo);

					pam->set_command("leftKickD.xar");
					readyToKick = 1;
				} else if (comp[6] >= 3 && comp[14] >= 3 && comp[18] == 1 && comp[19] == 1)
				{
					Logger::Instance().WriteMsg("MotionController", "CASE6 ", Logger::ExtraInfo);

					pam->set_command("leftKickC.xar");
					readyToKick = 1;
				} else if (comp[5] >= 2 && comp[13] >= 3 && comp[19] == 1)
				{
					Logger::Instance().WriteMsg("MotionController", "CASE7 ", Logger::ExtraInfo);

					pam->set_command("leftKickB.xar");
					readyToKick = 1;
				} else if (comp[4] >= 2 && comp[12] >= 3 && comp[19] == 1)
				{
					Logger::Instance().WriteMsg("MotionController", "CASE8 ", Logger::ExtraInfo);

					pam->set_command("leftKickA.xar");
					readyToKick = 1;
				}

				if (readyToKick == 1)
				{
					SpAssocCont::iterator it = SpActions.find(pam->command());
					if (it == SpActions.end())
						Logger::Instance().WriteMsg("MotionController", "SpAction " + pam->command() + " not found!", Logger::Error);
					else
					{
						killWalkCommand();
						actionPID = it->second->ExecutePost();
					}
					pam->set_command("NULL");
					readyToKick = 0;
				}
			}
			Logger::Instance().WriteMsg("MotionController", "  Action ID: " + _toString(actionPID), Logger::ExtraInfo);
			return;
		} else if ((am != NULL) && (actionPID == 0) && !KmeManager::isDCMKmeRunning())
		{
			Logger::Instance().WriteMsg("MotionController", am->command(), Logger::ExtraInfo);
			stopWalkCommand();
			if (am->command() == "LieDown")
			{
				killHeadCommand();
			} else if (am->command() == "PuntKick")
			{
				killHeadCommand();
				robotUp = false;
			}
			SpAssocCont::iterator it = SpActions.find(am->command());
			if (it == SpActions.end())
				Logger::Instance().WriteMsg("MotionController", "SpAction " + am->command() + " not found!", Logger::Error);
			else if (true)
			{

				actionPID = it->second->ExecutePost();
			} else
			{

				boost::shared_ptr<ISpecialAction> ptr = it->second;
				KmeAction* ptrdcmkme = (KmeAction*) ptr.get();
				KmeManager::set_end_time(ptrdcmkme->ExecuteDCM());
			}
			Logger::Instance().WriteMsg("MotionController", "  Action ID: " + _toString(actionPID), Logger::ExtraInfo);
		}
	}
	return;
}

void MotionController::killWalkCommand()
{
	motion->killWalk();
	walkPID = 0;
	Logger::Instance().WriteMsg("MotionController", "Killed Walk Command", Logger::ExtraInfo);
}

void MotionController::stopWalkCommand()
{
	if (motion->isRunning(walkPID) || walkingWithVelocity)
	{
		walkPID = motion->post.setWalkTargetVelocity(0.0, 0.0, 0.0, 0.0); // stop walk
		motion->waitUntilWalkIsFinished();
		walkingWithVelocity = false;
	}
	walkPID = 0;
}

void MotionController::killHeadCommand()
{
	if (headPID != 0)
	{
		motion->killTask(headPID);
		headPID = 0;
		Logger::Instance().WriteMsg("MotionController", "Killed Head Command", Logger::ExtraInfo);
	}
}

void MotionController::killActionCommand()
{
	if (actionPID != 0)
	{
		motion->killTask(actionPID);
		actionPID = 0;
		Logger::Instance().WriteMsg("MotionController", "Killed Action Command", Logger::ExtraInfo);
	}
}

void MotionController::killCommands()
{

	//TODO check if command stops !! framemanager->pCall("cleanBehaviors");
	motion->killAll();
	//while ( motion->isRunning(walkPID) || motion->isRunning(headPID) || motion->isRunning(actionPID) ) {
	//if ( motion->isRunning(walkPID) )
	//Logger::Instance().WriteMsg("MotionController","Walk Command is running",Logger::ExtraInfo);
	//if ( motion->isRunning(headPID) )
	//Logger::Instance().WriteMsg("MotionController","Head Command is running",Logger::ExtraInfo);
	//if ( motion->isRunning(actionPID) )
	//Logger::Instance().WriteMsg("MotionController","Action Command is running",Logger::ExtraInfo);
	//}
	walkPID = 0;
	headPID = 0;
	actionPID = 0;
	Logger::Instance().WriteMsg("MotionController", "Killed All Commands", Logger::ExtraInfo);
}

void MotionController::ALstandUp()
{
	Logger::Instance().WriteMsg("MotionController", "Choose standUp", Logger::ExtraInfo);

#ifdef WEBOTS
	if (AccXvalue > 1.0)
	{ // Webots
#else
	if (AccXvalue < 0.0)
	{ // Robot
#endif
		//tts->pCall<AL::ALValue>(std::string("say"), std::string("Face Up!"));
		ALstandUpBack();
		Logger::Instance().WriteMsg("MotionController", "Stand Up: From Back", Logger::ExtraInfo);
	}
#ifdef WEBOTS
	else if (AccXvalue < -1.0)
	{ // Webots
#else
	else if (AccXvalue > 0.0)
	{ // Robot
#endif
		//              tts->pCall<AL::ALValue>(std::string("say"), std::string("Face Down!"));
		ALstandUpFront();
		Logger::Instance().WriteMsg("MotionController", "Stand Up: From Front", Logger::ExtraInfo);
	}
	return;
}

void MotionController::ALstandUpCross()
{
	SpAssocCont::iterator it = SpActions.find("ALstandUpCross2011");//"cross.xar");
	if (it == SpActions.end())
		Logger::Instance().WriteMsg("MotionController", "SpAction ALstandUpCross not found!", Logger::Error);
	else
		actionPID = it->second->ExecutePost();
	Logger::Instance().WriteMsg("MotionController", "Stand Up Cross ", Logger::ExtraInfo);
}

void MotionController::ALstandUpBack()
{
	SpAssocCont::iterator it = SpActions.find("ALstandUpBack2011");
	if (it == SpActions.end())
		Logger::Instance().WriteMsg("MotionController", "SpAction ALstandUpBack2010 not found!", Logger::Error);
	else
	{
		actionPID = it->second->ExecutePost();
		Logger::Instance().WriteMsg("MotionController", "Stand Up 2011: From Back", Logger::ExtraInfo);
	}
}

void MotionController::ALstandUpFront()
{
	SpAssocCont::iterator it = SpActions.find("ALstandUpFront2011");
	if (it == SpActions.end())
		Logger::Instance().WriteMsg("MotionController", "SpAction ALstandUpFront2011 not found!", Logger::Error);
	else
	{
		actionPID = it->second->ExecutePost();
		Logger::Instance().WriteMsg("MotionController", "Stand Up 2011: From Front", Logger::ExtraInfo);
	}
}

void MotionController::testcommands()
{

	//	if ((headPID == 0) && (counter % 10 == 0)) {
	//		MotionHeadMessage* hmot = new MotionHeadMessage();
	//		float x = rand() / ((float) RAND_MAX);
	//		x = (x - 0.5) * 0.5;
	//		float y = rand() / ((float) RAND_MAX);
	//		y = (y - 0.5) * 0.5;
	//		hmot->set_topic("motion");
	//		hmot->set_command("changeHead");
	//		hmot->add_parameter(x);
	//		hmot->add_parameter(y);
	//		Logger::Instance().WriteMsg("MotionController","Sending Command: changeHead ", Logger::ExtraInfo);
	//		_blk->publishSignal(*hmot,"motion");
	//		delete hmot;
	//	}
	//
	//	if ((counter % 50 == 0)) {
	//		MotionWalkMessage* wmot = new MotionWalkMessage();
	//		float x = rand() / ((float) RAND_MAX);
	//		x = (x - 0.5) * 2.0;
	//		float y = rand() / ((float) RAND_MAX);
	//		y = (y - 0.5) * 2.0;
	//		float z = rand() / ((float) RAND_MAX);
	//		z = (z - 0.5) * 2.0;
	//		float s = rand() / ((float) RAND_MAX);
	//		wmot->set_topic("motion");
	//		wmot->set_command("setWalkTargetVelocity");
	//		wmot->add_parameter(x);
	//		wmot->add_parameter(y);
	//		wmot->add_parameter(z);
	//		wmot->add_parameter(s);
	//		Logger::Instance().WriteMsg("MotionController","Sending Command: setWalkTargetVelocity ", Logger::ExtraInfo);
	//		_blk->publishSignal(*wmot,"motion");
	//		delete wmot;
	//	}
	//
	//	if ((actionPID == 0) && ((counter + 30) % 500 == 0) && (counter > 0)) {
	//		MotionActionMessage* amot = new MotionActionMessage();
	//		//amot->set_topic("motion");
	//		amot->set_command("PenalizedZeroPos.xar");
	//		Logger::Instance().WriteMsg("MotionController", "Sending Command: action ", Logger::ExtraInfo);
	//		_blk->publishSignal(*amot, "motion");
	//		delete amot;
	//	}

	//	if ((actionPID == 0) && ((counter + 130) % 10 == 0) && (counter > 0))
	//	{
	//		MotionActionMessage* amot = new MotionActionMessage();
	//		amot->set_command("RightSideKickSlow.xar");
	//		Logger::Instance().WriteMsg("MotionController", "Sending Command: action ", Logger::ExtraInfo);
	//		_blk->publishSignal(*amot, "motion");
	//		delete amot;
	//	}
	//
	//	if ((actionPID == 0) && ((counter + 130) % 10 == 0) && (counter > 0))
	//	{
	//		MotionActionMessage* amot = new MotionActionMessage();
	//		amot->set_command("RightKick3.xar");
	//		Logger::Instance().WriteMsg("MotionController", "Sending Command: action ", Logger::ExtraInfo);
	//		_blk->publishSignal(*amot, "motion");
	//		delete amot;
	//	}
	//
	//	//	if (((counter+250) % 500 == 0) && (counter > 0)) {
	//	//		cout << "Killling alllll" << endl;
	//	//		killCommands();
	//	//	}
	//
	//	//	if ((actionPID == 0) && ((counter+330) % 500 == 0) && (counter > 0)) {
	//	//		MotionActionMessage* amot = new MotionActionMessage();
	//	//		amot->set_topic("motion");
	//	//		amot->set_command("LeftDive");
	//	//		Logger::Instance().WriteMsg("MotionController","Sending Command: action ", Logger::ExtraInfo);
	//	//		_blk->publishSignal(*amot,"motion");
	//	//		delete amot;
	//	//	}
}

void MotionController::createDCMAlias()
{
	cout << "Creating DCM aliases" << endl;
	AL::ALValue jointAliasses;
	vector<std::string> PosActuatorStrings = KDeviceLists::getPositionActuatorKeys();

	jointAliasses.arraySetSize(2);
	jointAliasses[0] = std::string("HeadjointActuator");

	jointAliasses[1].arraySetSize(2);

	// Joints actuator list
	jointAliasses[1][YAW] = PosActuatorStrings[HEAD + YAW];
	jointAliasses[1][PITCH] = PosActuatorStrings[HEAD + PITCH];

	// Create alias
	try
	{
		dcm->createAlias(jointAliasses);
	} catch (const AL::ALError &e)
	{
		throw ALERROR("mainModule", "createPositionActuatorAlias()", "Error when creating Alias : " + e.toString());
	}

	// Create Commands
	commands.arraySetSize(6);
	commands[0] = std::string("HeadjointActuator");
	commands[1] = std::string("ClearAll"); // Erase all previous commands
	commands[2] = std::string("time-separate");
	commands[3] = 0;

	commands[4].arraySetSize(1);
	//commands[4][0]  Will be the new time

	commands[5].arraySetSize(HEAD_SIZE); // For all joints

	for (int i = 0; i < (HEAD_SIZE); i++)
	{
		commands[5][i].arraySetSize(1);
		//commands[5][i][0] will be the new angle
	}
	cout << " Head PositionActuatorAlias created " << endl;
	/*
	 //STiffness Commands
	 vector<std::string> stiffnessactStrings = KDeviceLists::getHardnessActuatorKeys();
	 jointAliasses.arraySetSize(2);
	 jointAliasses[0] = std::string("AllHardnessActuators"); // Alias for all 25 joint actuators

	 jointAliasses[1].arraySetSize(stiffnessactStrings.size());
	 for(unsigned i=0;i<stiffnessactStrings.size();i++)
	 jointAliasses[1][i]=stiffnessactStrings[i];
	 try
	 {
	 dcm->createAlias(jointAliasses);
	 } catch (const AL::ALError &e)
	 {
	 throw ALERROR("mainModule", "createPositionActuatorAlias()", "Error when creating Alias : " + e.toString());
	 }

	 stiffnessCommand.arraySetSize(6);
	 stiffnessCommand[0] = std::string("AllHardnessActuators");
	 stiffnessCommand[1] = std::string("ClearAll"); // Erase all previous commands
	 stiffnessCommand[2] = std::string("time-separate");
	 stiffnessCommand[3] = 0;

	 stiffnessCommand[4].arraySetSize(1);
	 //commands[4][0]  Will be the new time

	 stiffnessCommand[5].arraySetSize(stiffnessactStrings.size()); // For all joints

	 for (unsigned i = 0; i < stiffnessactStrings.size(); i++)
	 {
	 stiffnessCommand[5][i].arraySetSize(1);
	 //commands[5][i][0] will be the new angle
	 }
	 cout << "  AllHardnessActuators  alias created " << endl;
	 */

}

void MotionController::setStiffnessDCM(float s)
{
	motion->setStiffnesses("Body", s);
	/*for (int p = 0; p < NUMOFJOINTS; p++)
	 stiffnessCommand[5][(p)][0] = s;

	 int DCMtime;

	 try
	 { // Get time in 0 ms
	 DCMtime = dcm->getTime(0);
	 } catch (const AL::ALError &e)
	 {
	 throw ALERROR("mainModule", "execute_action()", "Error on DCM getTime : " + e.toString());
	 }

	 stiffnessCommand[4][0] = DCMtime;
	 //Send command
	 try
	 {
	 dcm->setAlias(stiffnessCommand);
	 } catch (const AL::ALError &e)
	 {
	 throw ALERROR("mainModule", "execute_action", "Error when sending command to DCM : " + e.toString());
	 }
	 motion->setStiffnesses("Body", s);
	 */
}

void MotionController::AngleCompare()
{

	int diffW[6] = { 0, 0, 0, 0, 0, 0 };

	int kR = 0, pR = 0, vR = 0, v2R = 0, kL = 0, pL = 0, vL = 0, v2L = 0, lR = 0, qR = 0, wR = 0, w2R = 0, lL = 0, qL = 0, wL = 0, w2L = 0, xR = 0, zR = 0, xL = 0, zL = 0;
	if (allsm == NULL)
	{
		cout << "DEN PAIRNEI MINIMATA OLE AngleCompare " << endl;
		return;
	}

	LHipPitch = allsm->jointdata(L_LEG + HIP_PITCH);
	LHipRoll = allsm->jointdata(L_LEG + HIP_ROLL);
	RHipPitch = allsm->jointdata(R_LEG + HIP_PITCH);
	RHipRoll = allsm->jointdata(R_LEG + HIP_ROLL);
	RKneePitch = allsm->jointdata(R_LEG + KNEE_PITCH);
	LKneePitch = allsm->jointdata(L_LEG + KNEE_PITCH);

	float walkAngles[6] = {
							LHipPitch.sensorvalue() / TO_RAD,
							LHipRoll.sensorvalue() / TO_RAD,
							RHipPitch.sensorvalue() / TO_RAD,
							RHipRoll.sensorvalue() / TO_RAD,
							RKneePitch.sensorvalue() / TO_RAD,
							LKneePitch.sensorvalue() / TO_RAD };

	for (int i = 0; i < 6; i++)
	{
		Logger::Instance().WriteMsg("MotionController", _toString(walkAngles[i]), Logger::ExtraInfo);
	}

	for (int i = 0; i < 6; i++)
	{
		if (walkAngles[i] < walkPrevAng[i])
			diffW[i] = -1;
		else
			diffW[i] = 1;

		if (RKickAng1[i] < RKickAng2[i])
		{
			if ((walkAngles[i] <= RKickAng2[i]) and (walkAngles[i] > RKickAng1[i]))
			{
				kR = kR + 1;
				if (i == 0)
					xR = 1;
				if (i == 2)
					zR = 1;
			}
		} else
		{
			if ((walkAngles[i] <= RKickAng1[i]) and (walkAngles[i] > RKickAng2[i]))
			{
				kR = kR + 1;
				if (i == 0)
					xR = 1;
				if (i == 2)
					zR = 1;
			}
		}

		if (RKickAng2[i] < RKickAng3[i])
		{
			if ((walkAngles[i] <= RKickAng3[i]) and (walkAngles[i] > RKickAng2[i]))
			{
				pR = pR + 1;
				if (i == 0)
					xR = 1;
				if (i == 2)
					zR = 1;
			}
		} else
		{
			if ((walkAngles[i] <= RKickAng2[i]) and (walkAngles[i] > RKickAng3[i]))
			{
				pR = pR + 1;
				if (i == 0)
					xR = 1;
				if (i == 2)
					zR = 1;
			}
		}

		if (RKickAng3[i] < RKickAng4[i])
		{
			if ((walkAngles[i] <= RKickAng4[i]) and (walkAngles[i] > RKickAng3[i]))
			{
				vR = vR + 1;
				if (i == 0)
					xR = 1;
				if (i == 2)
					zR = 1;
			}
		} else
		{
			if ((walkAngles[i] <= RKickAng3[i]) and (walkAngles[i] > RKickAng4[i]))
			{
				vR = vR + 1;
				if (i == 0)
					xR = 1;
				if (i == 2)
					zR = 1;
			}
		}

		if (RKickAng4[i] < RKickAng5[i])
		{
			if ((walkAngles[i] <= RKickAng5[i]) and (walkAngles[i] > RKickAng4[i]))
			{
				v2R = v2R + 1;
				if (i == 0)
					xR = 1;
				if (i == 2)
					zR = 1;
			}
		} else
		{
			if ((walkAngles[i] <= RKickAng4[i]) and (walkAngles[i] > RKickAng5[i]))
			{
				v2R = v2R + 1;
				if (i == 0)
					xR = 1;
				if (i == 2)
					zR = 1;
			}
		}

		if (LKickAng1[i] < LKickAng2[i])
		{
			if ((walkAngles[i] <= LKickAng2[i]) and (walkAngles[i] > LKickAng1[i]))
			{
				kL = kL + 1;
				if (i == 0)
					xL = 1;
				if (i == 2)
					zL = 1;
			}
		} else
		{
			if ((walkAngles[i] <= LKickAng1[i]) and (walkAngles[i] > LKickAng2[i]))
			{
				kL = kL + 1;
				if (i == 0)
					xL = 1;
				if (i == 2)
					zL = 1;
			}
		}

		if (LKickAng2[i] < LKickAng3[i])
		{
			if ((walkAngles[i] <= LKickAng3[i]) and (walkAngles[i] > LKickAng2[i]))
			{
				pL = pL + 1;
				if (i == 0)
					xL = 1;
				if (i == 2)
					zL = 1;
			}
		} else
		{
			if ((walkAngles[i] <= LKickAng2[i]) and (walkAngles[i] > LKickAng3[i]))
			{
				pL = pL + 1;
				if (i == 0)
					xL = 1;
				if (i == 2)
					zL = 1;
			}
		}

		if (LKickAng3[i] < LKickAng4[i])
		{
			if ((walkAngles[i] <= LKickAng4[i]) and (walkAngles[i] > LKickAng3[i]))
			{
				vL = vL + 1;
				if (i == 0)
					xL = 1;
				if (i == 2)
					zL = 1;
			}
		} else
		{
			if ((walkAngles[i] <= LKickAng3[i]) and (walkAngles[i] > LKickAng4[i]))
			{
				vL = vL + 1;
				if (i == 0)
					xL = 1;
				if (i == 2)
					zL = 1;
			}
		}

		if (LKickAng4[i] < LKickAng5[i])
		{
			if ((walkAngles[i] <= LKickAng5[i]) and (walkAngles[i] > LKickAng4[i]))
			{
				v2L = v2L + 1;
				if (i == 0)
					xL = 1;
				if (i == 2)
					zL = 1;
			}
		} else
		{
			if ((walkAngles[i] <= LKickAng4[i]) and (walkAngles[i] > LKickAng5[i]))
			{
				v2L = v2L + 1;
				if (i == 0)
					xL = 1;
				if (i == 2)
					zL = 1;
			}
		}
	}

	for (int i = 0; i < 6; i++)
	{
		if (diffW[i] == diffRKick1[i])
			lR = lR + 1;
		if (diffW[i] == diffRKick2[i])
			qR = qR + 1;
		if (diffW[i] == diffRKick3[i])
			wR = wR + 1;
		if (diffW[i] == diffRKick4[i])
			w2R = w2R + 1;

		if (diffW[i] == diffLKick1[i])
			lL = lL + 1;
		if (diffW[i] == diffLKick2[i])
			qL = qL + 1;
		if (diffW[i] == diffLKick3[i])
			wL = wL + 1;
		if (diffW[i] == diffLKick4[i])
			w2L = w2L + 1;
	}

	comp[0] = kR;
	comp[1] = pR;
	comp[2] = vR;
	comp[3] = v2R;
	comp[4] = kL;
	comp[5] = pL;
	comp[6] = vL;
	comp[7] = v2L;
	comp[8] = lR;
	comp[9] = qR;
	comp[10] = wR;
	comp[11] = w2R;
	comp[12] = lL;
	comp[13] = qL;
	comp[14] = wL;
	comp[15] = w2L;
	comp[16] = xR;
	comp[17] = zR;
	comp[18] = xL;
	comp[19] = zL;

	for (int i = 0; i < 6; i++)
	{
		walkPrevAng[i] = walkAngles[i];
	}
}

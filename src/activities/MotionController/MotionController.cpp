#include "MotionController.h"

#include "hal/robot/generic_nao/kAlBroker.h"
#include "hal/robot/generic_nao/robot_consts.h"
#include "hal/robot/generic_nao/aldebaran-sensors.h"
#include "architecture/archConfig.h"
#include "tools/logger.h"
#include "tools/toString.h"
#include "tools/XML.h"
#include "tools/XMLConfig.h"
#include "messages/RoboCupGameControlData.h"

#include "ISpecialAction.h"
#include "KmeManager.h"
#include "KmeAction.h"
#include "XarManager.h"

#define LEANTOOMUCH 0.7
#define ANGLEHOR 1.6
#define INTTIME 0.2 //angle integration time. Look ahead for so many seconds Too large valies mean large sensitivity, too small means too late reaction
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
		Logger::Instance().WriteMsg("MotionController", "Error in getting dcm proxy", Logger::FatalError);
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

	createDCMAlias();

	motion->setWalkArmsEnable(true, true);

	//TODO motion->setMotionConfig([["ENABLE_STIFFNESS_PROTECTION",true]]);
	AL::ALValue config;
	config.arraySetSize(15);
	for (int i = 0; i < 15; ++i)
		config[i].arraySetSize(2);
	config[0][0] = "ENABLE_FOOT_CONTACT_PROTECTION";
	config[0][1] = true;
	config[1][0] = "ENABLE_FALL_MANAGEMENT_PROTECTION";
	config[1][1] = false;
	config[2][0] = "WALK_MAX_TRAPEZOID";
	config[2][1] = 2.7; // 4.5
	config[3][0] = "WALK_MIN_TRAPEZOID";
	config[3][1] = 0.95; // 3.5
	config[4][0] = "WALK_STEP_MAX_PERIOD";
	config[4][1] = 28; // 30
	config[5][0] = "WALK_STEP_MIN_PERIOD";
	config[5][1] = 19; // 21
	config[6][0] = "WALK_MAX_STEP_X";
	config[6][1] = 0.046; // 0.04
	config[7][0] = "WALK_MAX_STEP_Y";
	config[7][1] = 0.042; // 0.04
	config[8][0] = "WALK_MAX_STEP_THETA";
	config[8][1] = 35; // 20
	config[9][0] = "WALK_STEP_HEIGHT";
	config[9][1] = 0.01; // 0.015
	config[10][0] = "WALK_FOOT_SEPARATION";
	config[10][1] = 0.095; // 0.095
	config[11][0] = "WALK_FOOT_ORIENTATION";
	config[11][1] = 5.0;// 0
	config[12][0] = "WALK_TORSO_HEIGHT";
	config[12][1] = 0.316;
	config[13][0] = "WALK_TORSO_ORIENTATION_X";
	config[13][1] = 0.0; // 0
	config[14][0] = "WALK_TORSO_ORIENTATION_Y";
	config[14][1] = 0.0; // 0

	motion->setMotionConfig(config);


	//motion->setMotionConfig(AL::ALValue::array(AL::ALValue::array("ENABLE_FOOT_CONTACT_PROTECTION", true)));
	//motion->setMotionConfig(AL::ALValue::array(AL::ALValue::array("ENABLE_FALL_MANAGEMENT_PROTECTION", false)));

	Logger::Instance().WriteMsg("MotionController", "Subcribing to topics", Logger::Info);
	_blk->subscribeTo("motion", 0);
	_blk->subscribeTo("sensors", 0);
	_blk->subscribeTo("behavior", 0);

	AccZvalue = 0.0;
	AccXvalue = 0.0;
	AccYvalue = 0.0;

	robotDown = false;
	robotUp = true;

	walkPID = 0;
	headPID = 0;
	actionPID = 0;
	currentstate=1000;
	mam = new MotionActionMessage();
	mam->set_command("NULL");

	counter = 0;

	MotionSkillsInit();

	walkingWithVelocity = false;
	//setStiffnessDCM(1);
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
	wm = _blk->readSignal<MotionWalkMessage> ("motion");
	am = _blk->readSignal<MotionActionMessage> ("motion");

	/* Messages for Intertial Readings */
	allsm = _blk->readData<AllSensorValuesMessage> ("sensors");

	/* Messages from the Game Controller */
	gsm = _blk->readState<GameStateMessage> ("behavior");

	//Logger::Instance().WriteMsg("MotionController", "read_messages ", Logger::ExtraExtraInfo);
}

void MotionController::mglrun()
{

	/* Return if waiting time has not expired yet */
	if (waitfor > microsec_clock::universal_time())
		return;

	if (allsm != NULL && allsm->sensordata_size() >= L_FSR)//Has Accelerometers
	{

		AccZvalue = allsm->sensordata(ACC + AXIS_Z).sensorvalue();
		AccXvalue = allsm->sensordata(ACC + AXIS_X).sensorvalue();
		AccYvalue = allsm->sensordata(ACC + AXIS_Y).sensorvalue();

		accnorm = sqrt(AccZvalue * AccZvalue + AccYvalue * AccYvalue + AccXvalue * AccXvalue);

		angX = allsm->computeddata(ANGLE + AXIS_X).sensorvalue();
		angY = allsm->computeddata(ANGLE + AXIS_Y).sensorvalue();

		VangX= allsm->computeddata(ANGLE + AXIS_X).sensorvaluediff()*1000000000.0/allsm->timediff();
		VangX= allsm->computeddata(ANGLE + AXIS_Y).sensorvaluediff()*1000000000.0/allsm->timediff();
	}

	if (gsm != 0) {
		gameState = gsm->player_state();
	}
	else
		gameState=PLAYER_INITIAL;

	if(gameState==currentstate)
	{

	}
	else if(gameState==PLAYER_INITIAL)
	{
		killCommands();
		motion->setStiffnesses("Body", 0.2);

		SpAssocCont::iterator it = SpActions.find("PoseInitial.xar");
		if (it == SpActions.end())
			Logger::Instance().WriteMsg("MotionController", std::string("SpAction ") + "PoseInitial.xar" + " not found!", Logger::Error);
		else
			actionPID = it->second->ExecutePost();
		currentstate=gameState;
	}
	else if (gameState == PLAYER_PENALISED||gameState==PLAYER_FINISHED)
	{

		killActionCommand();
		stopWalkCommand();
		motion->setStiffnesses("Body", 0.68);
		SpAssocCont::iterator it = SpActions.find("PenalizedZeroPos.xar");
		if (it == SpActions.end())
			Logger::Instance().WriteMsg("MotionController", std::string("SpAction ") + "PenalizedZeroPos.xar"+ " not found!", Logger::Error);
		else
			actionPID = it->second->ExecutePost();
		currentstate=gameState;

	}
	else
	{
		motion->setStiffnesses("Body", 0.8);
		currentstate=gameState;

	}


	if (allsm != NULL)//Has Accelerometers
	{

		/* Check if the robot is falling and remove stiffness, kill all motions */

		float normdist = (accnorm - KDeviceLists::Interpret::GRAVITY_PULL) / KDeviceLists::Interpret::GRAVITY_PULL;

		if ((normdist < -0.35 || normdist > 0.75  ||( fabs(angX+VangX*INTTIME) > ANGLEHOR && fabs(angX)<LEANTOOMUCH) || (fabs(angY+VangY*INTTIME) > ANGLEHOR && fabs(angY)<LEANTOOMUCH) )
			||(robotUp&& actionPID==0&&(fabs(angX) > LEANTOOMUCH || fabs(angY) >LEANTOOMUCH)) )
		{
			Logger::Instance().WriteMsg("MotionController", "Robot falling: Stiffness off", Logger::ExtraInfo);

			if(currentstate==PLAYER_PLAYING)
			{
				robotUp = false;
				robotDown = false;
				killCommands();
			}
			motion->setStiffnesses("Body", 0.0);


			waitfor = microsec_clock::universal_time() + boost::posix_time::milliseconds(350);

			return;
		}
	}

	/* Check if an Action command has been completed */
	if ((actionPID != 0) && !motion->isRunning(actionPID) && !framemanager->isRunning(actionPID) /*isRunning(actionPID)*/)
	{
		actionPID = 0;
		if (robotDown)
		{
			robotDown = false;
			robotUp = true;
			if((fabs(angX) > LEANTOOMUCH || fabs(angY) > LEANTOOMUCH)) robotUp=false;
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
		motion->setStiffnesses("Body", 0.68);
		//usleep(300000);
		ALstandUpCross();
		return;
	}

	/* Check if the robot is down and stand up */
	if ((actionPID == 0) && robotDown)
	{
		Logger::Instance().WriteMsg("MotionController", "Will stand up now ...", Logger::ExtraInfo);
		motion->setStiffnesses("Body", 0.8);
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
				motion->setWalkTargetVelocity(walkParam1, walkParam2, walkParam3, walkParam4);
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
			}
			else if (hm->command() == "setHead")
			{
				for (int p = 0; p < HEAD_SIZE; p++)
					commands[5][(p)][0] = hm->parameter(p);
				int DCMtime;
				try
				{ // Get time in 0 ms
					DCMtime = dcm->getTime(100);
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
			}
			else
				Logger::Instance().WriteMsg("MotionController", "Invalid Head Command: " + hm->command(), Logger::ExtraInfo);
		}

		if ((actionPID == 0) && ((am != NULL) || (am == NULL && pam->command()!="NULL")) )
		{
			int readyToKick = 0;
			motionSkills = 0;

			if (am != NULL)
			{
				Logger::Instance().WriteMsg("MotionController", "AM: " + am->command(), Logger::ExtraInfo);
				pam->set_command(am->command());
			}

			if (pam->command() == "MyRightKick")
			{
				motionSkills = 1;
				Logger::Instance().WriteMsg("MotionController", "PAM: " + pam->command(), Logger::ExtraInfo);
				AngleCompare();

				Logger::Instance().WriteMsg("MotionController", _toString(comp[0]) + _toString(comp[1]) + _toString(comp[2]) + _toString(comp[3]), Logger::ExtraInfo);
				Logger::Instance().WriteMsg("MotionController", _toString(comp[8]) + _toString(comp[9]) + _toString(comp[10]) + _toString(comp[11]) + _toString(comp[16]) + _toString(comp[17]), Logger::ExtraInfo);

				if (comp[3] >= 4 && comp[11] >= 5 && comp[16] == 1 && comp[17] == 1)
				{
					Logger::Instance().WriteMsg("MotionController", "CASE1 ", Logger::ExtraInfo);

					pam->set_command("KickForwardRightCutD.xar");
					readyToKick = 1;
				} else if (comp[2] >= 3 && comp[10] >= 3 && comp[16] == 1 && comp[17] == 1)
				{
					Logger::Instance().WriteMsg("MotionController", "CASE2 ", Logger::ExtraInfo);

					pam->set_command("KickForwardRightCutC.xar");
					readyToKick = 1;
				} else if (comp[1] >= 2 && comp[9] >= 3 && comp[16] == 1)
				{
					Logger::Instance().WriteMsg("MotionController", "CASE3 ", Logger::ExtraInfo);

					pam->set_command("KickForwardRightCutB.xar");
					readyToKick = 1;
				} else if (comp[0] >= 2 && comp[8] >= 3 && comp[16] == 1)
				{
					Logger::Instance().WriteMsg("MotionController", "CASE4 ", Logger::ExtraInfo);

					pam->set_command("KickForwardRightCutA.xar");
					readyToKick = 1;
				}else
					Logger::Instance().WriteMsg("MotionController", "MPA ", Logger::ExtraInfo);

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
				motionSkills = 1;
				Logger::Instance().WriteMsg("MotionController", "PAM: " + pam->command(), Logger::ExtraInfo);
				AngleCompare();

				Logger::Instance().WriteMsg("MotionController", _toString(comp[4]) + _toString(comp[5]) + _toString(comp[6]) + _toString(comp[7]), Logger::ExtraInfo);
				Logger::Instance().WriteMsg("MotionController", _toString(comp[12]) + _toString(comp[13]) + _toString(comp[14]) + _toString(comp[15]) + _toString(comp[18]) + _toString(comp[19]), Logger::ExtraInfo);

				if (comp[7] >= 4 && comp[15] >= 5 && comp[18] == 1 && comp[19] == 1)
				{
					Logger::Instance().WriteMsg("MotionController", "CASE5 ", Logger::ExtraInfo);

					pam->set_command("KickForwardLeftCutD.xar");
					readyToKick = 1;
				} else if (comp[6] >= 3 && comp[14] >= 3 && comp[18] == 1 && comp[19] == 1)
				{
					Logger::Instance().WriteMsg("MotionController", "CASE6 ", Logger::ExtraInfo);

					pam->set_command("KickForwardLeftCutC.xar");
					readyToKick = 1;
				} else if (comp[5] >= 2 && comp[13] >= 3 && comp[19] == 1)
				{
					Logger::Instance().WriteMsg("MotionController", "CASE7 ", Logger::ExtraInfo);

					pam->set_command("KickForwardLeftCutB.xar");
					readyToKick = 1;
				} else if (comp[4] >= 2 && comp[12] >= 3 && comp[19] == 1)
				{
					Logger::Instance().WriteMsg("MotionController", "CASE8 ", Logger::ExtraInfo);

					pam->set_command("KickForwardLeftCutA.xar");
					readyToKick = 1;
				}else
					Logger::Instance().WriteMsg("MotionController", "MPA ", Logger::ExtraInfo);

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
			//return;
		}
		if ((am != NULL) && (actionPID == 0) && !KmeManager::isDCMKmeRunning() && (motionSkills == 0))
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
	walkingWithVelocity=false;
	//Logger::Instance().WriteMsg("MotionController", "Killed Walk Command", Logger::ExtraInfo);
}

void MotionController::stopWalkCommand()
{
	if (walkingWithVelocity||walkPID!=0)
	{
		motion->setWalkTargetVelocity(0.0, 0.0, 0.0, 0.8); // stop walk
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
		//Logger::Instance().WriteMsg("MotionController", "Killed Head Command", Logger::ExtraInfo);
	}
}

void MotionController::killActionCommand()
{
	if (actionPID != 0)
	{
		motion->killTask(actionPID);
		actionPID = 0;
		//Logger::Instance().WriteMsg("MotionController", "Killed Action Command", Logger::ExtraInfo);
	}
}

void MotionController::killCommands()
{

	//TODO check if command stops !! framemanager->pCall("cleanBehaviors");
	motion->killAll();
	walkPID = 0;
	headPID = 0;
	actionPID = 0;
	//Logger::Instance().WriteMsg("MotionController", "Killed All Commands", Logger::ExtraInfo);
}

void MotionController::ALstandUp()
{
	Logger::Instance().WriteMsg("MotionController", "Choose standUp", Logger::ExtraInfo);

	if (angY < 0.0)
	{ // Robot
		//tts->pCall<AL::ALValue>(std::string("say"), std::string("Face Up!"));
		ALstandUpBack();
		Logger::Instance().WriteMsg("MotionController", "Stand Up: From Back", Logger::ExtraInfo);
	}else
	{
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
	SpAssocCont::iterator it = SpActions.find("ALstandUpBack2011_2");
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
	SpAssocCont::iterator it = SpActions.find("ALstandUpFront2011_2");
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
	Logger::Instance().WriteMsg("MotionController","Creating DCM aliases",Logger::ExtraInfo);
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
	Logger::Instance().WriteMsg("MotionController"," Head PositionActuatorAlias created ",Logger::ExtraInfo);
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
//
//void MotionController::setStiffnessDCM(float s)
//{
//	motion->setStiffnesses("Body", s);
//	/*for (int p = 0; p < NUMOFJOINTS; p++)
//	 stiffnessCommand[5][(p)][0] = s;
//
//	 int DCMtime;
//
//	 try
//	 { // Get time in 0 ms
//	 DCMtime = dcm->getTime(0);
//	 } catch (const AL::ALError &e)
//	 {
//	 throw ALERROR("mainModule", "execute_action()", "Error on DCM getTime : " + e.toString());
//	 }
//
//	 stiffnessCommand[4][0] = DCMtime;
//	 //Send command
//	 try
//	 {
//	 dcm->setAlias(stiffnessCommand);
//	 } catch (const AL::ALError &e)
//	 {
//	 throw ALERROR("mainModule", "execute_action", "Error when sending command to DCM : " + e.toString());
//	 }
//	 motion->setStiffnesses("Body", s);
//	 */
//}

void MotionController::MotionSkillsInit()
{
	pam = new MotionActionMessage();
	pam->set_command("NULL");
	motionSkills = 0;

	for (int i = 0; i < NUM_OF_ANGLES; i++)
	{
		walkPrevAng[i] = 0;
	}

	for (int i = 0; i < 4*(NUM_OF_POSES - 1)+4; i++)
	{
		comp[i] = 0;
	}

	readRobotLegConfiguration(ArchConfig::Instance().GetConfigPrefix() + "/specialActions.xml");

	for(int j = 0; j < NUM_OF_POSES-1; j++)
	{
		for (int i = 0; i < NUM_OF_ANGLES; i++)
		{
			if (RKickAng[j+1][i] < RKickAng[j][i])
				diffRKick[j][i] = -1;
			else
				diffRKick[j][i] = 1;

			if (LKickAng[j+1][i] < LKickAng[j][i])
				diffLKick[j][i] = -1;
			else
				diffLKick[j][i] = 1;
		}
	}
	return;
}


void MotionController::AngleCompare()
{
	int diffW[NUM_OF_ANGLES];
	int temp[4*(NUM_OF_POSES - 1)+4];
	for (int i = 0; i < NUM_OF_ANGLES; i++)
	{
		diffW[i] = 0;
	}
	for (int i = 0; i < 4*(NUM_OF_POSES - 1)+4; i++)
	{
		temp[i] = 0;
	}

	if (allsm == NULL)
		return;

		LHipPitch = allsm->jointdata(L_LEG + HIP_PITCH);
		LHipRoll = allsm->jointdata(L_LEG + HIP_ROLL);
		RHipPitch = allsm->jointdata(R_LEG + HIP_PITCH);
		RHipRoll = allsm->jointdata(R_LEG + HIP_ROLL);
		RKneePitch = allsm->jointdata(R_LEG + KNEE_PITCH);
		LKneePitch = allsm->jointdata(L_LEG + KNEE_PITCH);

		float walkAngles[NUM_OF_ANGLES] = {
								LHipPitch.sensorvalue() / TO_RAD,
								LHipRoll.sensorvalue() / TO_RAD,
								RHipPitch.sensorvalue() / TO_RAD,
								RHipRoll.sensorvalue() / TO_RAD,
								RKneePitch.sensorvalue() / TO_RAD,
								LKneePitch.sensorvalue() / TO_RAD };

		for(int j = 0; j < NUM_OF_POSES-1; j++)
		{
			for (int i = 0; i < NUM_OF_ANGLES; i++)
			{
			//	Logger::Instance().WriteMsg("MotionController", _toString(walkAngles[i]), Logger::ExtraInfo);
				if (walkAngles[i] < walkPrevAng[i])
					diffW[i] = -1;
				else
					diffW[i] = 1;

				if (RKickAng[j][i] < RKickAng[j+1][i])
				{
					if ((walkAngles[i] <= RKickAng[j+1][i]) and (walkAngles[i] > RKickAng[j][i]))
					{
						temp[j] = temp[j] + 1;
						if (i == 0)
							temp[4*(NUM_OF_POSES - 1)] = 1;
						if (i == 2)
							temp[4*(NUM_OF_POSES - 1)+1] = 1;
					}
				} else
				{
					if ((walkAngles[i] <= RKickAng[j][i]) and (walkAngles[i] > RKickAng[j+1][i]))
					{
						temp[j] = temp[j] + 1;
						if (i == 0)
							temp[4*(NUM_OF_POSES - 1)] = 1;
						if (i == 2)
							temp[4*(NUM_OF_POSES - 1)+1] = 1;
					}
				}

				if (LKickAng[j][i] < LKickAng[j+1][i])
				{
					if ((walkAngles[i] <= LKickAng[j+1][i]) and (walkAngles[i] > LKickAng[j][i]))
					{
						temp[j+(NUM_OF_POSES - 1)] = temp[j+(NUM_OF_POSES - 1)] + 1;
						if (i == 0)
							temp[4*(NUM_OF_POSES - 1)+2] = 1;
						if (i == 2)
							temp[4*(NUM_OF_POSES - 1)+3] = 1;
					}
				} else
				{
					if ((walkAngles[i] <= LKickAng[j][i]) and (walkAngles[i] > LKickAng[j+1][i]))
					{
						temp[j+(NUM_OF_POSES - 1)] = temp[j+(NUM_OF_POSES - 1)] + 1;
						if (i == 0)
							temp[4*(NUM_OF_POSES - 1)+2] = 1;
						if (i == 2)
							temp[4*(NUM_OF_POSES - 1)+3] = 1;
					}
				}
			}
		}
		for (int j = 0; j < NUM_OF_POSES-1; j++)
		{
			for (int i = 0; i < NUM_OF_ANGLES; i++)
			{
				if (diffW[i] == diffRKick[j][i])
					temp[j+2*(NUM_OF_POSES - 1)] = temp[j+2*(NUM_OF_POSES - 1)] + 1;

				if (diffW[i] == diffLKick[j][i])
					temp[j+3*(NUM_OF_POSES - 1)] = temp[j+3*(NUM_OF_POSES - 1)] + 1;
			}
		}

		for (int i = 0; i < 4*(NUM_OF_POSES - 1)+4; i++)
		{
			comp[i] = temp[i];
		}

		for (int i = 0; i < NUM_OF_ANGLES; i++)
		{
			walkPrevAng[i] = walkAngles[i];
		}
}

void MotionController::readRobotLegConfiguration(const std::string& file_name)
{
	XML config(file_name);
	typedef std::vector<XMLNode<std::string, float, std::string> > NodeCont;
	NodeCont rightKickForwardPoses, leftKickForwardPoses, rightAngles, leftAngles;
	int j = 0;
	rightKickForwardPoses = config.QueryElement<std::string, float, std::string>( "KickForwardRight" );
	leftKickForwardPoses = config.QueryElement<std::string, float, std::string>( "KickForwardLeft" );

	if( rightKickForwardPoses.size()!=0)
		rightAngles = config.QueryElement<std::string, float, std::string>( "legsPose", &(rightKickForwardPoses[0]) );

	for ( NodeCont::iterator it = rightAngles.begin(); it != rightAngles.end(); it++ )
	{
		j = it->attrb["pose"];
		RKickAng[j][0] = it->attrb["angle1"];
		RKickAng[j][1] = it->attrb["angle2"];
		RKickAng[j][2] = it->attrb["angle3"];
		RKickAng[j][3] = it->attrb["angle4"];
		RKickAng[j][4] = it->attrb["angle5"];
		RKickAng[j][5] = it->attrb["angle6"];
	}
//	for (int test = 0; test<5; test++){
//		for(int l = 0; l<6; l++)
//			Logger::Instance().WriteMsg("MotionController", "RKICKANG: " + _toString(RKickAng[test][l]), Logger::ExtraInfo);
//	}

	if( leftKickForwardPoses.size()!=0)
		leftAngles = config.QueryElement<std::string, float, std::string>( "legsPose", &(leftKickForwardPoses[0]) );

	for ( NodeCont::iterator it = leftAngles.begin(); it != leftAngles.end(); it++ )
	{
		j = it->attrb["pose"];
		LKickAng[j][0] = it->attrb["angle1"];
		LKickAng[j][1] = it->attrb["angle2"];
		LKickAng[j][2] = it->attrb["angle3"];
		LKickAng[j][3] = it->attrb["angle4"];
		LKickAng[j][4] = it->attrb["angle5"];
		LKickAng[j][5] = it->attrb["angle6"];
	}
//	for (int test = 0; test<5; test++){
//		for(int l = 0; l<6; l++)
//			Logger::Instance().WriteMsg("MotionController", "LKICKANG: " + _toString(LKickAng[test][l]), Logger::ExtraInfo);
//	}
}

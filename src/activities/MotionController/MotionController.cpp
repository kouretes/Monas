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


#define FULLSTIFFNESS 0.9
#define LEANTOOMUCH 0.7
#define ANGLEHOR 1.6
#define INTTIME 0.35 //angle integration time. Look ahead for so many seconds Too large valies mean large sensitivity, too small means too late reaction

#define MAXHEADYAWSPEED 2.5
#define MAXHEADPITCHSPEED 2.5
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
	readWalkParameters();
	motion->setWalkArmsEnable(true, true);

	//TODO motion->setMotionConfig([["ENABLE_STIFFNESS_PROTECTION",true]]);



	//motion->setMotionConfig(AL::ALValue::array(AL::ALValue::array("ENABLE_FOOT_CONTACT_PROTECTION", true)));
	//motion->setMotionConfig(AL::ALValue::array(AL::ALValue::array("ENABLE_FALL_MANAGEMENT_PROTECTION", false)));

	Logger::Instance().WriteMsg("MotionController", "Subcribing to topics", Logger::Info);

	_blk->updateSubscription("motion", msgentry::SUBSCRIBE_ON_TOPIC);
	_blk->updateSubscription("sensors", msgentry::SUBSCRIBE_ON_TOPIC);
	_blk->updateSubscription("behavior", msgentry::SUBSCRIBE_ON_TOPIC);

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
	//counter++;
	//Logger::Instance().WriteMsg("MotionController","MotionController BEGIN execution "+_toString(counter),Logger::Info);
	//testcommands();
	//if(counter%100==0)
	//	readWalkParameters();
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
		motion->setStiffnesses("Body", FULLSTIFFNESS);
		currentstate=gameState;

	}
	if(gsm!=NULL&&gsm->override_state()==OVERRIDE_DROPDEAD)
	{
		motion->setStiffnesses("Body", 0.0);


		waitfor = microsec_clock::universal_time() + boost::posix_time::milliseconds(350);

		return;

	}


	if (allsm != NULL)//Has Accelerometers
	{

		/* Check if the robot is falling and remove stiffness, kill all motions */

		float normdist = (accnorm - KDeviceLists::Interpret::GRAVITY_PULL) / KDeviceLists::Interpret::GRAVITY_PULL;

		if (
			(
			 normdist < -0.65 || normdist > 0.65  ||
			( fabs(angX+VangX*INTTIME) > ANGLEHOR && fabs(angX)<LEANTOOMUCH) ||
			( fabs(angY+VangY*INTTIME) > ANGLEHOR && fabs(angY)<LEANTOOMUCH)
			)

			||
			(robotUp&& actionPID==0&&(fabs(angX) > LEANTOOMUCH || fabs(angY) >LEANTOOMUCH))
		   )
		{
			Logger::Instance().WriteMsg("MotionController", "Robot falling: Stiffness off", Logger::ExtraInfo);

			if(currentstate==PLAYER_PLAYING || currentstate == PLAYER_READY)
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
		motion->setStiffnesses("Body", FULLSTIFFNESS);
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
			if (hm->command() == "setHeadInstant" ||hm->command() == "setHead")
			{
				float lastyaw=allsm->jointdata(KDeviceLists::HEAD+KDeviceLists::YAW).sensorvalue();
				float lastpitch=allsm->jointdata(KDeviceLists::HEAD+KDeviceLists::PITCH).sensorvalue();
				float tyaw=fabs(hm->parameter(0)-lastyaw)/MAXHEADYAWSPEED;
				float tpitch=fabs(hm->parameter(1)-lastpitch)/MAXHEADPITCHSPEED;
				float t=tyaw>tpitch?tyaw:tpitch;
				for (int p = 0; p < HEAD_SIZE; p++)
					commands[5][(p)][0] = hm->parameter(p);
				int DCMtime;
				try
				{ // Get time in 0 ms
					if(hm->command() == "setHeadInstant" )
					DCMtime = dcm->getTime(10);
					else
						DCMtime = dcm->getTime(t*1000+10);
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
				Logger::Instance().WriteMsg("MotionController", "AM: " + am->command() +" "+ to_simple_string(boost::posix_time::microsec_clock::universal_time()), Logger::ExtraInfo);
				pam->set_command(am->command());
			}

			readyToKick = SpCutActionsManager();
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
			//Logger::Instance().WriteMsg("MotionController", "  Action ID: " + _toString(actionPID), Logger::ExtraInfo);
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
			//Logger::Instance().WriteMsg("MotionController", "  Action ID: " + _toString(actionPID), Logger::ExtraInfo);
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
		motion->setWalkTargetVelocity(0.0, 0.0, 0.0, 1.0); // stop walk
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

	for (int i = 0; i < 3*(POSES_FORWKICK - 1); i++)
	{
		comp[i] = 0;
	}

	readRobotLegConfiguration(ArchConfig::Instance().GetConfigPrefix() + "specialActions.xml");

	for(int j = 0; j < POSES_FORWKICK-1; j++)
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
	for(int j = 0; j < POSES_BACKKICK-1; j++)
	{
		for (int i = 0; i < NUM_OF_ANGLES; i++)
		{
			if (RBackKickAng[j+1][i] < RBackKickAng[j][i])
				diffRBackKick[j][i] = -1;
			else
				diffRBackKick[j][i] = 1;

			if (LBackKickAng[j+1][i] < LBackKickAng[j][i])
				diffLBackKick[j][i] = -1;
			else
				diffLBackKick[j][i] = 1;
		}
	}
	for(int j = 0; j < POSES_SIDEKICK-1; j++)
	{
		for (int i = 0; i < NUM_OF_ANGLES; i++)
		{
			if (RSideKickAng[j+1][i] < RSideKickAng[j][i])
				diffRSideKick[j][i] = -1;
			else
				diffRSideKick[j][i] = 1;

			if (LSideKickAng[j+1][i] < LSideKickAng[j][i])
				diffLSideKick[j][i] = -1;
			else
				diffLSideKick[j][i] = 1;
		}
	}

	return;
}


int MotionController::SpCutActionsManager()
{
	int ready = 0;

	if (pam->command() == "MyRightKick")
	{
		motionSkills = 1;
		Logger::Instance().WriteMsg("MotionController", "PAM: " + pam->command(), Logger::ExtraInfo);
		AngleCompare(POSES_FORWKICK);

		Logger::Instance().WriteMsg("MotionController", _toString(comp[0]) + _toString(comp[1]) + _toString(comp[2]) + _toString(comp[3]), Logger::ExtraInfo);
		Logger::Instance().WriteMsg("MotionController", _toString(comp[4]) + _toString(comp[5]) + _toString(comp[6]) + _toString(comp[7]), Logger::ExtraInfo);
		Logger::Instance().WriteMsg("MotionController", _toString(comp[8]) + _toString(comp[9]) + _toString(comp[10]) + _toString(comp[11]), Logger::ExtraInfo);

		if (comp[3] >= 4 && comp[7] >= 5 && comp[11] == 2)
		{
			Logger::Instance().WriteMsg("MotionController", "CASE1 ", Logger::ExtraInfo);

			pam->set_command("KickForwardRightCutD.xar");
			ready = 1;
		} else if (comp[2] >= 3 && comp[6] >= 3 && comp[10] == 2)
		{
			Logger::Instance().WriteMsg("MotionController", "CASE2 ", Logger::ExtraInfo);

			pam->set_command("KickForwardRightCutC.xar");
			ready = 1;
		} else if (comp[1] >= 2 && comp[5] >= 3 && comp[9] >= 1)
		{
			Logger::Instance().WriteMsg("MotionController", "CASE3 ", Logger::ExtraInfo);

			pam->set_command("KickForwardRightCutB.xar");
			ready = 1;
		} else if (comp[0] >= 2 && comp[4] >= 3 && comp[8] >= 1)
		{
			Logger::Instance().WriteMsg("MotionController", "CASE4 ", Logger::ExtraInfo);

			pam->set_command("KickForwardRightCutA.xar");
			ready = 1;
		}else
			Logger::Instance().WriteMsg("MotionController", "MPA ", Logger::ExtraInfo);

	} else if (pam->command() == "MyLeftKick")
	{
		motionSkills = 1;
		Logger::Instance().WriteMsg("MotionController", "PAM: " + pam->command(), Logger::ExtraInfo);
		AngleCompare(POSES_FORWKICK);

		Logger::Instance().WriteMsg("MotionController", _toString(comp[0]) + _toString(comp[1]) + _toString(comp[2]) + _toString(comp[3]), Logger::ExtraInfo);
		Logger::Instance().WriteMsg("MotionController", _toString(comp[4]) + _toString(comp[5]) + _toString(comp[6]) + _toString(comp[7]), Logger::ExtraInfo);
		Logger::Instance().WriteMsg("MotionController", _toString(comp[8]) + _toString(comp[9]) + _toString(comp[10]) + _toString(comp[11]), Logger::ExtraInfo);

		if (comp[3] >= 4 && comp[7] >= 5 && comp[11] == 2)
		{
			Logger::Instance().WriteMsg("MotionController", "CASE5 ", Logger::ExtraInfo);

			pam->set_command("KickForwardLeftCutD.xar");
			ready = 1;
		} else if (comp[2] >= 3 && comp[6] >= 3 && comp[10] == 2)
		{
			Logger::Instance().WriteMsg("MotionController", "CASE6 ", Logger::ExtraInfo);

			pam->set_command("KickForwardLeftCutC.xar");
			ready = 1;
		} else if (comp[1] >= 2 && comp[5] >= 3 && comp[9] >= 1)
		{
			Logger::Instance().WriteMsg("MotionController", "CASE7 ", Logger::ExtraInfo);

			pam->set_command("KickForwardLeftCutB.xar");
			ready = 1;
		} else if (comp[0] >= 2 && comp[4] >= 3 && comp[8] >= 1)
		{
			Logger::Instance().WriteMsg("MotionController", "CASE8 ", Logger::ExtraInfo);

			pam->set_command("KickForwardLeftCutA.xar");
			ready = 1;
		}else
			Logger::Instance().WriteMsg("MotionController", "MPA ", Logger::ExtraInfo);

	}else if (pam->command() == "MyBackRightKick")
	{
		motionSkills = 1;
		Logger::Instance().WriteMsg("MotionController", "PAM: " + pam->command(), Logger::ExtraInfo);
		AngleCompare(POSES_BACKKICK);

		Logger::Instance().WriteMsg("MotionController", _toString(comp[0]) + _toString(comp[1]), Logger::ExtraInfo);
		Logger::Instance().WriteMsg("MotionController", _toString(comp[2]) + _toString(comp[3]), Logger::ExtraInfo);
		Logger::Instance().WriteMsg("MotionController", _toString(comp[4]) + _toString(comp[5]), Logger::ExtraInfo);

		if (comp[1] >= 3 && comp[3] >= 4 && comp[5] == 2)
		{
			Logger::Instance().WriteMsg("MotionController", "CASE9 ", Logger::ExtraInfo);

			pam->set_command("KickBackRightCutB1.xar");
			ready = 1;
		} else if (comp[0] >= 2 && comp[2] >= 3 && comp[4] >= 1)
		{
			Logger::Instance().WriteMsg("MotionController", "CASE10 ", Logger::ExtraInfo);

			pam->set_command("KickBackRight26-8-2011.xar");
			ready = 1;
		}else
			Logger::Instance().WriteMsg("MotionController", "MPA ", Logger::ExtraInfo);

	}else if (pam->command() == "MyBackLeftKick")
	{
		motionSkills = 1;
		Logger::Instance().WriteMsg("MotionController", "PAM: " + pam->command(), Logger::ExtraInfo);
		AngleCompare(POSES_BACKKICK);

		Logger::Instance().WriteMsg("MotionController", _toString(comp[0]) + _toString(comp[1]), Logger::ExtraInfo);
		Logger::Instance().WriteMsg("MotionController", _toString(comp[2]) + _toString(comp[3]), Logger::ExtraInfo);
		Logger::Instance().WriteMsg("MotionController", _toString(comp[4]) + _toString(comp[5]), Logger::ExtraInfo);

		if (comp[1] >= 3 && comp[3] >= 4 && comp[5] == 2)
		{
			Logger::Instance().WriteMsg("MotionController", "CASE11 ", Logger::ExtraInfo);

			pam->set_command("KickBackLeftCutB1.xar");
			ready = 1;
		} else if (comp[0] >= 2 && comp[2] >= 3 && comp[4] >= 1)
		{
			Logger::Instance().WriteMsg("MotionController", "CASE12 ", Logger::ExtraInfo);

			pam->set_command("KickBackLeft26-8-2011.xar");
			ready = 1;
		}else
			Logger::Instance().WriteMsg("MotionController", "MPA ", Logger::ExtraInfo);
	}
	else if (pam->command() == "MySideRightKick")
	{
		motionSkills = 1;
		Logger::Instance().WriteMsg("MotionController", "PAM: " + pam->command(), Logger::ExtraInfo);
		AngleCompare(POSES_SIDEKICK);

		Logger::Instance().WriteMsg("MotionController", _toString(comp[0]) + _toString(comp[1]), Logger::ExtraInfo);
		Logger::Instance().WriteMsg("MotionController", _toString(comp[2]) + _toString(comp[3]), Logger::ExtraInfo);
		Logger::Instance().WriteMsg("MotionController", _toString(comp[4]) + _toString(comp[5]), Logger::ExtraInfo);

		if (comp[1] >= 3 && comp[3] >= 4 && comp[5] == 2)
		{
			Logger::Instance().WriteMsg("MotionController", "CASE13 ", Logger::ExtraInfo);

			pam->set_command("KickSideRightCutB.xar");
			ready = 1;
		} else if (comp[0] >= 2 && comp[2] >= 3 && comp[4] >= 1)
		{
			Logger::Instance().WriteMsg("MotionController", "CASE14 ", Logger::ExtraInfo);

			pam->set_command("KickSideRightCut.xar");
			ready = 1;
		}else
			Logger::Instance().WriteMsg("MotionController", "MPA ", Logger::ExtraInfo);

	}else if (pam->command() == "MySideLeftKick")
	{
		motionSkills = 1;
		Logger::Instance().WriteMsg("MotionController", "PAM: " + pam->command(), Logger::ExtraInfo);
		AngleCompare(POSES_SIDEKICK);

		Logger::Instance().WriteMsg("MotionController", _toString(comp[0]) + _toString(comp[1]), Logger::ExtraInfo);
		Logger::Instance().WriteMsg("MotionController", _toString(comp[2]) + _toString(comp[3]), Logger::ExtraInfo);
		Logger::Instance().WriteMsg("MotionController", _toString(comp[4]) + _toString(comp[5]), Logger::ExtraInfo);

		if (comp[1] >= 3 && comp[3] >= 4 && comp[5] == 2)
		{
			Logger::Instance().WriteMsg("MotionController", "CASE15 ", Logger::ExtraInfo);

			pam->set_command("KickSideLeftCutB.xar");
			ready = 1;
		} else if (comp[0] >= 2 && comp[2] >= 3 && comp[4] >= 1)
		{
			Logger::Instance().WriteMsg("MotionController", "CASE16 ", Logger::ExtraInfo);

			pam->set_command("KickSideLeftCut.xar");
			ready = 1;
		}else
			Logger::Instance().WriteMsg("MotionController", "MPA ", Logger::ExtraInfo);
	}

	return ready;
}

void MotionController::AngleCompare(int numOfPoses)
{
	float CompAng[numOfPoses][NUM_OF_ANGLES];
	int diffKick[numOfPoses - 1][NUM_OF_ANGLES];
	int temp[3*(numOfPoses - 1)];
	int diffW[NUM_OF_ANGLES];
	for (int i = 0; i < NUM_OF_ANGLES; i++)
	{
		diffW[i] = 0;
	}
	for (int i = 0; i < 3*(numOfPoses - 1); i++)
	{
		temp[i] = 0;
	}
	for(int j = 0; j < numOfPoses; j++)
	{
		for (int i = 0; i < NUM_OF_ANGLES; i++)
		{
			CompAng[j][i] = 0;
		}
	}
	for(int j = 0; j < numOfPoses-1; j++)
	{
		for (int i = 0; i < NUM_OF_ANGLES; i++)
		{
			diffKick[j][i] = 0;
		}
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

	for (int i = 0; i < NUM_OF_ANGLES; i++)
	{
		//Logger::Instance().WriteMsg("MotionController", _toString(walkAngles[i]), Logger::ExtraInfo);
		if (walkAngles[i] < walkPrevAng[i])
			diffW[i] = -1;
		else
			diffW[i] = 1;
	}

	if(pam->command() == "MyRightKick")
	{
		for (int i = 0; i < NUM_OF_ANGLES; i++)
		{
			for(int j = 0; j < numOfPoses; j++)
			{
				CompAng[j][i] = RKickAng[j][i];
			}

			for(int k = 0; k < numOfPoses-1; k++)
			{
				diffKick[k][i] = diffRKick[k][i];
			}
		}
	}else if(pam->command() == "MyLeftKick")
	{
		for (int i = 0; i < NUM_OF_ANGLES; i++)
		{
			for(int j = 0; j < numOfPoses; j++)
			{
				CompAng[j][i] = LKickAng[j][i];
			}

			for(int k = 0; k < numOfPoses-1; k++)
			{
				diffKick[k][i] = diffLKick[k][i];
			}
		}
	}else if(pam->command() == "MyBackRightKick")
	{
		for (int i = 0; i < NUM_OF_ANGLES; i++)
		{
			for(int j = 0; j < numOfPoses; j++)
			{
				CompAng[j][i] = RBackKickAng[j][i];
			}

			for(int k = 0; k < numOfPoses-1; k++)
			{
				diffKick[k][i] = diffRBackKick[k][i];
			}
		}
	}else if(pam->command() == "MyBackLeftKick")
	{
		for (int i = 0; i < NUM_OF_ANGLES; i++)
		{
			for(int j = 0; j < numOfPoses; j++)
			{
				CompAng[j][i] = LBackKickAng[j][i];
			}

			for(int k = 0; k < numOfPoses-1; k++)
			{
				diffKick[k][i] = diffLBackKick[k][i];
			}
		}
	}else if(pam->command() == "MySideRightKick")
	{
		for (int i = 0; i < NUM_OF_ANGLES; i++)
		{
			for(int j = 0; j < numOfPoses; j++)
			{
				CompAng[j][i] = RSideKickAng[j][i];
			}

			for(int k = 0; k < numOfPoses-1; k++)
			{
				diffKick[k][i] = diffRSideKick[k][i];
			}
		}
	}else if(pam->command() == "MySideLeftKick")
	{
		for (int i = 0; i < NUM_OF_ANGLES; i++)
		{
			for(int j = 0; j < numOfPoses; j++)
			{
				CompAng[j][i] = LSideKickAng[j][i];
			}

			for(int k = 0; k < numOfPoses-1; k++)
			{
				diffKick[k][i] = diffLSideKick[k][i];
			}
		}
	}


	for(int j = 0; j < numOfPoses-1; j++)
	{
		for (int i = 0; i < NUM_OF_ANGLES; i++)
		{
			if (CompAng[j][i] < CompAng[j+1][i])
			{
				if ((walkAngles[i] <= CompAng[j+1][i]) and (walkAngles[i] > CompAng[j][i]))
				{
					temp[j] = temp[j] + 1;
					if (i == 0)
						temp[j+2*(numOfPoses - 1)] = temp[j+2*(numOfPoses - 1)]+1;
					if (i == 2)
						temp[j+2*(numOfPoses - 1)] = temp[j+2*(numOfPoses - 1)]+1;
				}
			} else
			{
				if ((walkAngles[i] <= CompAng[j][i]) and (walkAngles[i] > CompAng[j+1][i]))
				{
					temp[j] = temp[j] + 1;
					if (i == 0)
						temp[j+2*(numOfPoses - 1)] = temp[j+2*(numOfPoses - 1)]+1;;
					if (i == 2)
						temp[j+2*(numOfPoses - 1)] = temp[j+2*(numOfPoses - 1)]+1;;
				}
			}
			if (diffW[i] == diffKick[j][i])
				temp[j+(numOfPoses - 1)] = temp[j+(numOfPoses - 1)] + 1;
		}
	}
	for (int i = 0; i < 3*(numOfPoses - 1); i++)
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
	NodeCont rightKickForwardPoses, leftKickForwardPoses, rightKickBackPoses, leftKickBackPoses, rightKickSidePoses, leftKickSidePoses, angles;
	int j = 0;
	rightKickForwardPoses = config.QueryElement<std::string, float, std::string>( "KickForwardRight" );
	leftKickForwardPoses = config.QueryElement<std::string, float, std::string>( "KickForwardLeft" );
	rightKickBackPoses = config.QueryElement<std::string, float, std::string>( "KickBackRight" );
	leftKickBackPoses = config.QueryElement<std::string, float, std::string>( "KickBackLeft" );
	rightKickSidePoses = config.QueryElement<std::string, float, std::string>( "KickSideRight" );
	leftKickSidePoses = config.QueryElement<std::string, float, std::string>( "KickSideLeft" );

	if( rightKickForwardPoses.size()!=0)
		angles = config.QueryElement<std::string, float, std::string>( "legsPose", &(rightKickForwardPoses[0]) );
	for ( NodeCont::iterator it = angles.begin(); it != angles.end(); it++ )
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

	angles.clear();
	if( leftKickForwardPoses.size()!=0)
		angles = config.QueryElement<std::string, float, std::string>( "legsPose", &(leftKickForwardPoses[0]) );
	for ( NodeCont::iterator it = angles.begin(); it != angles.end(); it++ )
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

	angles.clear();
	if( rightKickBackPoses.size()!=0)
		angles = config.QueryElement<std::string, float, std::string>( "legsPose", &(rightKickBackPoses[0]) );
	for ( NodeCont::iterator it = angles.begin(); it != angles.end(); it++ )
	{
		j = it->attrb["pose"];
		RBackKickAng[j][0] = it->attrb["angle1"];
		RBackKickAng[j][1] = it->attrb["angle2"];
		RBackKickAng[j][2] = it->attrb["angle3"];
		RBackKickAng[j][3] = it->attrb["angle4"];
		RBackKickAng[j][4] = it->attrb["angle5"];
		RBackKickAng[j][5] = it->attrb["angle6"];
	}
//	for (int test = 0; test<3; test++){
//		for(int l = 0; l<6; l++)
//			Logger::Instance().WriteMsg("MotionController", "RBackKickAng: " + _toString(RBackKickAng[test][l]), Logger::ExtraInfo);
//	}

	angles.clear();
	if( leftKickBackPoses.size()!=0)
		angles = config.QueryElement<std::string, float, std::string>( "legsPose", &(leftKickBackPoses[0]) );
	for ( NodeCont::iterator it = angles.begin(); it != angles.end(); it++ )
	{
		j = it->attrb["pose"];
		LBackKickAng[j][0] = it->attrb["angle1"];
		LBackKickAng[j][1] = it->attrb["angle2"];
		LBackKickAng[j][2] = it->attrb["angle3"];
		LBackKickAng[j][3] = it->attrb["angle4"];
		LBackKickAng[j][4] = it->attrb["angle5"];
		LBackKickAng[j][5] = it->attrb["angle6"];
	}
//	for (int test = 0; test<3; test++){
//		for(int l = 0; l<6; l++)
//			Logger::Instance().WriteMsg("MotionController", "LBackKickAng: " + _toString(LBackKickAng[test][l]), Logger::ExtraInfo);
//	}

	angles.clear();
	if( rightKickSidePoses.size()!=0)
		angles = config.QueryElement<std::string, float, std::string>( "legsPose", &(rightKickSidePoses[0]) );
	for ( NodeCont::iterator it = angles.begin(); it != angles.end(); it++ )
	{
		j = it->attrb["pose"];
		RSideKickAng[j][0] = it->attrb["angle1"];
		RSideKickAng[j][1] = it->attrb["angle2"];
		RSideKickAng[j][2] = it->attrb["angle3"];
		RSideKickAng[j][3] = it->attrb["angle4"];
		RSideKickAng[j][4] = it->attrb["angle5"];
		RSideKickAng[j][5] = it->attrb["angle6"];
	}
//	for (int test = 0; test<3; test++){
//		for(int l = 0; l<6; l++)
//			Logger::Instance().WriteMsg("MotionController", "RSideKickAng: " + _toString(RSideKickAng[test][l]), Logger::ExtraInfo);
//	}

	angles.clear();
	if( leftKickSidePoses.size()!=0)
		angles = config.QueryElement<std::string, float, std::string>( "legsPose", &(leftKickSidePoses[0]) );
	for ( NodeCont::iterator it = angles.begin(); it != angles.end(); it++ )
	{
		j = it->attrb["pose"];
		LSideKickAng[j][0] = it->attrb["angle1"];
		LSideKickAng[j][1] = it->attrb["angle2"];
		LSideKickAng[j][2] = it->attrb["angle3"];
		LSideKickAng[j][3] = it->attrb["angle4"];
		LSideKickAng[j][4] = it->attrb["angle5"];
		LSideKickAng[j][5] = it->attrb["angle6"];
	}
//	for (int test = 0; test<3; test++){
//		for(int l = 0; l<6; l++)
//			Logger::Instance().WriteMsg("MotionController", "LSideKickAng: " + _toString(LSideKickAng[test][l]), Logger::ExtraInfo);
//	}


}

void MotionController::readWalkParameters()
{

	std::string fname=ArchConfig::Instance().GetConfigPrefix() +"walk_parameters.xml";
	TiXmlDocument d(fname);
	if(!d.LoadFile())
	{
		Logger::Instance().WriteMsg("MotionController", "walk_parameters.xml cannot be parsed", Logger::Warning);
		return;
	}
/*
AL::ALValue config;
	config.arraySetSize(15);
	for (int i = 0; i < 15; ++i)
		config[i].arraySetSize(2);
	config[0][0] = "ENABLE_FOOT_CONTACT_PROTECTION";
	config[0][1] = true;
	config[1][0] = "ENABLE_FALL_MANAGEMENT_PROTECTION";
	config[1][1] = false;
	config[2][0] = "WALK_MAX_TRAPEZOID";
	config[2][1] = 2.5; // 4.5
	config[3][0] = "WALK_MIN_TRAPEZOID";
	config[3][1] = 1.65; // 3.5
	config[4][0] = "WALK_STEP_MAX_PERIOD";
	config[4][1] = 28; // 30
	config[5][0] = "WALK_STEP_MIN_PERIOD";
	config[5][1] = 18; // 21
	config[6][0] = "WALK_MAX_STEP_X";
	config[6][1] = 0.044; // 0.04
	config[7][0] = "WALK_MAX_STEP_Y";
	config[7][1] = 0.04; // 0.04
	config[8][0] = "WALK_MAX_STEP_THETA";
	config[8][1] = 30; // 20
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
	*/


	std::vector<std::string> names;
	std::vector<float> values;

	const TiXmlElement *c=d.FirstChildElement();
	while(c)
	{
		names.push_back(c->Value());

		std::istringstream strs( c->GetText() );
		float v;
		strs>>v;
		values.push_back(v);


		cout<<c->Value()<<":"<<v<<endl;

		c=c->NextSiblingElement();
	}
	AL::ALValue config;
	config.arraySetSize(names.size());

	for(unsigned i=0;i<names.size();i++)
	{
		config[i].arraySetSize(2);
		config[i][0]=names[i];
		config[i][1]=values[i];

	}
		motion->setMotionConfig(config);



}

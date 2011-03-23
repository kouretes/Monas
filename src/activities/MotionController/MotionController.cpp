#include "MotionController.h"

#include "hal/robot/generic_nao/kAlBroker.h"
#include "hal/robot/generic_nao/robot_consts.h"
#include "hal/robot/generic_nao/aldebaran-sensors.h"

#include "tools/logger.h"
#include "tools/toString.h"

#include "ISpecialAction.h"
#include "KmeManager.h"
#include "XarManager.h"
namespace
{
	ActivityRegistrar<MotionController>::Type temp("MotionController");
}
using namespace std;
using namespace KDeviceLists;
MotionController::MotionController()
{
	waitfor=microsec_clock::universal_time()-hours(1);
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

	motion->setStiffnesses("Body", 1.0);
	motion->setStiffnesses("Head", 0.95);

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

	createHeadPositionActuatorAlias();

	walkingWithVelocity = false;
	Logger::Instance().WriteMsg("MotionController", "Initialization Completed", Logger::Info);
}

int MotionController::Execute()
{
	counter++;
	//Logger::Instance().WriteMsg("MotionController","MotionController BEGIN execution "+_toString(counter),Logger::Info);
	//commands();
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

	float accnorm,angX,angY;

	if (allsm != NULL && allsm->sensordata_size()>=L_FSR)//Has Accelerometers
	{
		AccZvalue = allsm->sensordata(ACC+AXIS_Z).sensorvalue();
		AccXvalue = allsm->sensordata(ACC+AXIS_X).sensorvalue();
		AccYvalue = allsm->sensordata(ACC+AXIS_Y).sensorvalue();
		accnorm=sqrt(AccZvalue*AccZvalue+AccYvalue*AccYvalue+AccXvalue*AccXvalue);
		angX=atan(fabs(AccXvalue/AccZvalue));
		angY=atan(fabs(AccYvalue/AccZvalue));

		//		AccZ = im->sensordata(2);
		//		AccZvalue = AccZ.sensorvalue();
		//		AccX = im->sensordata(0);
		//		AccXvalue = AccX.sensorvalue();
		//		AccY = im->sensordata(1);
		//		AccYvalue = AccY.sensorvalue();
	}

	/* Check if the robot is falling and remove stiffness, kill all motions */
	//Logger::Instance().WriteMsg("MotionController", "Accz:"+_toString(accnorm), Logger::ExtraInfo);
	float normdist=fabs(accnorm-KDeviceLists::Interpret::GRAVITY_PULL)/KDeviceLists::Interpret::GRAVITY_PULL;
	if (allsm != NULL&&  (normdist>0.55 && (angX>0.9||angY>0.9)  ) )

	{
		Logger::Instance().WriteMsg("MotionController", "Robot falling: Stiffness off", Logger::ExtraInfo);

		robotUp = false;
		robotDown = false;
		killCommands();
		//		tts->pCall<AL::ALValue>(std::string("say"), std::string("Ouch!"));
		motion->setStiffnesses("Body", 0.0);
		waitfor=microsec_clock::universal_time()+boost::posix_time::milliseconds(200);
		//ALstandUpCross();

		return;
	}
	if(waitfor>microsec_clock::universal_time())
		return;
	/* Check if an Action command has been completed */
	if ((actionPID != 0) && !motion->isRunning(actionPID) && !framemanager->isRunning(actionPID) /*isRunning(actionPID)*/)
	{
		actionPID = 0;
		if(robotDown){ robotDown=false;
		robotUp=true;
		}
		Logger::Instance().WriteMsg("MotionController", "Action completed! Motion executed " + _toString(counter) + " times.", Logger::ExtraInfo);
	}
	if (!robotDown && !robotUp)
	{
		//Now execute an alstandupcross
		motion->setStiffnesses("Body", 0.5);
		//usleep(300000);
		ALstandUpCross();
		Logger::Instance().WriteMsg("MotionController", "Stand Up: Cross", Logger::ExtraInfo);
		robotDown = true;
		return;
	}

	/* Check if the robot is down and stand up */
	if ((actionPID == 0) && robotDown)
	{
		Logger::Instance().WriteMsg("MotionController", "Will stand up now ...", Logger::ExtraInfo);
		motion->setStiffnesses("Body", 1.0);
		motion->setStiffnesses("Head", 0.95);
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

		if ((am != NULL) && (actionPID == 0))
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
			else
				actionPID = it->second->ExecutePost();
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
	actionPID= 0;
	Logger::Instance().WriteMsg("MotionController", "Killed All Commands", Logger::ExtraInfo);
}

void MotionController::ALstandUp()
{

#ifdef WEBOTS
	if (AccXvalue > 1.0)
	{ // Webots
#else
	if (AccXvalue < 0.0)
	{ // Robot
#endif
		//tts->pCall<AL::ALValue>(std::string("say"), std::string("Face Up!"));
		ALstandUpBack2010();
		Logger::Instance().WriteMsg("MotionController", "Stand Up 2010: From Back", Logger::ExtraInfo);
	}
#ifdef WEBOTS
	else if (AccXvalue < -1.0)
	{ // Webots
#else
	else if (AccXvalue > 0.0)
	{ // Robot
#endif
		//              tts->pCall<AL::ALValue>(std::string("say"), std::string("Face Down!"));
		ALstandUpFront2009();
		Logger::Instance().WriteMsg("MotionController", "Stand Up 2009: From Front", Logger::ExtraInfo);
	}
	return;
}

void MotionController::ALstandUpCross()
{
	SpAssocCont::iterator it = SpActions.find("ALstandUpCross");
	if (it == SpActions.end())
		Logger::Instance().WriteMsg("MotionController", "SpAction ALstandUpCross not found!", Logger::Error);
	else
		actionPID = it->second->ExecutePost();
}

void MotionController::ALstandUpFront2009()
{
	SpAssocCont::iterator it = SpActions.find("ALstandUpFront2009");
	if (it == SpActions.end())
		Logger::Instance().WriteMsg("MotionController", "SpAction ALstandUpFront2009 not found!", Logger::Error);
	else
		actionPID = it->second->ExecutePost();
}

void MotionController::ALstandUpBack2009()
{
	SpAssocCont::iterator it = SpActions.find("ALstandUpBack2009");
	if (it == SpActions.end())
		Logger::Instance().WriteMsg("MotionController", "SpAction ALstandUpBack2009 not found!", Logger::Error);
	else
		actionPID = it->second->ExecutePost();
}

void MotionController::ALstandUpFront2010()
{
	SpAssocCont::iterator it = SpActions.find("ALstandUpFront2010");
	if (it == SpActions.end())
		Logger::Instance().WriteMsg("MotionController", "SpAction ALstandUpFront2010 not found!", Logger::Error);
	else
		actionPID = it->second->ExecutePost();
}

void MotionController::ALstandUpBack2010()
{
	SpAssocCont::iterator it = SpActions.find("ALstandUpBack2010");
	if (it == SpActions.end())
		Logger::Instance().WriteMsg("MotionController", "SpAction ALstandUpBack2010 not found!", Logger::Error);
	else
		actionPID = it->second->ExecutePost();
}

//void MotionController::commands()
//{

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
	//	if ((actionPID == 0) && ((counter+30) % 500 == 0) && (counter > 0)) {
	//		MotionActionMessage* amot = new MotionActionMessage();
	//		amot->set_topic("motion");
	//		amot->set_command("RightKick");
	//		Logger::Instance().WriteMsg("MotionController","Sending Command: action ", Logger::ExtraInfo);
	//		_blk->publishSignal(*amot,"motion");
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
//}

void MotionController::createHeadPositionActuatorAlias()
{
	AL::ALValue jointAliasses;
	vector<std::string> PosActuatorStrings = KDeviceLists::getPositionActuatorKeys();

	jointAliasses.arraySetSize(2);
	jointAliasses[0] = std::string("HeadjointActuator"); // Alias for all 25 joint actuators

	jointAliasses[1].arraySetSize(2);

	// Joints actuator list
	jointAliasses[1][YAW] = PosActuatorStrings[HEAD+YAW];
	jointAliasses[1][PITCH] = PosActuatorStrings[HEAD+PITCH];

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
}

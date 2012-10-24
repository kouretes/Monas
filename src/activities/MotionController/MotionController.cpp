#include "MotionController.h"

#include "hal/robot/generic_nao/kAlBroker.h"
#include "hal/robot/generic_nao/robot_consts.h"
#include "hal/robot/generic_nao/aldebaran-sensors.h"
#include "architecture/archConfig.h"


#include "ISpecialAction.h"
#include "KmeManager.h"
#include "KmeAction.h"
#include "XarManager.h"

#define FULLSTIFFNESS 0.9
#define LEANTOOMUCH 0.7
#define ANGLEHOR 1.6
#define INTTIME 0.35 //angle integration time. Look ahead for so many seconds Too large valies mean large sensitivity, too small means too late reaction

#define MAXHEADYAWSPEED 1.8
#define MAXHEADPITCHSPEED 1.8

#define KME_ACTIONPID -1

ACTIVITY_REGISTER(MotionController);

using namespace std;
using namespace KDeviceLists;

MotionController::MotionController(Blackboard &b, XmlNode &x) : IActivity(b, x)
{
	waitfor = microsec_clock::universal_time() - hours(1);
}

void MotionController::UserInit()
{

	try
	{
		dcm = new AL::DCMProxy(KAlBroker::Instance().GetBroker());
	}
	catch (AL::ALError& e)
	{
		Logger::Instance().WriteMsg("MotionController", "Error in getting dcm proxy", Logger::FatalError);
	}

	try
	{
		motion = new AL::ALMotionProxy(boost::shared_ptr<AL::ALBroker>(KAlBroker::Instance().GetBroker()));
	}
	catch (AL::ALError& e)
	{
		Logger::Instance().WriteMsg("MotionController", "Error in getting motion proxy" + e.getDescription(), Logger::FatalError);
	}

	try
	{
		framemanager = new AL::ALFrameManagerProxy(boost::shared_ptr<AL::ALBroker>(KAlBroker::Instance().GetBroker()));
		//pbroker = boost::shared_ptr<AL::ALBroker>(KAlBroker::Instance().GetBroker());
		//framemanager = boost::shared_ptr<AL::ALFrameManagerProxy>(new AL::ALFrameManagerProxy(pbroker));
	}
	catch (AL::ALError& e)
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
	{
		std::vector<KmexAction*> kmexActions = KmexManager::LoadActionsXML(ArchConfig::Instance().GetConfigPrefix() + "specialActions.xml", SpActions);
		std::vector<KmexAction*>::const_iterator it;

		for (it = kmexActions.begin(); it < kmexActions.end(); ++it)
		{
			SpKmexActions.insert(SpElement((*it)->GetName(), boost::shared_ptr<KmexAction>(*it)));
		}
	}
	createDCMAlias();

	motion->setWalkArmsEnable(true, true);
	Logger::Instance().WriteMsg("MotionController", "Subcribing to topics", Logger::Info);
	_blk.updateSubscription("motion", msgentry::SUBSCRIBE_ON_TOPIC);
	_blk.updateSubscription("sensors", msgentry::SUBSCRIBE_ON_TOPIC);
	_blk.updateSubscription("worldstate", msgentry::SUBSCRIBE_ON_TOPIC);
	_blk.updateSubscription("obstacle", msgentry::SUBSCRIBE_ON_TOPIC);
	AccZvalue = 0.0;
	AccXvalue = 0.0;
	AccYvalue = 0.0;
	robotDown = false;
	robotUp = true;
	walkPID = 0;
	headPID = 0;
	actionPID = 0;
	currentstate = 1000;
	pam = new MotionActionMessage();
	pam->set_command("NULL");
	sm.set_type(MotionStateMessage::IDLE);
	sm.set_detail("");
	sm.set_lastaction("");
	standUpStartTime = boost::posix_time::microsec_clock::universal_time();
	walkingWithVelocity = false;
	//setStiffnessDCM(1);
	BodyID = KRobotConfig::Instance().getConfig(KDeviceLists::Interpret::BODY_ID);
	Logger::Instance().WriteMsg("MotionController", "The Body ID is " + BodyID, Logger::Info);
	Logger::Instance().WriteMsg("MotionController", "Initialization Completed", Logger::Info);
	
	//Self Reset for initialization
	Reset();
}

void MotionController::Reset(){
	readWalkParameters();
}

int MotionController::Execute()
{
	//testcommands();
	//if(counter%100==0)
	//	readWalkParameters();
	read_messages();
	vector<float> AangleTemp(22);
	vector<float> KangleTemp(22);
	vector<float> CangleTemp(22);

	/* Return if waiting time has not expired yet */
	if (waitfor > microsec_clock::universal_time())
		return 0;

	if (allsm != NULL && allsm->sensordata_size() >= L_FSR)//Has Accelerometers
	{
		AccZvalue = allsm->sensordata(ACC + AXIS_Z).sensorvalue();
		AccXvalue = allsm->sensordata(ACC + AXIS_X).sensorvalue();
		AccYvalue = allsm->sensordata(ACC + AXIS_Y).sensorvalue();
		accnorm = sqrt(AccZvalue * AccZvalue + AccYvalue * AccYvalue + AccXvalue * AccXvalue);
		angX = allsm->computeddata(ANGLE + AXIS_X).sensorvalue();
		angY = allsm->computeddata(ANGLE + AXIS_Y).sensorvalue();
		VangX = allsm->computeddata(ANGLE + AXIS_X).sensorvaluediff() * 1000000000.0 / allsm->timediff();
		VangX = allsm->computeddata(ANGLE + AXIS_Y).sensorvaluediff() * 1000000000.0 / allsm->timediff();
	}

	if (gsm != 0)
	{
		gameState = gsm->player_state();
	}
	else
		gameState = PLAYER_INITIAL;

	if(gameState == currentstate)
	{
	}
	else if(gameState == PLAYER_INITIAL)
	{
		killCommands();
		motion->setStiffnesses("Body", 0.2);
		SpAssocCont::iterator it = SpActions.find("PoseInitial.xar");

		if (it == SpActions.end())
			Logger::Instance().WriteMsg("MotionController", std::string("SpAction ") + "PoseInitial.xar" + " not found!", Logger::Error);
		else
			actionPID = it->second->ExecutePost();

		currentstate = gameState;
		sm.set_type(MotionStateMessage::IDLE);
		sm.set_detail("");
		_blk.publishState(sm, "worldstate");
	}
	else if (gameState == PLAYER_PENALISED || gameState == PLAYER_FINISHED)
	{
		killActionCommand();
		stopWalkCommand();
		motion->setStiffnesses("Body", 0.68);
		SpAssocCont::iterator it = SpActions.find("PenalizedZeroPos.xar");

		if (it == SpActions.end())
			Logger::Instance().WriteMsg("MotionController", std::string("SpAction ") + "PenalizedZeroPos.xar" + " not found!", Logger::Error);
		else
			actionPID = it->second->ExecutePost();

		currentstate = gameState;
		sm.set_type(MotionStateMessage::IDLE);
		sm.set_detail("");
		_blk.publishState(sm, "worldstate");
	}
	else
	{
		motion->setStiffnesses("Body", FULLSTIFFNESS);
		currentstate = gameState;
	}

	if(gsm != NULL && gsm->override_state() == OVERRIDE_DROPDEAD)
	{
		motion->setStiffnesses("Body", 0.0);

		if(sm.type() != MotionStateMessage::FALL)
		{
			sm.set_type(MotionStateMessage::FALL);
			sm.set_detail("");
			_blk.publishState(sm, "worldstate");
		}

		waitfor = microsec_clock::universal_time() + boost::posix_time::milliseconds(350);
		return 0;
	}

	if (allsm != NULL)//Has Accelerometers
	{
		/* Check if the robot is falling and remove stiffness, kill all motions */
		float normdist = (accnorm - KDeviceLists::Interpret::GRAVITY_PULL) / KDeviceLists::Interpret::GRAVITY_PULL;
		if (
		    (
		        normdist < -0.65 || normdist > 0.65  ||
		        ( fabs(angX + VangX * INTTIME) > ANGLEHOR && fabs(angX) < LEANTOOMUCH) ||
		        ( fabs(angY + VangY * INTTIME) > ANGLEHOR && fabs(angY) < LEANTOOMUCH)
		    )
		    ||
		    (robotUp && actionPID == 0 && (fabs(angX) > LEANTOOMUCH || fabs(angY) > LEANTOOMUCH))
		)
		{
			Logger::Instance().WriteMsg("MotionController", "Robot falling: Stiffness off", Logger::ExtraInfo);
			timeLapsed = boost::posix_time::microsec_clock::universal_time();

			if(timeLapsed - standUpStartTime >= boost::posix_time::seconds(3.5))
			{
				if(currentstate == PLAYER_PLAYING || currentstate == PLAYER_READY)
				{
					//Message edw
					robotUp = false;
					robotDown = false;
					killCommands();
				}

				motion->setStiffnesses("Body", 0.0);
				sm.set_type(MotionStateMessage::FALL);
				sm.set_detail("");
				_blk.publishState(sm, "worldstate");
				waitfor = microsec_clock::universal_time() + boost::posix_time::milliseconds(350);
				return 0;
			}
		}
	}

	/* Check if an Action command has been completed */
	if ( ((actionPID > 0) && !motion->isRunning(actionPID) && !framemanager->isRunning(actionPID)) || ((actionPID == KME_ACTIONPID) && !KmeManager::isDCMKmeRunning()) )
	{
		if (actionPID == KME_ACTIONPID)
		{
			SpAssocCont::iterator it = SpActions.find("InitPose.xar");

			if (it == SpActions.end())
				Logger::Instance().WriteMsg("MotionController", std::string("SpAction ") + "InitPose.xar" + " not found!", Logger::Error);
			else
				actionPID = it->second->ExecutePost();

			return 0;
		}

		actionPID = 0;

		if (robotDown)
		{
			robotDown = false;
			robotUp = true;

			if((fabs(angX) > LEANTOOMUCH || fabs(angY) > LEANTOOMUCH)) robotUp = false;
		}

		if (!robotDown && !robotUp)
		{
			robotDown = true;
			robotUp = false;
		}

		Logger::Instance().WriteMsg("MotionController", "Action completed!", Logger::ExtraInfo);
	}

	if ((actionPID == 0) && !robotDown && !robotUp)
	{
		//Now execute an alstandupcross
		motion->setStiffnesses("Body", 0.68);
		ALstandUpCross();
		return 0;
	}

	/* Check if the robot is down and stand up */
	if ((actionPID == 0) && robotDown)
	{
		if(sm.type() != MotionStateMessage::STANDUP)
		{
			sm.set_type(MotionStateMessage::STANDUP);
			sm.set_detail("");
			_blk.publishState(sm, "worldstate");
		}

		motion->setStiffnesses("Body", FULLSTIFFNESS);
		robotDown = true;
		robotUp = false;
		ALstandUp();
		standUpStartTime = boost::posix_time::microsec_clock::universal_time();
		Logger::Instance().WriteMsg("MotionController", "StandUp ID: " + _toString(actionPID), Logger::ExtraInfo);
		return 0;
	}

	/* The robot is up and ready to execute motions */
	if (robotUp)
	{
		/* Check if a Walk command has been completed */
		if ((walkPID != 0) && !motion->isRunning(walkPID) && !motion->walkIsActive())
		{
			walkPID = 0;
		}

		/* Check if a Head command has been completed */
		if ((headPID != 0) && !motion->isRunning(headPID))
		{
			headPID = 0;
		}

		/* Check if there is a command to execute */

		if ((wm != NULL) && (actionPID == 0))
		{
			if (wm->command() == "walkTo")
			{
				if(sm.detail().compare("walkTo") != 0)
				{
					sm.set_type(MotionStateMessage::WALKING);
					sm.set_detail("walkTo");
					_blk.publishState(sm, "worldstate");
				}

				walkParam1 = wm->parameter(0);
				walkParam2 = wm->parameter(1);
				walkParam3 = wm->parameter(2);
				Logger::Instance().WriteMsg("MotionController", wm->command() + " with parameters " + _toString(walkParam1) + " " + _toString(walkParam2) + " "
				                            + _toString(walkParam3), Logger::ExtraInfo);
				walkPID = motion->post.walkTo(walkParam1, walkParam2, walkParam3, walkConfig);
				Logger::Instance().WriteMsg("MotionController", "Walk ID: " + _toString(walkPID), Logger::ExtraInfo);
			}
			else if (wm->command() == "setWalkTargetVelocity")
			{
				if(sm.detail().compare("setWalkTargetVelocity") != 0)
				{
					sm.set_type(MotionStateMessage::WALKING);
					sm.set_detail("setWalkTargetVelocity");
					_blk.publishState(sm, "worldstate");
				}

				walkParam1 = wm->parameter(0);
				walkParam2 = wm->parameter(1);
				walkParam3 = wm->parameter(2);
				walkParam4 = wm->parameter(3);

				motion->setWalkTargetVelocity(walkParam1, walkParam2, walkParam3, walkParam4, walkConfig);
				walkingWithVelocity = true;
			}
			else
				Logger::Instance().WriteMsg("MotionController", "Invalid Walk Command: " + wm->command(), Logger::ExtraInfo);
		}

		if (hm != NULL && allsm != NULL)
		{
			killHeadCommand();

			if (hm->command() == "setHeadInstant" || hm->command() == "setHead")
			{
				float lastyaw = allsm->jointdata(KDeviceLists::HEAD + KDeviceLists::YAW).sensorvalue();
				float lastpitch = allsm->jointdata(KDeviceLists::HEAD + KDeviceLists::PITCH).sensorvalue();
				float tyaw = fabs(hm->parameter(0) - lastyaw) / MAXHEADYAWSPEED;
				float tpitch = fabs(hm->parameter(1) - lastpitch) / MAXHEADPITCHSPEED;
				float t = tyaw > tpitch ? tyaw : tpitch;

				for (int p = 0; p < HEAD_SIZE; p++)
					commands[5][(p)][0] = hm->parameter(p);

				int DCMtime;

				try
				{
					// Get time in 0 ms
					if(hm->command() == "setHeadInstant" )
						DCMtime = dcm->getTime(10);
					else
						DCMtime = dcm->getTime(t * 1000 + 10);
				}
				catch (const AL::ALError &e)
				{
					throw ALERROR("mainModule", "execute_action()", "Error on DCM getTime : " + e.toString());
				}

				commands[4][0] = DCMtime;

				//Send command
				try
				{
					dcm->setAlias(commands);
				}
				catch (const AL::ALError &e)
				{
					throw ALERROR("mainModule", "execute_action", "Error when sending command to DCM : " + e.toString());
				}
			}
			else
				Logger::Instance().WriteMsg("MotionController", "Invalid Head Command: " + hm->command(), Logger::ExtraInfo);
		}

		if ((actionPID == 0) && ((am != NULL) || (am == NULL && pam->command() != "NULL")) )
		{
			if (am != NULL)
			{
				Logger::Instance().WriteMsg("MotionController", "AM: " + am->command(), Logger::ExtraInfo);
				pam->set_command(am->command());
			}

			std::string str = pam->command();
			std::string strKick = pam->command();
			unsigned int pos = 0;
			unsigned int posKick = 0;
			pos = str.find_first_of(".");
			str.erase(0, pos + 1);
			strKick.erase(pos, strKick.size());
			sm.set_type(MotionStateMessage::ACTION);
			sm.set_detail(str);
			sm.set_lastaction(pam->command());
			_blk.publishState(sm, "worldstate");

			if (str.compare("kmex") == 0)
			{
				vector<int> frames = SpCutActionsManager();

				if ( frames.size() > 0 )
				{
					std::string str2 = pam->command();
					str2.erase(str2.size() - 1, str2.size());
					SpAssocCont::iterator it = SpActions.find(str2);

					if (it == SpActions.end())
						Logger::Instance().WriteMsg("MotionController", "SpAction " + str2 + " not found!", Logger::Error);
					else
					{
						killWalkCommand();
						boost::shared_ptr<ISpecialAction> ptr = it->second;
						KmeAction* ptrdcmkme = (KmeAction*) ptr.get();
						Logger::Instance().WriteMsg("MotionController", "Frame start : " + _toString(frames.front()) + "  Frame end : " + _toString(frames.back()), Logger::ExtraInfo);
						KmeManager::set_end_time(ptrdcmkme->ExecuteFrameDCM(frames.front(), frames.back()));
						actionPID = KME_ACTIONPID;
					}

					pam->set_command("NULL");
				}
			}
			else
			{
				SpAssocCont::iterator it = SpActions.find(pam->command());

				if (it == SpActions.end())
				{
					Logger::Instance().WriteMsg("MotionController", "SpAction " + pam->command() + " not found!", Logger::Error);
				}
				else
				{
					stopWalkCommand();

					if (pam->command() == "LieDown")
					{
						killHeadCommand();
					}
					else if (pam->command() == "PuntKick")
					{
						killHeadCommand();
						robotUp = false;
					}

					if (str.compare("kme") == 0)
					{
						boost::shared_ptr<ISpecialAction> ptr = it->second;
						KmeAction* ptrdcmkme = (KmeAction*) ptr.get();
						KmeManager::set_end_time(ptrdcmkme->ExecuteDCM());
						actionPID = KME_ACTIONPID;
					}
					else
					{
						actionPID = it->second->ExecutePost();
					}

					pam->set_command("NULL");
					Logger::Instance().WriteMsg("MotionController", "  Action ID: " + _toString(actionPID), Logger::ExtraInfo);
				}
			}
		}
	}

	return 0;
}

void MotionController::read_messages()
{
	/* Messages for Walk, Head, Action */
	hm = _blk.readSignal<MotionHeadMessage> ("motion");
	wm = _blk.readSignal<MotionWalkMessage> ("motion");
	am = _blk.readSignal<MotionActionMessage> ("motion");
	/* Messages for Intertial Readings */
	allsm = _blk.readData<AllSensorValuesMessage> ("sensors");
	/* Messages from the Game Controller */
	gsm = _blk.readState<GameStateMessage> ("worldstate");
	//Logger::Instance().WriteMsg("MotionController", "read_messages ", Logger::ExtraExtraInfo);
}

void MotionController::killWalkCommand()
{
	motion->killWalk();
	walkingWithVelocity = false;
	Logger::Instance().WriteMsg("MotionController", "Killed Walk Command", Logger::ExtraInfo);
}

void MotionController::stopWalkCommand()
{
	if (walkingWithVelocity || walkPID != 0)
	{
		motion->setWalkTargetVelocity(0.0, 0.0, 0.0, 1.0, walkConfig); // stop walk
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
	walkPID = 0;
	headPID = 0;
	actionPID = 0;
	Logger::Instance().WriteMsg("MotionController", "Killed All Commands", Logger::ExtraInfo);
}

void MotionController::ALstandUp()
{
	Logger::Instance().WriteMsg("MotionController", "Choose standUp", Logger::ExtraInfo);

	if (angY < 0.0)
	{
		// Robot
		//tts->pCall<AL::ALValue>(std::string("say"), std::string("Face Up!"));
		ALstandUpBack();
		Logger::Instance().WriteMsg("MotionController", "Stand Up: From Back", Logger::ExtraInfo);
	}
	else
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

	//Logger::Instance().WriteMsg("MotionController", "Stand Up Cross ", Logger::ExtraInfo);
}

void MotionController::ALstandUpBack()
{
	SpAssocCont::iterator it = SpActions.find("StandUpFromBack.xar");

	if (it == SpActions.end())
		Logger::Instance().WriteMsg("MotionController", "SpAction ALstandUpBack2010 not found!", Logger::Error);
	else
	{
		actionPID = it->second->ExecutePost();
		//Logger::Instance().WriteMsg("MotionController", "Stand Up 2011: From Back", Logger::ExtraInfo);
	}
}

void MotionController::ALstandUpFront()
{
	SpAssocCont::iterator it = SpActions.find("StandFromFrontMexico.xar");

	if (it == SpActions.end())
		Logger::Instance().WriteMsg("MotionController", "SpAction ALstandUpFront2011 not found!", Logger::Error);
	else
	{
		actionPID = it->second->ExecutePost();
		//	Logger::Instance().WriteMsg("MotionController", "Stand Up 2011: From Front", Logger::ExtraInfo);
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
	//		_blk.publishSignal(*hmot,"motion");
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
	//		_blk.publishSignal(*wmot,"motion");
	//		delete wmot;
	//	}
	//
	//	if ((actionPID == 0) && ((counter + 30) % 500 == 0) && (counter > 0)) {
	//		MotionActionMessage* amot = new MotionActionMessage();
	//		//amot->set_topic("motion");
	//		amot->set_command("PenalizedZeroPos.xar");
	//		Logger::Instance().WriteMsg("MotionController", "Sending Command: action ", Logger::ExtraInfo);
	//		_blk.publishSignal(*amot, "motion");
	//		delete amot;
	//	}
	//
	//		if ((actionPID == 0) && ((counter + 130) % 10 == 0) && (counter > 0))
	//		{
	//			MotionActionMessage* amot = new MotionActionMessage();
	//			amot->set_command("KickForwardRight.kme");
	//			Logger::Instance().WriteMsg("MotionController", "Sending Command: action ", Logger::ExtraInfo);
	//			_blk.publishSignal(*amot, "motion");
	//			delete amot;
	//		}
	//
	//	if ((actionPID == 0) && ((counter + 130) % 10 == 0) && (counter > 0))
	//	{
	//		MotionActionMessage* amot = new MotionActionMessage();
	//		amot->set_command("RightKick3.xar");
	//		Logger::Instance().WriteMsg("MotionController", "Sending Command: action ", Logger::ExtraInfo);
	//		_blk.publishSignal(*amot, "motion");
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
	//	//		_blk.publishSignal(*amot,"motion");
	//	//		delete amot;
	//	//	}
}

void MotionController::createDCMAlias()
{
	//Logger::Instance().WriteMsg("MotionController","Creating DCM aliases",Logger::ExtraInfo);
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
	}
	catch (const AL::ALError &e)
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

	//Logger::Instance().WriteMsg("MotionController"," Head PositionActuatorAlias created ",Logger::ExtraInfo);
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


vector<int> MotionController::SpCutActionsManager()
{
	vector<int> frames;
	std::string str = pam->command();
	str.erase(str.size() - 1, str.size());
	SpCont::iterator it = SpKmexActions.find(str);

	//	Logger::Instance().WriteMsg("MotionController", "SpCutActionsManager - PAM: " + str, Logger::ExtraInfo);
	if (it == SpKmexActions.end())
		Logger::Instance().WriteMsg("MotionController", "SpKmexActions " + str + " not found!", Logger::Error);
	else
	{
		boost::shared_ptr<KmexAction> ptr = it->second;
		KmexAction* ptrkme = (KmexAction*) ptr.get();
		frames = ptrkme->AngleCompare(allsm);
	}

	return frames;
}

vector<float> MotionController::KGetAngles()
{
	static vector<float> angleStore(22);

	if(allsm != 0)
	{
		angleStore[0] = allsm->jointdata(HEAD + YAW).sensorvalue();
		angleStore[1] = allsm->jointdata(HEAD + PITCH).sensorvalue();
		angleStore[2] = allsm->jointdata(L_ARM + SHOULDER_PITCH).sensorvalue();
		angleStore[3] = allsm->jointdata(L_ARM + SHOULDER_ROLL).sensorvalue();
		angleStore[4] = allsm->jointdata(L_ARM + ELBOW_YAW).sensorvalue();
		angleStore[5] = allsm->jointdata(L_ARM + ELBOW_ROLL).sensorvalue();
		angleStore[6] = allsm->jointdata(L_LEG + HIP_YAW_PITCH).sensorvalue();
		angleStore[7] = allsm->jointdata(L_LEG + HIP_ROLL).sensorvalue();
		angleStore[8] = allsm->jointdata(L_LEG + HIP_PITCH).sensorvalue();
		angleStore[9] = allsm->jointdata(L_LEG + KNEE_PITCH).sensorvalue();
		angleStore[10] = allsm->jointdata(L_LEG + ANKLE_PITCH).sensorvalue();
		angleStore[11] = allsm->jointdata(L_LEG + ANKLE_ROLL).sensorvalue();
		angleStore[12] = allsm->jointdata(R_LEG + HIP_YAW_PITCH).sensorvalue();
		angleStore[13] = allsm->jointdata(R_LEG + HIP_ROLL).sensorvalue();
		angleStore[14] = allsm->jointdata(R_LEG + HIP_PITCH).sensorvalue();
		angleStore[15] = allsm->jointdata(R_LEG + KNEE_PITCH).sensorvalue();
		angleStore[16] = allsm->jointdata(R_LEG + ANKLE_PITCH).sensorvalue();
		angleStore[17] = allsm->jointdata(R_LEG + ANKLE_ROLL).sensorvalue();
		angleStore[18] = allsm->jointdata(R_ARM + SHOULDER_PITCH).sensorvalue();
		angleStore[19] = allsm->jointdata(R_ARM + SHOULDER_ROLL).sensorvalue();
		angleStore[20] = allsm->jointdata(R_ARM + ELBOW_YAW).sensorvalue();
		angleStore[21] = allsm->jointdata(R_ARM + ELBOW_ROLL).sensorvalue();
	}

	return angleStore;
}

void MotionController::readWalkParameters()
{
	//setMotionConfig is deprecated
	walkConfig.arraySetSize(7);

	string filename = "walk_parameters";
	XmlNode * walkPamNode = _xml.findNodeForKey(filename);
	int itteration = 0;
	for(map<string,vector<XmlNode> >::iterator it = walkPamNode->kids.begin(); it != walkPamNode->kids.end(); it++){

		std::istringstream strs( (walkPamNode->findValueForKey((*it).first)).front() );
		float value;
		strs>>value;
		if((*it).first.compare("EnableFallManager")!=0){
			walkConfig[itteration].arraySetSize(2);
			walkConfig[itteration][0] = (*it).first;
			walkConfig[itteration][1] = value;
			itteration++;
		}else{
			motion->setFallManagerEnabled(value);
		}
	}

}

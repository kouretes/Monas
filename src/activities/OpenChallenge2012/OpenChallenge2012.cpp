#include "OpenChallenge2012.h"

#include "hal/robot/generic_nao/kAlBroker.h"
#include "hal/robot/generic_nao/robot_consts.h"
#include "hal/robot/generic_nao/aldebaran-sensors.h"
#include "architecture/archConfig.h"


#include "tools/toString.h"
#include "XarManager.h"

#define FULLSTIFFNESS 0.9
#define LEANTOOMUCH 0.7
#define ANGLEHOR 1.6
#define INTTIME 0.35 //angle integration time. Look ahead for so many seconds Too large valies mean large sensitivity, too small means too late reaction

#define MAXHEADYAWSPEED 2.5
#define MAXHEADPITCHSPEED 2.5

#define KME_ACTIONPID -1

ACTIVITY_REGISTER(OpenChallenge2012);
namespace
{
	ActivityRegistrar<OpenChallenge2012>::Type temp("OpenChallenge2012");
}
using namespace std;
using namespace KDeviceLists;
OpenChallenge2012::OpenChallenge2012(Blackboard &b) : IActivity(b)
{
	waitfor = microsec_clock::universal_time() - hours(1);
}

void OpenChallenge2012::UserInit()
{

	try
	{
		dcm = KAlBroker::Instance().GetBroker()->getDcmProxy();
	} catch (AL::ALError& e)
	{
		Logger::Instance().WriteMsg("OpenChallenge2012", "Error in getting dcm proxy", Logger::FatalError);
	}

	try
	{
		motion = KAlBroker::Instance().GetBroker()->getMotionProxy();
	} catch (AL::ALError& e)
	{
		Logger::Instance().WriteMsg("OpenChallenge2012", "Error in getting motion proxy", Logger::FatalError);
	}

	try
	{
		pbroker = AL::ALPtr<AL::ALBroker>(KAlBroker::Instance().GetBroker());
		framemanager = AL::ALPtr<AL::ALFrameManagerProxy>(new AL::ALFrameManagerProxy(pbroker));
	} catch (AL::ALError& e)
	{
		Logger::Instance().WriteMsg("OpenChallenge2012", "Error in getting frameManager proxy" + e.getDescription(), Logger::FatalError);
	}

	Logger::Instance().WriteMsg("OpenChallenge2012", "Loading special actions!", Logger::Info);

	{
		std::vector<std::string> registeredSpecialActions = SpecialActionFactory::Instance()->GetRegisteredProducts();
		std::vector<std::string>::const_iterator it;
		for (it = registeredSpecialActions.begin(); it < registeredSpecialActions.end(); ++it)
		{
			SpActions.insert(SpAsoocElement(*it, boost::shared_ptr<ISpecialAction>(SpecialActionFactory::Instance()->CreateObject(*it))));
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
	//motion->setWalkArmsEnable(true, true);

	//TODO motion->setMotionConfig([["ENABLE_STIFFNESS_PROTECTION",true]]);



	//motion->setMotionConfig(AL::ALValue::array(AL::ALValue::array("ENABLE_FOOT_CONTACT_PROTECTION", true)));
	//motion->setMotionConfig(AL::ALValue::array(AL::ALValue::array("ENABLE_FALL_MANAGEMENT_PROTECTION", false)));

	Logger::Instance().WriteMsg("OpenChallenge2012", "Subcribing to topics", Logger::Info);

	_blk.updateSubscription("motion", msgentry::SUBSCRIBE_ON_TOPIC);
	_blk.updateSubscription("sensors", msgentry::SUBSCRIBE_ON_TOPIC);
	_blk.updateSubscription("vision", msgentry::SUBSCRIBE_ON_TOPIC);
	_blk.updateSubscription("worldstate", msgentry::SUBSCRIBE_ON_TOPIC);

	donee = false;
	AccZvalue = 0.0;
	AccXvalue = 0.0;
	AccYvalue = 0.0;

	robotDown = false;
	robotUp = true;

	walkPID = 0;
	headPID = 0;
	actionPID = 0;
	currentstate=1000;

	counter = 0;
	startl =boost::posix_time::microsec_clock::universal_time();
	startr =boost::posix_time::microsec_clock::universal_time();
	pam = new MotionActionMessage();
	pam->set_command("NULL");

	walkingWithVelocity = false;
	//setStiffnessDCM(1);
	Logger::Instance().WriteMsg("OpenChallenge2012", "Initialization Completed", Logger::Info);
}

int OpenChallenge2012::Execute()
{
	//counter++;
	//Logger::Instance().WriteMsg("OpenChallenge2012","OpenChallenge2012 BEGIN execution "+_toString(counter),Logger::Info);
	//testcommands();
	//if(counter%100==0)
	read_messages();
	mglrun();
	//Logger::Instance().WriteMsg("OpenChallenge2012","OpenChallenge2012 END   execution "+_toString(counter),Logger::Info);
	return 0;
}

void OpenChallenge2012::read_messages()
{

	vector<float> AangleTemp(22);
	vector<float> KangleTemp(22);
	vector<float> CangleTemp(22);
	/* Messages for Walk, Head, Action */
	hm = _blk.readSignal<MotionHeadMessage> ("motion");
	wm = _blk.readSignal<MotionWalkMessage> ("motion");
	am = _blk.readSignal<MotionActionMessage> ("motion");

	/* Messages for Intertial Readings */
	allsm = _blk.readData<AllSensorValuesMessage> ("sensors");

	/* Messages from the Game Controller */
	gsm = _blk.readState<GameStateMessage> ("worldstate");
	
	obsm = _blk.readSignal<ObservationMessage> ("vision");

	wim  = _blk.readData<WorldInfo> ("worldstate");
	//Logger::Instance().WriteMsg("OpenChallenge2012", "read_messages ", Logger::ExtraExtraInfo);
}

void OpenChallenge2012::mglrun()
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
		if(allsm != 0){
			std::vector<float> fll,empty;
			LHYPitch = allsm->jointdata(KDeviceLists::L_LEG+KDeviceLists::HIP_YAW_PITCH);
			LHRoll = allsm->jointdata(KDeviceLists::L_LEG+KDeviceLists::HIP_ROLL);
			LHPitch = allsm->jointdata(KDeviceLists::L_LEG+KDeviceLists::HIP_PITCH);
			LKPitch = allsm->jointdata(KDeviceLists::L_LEG+KDeviceLists::KNEE_PITCH);
			LARoll = allsm->jointdata(KDeviceLists::L_LEG+KDeviceLists::ANKLE_ROLL);
			LAPitch = allsm->jointdata(KDeviceLists::L_LEG+KDeviceLists::ANKLE_PITCH);
			fll.push_back(LHYPitch.sensorvalue());fll.push_back(LHRoll.sensorvalue());fll.push_back(LHPitch.sensorvalue());fll.push_back(LKPitch.sensorvalue());fll.push_back(LAPitch.sensorvalue());fll.push_back(LARoll.sensorvalue());
			FKin::FKvars output;
			output = FKin::filterForwardFromTo("Torso","LeftLeg",empty,fll);
			RSPitch = allsm->jointdata(KDeviceLists::R_ARM+KDeviceLists::SHOULDER_PITCH);
			RSRoll= allsm->jointdata(KDeviceLists::R_ARM+KDeviceLists::SHOULDER_ROLL);
			REYaw = allsm->jointdata(KDeviceLists::R_ARM+KDeviceLists::ELBOW_YAW);
			RERoll = allsm->jointdata(KDeviceLists::R_ARM+KDeviceLists::ELBOW_ROLL);
			torsoHeight = output.pointZ;
			donee = true;
		}
	}
	else if(gameState==PLAYER_INITIAL)
	{
		killCommands();
		motion->setStiffnesses("Body", 0.2);

		SpAssocCont::iterator it = SpActions.find("PoseInitial.xar");
		if (it == SpActions.end())
			Logger::Instance().WriteMsg("OpenChallenge2012", std::string("SpAction ") + "PoseInitial.xar" + " not found!", Logger::Error);
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
			Logger::Instance().WriteMsg("OpenChallenge2012", std::string("SpAction ") + "PenalizedZeroPos.xar"+ " not found!", Logger::Error);
		else
			actionPID = it->second->ExecutePost();
		currentstate=gameState;

	}
	else
	{
		motion->setStiffnesses("Body", FULLSTIFFNESS);
		//motion->setStiffnesses("LArm", 0.0f);
		float PI = 3.1415;
		//attack here
		if(donee && wim != 0 && wim->balls_size() > 0){
			float bx = wim->balls(0).relativex()+wim->balls(0).relativexspeed()*0.200;
			float by = wim->balls(0).relativey()+wim->balls(0).relativeyspeed()*0.200;
			float bd = sqrt(pow(bx,2)+pow(by,2));
			float bb = atan2(by,bx);
			float dist = bd*1000;
			float angle = bb;
			float yPoint = dist*sin(angle);
			float xPoint = dist*cos(angle);
			float zPoint = torsoHeight;
			
			float startX =0,startY = 113,startZ = 100;
			
			float C = 218.7/sqrt(pow(startX-xPoint,2)+pow(startY-yPoint,2)+pow(startZ-zPoint,2));
			float zK = C*(zPoint-startZ) + startZ;
			float xK = C*(xPoint-startX) + startX;
			float yK = C*(yPoint-startY) + startY;
			float height = startZ -zK;
			float angley = fabs(asin(height/218.7));
			float dist2 = sqrt(pow(startX-xK,2)+pow(startY-yK,2)+pow(startZ-zK,2));

			float dist3 = startY - yK;
			float anglez = atan2(xK,dist3) -PI/2;
			AL::ALValue names;
			AL::ALValue angleLists;
			AL::ALValue timeLists  = 0.05f;
			bool isAbsolute        = true;
			vector<vector<float> > results;
			results = IKin::inverseLeftHand(xK, yK, zK, 0,angley, anglez);
			if(!results.empty()){
				startl =boost::posix_time::microsec_clock::universal_time();
				vector<float> result = results.front();
				std::vector<float>::iterator iter;
				iter = result.begin();
				names = AL::ALValue::array("LShoulderPitch", "LShoulderRoll", "LElbowYaw", "LElbowRoll");
				float angle1 = *iter;
				iter++;
				float angle2 = *iter;
				iter++;
				float angle3 = *iter;
				iter++;
				float angle4 = *iter;
				angleLists = AL::ALValue::array(angle1,angle2,angle3,angle4);
				motion->angleInterpolation(names, angleLists, timeLists, isAbsolute);
			}else{
				stopl = boost::posix_time::microsec_clock::universal_time();
				if(stopl-startl >= boost::posix_time::seconds(0.1)){
					AL::ALValue names;
					AL::ALValue angleLists;
					AL::ALValue timeLists  = 0.2f;
					bool isAbsolute        = true;
					names = AL::ALValue::array("LShoulderPitch", "LShoulderRoll", "LElbowRoll", "LElbowYaw");
					
					angleLists = AL::ALValue::array(PI/2,PI/10,0.0f,0.0f);
					motion->angleInterpolation(names, angleLists, timeLists, isAbsolute);
				}
			}
			//RIGHT HAND------------------------------------------------------------------------------------------------------------------------------------
			
			startY = -113;
			
			C = 218.7/sqrt(pow(startX-xPoint,2)+pow(startY-yPoint,2)+pow(startZ-zPoint,2));
			zK = C*(zPoint-startZ) + startZ;
			xK = C*(xPoint-startX) + startX;
			yK = C*(yPoint-startY) + startY;
			height = startZ -zK;
			angley = abs(asin(height/218.7));
			dist2 = sqrt(pow(startX-xK,2)+pow(startY-yK,2)+pow(startZ-zK,2));

			dist3 = startY - yK;
			anglez = atan2(xK,dist3) -PI/2;

			results = IKin::inverseRightHand(xK, yK, zK, 0,angley, anglez);
			if(!results.empty()){
				startr =boost::posix_time::microsec_clock::universal_time();
				vector<float> result = results.front();
				Logger::Instance().WriteMsg("Kofi","Mpika Deksia", Logger::Info);
				std::vector<float>::iterator iter;
				iter = result.begin();
				names = AL::ALValue::array("RShoulderPitch", "RShoulderRoll", "RElbowYaw", "RElbowRoll");
				float angle1 = *iter;
				iter++;
				float angle2 = *iter;
				iter++;
				float angle3 = *iter;
				iter++;
				float angle4 = *iter;
				angleLists = AL::ALValue::array(angle1,angle2,angle3,angle4);
				motion->angleInterpolation(names, angleLists, timeLists, isAbsolute);
			}else{
				stopr = boost::posix_time::microsec_clock::universal_time();
				if(stopr-startr >= boost::posix_time::seconds(0.1)){
					AL::ALValue names;
					AL::ALValue angleLists;
					AL::ALValue timeLists  = 0.2f;
					bool isAbsolute        = true;
					names = AL::ALValue::array("RShoulderPitch", "RShoulderRoll", "RElbowRoll", "RElbowYaw");
					
					angleLists = AL::ALValue::array(PI/2,-PI/10,0.0f,0.0f);
					motion->angleInterpolation(names, angleLists, timeLists, isAbsolute);
				}
			}
		}else{
			stopl = boost::posix_time::microsec_clock::universal_time();
			if(stopl-startl >= boost::posix_time::seconds(0.1)){
				AL::ALValue names;
				AL::ALValue angleLists;
				AL::ALValue timeLists  = 0.2f;
				bool isAbsolute        = true;
				names = AL::ALValue::array("LShoulderPitch", "LShoulderRoll", "LElbowRoll", "LElbowYaw");
				
				angleLists = AL::ALValue::array(PI/2,PI/10,0.0f,0.0f);
				motion->angleInterpolation(names, angleLists, timeLists, isAbsolute);
			}
			stopr = boost::posix_time::microsec_clock::universal_time();
			if(stopr-startr >= boost::posix_time::seconds(0.1)){
				AL::ALValue names;
				AL::ALValue angleLists;
				AL::ALValue timeLists  = 0.2f;
				bool isAbsolute        = true;
				names = AL::ALValue::array("RShoulderPitch", "RShoulderRoll", "RElbowRoll", "RElbowYaw");
				
				angleLists = AL::ALValue::array(PI/2,-PI/10,0.0f,0.0f);
				motion->angleInterpolation(names, angleLists, timeLists, isAbsolute);
			}		
		}
	}

	/* Check if an Action command has been completed */
	if ( ((actionPID > 0) && !motion->isRunning(actionPID) && !framemanager->isRunning(actionPID)) || (actionPID == KME_ACTIONPID) )
	{
		if (actionPID == KME_ACTIONPID)
		{
			SpAssocCont::iterator it = SpActions.find("InitPose.xar");
			if (it == SpActions.end())
				Logger::Instance().WriteMsg("OpenChallenge2012", std::string("SpAction ") + "InitPose.xar" + " not found!", Logger::Error);
			else
				actionPID = it->second->ExecutePost();

			return;
		}
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
		Logger::Instance().WriteMsg("OpenChallenge2012", "Action completed! Motion executed " + _toString(counter) + " times.", Logger::ExtraInfo);
	}




	/* The robot is up and ready to execute motions */
	if (robotUp)
	{

		/* Check if a Walk command has been completed */
		if ((walkPID != 0) && !motion->isRunning(walkPID) && !motion->walkIsActive())
		{
			walkPID = 0;
		//	Logger::Instance().WriteMsg("OpenChallenge2012", "Walk completed! Motion executed " + _toString(counter) + " times.", Logger::ExtraInfo);
		}

		/* Check if a Head command has been completed */
		if ((headPID != 0) && !motion->isRunning(headPID))
		{
			headPID = 0;
		//	Logger::Instance().WriteMsg("OpenChallenge2012", "Head completed! Motion executed " + _toString(counter) + " times.", Logger::ExtraInfo);
		}

		/* Check if there is a command to execute */

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
				Logger::Instance().WriteMsg("OpenChallenge2012", "Invalid Head Command: " + hm->command(), Logger::ExtraInfo);
		}

		if ((actionPID == 0) && ((am != NULL) || (am == NULL && pam->command()!="NULL")) )
		{
			if (am != NULL)
			{
				pam->set_command(am->command());

			std::string str = pam->command();
			std::string strKick = pam->command();
			unsigned int pos = 0;
			unsigned int posKick = 0;
			pos = str.find_first_of(".");
			str.erase(0,pos+1);
			strKick.erase(pos, strKick.size());

        	sm.set_type(MotionStateMessage::ACTION);
			sm.set_detail(str);
			sm.set_lastaction(pam->command());
        	_blk.publishState(sm,"worldstate");
			SpAssocCont::iterator it = SpActions.find(pam->command());
			if (it == SpActions.end()){
				;
			}
			else{
				stopWalkCommand();
				if (pam->command() == "LieDown")
				{
					killHeadCommand();
				} else if (pam->command() == "PuntKick")
				{
					killHeadCommand();
					robotUp = false;
				}
				if (str.compare("kme") == 0)
				{
				}
				else
				{
					actionPID = it->second->ExecutePost();
				}
				pam->set_command("NULL");
			}
		}
	}
	return;
}
	return;
}

void OpenChallenge2012::killWalkCommand()
{
	motion->killWalk();
	walkingWithVelocity=false;
	Logger::Instance().WriteMsg("OpenChallenge2012", "Killed Walk Command", Logger::ExtraInfo);
}

void OpenChallenge2012::stopWalkCommand()
{
	if (walkingWithVelocity||walkPID!=0)
	{
		motion->setWalkTargetVelocity(0.0, 0.0, 0.0, 1.0); // stop walk
		motion->waitUntilWalkIsFinished();
		walkingWithVelocity = false;
	}
	walkPID = 0;
}

void OpenChallenge2012::killHeadCommand()
{
	if (headPID != 0)
	{
		motion->killTask(headPID);
		headPID = 0;
		Logger::Instance().WriteMsg("OpenChallenge2012", "Killed Head Command", Logger::ExtraInfo);
	}
}

void OpenChallenge2012::killActionCommand()
{
	if (actionPID != 0)
	{
		motion->killTask(actionPID);
		actionPID = 0;
		Logger::Instance().WriteMsg("OpenChallenge2012", "Killed Action Command", Logger::ExtraInfo);
	}
}

void OpenChallenge2012::killCommands()
{

	//TODO check if command stops !! framemanager->pCall("cleanBehaviors");
	motion->killAll();
	walkPID = 0;
	headPID = 0;
	actionPID = 0;
	Logger::Instance().WriteMsg("OpenChallenge2012", "Killed All Commands", Logger::ExtraInfo);
}


void OpenChallenge2012::createDCMAlias()
{
	//Logger::Instance().WriteMsg("OpenChallenge2012","Creating DCM aliases",Logger::ExtraInfo);
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

	}
	

}



vector<float> OpenChallenge2012::KGetAngles(){
	 static vector<float> angleStore(22);

	if(allsm != 0){
		angleStore[0] = allsm->jointdata(HEAD+YAW).sensorvalue();
		angleStore[1] = allsm->jointdata(HEAD+PITCH).sensorvalue();

		angleStore[2] = allsm->jointdata(L_ARM+SHOULDER_PITCH).sensorvalue();
		angleStore[3] = allsm->jointdata(L_ARM+SHOULDER_ROLL).sensorvalue();
		angleStore[4] = allsm->jointdata(L_ARM+ELBOW_YAW).sensorvalue();
		angleStore[5] = allsm->jointdata(L_ARM+ELBOW_ROLL).sensorvalue();

		angleStore[6] = allsm->jointdata(L_LEG+HIP_YAW_PITCH).sensorvalue();
		angleStore[7] = allsm->jointdata(L_LEG+HIP_ROLL).sensorvalue();
		angleStore[8] = allsm->jointdata(L_LEG+HIP_PITCH).sensorvalue();
		angleStore[9] = allsm->jointdata(L_LEG+KNEE_PITCH).sensorvalue();
		angleStore[10] = allsm->jointdata(L_LEG+ANKLE_PITCH).sensorvalue();
		angleStore[11] = allsm->jointdata(L_LEG+ANKLE_ROLL).sensorvalue();

		angleStore[12] = allsm->jointdata(R_LEG+HIP_YAW_PITCH).sensorvalue();
		angleStore[13] = allsm->jointdata(R_LEG+HIP_ROLL).sensorvalue();
		angleStore[14] = allsm->jointdata(R_LEG+HIP_PITCH).sensorvalue();
		angleStore[15] = allsm->jointdata(R_LEG+KNEE_PITCH).sensorvalue();
		angleStore[16] = allsm->jointdata(R_LEG+ANKLE_PITCH).sensorvalue();
		angleStore[17] = allsm->jointdata(R_LEG+ANKLE_ROLL).sensorvalue();

		angleStore[18] = allsm->jointdata(R_ARM+SHOULDER_PITCH).sensorvalue();
		angleStore[19] = allsm->jointdata(R_ARM+SHOULDER_ROLL).sensorvalue();
		angleStore[20] = allsm->jointdata(R_ARM+ELBOW_YAW).sensorvalue();
		angleStore[21] = allsm->jointdata(R_ARM+ELBOW_ROLL).sensorvalue();

	}
	return angleStore;
}

void OpenChallenge2012::readWalkParameters()
{

	std::string fname=ArchConfig::Instance().GetConfigPrefix() +"walk_parameters.xml";
	TiXmlDocument d(fname);
	if(!d.LoadFile())
	{
		Logger::Instance().WriteMsg("OpenChallenge2012", "walk_parameters.xml cannot be parsed", Logger::Warning);
		return;
	}
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

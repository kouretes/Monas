#include "MotionController.h"

#include "hal/robot/generic_nao/kAlBroker.h"

#include "tools/logger.h"
#include "tools/toString.h"
#include "architecture/narukom/pub_sub/filters/special_filters.h"

#include "ISpecialAction.h"
#include "KmeManager.h"

namespace {
	ActivityRegistrar<MotionController>::Type temp("MotionController");
}


MotionController::MotionController(){
}

void MotionController::UserInit() {
	//try {
		//tts = KAlBroker::Instance().GetBroker()->getProxy("ALTextToSpeech");
	//} catch (AL::ALError& e) {
		//Logger::Instance().WriteMsg("MotionController","Error in getting TextToSpeech proxy",Logger::FatalError);
	//}
	try {
		motion = KAlBroker::Instance().GetBroker()->getMotionProxy();
	} catch (AL::ALError& e) {
		Logger::Instance().WriteMsg("MotionController","Error in getting motion proxy",Logger::FatalError);
	}
	motion->setStiffnesses("Body", 0.9);
	motion->setWalkArmsEnable(true, true);
	//AL::ALValue temp;
	//temp.arraySetSize(2);
	//temp[0] = "ENABLE_FOOT_CONTACT_PROTECTION";
	//temp[1] = true;
	//motion->setMotionConfig(temp);
	//TODO motion->setMotionConfig([["ENABLE_STIFFNESS_PROTECTION",true]]);

	Logger::Instance().WriteMsg("MotionController", "Subcribing to topics", Logger::Info);
	//_com->get_message_queue()->add_subscriber(_blk);
	_com->get_message_queue()->subscribe("motion", _blk, 0);
	_com->get_message_queue()->subscribe("sensors", _blk, 0);
	//_com->get_message_queue()->add_publisher(this);

	//wm = NULL;
	//hm = NULL;
	//am = NULL;
	//im = NULL;


	AccZvalue = 0.0;
	AccXvalue = 0.0;
	AccYvalue = 0.0;

	robotDown = false;
	robotUp = true;

	walkPID = 0;
	headPID = 0;
	actionPID = 0;

	counter = 0;

	walkingWithVelocity = false;

        Logger::Instance().WriteMsg("MotionController","Loading special actions!",Logger::Info);

        {
          std::vector<std::string> registeredSpecialActions = SpecialActionFactory::Instance()->GetRegisteredProducts();
          std::vector<std::string>::const_iterator it;
          for ( it = registeredSpecialActions.begin(); it < registeredSpecialActions.end(); ++it) {
            SpActions.insert(
                SpAsoocElement(*it,boost::shared_ptr<ISpecialAction>(SpecialActionFactory::Instance()->CreateObject(*it)) )
            );
          }
        }

        {
          std::vector<ISpecialAction*> kmeActions = KmeManager::LoadActionsKME();
          std::vector<ISpecialAction*>::const_iterator it;
          for ( it = kmeActions.begin(); it < kmeActions.end(); ++it) {
            SpActions.insert(
                SpAsoocElement((*it)->GetName(),boost::shared_ptr<ISpecialAction>(*it) )
            );
          }

        }
}

int MotionController::Execute() {
	counter++;
	//Logger::Instance().WriteMsg("MotionController","MotionController BEGIN execution "+_toString(counter),Logger::Info);
	//commands();
	read_messages();
	mglrun();
	//Logger::Instance().WriteMsg("MotionController","MotionController END   execution "+_toString(counter),Logger::Info);
	return 0;
}

void MotionController::read_messages() {


	/* Messages for Calibration */
	hm = _blk->read_signal<MotionHeadMessage>("MotionHeadMessage");

	/* Messages for Walk, Head, Action */
	wm = _blk->read_signal<MotionWalkMessage>("MotionWalkMessage");
	//dif (hm == NULL) hm = _blk->in_msg_nb<MotionHeadMessage>("MotionHeadMessage");
	am = _blk->read_signal<MotionActionMessage>("MotionActionMessage");

	/* Messages for Intertial Readings */
	im = _blk->read_data<InertialSensorsMessage>("InertialSensorsMessage");

	//Logger::Instance().WriteMsg("MotionController", "read_messages ", Logger::ExtraExtraInfo);

}


void MotionController::mglrun() {

	if (im != NULL) {
		AccZ = im->sensordata(2);
		AccZvalue = AccZ.sensorvalue();
		AccX = im->sensordata(0);
		AccXvalue = AccX.sensorvalue();
		AccY = im->sensordata(1);
		AccYvalue = AccY.sensorvalue();
	}


	/* Check if the robot is falling and remove stiffness, kill all motions */
#ifdef WEBOTS
	if ( (im != NULL) && (!robotDown) && (robotUp) && (AccZvalue < 5.5) ) { // Webots
#else
	if ( (im != NULL) && (!robotDown) && (robotUp) && (AccZvalue > -40) ) { // Robot
#endif
		motion->setStiffnesses("Body", 0.0);
		Logger::Instance().WriteMsg("MotionController","Robot falling: Stiffness off",Logger::ExtraInfo);
		RejectAllFilter reject_filter("RejectFilter");
		_blk->getBuffer()->add_filter(&reject_filter);
		usleep(800000);
		robotUp = false;
		robotDown = true;
		killCommands();
//		tts->pCall<AL::ALValue>(std::string("say"), std::string("Ouch!"));
		motion->setStiffnesses("Body", 0.5);
		usleep(300000);
		ALstandUpCross();
		_blk->getBuffer()->remove_filter(&reject_filter);
		Logger::Instance().WriteMsg("MotionController", "Stand Up: Cross", Logger::ExtraInfo);
		return;
	}

	/* Check if the robot is down and stand up */
	if (robotDown) {
		Logger::Instance().WriteMsg("MotionController", "Will stand up now ...", Logger::ExtraInfo);
		motion->setStiffnesses("Body", 1.0);
		robotDown = false;
		ALstandUp();
		Logger::Instance().WriteMsg("MotionController", "StandUp ID: "+_toString(actionPID), Logger::ExtraInfo);
		return;
	}

	/* Check if an Action command has been completed */
	if ( (actionPID != 0) && !motion->isRunning(actionPID) ) {
		actionPID = 0;
		Logger::Instance().WriteMsg("MotionController","Action completed! Motion executed "+_toString(counter)+" times.", Logger::ExtraInfo);
	}

	/* Check if the robot stood up after a stand up procedure */
	if (!robotUp && !robotDown) {
#ifdef WEBOTS
		if ( (actionPID == 0) && (AccZvalue > 8.5) ) { // Webots
#else
		if ( (actionPID == 0) && (AccZvalue < -40) ) { // Robot
#endif
			robotUp = true;
			Logger::Instance().WriteMsg("MotionController","Stood up ...",Logger::ExtraInfo);
		} else if (actionPID == 0) {
			RejectAllFilter reject_filter("RejectFilter");
			_blk->getBuffer()->add_filter(&reject_filter);
			robotDown = true;
			motion->setStiffnesses("Body", 0.5);
			usleep(300000);
			ALstandUpCross();
			_blk->getBuffer()->remove_filter(&reject_filter);
			Logger::Instance().WriteMsg("MotionController", "Stand Up: Cross", Logger::ExtraInfo);
			return;
		}
	}

	/* The robot is up and ready to execute motions */
	if (robotUp) {

		/* Check if a Walk command has been completed */
		if ((walkPID != 0) && !motion->isRunning(walkPID) && !motion->walkIsActive()) {
			walkPID = 0;
			Logger::Instance().WriteMsg("MotionController","Walk completed! Motion executed "+_toString(counter)+" times.",Logger::ExtraInfo);
		}

		/* Check if a Head command has been completed */
		if ((headPID != 0) && !motion->isRunning(headPID)) {
			headPID = 0;
			Logger::Instance().WriteMsg("MotionController","Head completed! Motion executed "+_toString(counter)+" times.",Logger::ExtraInfo);
		}

		/* Check if there is a command to execute */

		if ( (wm != NULL) && (actionPID==0) ) {
			if (wm->command() == "walkTo") {
				walkParam1 = wm->parameter(0);
				walkParam2 = wm->parameter(1);
				walkParam3 = wm->parameter(2);
				Logger::Instance().WriteMsg("MotionController", wm->command()+" with parameters "+_toString(walkParam1)+" "+_toString(walkParam2)+" "+_toString(walkParam3),Logger::ExtraInfo);
				walkPID = motion->post.walkTo(walkParam1, walkParam2, walkParam3);
				Logger::Instance().WriteMsg("MotionController","Walk ID: "+_toString(walkPID),Logger::ExtraInfo);
			}
			else if (wm->command() == "setWalkTargetVelocity") {
				walkParam1 = wm->parameter(0);
				walkParam2 = wm->parameter(1);
				walkParam3 = wm->parameter(2);
				walkParam4 = wm->parameter(3);
				Logger::Instance().WriteMsg("MotionController", wm->command()+" with parameters "+_toString(walkParam1)+" "+_toString(walkParam2)+" "+_toString(walkParam3)+" "+_toString(walkParam4),Logger::ExtraInfo);
				walkPID = motion->post.setWalkTargetVelocity(walkParam1, walkParam2, walkParam3, walkParam4);
                walkingWithVelocity = true;
				Logger::Instance().WriteMsg("MotionController","Walk ID: "+_toString(walkPID),Logger::ExtraInfo);
			}
			else
				Logger::Instance().WriteMsg("MotionController","Invalid Walk Command: "+wm->command(),Logger::ExtraInfo);
		}

		if (hm != NULL) {
			if (hm->command() == "setHead") {
				headParam1 = hm->parameter(0);
				headParam2 = hm->parameter(1);
				Logger::Instance().WriteMsg( "MotionController", hm->command()+" with parameters "+_toString(headParam1)+" "+_toString(headParam2),Logger::ExtraInfo);
				names.arraySetSize(2);
				values.arraySetSize(2);
				names[0] = "HeadYaw";
				values[0] = headParam1;
				names[1] = "HeadPitch";
				values[1] = headParam2;
				float fractionMaxSpeed = 0.8;
				headPID = motion->post.setAngles(names, values, fractionMaxSpeed);
				Logger::Instance().WriteMsg( "MotionController"," Head ID: "+_toString(headPID),Logger::ExtraInfo);
			}
			else if (hm->command() == "changeHead") {
				headParam1 = hm->parameter(0);
				headParam2 = hm->parameter(1);
				Logger::Instance().WriteMsg("MotionController", hm->command()+" with parameters "+_toString(headParam1)+" "+_toString(headParam2),Logger::ExtraInfo);
				names.arraySetSize(2);
				values.arraySetSize(2);
				names[0] = "HeadYaw";
				values[0] = headParam1;
				names[1] = "HeadPitch";
				values[1] = headParam2;
				float fractionMaxSpeed = 0.6;
				headPID = motion->post.changeAngles(names, values, fractionMaxSpeed);
				Logger::Instance().WriteMsg("MotionController", " Head ID: " +_toString(headPID),Logger::ExtraInfo);
			}
			else
				Logger::Instance().WriteMsg("MotionController","Invalid Head Command: "+hm->command(),Logger::ExtraInfo);
		}

		if ( (am != NULL) && (actionPID==0) ) {

			Logger::Instance().WriteMsg("MotionController", am->command(),Logger::ExtraInfo);
                        stopWalkCommand();
                        if (am->command() == "LieDown") {
                          killHeadCommand();
                        }
                        else if (am->command() == "PuntKick") {
                          killHeadCommand();
                          robotUp = false;
                        }
                        SpAssocCont::iterator it = SpActions.find( am->command() );
                        if ( it == SpActions.end() )
                          Logger::Instance().WriteMsg("MotionController", "SpAction " +am->command()+ " not found!",Logger::Error);
                        else
                          actionPID = it->second->ExecutePost();
			Logger::Instance().WriteMsg("MotionController", "  Action ID: " +_toString(actionPID),Logger::ExtraInfo);
		}

	}

	return;
}

void MotionController::killWalkCommand() {
	motion->killWalk();
	walkPID = 0;
	Logger::Instance().WriteMsg("MotionController","Killed Walk Command",Logger::ExtraInfo);
}

void MotionController::stopWalkCommand() {
    if ( motion->isRunning(walkPID) || walkingWithVelocity ) {
	  walkPID = motion->post.setWalkTargetVelocity(0.0, 0.0, 0.0, 0.0); // stop walk
	  motion->waitUntilWalkIsFinished();
      walkingWithVelocity = false;
    }
	walkPID = 0;
}

void MotionController::killHeadCommand() {
	if (headPID != 0) {
		motion->killTask(headPID);
		headPID = 0;
		Logger::Instance().WriteMsg("MotionController","Killed Head Command",Logger::ExtraInfo);
	}
}

void MotionController::killActionCommand() {
	if (actionPID != 0) {
		motion->post.killTask(actionPID);
		actionPID = 0;
		Logger::Instance().WriteMsg("MotionController","Killed Action Command",Logger::ExtraInfo);
	}
}

void MotionController::killCommands() {

	motion->post.killAll();
	//while ( motion->isRunning(walkPID) || motion->isRunning(headPID) || motion->isRunning(actionPID) ) {
		//if ( motion->isRunning(walkPID) )
			//Logger::Instance().WriteMsg("MotionController","Walk Command is running",Logger::ExtraInfo);
		//if ( motion->isRunning(headPID) )
			//Logger::Instance().WriteMsg("MotionController","Head Command is running",Logger::ExtraInfo);
		//if ( motion->isRunning(actionPID) )
			//Logger::Instance().WriteMsg("MotionController","Action Command is running",Logger::ExtraInfo);
	//}
	walkPID   = 0;
	headPID   = 0;
	actionPID = 0;
	Logger::Instance().WriteMsg("MotionController","Killed All Commands",Logger::ExtraInfo);
}

void MotionController::ALstandUp() {

  #ifdef WEBOTS
  if (AccXvalue > 1.0) { // Webots
    #else
    if (AccXvalue < 0.0) { // Robot
      #endif
      //tts->pCall<AL::ALValue>(std::string("say"), std::string("Face Up!"));
      ALstandUpBack2010();
      Logger::Instance().WriteMsg("MotionController", "Stand Up 2010: From Back", Logger::ExtraInfo);
    }
    #ifdef WEBOTS
    else if (AccXvalue < -1.0) { // Webots
      #else
      else if (AccXvalue > 0.0) { // Robot
        #endif
        //              tts->pCall<AL::ALValue>(std::string("say"), std::string("Face Down!"));
        ALstandUpFront2009();
        Logger::Instance().WriteMsg("MotionController", "Stand Up 2009: From Front", Logger::ExtraInfo);
      }
      return;
    }

void MotionController::ALstandUpCross() {
  SpAssocCont::iterator it = SpActions.find( "ALstandUpCross" );
  if ( it == SpActions.end() )
    Logger::Instance().WriteMsg("MotionController", "SpAction ALstandUpCross not found!",Logger::Error);
  else
    actionPID = it->second->ExecutePost();
}

void MotionController::ALstandUpFront2009()  {
  SpAssocCont::iterator it = SpActions.find( "ALstandUpFront2009" );
  if ( it == SpActions.end() )
    Logger::Instance().WriteMsg("MotionController", "SpAction ALstandUpFront2009 not found!",Logger::Error);
  else
    actionPID = it->second->ExecutePost();
}

void MotionController::ALstandUpBack2009() {
  SpAssocCont::iterator it = SpActions.find( "ALstandUpBack2009" );
  if ( it == SpActions.end() )
    Logger::Instance().WriteMsg("MotionController", "SpAction ALstandUpBack2009 not found!",Logger::Error);
  else
    actionPID = it->second->ExecutePost();
}

void MotionController::ALstandUpFront2010() {
  SpAssocCont::iterator it = SpActions.find( "ALstandUpFront2010" );
  if ( it == SpActions.end() )
    Logger::Instance().WriteMsg("MotionController", "SpAction ALstandUpFront2010 not found!",Logger::Error);
  else
    actionPID = it->second->ExecutePost();
}

void MotionController::ALstandUpBack2010() {
  SpAssocCont::iterator it = SpActions.find( "ALstandUpBack2010" );
  if ( it == SpActions.end() )
    Logger::Instance().WriteMsg("MotionController", "SpAction ALstandUpBack2010 not found!",Logger::Error);
  else
    actionPID = it->second->ExecutePost();
}






void MotionController::commands() {

	if ((headPID == 0) && (counter % 10 == 0)) {
		MotionHeadMessage* hmot = new MotionHeadMessage();
		float x = rand() / ((float) RAND_MAX);
		x = (x - 0.5) * 0.5;
		float y = rand() / ((float) RAND_MAX);
		y = (y - 0.5) * 0.5;
		hmot->set_topic("motion");
		hmot->set_command("changeHead");
		hmot->add_parameter(x);
		hmot->add_parameter(y);
		Logger::Instance().WriteMsg("MotionController","Sending Command: changeHead ", Logger::ExtraInfo);
		_blk->publish_signal(*hmot,"motion");
		delete hmot;
	}

	if ((counter % 50 == 0)) {
		MotionWalkMessage* wmot = new MotionWalkMessage();
		float x = rand() / ((float) RAND_MAX);
		x = (x - 0.5) * 2.0;
		float y = rand() / ((float) RAND_MAX);
		y = (y - 0.5) * 2.0;
		float z = rand() / ((float) RAND_MAX);
		z = (z - 0.5) * 2.0;
		float s = rand() / ((float) RAND_MAX);
		wmot->set_topic("motion");
		wmot->set_command("setWalkTargetVelocity");
		wmot->add_parameter(x);
		wmot->add_parameter(y);
		wmot->add_parameter(z);
		wmot->add_parameter(s);
		Logger::Instance().WriteMsg("MotionController","Sending Command: setWalkTargetVelocity ", Logger::ExtraInfo);
		_blk->publish_signal(*wmot,"motion");
		delete wmot;
	}

	if ((actionPID == 0) && ((counter+30) % 500 == 0) && (counter > 0)) {
		MotionActionMessage* amot = new MotionActionMessage();
		amot->set_topic("motion");
		amot->set_command("RightKick");
		Logger::Instance().WriteMsg("MotionController","Sending Command: action ", Logger::ExtraInfo);
		_blk->publish_signal(*amot,"motion");
		delete amot;
	}


	if ((actionPID == 0) && ((counter+130) % 500 == 0) && (counter > 0)) {
		MotionActionMessage* amot = new MotionActionMessage();
		amot->set_topic("motion");
		amot->set_command("leftFall.kme");
		Logger::Instance().WriteMsg("MotionController","Sending Command: action ", Logger::ExtraInfo);
		_blk->publish_signal(*amot,"motion");
		delete amot;
	}

	if ((actionPID == 0) && ((counter+330) % 500 == 0) && (counter > 0)) {
		MotionActionMessage* amot = new MotionActionMessage();
		amot->set_topic("motion");
		amot->set_command("LeftDive");
		Logger::Instance().WriteMsg("MotionController","Sending Command: action ", Logger::ExtraInfo);
		_blk->publish_signal(*amot,"motion");
		delete amot;
	}
}

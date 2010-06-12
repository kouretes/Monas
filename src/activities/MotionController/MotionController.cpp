#include "MotionController.h"

#include "hal/robot/generic_nao/kAlBroker.h"

#include "tools/logger.h"
#include "tools/toString.h"

namespace {
    ActivityRegistrar<MotionController>::Type temp("MotionController");
}


MotionController::MotionController() :
	Publisher("MotionController") {
        ;
}

void MotionController::UserInit() {
	try {
		motion = KAlBroker::Instance().GetBroker()->getMotionProxy();
	} catch (AL::ALError& e) {
		Logger::Instance().WriteMsg("MotionController","Error in getting motion proxy",Logger::FatalError);
	}
	motion->setStiffnesses("Body", 0.9);
	motion->setWalkArmsEnable(true, true);

	Logger::Instance().WriteMsg("MotionController", "Subcribing to topics", Logger::Info);
	_com->get_message_queue()->add_subscriber(_blk);
	_com->get_message_queue()->subscribe("motion", _blk, 0);
	_com->get_message_queue()->subscribe("sensors", _blk, 0);
	_com->get_message_queue()->add_publisher(this);
    	
	wm = NULL;
	hm = NULL;
	am = NULL;
	im = NULL;
	
	AccZvalue = 0.0;
	AccXvalue = 0.0;

	robotDown = false;
	robotUp = true;

	walkPID = 0;
	headPID = 0;
	actionPID = 0;

	Logger::Instance().WriteMsg("MotionController","Loading special actions!",Logger::Info);
	loadActions();
	
	
	counter = 0;
}

int MotionController::Execute() {
	//std::cout << " MotionController START" << std::endl;
	//commands();
	mglrun(); /* Uncomment for action! */
	//std::cout << " MotionController END" << std::endl;
	return 0;
}

void MotionController::read_messages() {
	if (wm != NULL) delete wm;
	if (hm != NULL) delete hm;
	if (am != NULL) delete am;
	if (im != NULL) delete im;
	_blk->process_messages();
	wm = _blk->in_nb<MotionWalkMessage>("MotionWalkMessage", "Behavior");
	//wm = _blk->in_nb<MotionWalkMessage>("MotionWalkMessage", "MotionController");

	hm = _blk->in_nb<MotionHeadMessage>("MotionHeadMessage", "KImageExtractor");
	if (hm == NULL) {
		hm = _blk->in_nb<MotionHeadMessage>("MotionHeadMessage", "Behavior");
		//hm = _blk->in_nb<MotionHeadMessage>("MotionHeadMessage", "MotionController");
	}

	am = _blk->in_nb<MotionActionMessage>("MotionActionMessage", "Behavior");
	//am = _blk->in_nb<MotionActionMessage>("MotionActionMessage", "MotionController");

	im = _blk->in_nb<InertialSensorsMessage>("InertialSensorsMessage", "Sensors");

	Logger::Instance().WriteMsg("MotionController", "read_messages ", Logger::ExtraExtraInfo);

}


void MotionController::mglrun() {

	counter++;
	read_messages();
		
	if (im != NULL) {
		AccZ = im->sensordata(2);
		AccZvalue = AccZ.sensorvalue();
	}
	//cout << counter << "  " << AccZvalue << "  " << robotUp << "  " << robotDown << " " << actionPID << std::endl;


	/* Check if the robot is falling and remove stiffness, kill all motions */
#ifdef WEBOTS
	if ( (im != NULL) && (!robotDown) && (robotUp) && (AccZvalue < 5.5) ) { // Webots
#else
	if ( (im != NULL) && (!robotDown) && (robotUp) && (AccZvalue > -40)) { // Robot
#endif
		Logger::Instance().WriteMsg("MotionController","Robot falling: Stiffness off",Logger::ExtraInfo);
		motion->setStiffnesses("Body", 0.0);
		robotUp = false;
		robotDown = true;
		killWalkCommand();
		killHeadCommand();
		killActionCommand();
		sleep(1);
		return;
	}

	/* Check if the robot is down and stand up */
	if (robotDown) {
		Logger::Instance().WriteMsg("MotionController", "Will stand up now...", Logger::ExtraInfo);
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
		if (AccZvalue > 8.5) { // Webots
#else
		if (AccZvalue < -40) { // Robot
#endif
			robotUp = true;
			sleep(1);
			Logger::Instance().WriteMsg("MotionController","Stood up ...",Logger::ExtraInfo);
		} else if (actionPID == 0)
			robotDown = true;
		return;
	}

	//cout << "The robot is up and ready to execute motions!" << std::endl;
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
				Logger::Instance().WriteMsg("MotionController","Walk ID: "+_toString(walkPID),Logger::ExtraInfo);
			}
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
				float fractionMaxSpeed = 0.2;
				headPID = motion->post.changeAngles(names, values, fractionMaxSpeed);
				Logger::Instance().WriteMsg("MotionController", " Head ID: " +_toString(headPID),Logger::ExtraInfo);
			}
		}
		
		if ( (am != NULL) && (actionPID==0) ) {
			
			Logger::Instance().WriteMsg("MotionController", hm->command(),Logger::ExtraInfo);
		
			//actionPID = motion->post.xxxxxxxxxxxxxx
			if (am->command() == "lieDown") {
				stopWalkCommand();
				killHeadCommand();
				actionPID = motion->post.angleInterpolationBezier(LieDown_names, LieDown_times, LieDown_keys);
			}
			else if (am->command() == "leftKick") {
				stopWalkCommand();
				actionPID = motion->post.angleInterpolationBezier(LeftKick_names, LeftKick_times, LeftKick_keys);
				}
			else if (am->command() == "rightKick") {
				stopWalkCommand();
				actionPID = motion->post.angleInterpolationBezier(RightKick_names, RightKick_times, RightKick_keys);
			}
			
			Logger::Instance().WriteMsg("MotionController", "  Action ID: " +_toString(actionPID),Logger::ExtraInfo);
		}

	}

	return;
}

void MotionController::killWalkCommand() {
	motion->post.killWalk();
	walkPID = 0;
}

void MotionController::stopWalkCommand() {
	walkPID = motion->post.setWalkTargetVelocity(0.0, 0.0, 0.0, 0.0); // stop walk
	motion->waitUntilWalkIsFinished();
	walkPID = 0;
}

void MotionController::killHeadCommand() {
	if (headPID != 0) {
	        motion->post.killTask(headPID);
	        headPID = 0;
	}
}

void MotionController::killActionCommand() {
	if (actionPID != 0) {
	        motion->post.killTask(actionPID);
	        actionPID = 0;
	}
}

void MotionController::commands() {
	//cout << "Commands " << counter << " headPid " << headPID << endl;

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
		Publisher::publish(hmot,"motion");
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
		Publisher::publish(wmot,"motion");
		delete wmot;
	}

	if ((actionPID == 0) && (counter % 200 == 0) && (counter > 0)) {
		MotionActionMessage* amot = new MotionActionMessage();
		amot->set_topic("motion");
		amot->set_command("leftKick");
		Logger::Instance().WriteMsg("MotionController","Sending Command: action ", Logger::ExtraInfo);
		Publisher::publish(amot,"motion");
		delete amot;
	}

	return;
}



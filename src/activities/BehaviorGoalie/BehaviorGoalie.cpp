#include "BehaviorGoalie.h"
#include "hal/robot/generic_nao/kAlBroker.h"

#include "alxplatform.h"
#include <albroker.h>
#include <alproxy.h>
#include <almemoryfastaccess.h>
#include "almotionproxy.h"
#include "almemoryproxy.h"
#include "albrokermanager.h"
#include "alvalue.h"

#include "tools/logger.h"
#include "tools/toString.h"
#include "messages/RoboCupGameControlData.h"

#define TO_RAD 0.01745329f

namespace {
	ActivityRegistrar<BehaviorGoalie>::Type temp("BehaviorGoalie");
}

BehaviorGoalie::BehaviorGoalie() :
	Publisher("Behavior") {
}

void BehaviorGoalie::UserInit() {

	_com->get_message_queue()->add_publisher(this);
	_com->get_message_queue()->add_subscriber(_blk);
	_com->get_message_queue()->subscribe("vision", _blk, 0);
	_com->get_message_queue()->subscribe("sensors", _blk, 0);
	_com->get_message_queue()->subscribe("behavior", _blk, 0);

	wmot = new MotionWalkMessage();
	wmot->set_topic("motion");
	wmot->add_parameter(0.0f);
	wmot->add_parameter(0.0f);
	wmot->add_parameter(0.0f);
	wmot->add_parameter(0.0f);

	hmot = new MotionHeadMessage();
	hmot->set_topic("motion");
	hmot->add_parameter(0.0f);
	hmot->add_parameter(0.0f);

	amot = new MotionActionMessage();
	amot->set_topic("motion");

	ballfound = 0;

	balllastseendirection = -0.02;
	reachedlimitup = false;
	reachedlimitdown = false;
	reachedlimitleft = false;
	reachedlimitright = false;

	scanforball = true;
	startscan = true;

	calibrated = 0;

	stopped = false;
	play = false;
	Logger::Instance().WriteMsg("BehaviorGoalie", "Controller Initialized", Logger::Info);
	//cout << "BehaviorGoalie Controller Initialized" << endl;

	hjsm = 0;
	bmsg = 0;
	gsm = 0;
        obsm = 0;
	
	readytokick = false;
	back = 0;
	direction = 1;
	turning = false; 
	count = 0; 
}

int BehaviorGoalie::MakeTrackBallAction() {

	float overshootfix = 0.3;
	float cx = bmsg->cx();
	float cy = bmsg->cy();
	if (fabs(cx) > 0.015 || fabs(cy) > 0.015) {
		//hmot->set_command("setHead");
		//hmot->set_parameter(0, bmsg->referenceyaw() - overshootfix * cx);
		//hmot->set_parameter(1, bmsg->referencepitch() - overshootfix * cy);
		hmot->set_command("changeHead");
		hmot->set_parameter(0, - overshootfix * cx);
		hmot->set_parameter(1, - overshootfix * cy);
		Publisher::publish(hmot, "motion");
	}
	return 1;
}



int BehaviorGoalie::Execute() {

	read_messages();
	
	if (gsm != 0) {
		Logger::Instance().WriteMsg("BehaviorGoalie", " Player_state " + _toString(gsm->player_state()), Logger::ExtraExtraInfo);

		/* PLAY state */
		if (gsm->player_state() == PLAYER_PLAYING && calibrated == 2)
			play = true;
		else
			play = false;
		
		/* Calibration */
		if ( (gsm->player_state() == PLAYER_PENALISED) || (gsm->player_state() == PLAYER_READY) ) {
			CalibrateCam v;
			v.set_status(0);
			publish(&v, "vision");
			calibrated = 1;
		}
	}

	if (play) {
		if (bmsg != 0) {
			Logger::Instance().WriteMsg("BehaviorGoalie", "BallTrackMessage", Logger::ExtraExtraInfo);
			if (bmsg->radius() > 0) { //This means that a ball was found
				scanforball = false; //if you are scanning for ball please stop now
				MakeTrackBallAction();
				ballfound += 5;
				if (ballfound > 10) 
					ballfound = 10; //Increase this value when we see the ball
			}
			else {
				if (ballfound > 0) 
					ballfound -= 1; //Decrease it when we don't see the ball
			}
		}
		Logger::Instance().WriteMsg("BehaviorGoalie", "ballfound Value: " + _toString(ballfound), Logger::ExtraInfo);
	
		float X=0.0, Y=0.0, theta=0.0;
		float bd=0.0, bx=0.0, by=0.0, bb=0.0;
		double gain = 0.8;
		if ((obsm != 0) && !turning) {
			scanforball = false; //be sure to stop scanning
			int side=1;
			bd = obsm->ball().dist();
			bb = obsm->ball().bearing();
			bx = obsm->ball().dist() * cos( obsm->ball().bearing() );
			by = obsm->ball().dist() * sin( obsm->ball().bearing() );
			side = (bb > 0) ? 1 : -1; 
			Logger::Instance().WriteMsg("BehaviorGoalie", "Measurements - Distance: " + _toString(bd) + "  Bearing: " + _toString(bb) + "  BX: " + _toString(bx) + "  BY: " + _toString(by), Logger::Info);

			theta = 0;
			if ( bd < 0.5 ) {	// If distance is small, approach and kick ball 
			    readytokick = true;
			    float posx=0.16, posy=0.05;
			    if (bd > 0.40) { 
				    X = gain * bx;
				    Y = gain * by;
				    if (fabs(bb) > 0.055) 
					    theta = gain * bb;
				    readytokick = false;
			    } else if (bd > 0.25) { 
				    X = gain * (bx - posx);
				    Y = gain * ( by - (side*posy) );
				    readytokick = false;
			    } else { 
				    if ( fabs( bx - posx ) > 0.02) {
					    X = gain * (bx - posx);
					    readytokick = false;
				    }
				    if ( fabs( by - (side*posy) ) > 0.02) {
					    Y = gain * ( by - (side*posy) );
					    readytokick = false;
				    }
			    }
			}
			else if ( (bb > +35*TO_RAD) && (bd < 0.8) ) {		// Ball is close and head to the left
			    amot->set_command("leftFall.kme");
			    Publisher::publish(amot, "motion");
			    stopped = false;
			}
			else if ( (bb < -35*TO_RAD) && (bd < 0.8) ) {		// Ball is close and head to the right
			    amot->set_command("rightFall.kme");
			    Publisher::publish(amot, "motion");
			    stopped = false;
			}
			else if ( (bb > +50*TO_RAD) && (bd < 1.5) ) {		// Ball is not too far and head to the left
			    amot->set_command("leftDive");
			    Publisher::publish(amot, "motion");
			    stopped = false;
			}
			else if ( (bb < -50*TO_RAD) && (bd < 1.5) ) {		// Ball is not too far and head to the right
			    amot->set_command("rightDive");
			    Publisher::publish(amot, "motion");
			    stopped = false;
			}


			if (fabs(X) > 1.0)
				X = (X > 0.0) ? 1.0 : -1.0;
			if (fabs(Y) > 1.0)
				Y = (Y > 0.0) ? 1.0 : -1.0;
			if (fabs(theta) > 1.0)
				theta = (theta > 0.0) ? 1.0 : -1.0;
			
			if (!readytokick) {
				wmot->set_command("setWalkTargetVelocity");
				wmot->set_parameter(0, X);
				wmot->set_parameter(1, Y);
				wmot->set_parameter(2, theta);
				wmot->set_parameter(3, 1.0);
				Publisher::publish(wmot, "motion");
			}
		}
		
		/* Ready to take action */
		if (readytokick && !turning) {
			if (by > 0.0)
				amot->set_command("leftKick");
			else
				amot->set_command("rightKick");
			Publisher::publish(amot, "motion");
			back = 1;
			readytokick = false;
		}
		
		if ( (ballfound == 0) && !readytokick && !turning ) {
			if (!scanforball) {
				startscan = true;
				wmot->set_command("setWalkTargetVelocity");
				wmot->set_parameter(0, 0);
				wmot->set_parameter(1, 0);
				wmot->set_parameter(2, 0);
				wmot->set_parameter(3, 0);
				Publisher::publish(wmot, "motion");
			}
			scanforball = true;
		}
			
		if (scanforball && !readytokick && !turning && (hjsm != 0) ) {
			HeadYaw = hjsm->sensordata(0);
			HeadPitch = hjsm->sensordata(1);
			HeadScanStep();
		}
		
	} else if (!play) {   // Non-Play state
		wmot->set_command("setWalkTargetVelocity");
		wmot->set_parameter(0, 0);
		wmot->set_parameter(1, 0);
		wmot->set_parameter(2, 0);
		wmot->set_parameter(3, 0);
		Publisher::publish(wmot, "motion");
		
		//hmot->set_command("setHead");
		//hmot->set_parameter(0, 0.0);
		//hmot->set_parameter(1, 0.0);
		//Publisher::publish(hmot, "motion");
	}
	
	return 0;
}



void BehaviorGoalie::HeadScanStep() {

	if (startscan) {
		//BE CAREFULL the max sign is according to sensors values (max maybe negative! :p)
		if (HeadPitch.sensorvalue() < LIMITDOWN) { // first go down
			scandirectionpitch = 1;
		} else {
			scandirectionpitch = -1; // go up
		}
		reachedlimitup = false;
		reachedlimitdown = false;
		reachedlimitleft = false;
		reachedlimitright = false;
		scandirectionyaw = (HeadYaw.sensorvaluediff() > 0) ? 1 : -1;
		startscan = false;

		Logger::Instance().WriteMsg("BehaviorGoalie", "Sending stop command because of scanning ", Logger::ExtraExtraInfo);
		wmot->set_command("setWalkTargetVelocity");
		wmot->set_parameter(0, 0);
		wmot->set_parameter(1, 0);
		wmot->set_parameter(2, 0);
		wmot->set_parameter(3, 0);
		Publisher::publish(wmot, "motion");
	}
	reachedlimitright = false;
	reachedlimitleft = false;

	//continue scan
	if (HeadPitch.sensorvalue() < LIMITUP) { // upper position
		Logger::Instance().WriteMsg("BehaviorGoalie", " LIMITUP ", Logger::ExtraExtraInfo);
		reachedlimitup = true;
		scandirectionpitch = 1;
	}
	if (HeadPitch.sensorvalue() > LIMITDOWN) {
		Logger::Instance().WriteMsg("BehaviorGoalie", " LIMITDOWN ", Logger::ExtraExtraInfo);
		reachedlimitdown = true;
		scandirectionpitch = -1;
	}
	if (HeadYaw.sensorvalue() > LIMITLEFT) {
		Logger::Instance().WriteMsg("BehaviorGoalie", "LIMITLEFT  ", Logger::ExtraExtraInfo);
		reachedlimitleft = true;
		scandirectionyaw = -1;
	}
	if (HeadYaw.sensorvalue() < LIMITRIGHT) {
		Logger::Instance().WriteMsg("BehaviorGoalie", " LIMITRIGHT  ", Logger::ExtraExtraInfo);
		reachedlimitright = true;
		scandirectionyaw = 1;
	}

	hmot->set_command("changeHead");
	hmot->set_parameter(0, scandirectionyaw * 0.18); // headYaw
	if (reachedlimitleft || reachedlimitright) {
		hmot->set_parameter(1, scandirectionpitch * 0.23); // headPitch
		reachedlimitleft = false;
		reachedlimitright = false;
	}
	else {
          hmot->set_parameter(1, 0.0); // headPitch
        }
	Publisher::publish(hmot, "motion");

        if (reachedlimitup && reachedlimitdown) {
                  startscan = true;
                  reachedlimitdown = false;
                  reachedlimitup = false;
                  reachedlimitright = false;
                  reachedlimitleft = false;
        }
}


void BehaviorGoalie::read_messages() {
	
	if (gsm != 0) delete gsm;
	if (bmsg != 0) delete bmsg;
	if (hjsm != 0) delete hjsm;
	if (obsm != 0) delete obsm;
	
	_blk->process_messages();
	
	gsm  = _blk->in_msg_nb<GameStateMessage> ("GameStateMessage");
	bmsg = _blk->in_msg_nb<BallTrackMessage> ("BallTrackMessage");
	hjsm = _blk->in_msg_nb<HeadJointSensorsMessage> ("HeadJointSensorsMessage");
	obsm = _blk->in_msg_nb<ObservationMessage> ("ObservationMessage");

	Logger::Instance().WriteMsg("BehaviorGoalie", "read_messages ", Logger::ExtraExtraInfo);
	CalibrateCam *c = _blk->in_msg_nb<CalibrateCam> ("CalibrateCam");
	if (c != NULL) {
		if (c->status() == 1)
			calibrated = 2;
		delete c;
		cout << "calibrated:" << calibrated << endl;
	}
}
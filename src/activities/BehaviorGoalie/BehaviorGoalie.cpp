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
#include "architecture/narukom/pub_sub/filters/special_filters.h"

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

	reachedlimitup = false;
	reachedlimitdown = false;
	reachedlimitleft = false;
	reachedlimitright = false;

	scanforball = true;
	startscan = true;

	calibrated = 0;

	stopped = false;
	play = false;
	Logger::Instance().WriteMsg("BehaviorGoalie", "Initialized", Logger::Info);

	hjsm = 0;
	bmsg = 0;
	gsm = 0;
	obsm = 0;

	readytokick = false;
	back = 0;
	direction = 1;
	turning = false;
	count = 0;

	ballSpeedX = 0;
	ballSpeedY = 0;
	prevX = 0;
	prevY = 0;
	prevTimestamp = boost::posix_time::from_time_t(0);
	speedIsValid = false;
	
	prevbd=1.5;
	prevbb=0.0;
	go=0; 
	gameState = PLAYER_INITIAL;
}

int BehaviorGoalie::MakeTrackBallAction() {

	float overshootfix = 0.50;
	float cx = bmsg->cx();
	float cy = bmsg->cy();
	if (fabs(cx) > 0.015 || fabs(cy) > 0.015) {
		//hmot->set_command("setHead");
		//hmot->set_parameter(0, bmsg->referenceyaw() - overshootfix * cx);
		//hmot->set_parameter(1, bmsg->referencepitch() - overshootfix * cy);
		hmot->set_command("changeHead");
		if (fabs(cx) > 0.015)
			hmot->set_parameter(0, - overshootfix * cx);
		else
			hmot->set_parameter(0, 0.0);
		if (fabs(cy) > 0.015)
			hmot->set_parameter(1, - overshootfix * cy);
		else
			hmot->set_parameter(1, 0.0);
		Publisher::publish(hmot, "motion");
	}
	return 1;
}

void BehaviorGoalie::CalculateBallSpeed () {
	boost::posix_time::ptime timestamp = boost::posix_time::from_iso_string( obsm->image_timestamp() );

	float bx = obsm->ball().dist() * cos( obsm->ball().bearing() );
	float by = obsm->ball().dist() * sin( obsm->ball().bearing() );

	if ( (timestamp - prevTimestamp) < boost::posix_time::millisec(150) ) {
	  speedIsValid = false;
	  ballSpeedX = 0.0;
	  ballSpeedY = 0.0;
	}
	else {
	  speedIsValid = true;

	  double newSpeedX = ( fabs(bx - prevX) ) / ( ( timestamp - prevTimestamp ).total_seconds() );
	  double newSpeedY = ( fabs(by - prevY) ) / ( ( timestamp - prevTimestamp ).total_seconds() );

	  float alpha = 0.8;
	  ballSpeedX = alpha * newSpeedX + (1-alpha) * ballSpeedX;
	  ballSpeedY = alpha * newSpeedY + (1-alpha) * ballSpeedY;


	}
	prevX = bx;
	prevY = by;
	prevTimestamp = timestamp;
}



int BehaviorGoalie::Execute() {

	read_messages();

	if (gsm != 0) {
		Logger::Instance().WriteMsg("BehaviorGoalie", " Player_state " + _toString(gsm->player_state()), Logger::ExtraExtraInfo);
		gameState = gsm->player_state();
		
		if (gameState == PLAYER_PLAYING) {
			if (calibrated == 2) {
				play = true;
				prevbd=1.5;
				prevbb=0.0;
				go=0;
				littleWalk(0.01,0.0,0.0,1);
			}
			else if (calibrated == 0) {
				calibrate();
			}
			else if (calibrated == 1) {
				// wait
			}
		}
		else if (gameState == PLAYER_INITIAL) {
			play = false;
		}
		else if (gameState == PLAYER_READY) {
			play = false;
		}
		else if (gameState == PLAYER_SET) {
			play = false;
			calibrate();
		}
		else if (gameState == PLAYER_FINISHED) {
			play = false;
		}
		else if (gameState == PLAYER_PENALISED) {
			play = false;
			calibrate();
		}
	}
	
	if (gameState == PLAYER_PLAYING) {
		if (calibrated == 2) {
			play = true;
		}
		else if (calibrated == 0) {
			calibrate();
		}
	}

	if (play) {

		if (bmsg != 0) {
			Logger::Instance().WriteMsg("BehaviorGoalie", "BallTrackMessage", Logger::ExtraExtraInfo);
			if (bmsg->radius() > 0) { //This means that a ball was found
				scanforball = false; //if you are scanning for ball please stop now
				MakeTrackBallAction();
				ballfound += 5;
				if (ballfound > 20)
					ballfound = 20; //Increase this value when we see the ball
			}
			else {
				if (ballfound > 0)
					ballfound -= 1; //Decrease it when we don't see the ball
			}
		}
		Logger::Instance().WriteMsg("BehaviorGoalie", "ballfound Value: " + _toString(ballfound), Logger::ExtraInfo);

		float X=0.0, Y=0.0, theta=0.0;
		float bd=0.0, bx=0.0, by=0.0, bb=0.0;
		
		if ((obsm != 0) && (obsm->has_ball()) && !turning) {
			
			scanforball = false; //be sure to stop scanning
			//CalculateBallSpeed();
			int side=1;
			//double smoothing = 0.2; 
			//bd = (1.0-smoothing)*obsm->ball().dist() + smoothing*prevbd;
			//bb = (1.0-smoothing)*obsm->ball().bearing()+ smoothing*prevbb;
			bd = obsm->ball().dist();
			bb = obsm->ball().bearing();
			bx = bd * cos( bb );
			by = bd * sin( bb );
			side = (bb > 0) ? 1 : -1;
			//Logger::Instance().WriteMsg("BehaviorGoalie", "Measurements - Distance: " + _toString(bd) + "  Bearing: " + _toString(bb) + "  BX: " + _toString(bx) + "  BY: " + _toString(by), Logger::Info);
			Logger::Instance().WriteMsg("BehaviorGoalie", "BallData: " + _toString(bd) + " " + _toString(bb) + " " + _toString(bx) + " " + _toString(by), Logger::Info);

			if (!readytokick) {
				double gain = 1.0;
				double gainTheta = 0.3;
				double gainFine = 1.0;
				
				if ( (bb>+5*TO_RAD) && (bb-prevbb > +1*TO_RAD) && (bd>0.3) && (bd < 4.0) ) {		// Ball is not too far and head to the left
					go--;
				}
				else if ( (bb<-5*TO_RAD) && (bb-prevbb < -1*TO_RAD) && (bd>0.3) && (bd < 4.0) ) {		// Ball is not too far and head to the right
					go++;
				}
				else if ( (bd < 0.8)  ) {	// If distance is small, approach and kick ball
					go = 0;
					readytokick = true;
					float posx=0.17, posy=0.05;
					if (bd > 0.25) {
						X = gain * bx;
						Y = gain * by;
						if (fabs(bb) > 3*TO_RAD)
							theta = gainTheta * bb;
						readytokick = false;
					} else if (bd > 0.25) {
						X = gain * (bx - posx);
						Y = gain * ( by - (side*posy) );
						readytokick = false;
					} else {
						if ( fabs( bx - posx ) > 0.025) {
							X = gainFine * (bx - posx);
							readytokick = false;
						}
						if ( fabs( by - (side*posy) ) > 0.025) {
							Y = gainFine * ( by - (side*posy) );
							readytokick = false;
						}
					}
				}
				//else if ( (bb > +30*TO_RAD) && (bd < 0.8) ) {		// Ball is close and head to the left
					//amot->set_command("leftFall.kme");
					//Publisher::publish(amot, "motion");
					//stopped = false;
				//}
				//else if ( (bb < -30*TO_RAD) && (bd < 0.8) ) {		// Ball is close and head to the right
					//amot->set_command("rightFall.kme");
					//Publisher::publish(amot, "motion");
					//stopped = false;
				//}
				else {
					if (go>0)
						go--;
					if (go<0)
						go++;
					if (fabs(bb) > 10*TO_RAD)
						theta = 0.8 * gainTheta * bb;
					readytokick = false;
				}
				
				if (!readytokick) {
					
					if (fabs(X) > 1.0)
						X = (X > 0.0) ? 1.0 : -1.0;
					if (fabs(Y) > 1.0)
						Y = (Y > 0.0) ? 1.0 : -1.0;
					if (fabs(theta) > 1.0)
						theta = (theta > 0.0) ? 1.0 : -1.0;
						
					velocityWalk(X, Y, theta, 1.0);
				}
				else {
					velocityWalk(0.0, 0.0, 0.0, 1.0);
					return 1;
				}
			}
			prevbd = bd;
			prevbb = bb;
		}
		else {
			readytokick = false;
		}

		Logger::Instance().WriteMsg("BehaviorGoalie", "go: " + _toString(go), Logger::ExtraInfo);
		/* Ready to take action */
		if ( abs(go) >= 4 ) {
			if ( go <= -4 ) 
				amot->set_command("LeftDive");
			else
				amot->set_command("RightDive");
			Publisher::publish(amot, "motion");
			stopped = false;
			go = 0;
			prevbd=1.5;
			prevbb=0.0;
		}
		else if ( (abs(go) >= 2) && (fabs(bb)>15*TO_RAD) && (fabs(bb)<25*TO_RAD) && (bd<0.8) ) {
			if ( go <= -2 ) 
				amot->set_command("leftFall.kme");
			else
				amot->set_command("rightFall.kme");
			Publisher::publish(amot, "motion");
			stopped = false;
			go = 0;
			prevbd=1.5;
			prevbb=0.0;
		}
		else if (readytokick && !turning) {
			if (mglRand()<0.8) {
				if (by > 0.0) 
					amot->set_command("LeftKick");
				else 
					amot->set_command("RightKick");
			}
			else {
				if (by > 0.0)
					amot->set_command("HardLeftSideKick");
				else 
					amot->set_command("HardRightSideKick");
			}
			Publisher::publish(amot, "motion");
			back = 1;
			readytokick = false;
			go = 0;
			prevbd=1.5;
			prevbb=0.0;
		}

		if ( (ballfound == 0) && !readytokick && !turning ) {
			if (!scanforball) {
				startscan = true;
				velocityWalk(0.0, 0.0, 0.0, 1.0);
			}
			scanforball = true;
		}

		if (scanforball && !readytokick && !turning && (hjsm != 0) ) {
			HeadYaw = hjsm->sensordata(0);
			HeadPitch = hjsm->sensordata(1);
			HeadScanStep();
		}

	} else if (!play) {   // Non-Play state
		velocityWalk(0.0, 0.0, 0.0, 1.0);
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
	}

	//continue scan
	if (HeadPitch.sensorvalue() < LIMITUP) { // upper position
		//Logger::Instance().WriteMsg("BehaviorGoalie", " LIMITUP ", Logger::ExtraExtraInfo);
		reachedlimitup = true;
		scandirectionpitch = 1;
	}
	if (HeadPitch.sensorvalue() > LIMITDOWN) {
		//Logger::Instance().WriteMsg("BehaviorGoalie", " LIMITDOWN ", Logger::ExtraExtraInfo);
		reachedlimitdown = true;
		scandirectionpitch = -1;
	}
	if (HeadYaw.sensorvalue() > LIMITLEFT) {
		//Logger::Instance().WriteMsg("BehaviorGoalie", "LIMITLEFT  ", Logger::ExtraExtraInfo);
		reachedlimitleft = true;
		scandirectionyaw = -1;
	}
	if (HeadYaw.sensorvalue() < LIMITRIGHT) {
		//Logger::Instance().WriteMsg("BehaviorGoalie", " LIMITRIGHT  ", Logger::ExtraExtraInfo);
		reachedlimitright = true;
		scandirectionyaw = 1;
	}

	hmot->set_command("changeHead");
	hmot->set_parameter(0, scandirectionyaw * STEPHOR); // Headyaw
	hmot->set_parameter(1, 0.0); // headPitch

	if (reachedlimitleft && reachedlimitright) {
		Logger::Instance().WriteMsg("BehaviorGoalie", " reachedlimitleft && reachedlimitright ", Logger::ExtraExtraInfo);
		hmot->set_parameter(1, scandirectionpitch * STEPVER); // headPitch
		reachedlimitleft = false;
		reachedlimitright = false;
	}
	Publisher::publish(hmot, "motion");

	if (reachedlimitup && reachedlimitdown) { //scanning completed
		Logger::Instance().WriteMsg("BehaviorGoalie", " reachedlimitup && reachedlimitdown ", Logger::ExtraExtraInfo);
		startscan = true;
		reachedlimitdown = false;
		reachedlimitup = false;
		reachedlimitright = false;
		reachedlimitleft = false;
		///we should do something;
		if (back>0) {
			littleWalk(-0.2, 0.0, 0.0, 2);
			back--;
		} 
		else {
			littleWalk(0.0, 0.0, direction * 90 * TO_RAD, 1);
			//direction = - direction;
		}
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
	}
}

double BehaviorGoalie::mglRand()
{
    return rand() / double(RAND_MAX);
}

void BehaviorGoalie::velocityWalk(double x, double y, double th, double f)
{
	wmot->set_command("setWalkTargetVelocity");
	wmot->set_parameter(0, x);
	wmot->set_parameter(1, y);
	wmot->set_parameter(2, th);
	wmot->set_parameter(3, f);
	Publisher::publish(wmot, "motion");
}

void BehaviorGoalie::littleWalk(double x, double y, double th, int s)
{
	RejectAllFilter reject_filter("RejectFilter");
	_blk->getBuffer()->add_filter(&reject_filter);
	wmot->set_command("walkTo");
	wmot->set_parameter(0, x);
	wmot->set_parameter(1, y);
	wmot->set_parameter(2, th);
	Publisher::publish(wmot, "motion");
	sleep(s);
	_blk->getBuffer()->remove_filter(&reject_filter);
}

void BehaviorGoalie::calibrate()
{
	CalibrateCam v;
	v.set_status(0);
	publish(&v, "vision");
	calibrated = 1;
}

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
	try {
		memory = KAlBroker::Instance().GetBroker()->getMemoryProxy();
	} catch (AL::ALError& e) {
		Logger::Instance().WriteMsg("BehaviorGoalie", "Error in getting memory proxy", Logger::Error);
		//cout << "Error in getting memory proxy" << std::endl;
	}

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
        obs = 0;
}

int BehaviorGoalie::MakeTrackBallAction() {
  scanforball = false; //if you are scanning for ball please stop now

  float overshootfix = 1 - fabs(bmsg->referencepitch()) / (3.14 / 2.0);
  //overshootfix = 2 * (0.4f - overshootfix);
  Logger::Instance().WriteMsg("Behavior", "bmsg->referencepitch() Value: " + _toString(bmsg->referencepitch()), Logger::ExtraInfo);

  Logger::Instance().WriteMsg("Behavior", "Overshoot Value: " + _toString(overshootfix), Logger::ExtraInfo);

  float cx = bmsg->cx();
  float cy = bmsg->cy();

  balllastseendirection = bmsg->referenceyaw() - 0.9f * cx;// HeadYaw.sensorvalue();
  Logger::Instance().WriteMsg("Behavior", "I want the freaking head to move towards (cx,cy):" + _toString(0.9f * (cx)) + "," + _toString(-0.9f * (cy)), Logger::ExtraInfo);

  if (fabs(cx) > 0.015 || fabs(cy) > 0.015) {
    //Sending command to motion in order to move the head
    hmot->set_command("setHead");
    hmot->set_parameter(0, bmsg->referenceyaw() - overshootfix * cx);
    Logger::Instance().WriteMsg("Behavior", "bmsg->referenceyaw(): " + _toString(bmsg->referenceyaw()), Logger::ExtraInfo);

    hmot->set_parameter(1, bmsg->referencepitch() - overshootfix * cy);
    //hmot->set_parameter(0, 0.9f * overshootfix * (cx));
    //hmot->set_parameter(1, -0.9f * overshootfix * (cy));
    Publisher::publish(hmot, "motion");
    Logger::Instance().WriteMsg("Behavior", "I send motion to head to move towards (cx,cy):" + _toString(hmot->parameter(0)) + "," + _toString(hmot->parameter(1)), Logger::ExtraInfo);
  }
  return 1;
}

int BehaviorGoalie::Execute() {

        read_messages();
	if (gsm != 0) {
		Logger::Instance().WriteMsg("BehaviorGoalie", " Player_state " + _toString(gsm->player_state()), Logger::ExtraExtraInfo);

                if (gsm->player_state() == PLAYER_PLAYING && calibrated == 2) // this is the playing state
			play = true;
		else
			play = false;
                if ((gsm->player_state() != PLAYER_PLAYING && calibrated == 0) || gsm->player_state() == PLAYER_READY) {
			CalibrateCam v;
			v.set_status(0);
			publish(&v, "vision");
			//cout<<"calibrate cam now!"<<endl;
			calibrated = 1;
		}

		delete gsm;
                gsm = 0;
	}


	if (bmsg != 0) {

		if (bmsg->radius() > 0) { //This meens that a ball was found
			MakeTrackBallAction();
			ballfound += 2; //Increase this value when we see the ball
		}
		else {
                    if (ballfound > 0) {
                            ballfound -= 1; //Decrease it when we don't see the ball
                            startscan = false;
                            scanforball = false; // stop scanning
                    }
                    else {
                            ballfound = 0;
                            if (!scanforball) { //Start the scan only when it is not scanning ... obviously
                                    scanforball = true;
                                    startscan = true;
                            }
                    }
		}
		//just dont scan if ones you didn't saw the ball
		if (ballfound > 4)
			ballfound = 4;
		if (ballfound < 0)
			ballfound = 0;

		lastballseen = *bmsg;

		delete bmsg;
                bmsg = 0;
	}

	if (play) {
		//Head joint related BehaviorGoalie
		//Under this block we use the head joint values to calculated
		//1) the head scanning procedure
		//2) the walk parameters (how much to turn, walk ..) and the kicks
		if (hjsm != 0) {
			HeadYaw = hjsm->sensordata(0);
			HeadPitch = hjsm->sensordata(1);
			Logger::Instance().WriteMsg("BehaviorGoalie", "HeadJointSensorsMessage arrived", Logger::ExtraExtraInfo);
			Logger::Instance().WriteMsg("BehaviorGoalie", "HeadPitch.sensorvalue()" + _toString(HeadPitch.sensorvalue()) + "HeadYaw.sensorvalue()   " + _toString(HeadYaw.sensorvalue()), Logger::ExtraInfo);
		}

		//HeadScan
		if ((hjsm != 0) && (scanforball == true)) {//start or continue scan
			HeadScanStep();
		}



		//We have seen a ball for sure and we should walk
		//or we should kick
		if (hjsm != 0) { //ActionDesision
			float X, Y, theta, freq;
			//Calculate approach
			X = 0;
			Y = 0;
			theta = 0;

			if (ballfound > 1 && obs != 0) {
				//scanforball = false; //be sure to stop scanning

                                float bearing = 0.0;
                                float dist = 0.0;
                                if ( obs->has_ball() ) {
                                  bearing = obs->ball().bearing();
                                  dist =  obs->ball().dist();
                                }


                                if (bearing > 25*TO_RAD && dist < 1.0 ) {
                                  amot->set_command("leftFall.kme");
                                  Publisher::publish(amot, "motion");
                                  stopped = false;
                                }
                                else if(bearing < -25*TO_RAD && dist < 1.0) {
                                  amot->set_command("rightFall.kme");
                                  Publisher::publish(amot, "motion");
                                  stopped = false;
                                }
                                else if (bearing > 25*TO_RAD && dist < 1.5 ) {
                                  amot->set_command("leftDive");
                                  Publisher::publish(amot, "motion");
                                  stopped = false;
                                }
                                else if(bearing < -25*TO_RAD && dist < 1.5) {
                                  amot->set_command("rightDive");
                                  Publisher::publish(amot, "motion");
                                  stopped = false;
                                }
                                else if ( dist < 0.5 ) {
                                  bool readytokick = true;
                                  //Check max values !
                                  if (fabs(HeadYaw.sensorvalue()) > 0.055) {
                                    theta = HeadYaw.sensorvalue() * 0.5 * (0.6 - fabs(HeadPitch.sensorvalue()));
                                    readytokick = false;
                                  }
                                  if ((HeadPitch.sensorvalue()) < 0.51) { //// Auto edw, to poso konta einai stin mpala gia na soutarei
                                    X = 0.75 * (0.5 - HeadPitch.sensorvalue());
                                    Y = HeadYaw.sensorvalue() * 0.6 * (1.4 - fabs(HeadYaw.sensorvalue()));
                                    readytokick = false;
                                }

                                if (fabs(HeadYaw.sensorvalue()) < 0.025) {
                                  Y = (HeadYaw.sensorvalue());
                                  readytokick = false;
                                }

                                if (!readytokick) {
                                  wmot->set_command("setWalkTargetVelocity");

                                  if (fabs(X) > 1.0)
                                    X = (X > 0) ? 1 : -1;
                                  if (fabs(Y) > 1.0)
                                    Y = (Y > 0) ? 1 : -1;
                                  if (fabs(theta) > 1.0)
                                    theta = (theta > 0) ? 1 : -1;
                                  freq = 1.4 - fabs(HeadPitch.sensorvalue());
                                  if (fabs(freq) > 1.0)
                                    freq = (freq > 0) ? 1 : -1;

                                  wmot->set_parameter(0, X);
                                  wmot->set_parameter(1, Y);
                                  wmot->set_parameter(2, theta);
                                  wmot->set_parameter(3, freq);
                                  cout << "  setWalkTargetVelocity " << endl;
                                  Logger::Instance().WriteMsg("Behavior", "Walk Command" + _toString(wmot->command()) + "X:  " + _toString(wmot->parameter(0)) + "Y:  "
                                  + _toString(wmot->parameter(1)) + "theta:  " + _toString(wmot->parameter(2)), Logger::ExtraExtraInfo);
                                  Publisher::publish(wmot, "motion");

                                } else {
                                  cout << "Kicking" << endl;
                                  if (HeadYaw.sensorvalue() > 0.0)
                                    amot->set_command("leftKick");
                                  else
                                    amot->set_command("rightKick");
                                  Logger::Instance().WriteMsg("Behavior", "Kicking with " + _toString(amot->command()), Logger::Info);
                                  Publisher::publish(amot, "motion");
                                }
                                stopped = false;
                                }


//                                   if (HeadYaw.sensorvalue() > 30*TO_RAD) {
//                                     wmot->set_command("walkTo");
//                                     wmot->set_parameter(0, 0.0);
//                                     wmot->set_parameter(1, 0.05);
//                                     wmot->set_parameter(2, 0);
//                                     Publisher::publish(wmot, "motion");
//                                   }
//                                   if(HeadYaw.sensorvalue() < -30*TO_RAD) {
//                                     wmot->set_command("walkTo");
//                                     wmot->set_parameter(0, 0.0);
//                                     wmot->set_parameter(1, -0.05);
//                                     wmot->set_parameter(2, 0);
//                                     Publisher::publish(wmot, "motion");
//                                   }
//                                 }



			} else {

				if (ballfound == 0) { //stop when we don't see anything
					scanforball = true;
					if (!stopped) {
						Logger::Instance().WriteMsg("BehaviorGoalie", "Setting stop command", Logger::ExtraInfo);
						stopped = true;
						wmot->set_command("setWalkTargetVelocity");
						wmot->set_parameter(0, 0);
						wmot->set_parameter(1, 0);
						wmot->set_parameter(2, 0);
						wmot->set_parameter(3, 0);
						Publisher::publish(wmot, "motion");
					}
				}
			}
		}
	} else {
		if (!stopped) {
			stopped = true;
			Logger::Instance().WriteMsg("BehaviorGoalie", "Setting stop command because of playing state ", Logger::ExtraExtraInfo);

			wmot->set_command("setWalkTargetVelocity");
			wmot->set_parameter(0, 0);
			wmot->set_parameter(1, 0);
			wmot->set_parameter(2, 0);
			wmot->set_parameter(3, 0);
			Publisher::publish(wmot, "motion");
		}
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


                Logger::Instance().WriteMsg("BehaviorGoalie", "Setting stop command because of playing state ", Logger::ExtraExtraInfo);

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
	hmot->set_parameter(0, scandirectionyaw * 0.18); // Headyaw

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



// 	if (reachedlimitup && reachedlimitdown) {
// 		Logger::Instance().WriteMsg("BehaviorGoalie", " reachedlimitup && reachedlimitdown ", Logger::ExtraExtraInfo);
// 		startscan = true;
// 		reachedlimitdown = false;
// 		reachedlimitup = false;
// 		reachedlimitright = false;
// 		reachedlimitleft = false;
// 		///we should turn;
// 		wmot->set_command("walkTo");
// 		wmot->set_parameter(0, 0.00001);
// 		wmot->set_parameter(1, 0.00001);
// 		wmot->set_parameter(2, (balllastseendirection > 0) ? (1) : (-1) * 0.52); //turn 30 degrees?
// 		Logger::Instance().WriteMsg("BehaviorGoalie", "Command HeadScan" + _toString(wmot->command()) + "1:  " + _toString(wmot->parameter(0)) + "2:  " + _toString(wmot->parameter(1))
// 				+ "3:  " + _toString(wmot->parameter(2)), Logger::Info);
// 		stopped=false;
// 		Publisher::publish(wmot, "motion"); //Send the message to the motion Controller
// 	}

}

void BehaviorGoalie::read_messages() {
	_blk->process_messages();
	gsm = _blk->in_nb<GameStateMessage> ("GameStateMessage", "RobotController");
	bmsg = _blk->in_nb<BallTrackMessage> ("BallTrackMessage", "Vision");
	hjsm = _blk->in_nb<HeadJointSensorsMessage> ("HeadJointSensorsMessage", "Sensors");

        obs = _blk->in_nb<ObservationMessage> ("ObservationMessage", "Vision");

	Logger::Instance().WriteMsg("BehaviorGoalie", "read_messages ", Logger::ExtraExtraInfo);
	CalibrateCam *c = _blk->in_msg_nb<CalibrateCam> ("CalibrateCam");
	if (c != NULL) {
		if (c->status() == 1)
			calibrated = 2;

		delete c;
	}
	cout << "calibrated:" << calibrated << endl;
}
#include "Behavior.h"
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

namespace {
	ActivityRegistrar<Behavior>::Type temp("Behavior");
}

Behavior::Behavior() :
	Publisher("Behavior") {
}

void Behavior::UserInit() {

	_com->get_message_queue()->add_publisher(this);
	_com->get_message_queue()->add_subscriber(_blk);
	_com->get_message_queue()->subscribe("vision", _blk, 0);
	_com->get_message_queue()->subscribe("sensors", _blk, 0);
	_com->get_message_queue()->subscribe("behavior", _blk, 0);
	try {
		memory = KAlBroker::Instance().GetBroker()->getMemoryProxy();
	} catch (AL::ALError& e) {
		Logger::Instance().WriteMsg("Behavior", "Error in getting memory proxy", Logger::Error);
		//cout << "Error in getting memory proxy" << std::endl;
	}

	mot = new MotionMessage();
	mot->set_topic("motion");
	mot->add_parameter(0.0f);
	mot->add_parameter(0.0f);
	mot->add_parameter(0.0f);
	mot->add_parameter(0.0f);
	mot->add_parameter(0.0f);

	ballfound = 0;

	balllastseendirection = -0.02;
	reachedlimitup = false;
	reachedlimitdown = false;
	reachedlimitleft = false;
	reachedlimitright = false;

	scanforball = true;
	startscan = true;

	calibrated = false;

	stopped = false;
	play = true;
	Logger::Instance().WriteMsg("Behavior", "Controller Initialized", Logger::Info);
	//cout << "Behavior Controller Initialized" << endl;

	hjsm = 0;
	bmsg = 0;
	gsm = 0;
}

int Behavior::Execute() {

	//MessageBuffer* sub_buf = _blk->getBuffer();
	//while (sub_buf->size() > 0) {
	read_messages();
	//};
	if (gsm != 0) {
		Logger::Instance().WriteMsg("Behavior", " Player_state " + _toString(gsm->player_state()), Logger::ExtraExtraInfo);
		//return 0;
		if (gsm->player_state() == 3) // this is the playing state
			play = true;
		else
			play = false;

		delete gsm;
	}
	//return 0;

	if (bmsg != 0) {
		calibrated = true;
		Logger::Instance().WriteMsg("Behavior", "BallTrackMessage", Logger::ExtraExtraInfo);

		if (bmsg->radius() > 0) { //This meens that a ball was found
			scanforball = false; //if you are scanning for ball please stop now

			float overshootfix = bmsg->radius();
			overshootfix = 2 * (0.4f - overshootfix);
			Logger::Instance().WriteMsg("Behavior", "Overshoot Value: " + _toString(overshootfix), Logger::ExtraInfo);

			float cx = bmsg->cx();
			float cy = bmsg->cy();

			balllastseendirection = HeadYaw.sensorvalue();
			Logger::Instance().WriteMsg("Behavior", "I want the freaking head to move towards (cx,cy):" + _toString(0.9f * (cx)) + _toString(-0.9f * (cy)), Logger::ExtraInfo);

			if (fabs(cx) > 0.015 || fabs(cy) > 0.015) {
				//Sending command to motion in order to move the head
				//mot->set_topic("motion");
				mot->set_command("changeHead");
				mot->set_parameter(0, 0.9f * overshootfix * (cx));
				mot->set_parameter(1, -0.9f * overshootfix * (cy));
				Publisher::publish( mot);
				Logger::Instance().WriteMsg("Behavior", "I send motion to head to move towards (cx,cy):" + _toString(mot->parameter(0)) + "," + _toString(mot->parameter(1)), Logger::ExtraInfo);
			}
			Logger::Instance().WriteMsg("Behavior", "Ball Found ole ", Logger::Info);

			ballfound += 2; //Increase this value when we see the ball
		} else {
			if (ballfound > 0) {
				ballfound -= 1; //Decrease it when we don't see the ball
				startscan = false;
				scanforball = false; // stop scanning
			} else {
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
	}

	Logger::Instance().WriteMsg("Behavior", "ballfound Value: " + _toString(ballfound), Logger::ExtraInfo);
	if (play && calibrated) {
		//Head joint related Behavior
		//Under this block we use the head joint values to calculated
		//1) the head scanning procedure
		//2) the walk parameters (how much to turn, walk ..) and the kicks
		if (hjsm != 0) {
			HeadYaw = hjsm->sensordata(0);
			HeadPitch = hjsm->sensordata(1);
			Logger::Instance().WriteMsg("Behavior", "HeadJointSensorsMessage arrived", Logger::ExtraExtraInfo);
			Logger::Instance().WriteMsg("Behavior", "HeadPitch.sensorvalue()" + _toString(HeadPitch.sensorvalue()) + "HeadYaw.sensorvalue()   " + _toString(HeadYaw.sensorvalue()), Logger::ExtraInfo);

		}

		//HeadScan
		if ((hjsm != 0) && (scanforball == true)) {//start or continue scan
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
			reachedlimitright = false;
			reachedlimitleft = false;

			//continue scan
			if (HeadPitch.sensorvalue() < LIMITUP) { // upper position
				Logger::Instance().WriteMsg("Behavior", " LIMITUP ", Logger::ExtraExtraInfo);
				reachedlimitup = true;
				scandirectionpitch = 1;
			}
			if (HeadPitch.sensorvalue() > LIMITDOWN) {
				Logger::Instance().WriteMsg("Behavior", " LIMITDOWN ", Logger::ExtraExtraInfo);
				reachedlimitdown = true;
				scandirectionpitch = -1;
			}
			if (HeadYaw.sensorvalue() > LIMITLEFT) {
				Logger::Instance().WriteMsg("Behavior", "LIMITLEFT  ", Logger::ExtraExtraInfo);
				reachedlimitleft = true;
				scandirectionyaw = -1;
			}
			if (HeadYaw.sensorvalue() < LIMITRIGHT) {
				Logger::Instance().WriteMsg("Behavior", " LIMITRIGHT  ", Logger::ExtraExtraInfo);
				reachedlimitright = true;
				scandirectionyaw = 1;
			}
			//			if (fabs(HeadYaw.sensorvalue()) > 0.40) {
			//				Logger::Instance()->WriteMsg("Behavior", " fabs(HeadYaw.sensorvalue()) >  0.40  ", Logger::ExtraExtraInfo);
			//				reachedlimitright = true;
			//				reachedlimitleft = true;
			//				scandirectionyaw *= -1; //change direction
			//			}

			mot->set_command("changeHead");
			mot->set_parameter(0, scandirectionyaw * 0.22); // Headyaw
			mot->set_parameter(1, 0.0); // headPitch

			if (reachedlimitleft || reachedlimitright) {
				Logger::Instance().WriteMsg("Behavior", " reachedlimitleft || reachedlimitright ", Logger::ExtraExtraInfo);
				mot->set_parameter(1, scandirectionpitch * 0.28); // headPitch
				//mot->set_parameter(0, 0.0); // headYaw
				reachedlimitleft = false;
				reachedlimitright = false;
			}

			//Publisher::publish( mot);
			if (reachedlimitup && reachedlimitdown) {
				Logger::Instance().WriteMsg("Behavior", " reachedlimitup && reachedlimitdown ", Logger::ExtraExtraInfo);
				startscan = true;
				reachedlimitdown = false;
				reachedlimitup = false;
				reachedlimitright = false;
				reachedlimitleft = false;
				///we should turn;
				mot->set_command("walkTo");
				mot->set_parameter(0, 0.00001);
				mot->set_parameter(1, 0.00001);
				mot->set_parameter(2, (balllastseendirection > 0) ? (1) : (-1) * 1.22); //turn 70 degrees?
			}
			Logger::Instance().WriteMsg("Behavior", "Command HeadScan" + _toString(mot->command()) + "1:  " + _toString(mot->parameter(0)) + "2:  " + _toString(mot->parameter(1))
					+ "3:  " + _toString(mot->parameter(2)), Logger::Info);

			Publisher::publish( mot); //Send the message to the motion Controller
		}

		//We have seen a ball for sure and we should walk
		//or we should kick
		if (hjsm != 0) {
			float X, Y, theta, freq;
			//Calculate approach
			X = 0;
			Y = 0;
			theta = 0;
			//mot->set_topic("motion");

			if (ballfound > 1) {
				//scanforball = false; //be sure to stop scanning

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

				//mot->set_topic("motion");
				if (!readytokick) {
					mot->set_command("setWalkTargetVelocity");

					if (fabs(X) > 1.0)
						X = (X > 0) ? 1 : -1;
					if (fabs(Y) > 1.0)
						Y = (Y > 0) ? 1 : -1;
					if (fabs(theta) > 1.0)
						theta = (theta > 0) ? 1 : -1;
					freq = 1.4 - fabs(HeadPitch.sensorvalue());
					if (fabs(freq) > 1.0)
						freq = (freq > 0) ? 1 : -1;

					mot->set_parameter(0, X);
					mot->set_parameter(1, Y);
					mot->set_parameter(2, theta);
					mot->set_parameter(3, freq);
					cout << "  setWalkTargetVelocity " << endl;
					Logger::Instance().WriteMsg("Behavior", "Walk Command" + _toString(mot->command()) + "X:  " + _toString(mot->parameter(0)) + "Y:  "
							+ _toString(mot->parameter(1)) + "theta:  " + _toString(mot->parameter(2)), Logger::ExtraExtraInfo);

				} else {
					cout << "Kicking" << endl;
					if (HeadYaw.sensorvalue() > 0.0)
						mot->set_command("leftKick");
					else
						mot->set_command("rightKick");
					Logger::Instance().WriteMsg("Behavior", "Kicking with " + _toString(mot->command()), Logger::Info);
				}
				Logger::Instance().WriteMsg("Behavior", "Command" + _toString(mot->command()) + "1:  " + _toString(mot->parameter(0)) + "2:  " + _toString(mot->parameter(1))
						+ "3:  " + _toString(mot->parameter(2)), Logger::ExtraInfo);
				stopped = false;
				Publisher::publish( mot);

			} else {

				if (ballfound == 0) { //stop when we don't see anything
					scanforball = true;
					if (!stopped) {
						Logger::Instance().WriteMsg("Behavior", "Setting stop command", Logger::ExtraInfo);
						stopped = true;
						mot->set_command("setWalkTargetVelocity");
						mot->set_parameter(0, 0);
						mot->set_parameter(1, 0);
						mot->set_parameter(2, 0);
						mot->set_parameter(3, 0);
						Publisher::publish( mot);
					}
				}
			}
		}
	} else {
		if (!stopped) {
			stopped = true;
			Logger::Instance().WriteMsg("Behavior", "Setting stop command because of playing state ", Logger::ExtraExtraInfo);

			mot->set_command("setWalkTargetVelocity");
			mot->set_parameter(0, 0);
			mot->set_parameter(1, 0);
			mot->set_parameter(2, 0);
			mot->set_parameter(3, 0);
			Publisher::publish( mot);
		}
	}
	return 0;
}

void Behavior::read_messages() {
	_blk->process_messages();
	gsm = dynamic_cast<GameStateMessage*> (_blk->in_nb("GameStateMessage", "RobotController"));
	bmsg = dynamic_cast<BallTrackMessage*> (_blk->in_nb("BallTrackMessage", "Vision"));
	hjsm = dynamic_cast<HeadJointSensorsMessage*> (_blk->in_nb("HeadJointSensorsMessage", "Sensors"));

	Logger::Instance().WriteMsg("Behavior", "read_messages ", Logger::ExtraExtraInfo);

}

#include "HeadBehavior.h"

namespace {
	ActivityRegistrar<HeadBehavior>::Type temp("HeadBehavior");
}

HeadBehavior::HeadBehavior() {
}

void HeadBehavior::UserInit() {

	_com->get_message_queue()->subscribe("vision", _blk, 0);
	_com->get_message_queue()->subscribe("sensors", _blk, 0);
	_com->get_message_queue()->subscribe("behavior", _blk, 0);

	hmot = new MotionHeadMessage();
	hmot->add_parameter(0.0f);
	hmot->add_parameter(0.0f);

	hbmot = new HeadBehaviorMessage();

	ballfound = 0;

	reachedlimitup = false;
	reachedlimitdown = false;
	reachedlimitleft = false;
	reachedlimitright = false;

	turnforscan = true;
	headstartscan = true;
	scanforball = true;
	calibrated = 0;

	play = false;

	gameState = PLAYER_INITIAL;

	Logger::Instance().WriteMsg("HeadBehavior", "Initialized", Logger::Info);
}

int HeadBehavior::MakeTrackBallAction() {

	//float overshootfix = 0.50;
	//float cx = bmsg->cx();
	//float cy = bmsg->cy();
	//if (fabs(cx) > 0.015 || fabs(cy) > 0.015) {
	//hmot->set_command("setHead");
	//hmot->set_parameter(0, bmsg->referenceyaw() - overshootfix * cx);
	//hmot->set_parameter(1, bmsg->referencepitch() - overshootfix * cy);
	hmot->set_command("setHead");
	hmot->set_parameter(0, bmsg->referenceyaw());
	hmot->set_parameter(1, bmsg->referencepitch());
	_blk->publish_signal(*hmot, "motion");
	cout << "Track step" << endl;
	//}

	return 1;
}

int HeadBehavior::Execute() {

	read_messages();

	if (gsm != 0) {
		Logger::Instance().WriteMsg("HeadBehavior", " Player_state " + _toString(gsm->player_state()), Logger::ExtraExtraInfo);
		int oldgamestate = gameState;
		gameState = gsm->player_state();

		bool stateChanged = oldgamestate != gameState;

		if (stateChanged) {

			if (gameState == PLAYER_PLAYING) {
				if (calibrated == 2) {
					play = true;
				} else if (calibrated == 0) {
					calibrate();
				}
			} else if (gameState == PLAYER_INITIAL) {
				play = false;
			} else if (gameState == PLAYER_READY) {
				calibrate();
				play = false;
			} else if (gameState == PLAYER_SET) {
				play = false;

			} else if (gameState == PLAYER_FINISHED) {
				play = false;
			} else if (gameState == PLAYER_PENALISED) {
				play = false;
				calibrate();
			}
		}
	}

	if (gameState == PLAYER_PLAYING) {
		if (calibrated == 2) {
			play = true;
		} else if (calibrated == 0) {
			calibrate();
		}
	}

	hbmot->set_calibrated(calibrated);
	turnforscan = false;

	if (play) {

		if (bmsg != 0) {
			Logger::Instance().WriteMsg("HeadBehavior", "BallTrackMessage", Logger::ExtraExtraInfo);
			if (bmsg->radius() > 0) { //This means that a ball was found
				MakeTrackBallAction();
				ballfound += 5;
				scanforball = false;
				if (ballfound > 20)
					ballfound = 20; //Increase this value when we see the ball

			} else {
				if (ballfound > 0)
					ballfound -= 1; //Decrease it when we don't see the ball
			}
		}
		Logger::Instance().WriteMsg("HeadBehavior", "ballfound Value: " + _toString(ballfound), Logger::ExtraInfo);

		if (ballfound == 0) {
			if (!scanforball) {
				headstartscan = true;
				scanforball = true;
				hbmot->set_ballfound(ballfound);
				hbmot->set_turnforscan(turnforscan);
				hbmot->set_scanforball(scanforball);
				_blk->publish_state(*hbmot, "behavior");
			}

			if (hjsm != 0) {
				HeadYaw = hjsm->sensordata(0);
				HeadPitch = hjsm->sensordata(1);
				HeadScanStep();
			}
		} else if (ballfound > 0) {
			hbmot->set_ballfound(ballfound);
			hbmot->set_turnforscan(turnforscan);
			hbmot->set_scanforball(scanforball);
			_blk->publish_state(*hbmot, "behavior");

		}
	}

	return 0;
}

void HeadBehavior::HeadScanStep() {

	if (headstartscan) {
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
		headstartscan = false;
	}

	//continue scan
	if (HeadPitch.sensorvalue() < LIMITUP) {
		//Logger::Instance().WriteMsg("HeadBehavior", " LIMITUP ", Logger::ExtraExtraInfo);
		reachedlimitup = true;
		scandirectionpitch = 1;
	}
	if (HeadPitch.sensorvalue() > LIMITDOWN) {
		//Logger::Instance().WriteMsg("HeadBehavior", " LIMITDOWN ", Logger::ExtraExtraInfo);
		reachedlimitdown = true;
		scandirectionpitch = -1;
	}
	if (HeadYaw.sensorvalue() > LIMITLEFT) {
		//Logger::Instance().WriteMsg("HeadBehavior", "LIMITLEFT  ", Logger::ExtraExtraInfo);
		reachedlimitleft = true;
		scandirectionyaw = -1;
	}
	if (HeadYaw.sensorvalue() < LIMITRIGHT) {
		//Logger::Instance().WriteMsg("HeadBehavior", " LIMITRIGHT  ", Logger::ExtraExtraInfo);
		reachedlimitright = true;
		scandirectionyaw = 1;
	}

	hmot->set_command("changeHead");
	hmot->set_parameter(0, scandirectionyaw * STEPHOR); // Headyaw
	hmot->set_parameter(1, 0.0); // headPitch

	if (reachedlimitleft && reachedlimitright) {
		Logger::Instance().WriteMsg("HeadBehavior", " reachedlimitleft && reachedlimitright ", Logger::ExtraExtraInfo);
		hmot->set_parameter(1, scandirectionpitch * STEPVER); // headPitch
		reachedlimitleft = false;
		reachedlimitright = false;
	}
	_blk->publish_signal(*hmot, "motion");

	if (reachedlimitup && reachedlimitdown) { //scanning completed
		Logger::Instance().WriteMsg("HeadBehavior", " reachedlimitup && reachedlimitdown ", Logger::ExtraExtraInfo);
		headstartscan = true;
		reachedlimitdown = false;
		reachedlimitup = false;
		reachedlimitright = false;
		reachedlimitleft = false;
		///we should do something;
		turnforscan = true;
		hbmot->set_ballfound(ballfound);
		hbmot->set_turnforscan(turnforscan);
		hbmot->set_scanforball(scanforball);
		_blk->publish_state(*hbmot, "behavior");
	}

	//startscan = false;
}

void HeadBehavior::read_messages() {

	//if (gsm != 0) delete gsm;
	//if (bmsg != 0) delete bmsg;
	//if (hjsm != 0) delete hjsm;
	//if (obsm != 0) delete obsm;

	gsm = _blk->read_state<GameStateMessage> ("GameStateMessage");
	bmsg = _blk->read_signal<BallTrackMessage> ("BallTrackMessage");
	hjsm = _blk->read_data<HeadJointSensorsMessage> ("HeadJointSensorsMessage");
	obsm = _blk->read_signal<ObservationMessage> ("ObservationMessage");

	Logger::Instance().WriteMsg("HeadBehavior", "read_messages ", Logger::ExtraExtraInfo);
	boost::shared_ptr<const CalibrateCam> c = _blk->read_state<CalibrateCam> ("CalibrateCam");
	if (c != NULL) {
		if (c->status() == 1) {
			calibrated = 2;
			hbmot->set_calibrated(calibrated);
		}
	}
}

void HeadBehavior::calibrate() {
	CalibrateCam v;
	v.set_status(0);
	_blk->publish_signal(v, "vision");
	calibrated = 1;
}


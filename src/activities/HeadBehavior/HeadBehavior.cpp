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

	hbmsg = new HeadToBMessage();
	scmsg = new ScanMessage();

	ballfound = 0;

	reachedlimitup = false;
	reachedlimitdown = false;
	reachedlimitleft = false;
	reachedlimitright = false;
	//vcalibrated = false;
	scancompleted = false;
	headstartscan = true;
	calibrated = 0;
	headaction = 0;
	oldheadaction = 0;
	choosemyaction = false;
	//counttime = false;
	//time(&start);
	//time(&end);
	Logger::Instance().WriteMsg("HeadBehavior", "Initialized", Logger::Info);
}

int HeadBehavior::Execute() {

	oldheadaction = headaction;
	read_messages();

	if (bhm != 0)
		headaction = bhm->headaction();
	if (choosemyaction) {
		headaction = oldheadaction;
		choosemyaction = false;
	}

	switch (headaction) {

		case (DONOTHING):
			//vcalibrated=false;
			break;
		case (CALIBRATE):
		//	if (!vcalibrated) {
				calibrate();
				calibrated = 1;
				hbmsg->set_calibrated(calibrated);
				//counttime = false;
		//	}
			headaction = DONOTHING;
			choosemyaction = true;
			//	_blk->publish_state(*hbmsg, "behavior"); ///signal or state
			break;
		case (SCANFORBALL):
		//	vcalibrated = false;
			scancompleted = false;
			//	if (obsm!=0 && obsm->has_ball()){
			if (bmsg != 0 && bmsg->radius() > 0) {
				headaction = BALLTRACK;
				ballfound += 5;
				choosemyaction = true;
				hbmsg->set_ballfound(ballfound);
				cout << "ballfound " << ballfound << "HeadBehavior" << endl;

				//	_blk->publish_state(*hbmsg, "behavior"); ///signal or state
				//time_t(&start);
				//counttime = true;
			} else if (hjsm != 0) {
				//counttime = false;
				HeadYaw = hjsm->sensordata(0);
				HeadPitch = hjsm->sensordata(1);
				HeadScanStep();
			}
			break;
		case (SCANFORPOST):
		//	vcalibrated = false;
			break;
		case (BALLTRACK):
			//vcalibrated = false;
			if (bmsg != 0) {
				Logger::Instance().WriteMsg("HeadBehavior", "BallTrackMessage", Logger::ExtraExtraInfo);
				if (bmsg->radius() > 0) { //This means that a ball was found
					MakeTrackBallAction();
					//time(&end);

					ballfound += 5;
					if (ballfound > 20)
						ballfound = 20; //Increase this value when we see the ball

				} else {
					if (ballfound > 0)
						ballfound -= 1; //Decrease it when we don't see the ball
					if (ballfound == 0) {
						headstartscan = true;
						//counttime = false;
					}
				}
			}
			cout << "ballfound " << ballfound << "HeadBehavior" << endl;

			hbmsg->set_ballfound(ballfound);
			Logger::Instance().WriteMsg("HeadBehavior", "ballfound Value: " + _toString(ballfound), Logger::ExtraInfo);
			//_blk->publish_state(*hbmsg, "behavior"); ///signal or state
			break;

	}
	//if (headaction == BALLTRACK || oldheadaction!=headaction || scancompleted)
	_blk->publish_state(*hbmsg, "behavior"); ///signal or state
	return 0;
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
		scancompleted = true;

		cout << "scancompleted " << scancompleted << "HeadBehavior" << endl;

		scmsg->set_scancompleted(scancompleted);
		_blk->publish_signal(*scmsg, "behavior");
	}

	//startscan = false;
}

void HeadBehavior::read_messages() {

	//if (bmsg != 0) delete bmsg;
	//if (hjsm != 0) delete hjsm;

	bhm = _blk->read_signal<BToHeadMessage> ("BToHeadMessage");
	bmsg = _blk->read_signal<BallTrackMessage> ("BallTrackMessage");
	hjsm = _blk->read_data<HeadJointSensorsMessage> ("HeadJointSensorsMessage");
	//obsm = _blk->read_signal<ObservationMessage> ("ObservationMessage");

	Logger::Instance().WriteMsg("HeadBehavior", "read_messages ", Logger::ExtraExtraInfo);
	boost::shared_ptr<const CalibrateCam> c = _blk->read_state<CalibrateCam> ("CalibrateCam");
	if (c != NULL) {
		if (c->status() == 1) {
			calibrated = 2;
			hbmsg->set_calibrated(calibrated);
		//	vcalibrated = true;
		}
	}
}

void HeadBehavior::calibrate() {
	CalibrateCam v;
	v.set_status(0);
	_blk->publish_signal(v, "vision");
	calibrated = 1;
}

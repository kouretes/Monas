#include "HeadBehavior.h"

#include "architecture/archConfig.h"
#include "tools/logger.h"
#include "tools/toString.h"
#include "messages/RoboCupGameControlData.h"

using namespace boost::posix_time;


namespace {
	ActivityRegistrar<HeadBehavior>::Type temp("HeadBehavior");
}

HeadBehavior::HeadBehavior() {
}
using namespace std;

void HeadBehavior::UserInit() {

	_blk->subscribeTo("vision", 0);
	_blk->subscribeTo("sensors",0);
	_blk->subscribeTo("behavior",0);

	hmot = new MotionHeadMessage();
	hmot->add_parameter(0.0f);
	hmot->add_parameter(0.0f);

	hbmsg = new HeadToBMessage();
	scmsg = new ScanMessage();
	ballLastSeen = microsec_clock::universal_time();
	ballfound = 0;

	reachedlimitup = false;
	reachedlimitdown = false;
	reachedlimitleft = false;
	reachedlimitright = false;
	scancompleted = false;
	headstartscan = true;
	calibrated = 0;
	headaction = 0;
	oldheadaction = 0;
	choosemyaction = false;

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
			std::cout << "HEADBEHAVIOR DONOTHING" <<std::endl;
			ballfound = 0;
			hbmsg->set_ballfound(ballfound);

			break;
		case (CALIBRATE):
			std::cout << "HEADBEHAVIOR CALIBRATE" <<std::endl;			
			calibrate();
			calibrated = 1;
			hbmsg->set_calibrated(calibrated);
			headaction = DONOTHING;
			choosemyaction = true;			
			
			break;
		case (SCANFORBALL):
			std::cout << "HEADBEHAVIOR SCANFORBALL" <<std::endl;
			scancompleted = false;
			if (bmsg != 0 && bmsg->radius() > 0) {
				headaction = BALLTRACK;
				ballfound =1;
				ballLastSeen = microsec_clock::universal_time()+seconds(2);
				choosemyaction = true;
				hbmsg->set_ballfound(ballfound);
				cout << "ballfound " << ballfound << "HeadBehavior" << endl;
			} else if (hjsm != 0) {
				HeadYaw = hjsm->sensordata(0);
				HeadPitch = hjsm->sensordata(1);
				HeadScanStep();
			}
			break;
		case (SCANFORPOST):
			std::cout << "HEADBEHAVIOR SCANFORPOST" <<std::endl;
			break;
		case (BALLTRACK):
			std::cout << "HEADBEHAVIOR BALLTRACK" <<std::endl;
			if (bmsg != 0) {
				Logger::Instance().WriteMsg("HeadBehavior", "BallTrackMessage", Logger::ExtraExtraInfo);
				if (bmsg->radius() > 0) { //This means that a ball was found
					MakeTrackBallAction();

					ballfound = 1;
					ballLastSeen = microsec_clock::universal_time()+seconds(2);

				} else {
					if (ballLastSeen <=microsec_clock::universal_time())
						ballfound = 1;
					else{
						ballfound = 0;
						headstartscan = true;
					}
				}
			}
			cout << "ballfound " << ballfound << "HeadBehavior" << endl;

			hbmsg->set_ballfound(ballfound);
			break;
	}
	_blk->publish_state(*hbmsg, "behavior");
	return 0;
}

int HeadBehavior::MakeTrackBallAction() {

	hmot->set_command("setHead");
	hmot->set_parameter(0, bmsg->referenceyaw());
	hmot->set_parameter(1, bmsg->referencepitch());
	_blk->publish_signal(*hmot, "motion");
	cout << "Track step" << endl;

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
}

void HeadBehavior::read_messages() {

	bhm = _blk->read_signal<BToHeadMessage> ("BToHeadMessage");
	bmsg = _blk->read_signal<BallTrackMessage> ("BallTrackMessage");
	hjsm = _blk->read_data<HeadJointSensorsMessage> ("HeadJointSensorsMessage");

	Logger::Instance().WriteMsg("HeadBehavior", "read_messages ", Logger::ExtraExtraInfo);
	boost::shared_ptr<const CalibrateCam> c = _blk->read_state<CalibrateCam> ("CalibrateCam");
	if (c != NULL) {
		if (c->status() == 1) {
			calibrated = 2;
			hbmsg->set_calibrated(calibrated);
		}
	}
}

void HeadBehavior::calibrate() {
	CalibrateCam v;
	v.set_status(0);
	_blk->publish_signal(v, "vision");
	calibrated = 1;
}

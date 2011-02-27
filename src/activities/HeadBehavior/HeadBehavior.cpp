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

//	Logger::Instance().WriteMsg("HeadBehavior", "Initialized", Logger::Info);
}

int HeadBehavior::Execute() {

	read_messages();

	if (bhm != 0)
		headaction = bhm->headaction();
	if (choosemyaction) {
		headaction = oldheadaction;
		choosemyaction = false;
	}
	
	switch (headaction) {

		case (DONOTHING):
			//std::cout << "HEADBEHAVIOR DONOTHING" <<std::endl;
			//Logger::Instance().WriteMsg("HeadBehavior",  " DONOTHING", Logger::Info);
			ballfound = 0;
			hbmsg->set_ballfound(ballfound);

			break;
		case (CALIBRATE):
			if(oldheadaction != CALIBRATE){
				//std::cout << "HEADBEHAVIOR CALIBRATE" <<std::endl;			
				//Logger::Instance().WriteMsg("HeadBehavior",  " CALIBRATE", Logger::Info);
				calibrate();
				calibrated = 1;
				hbmsg->set_calibrated(calibrated);
				ballfound = 0;
				hbmsg->set_ballfound(ballfound);
				//headaction = DONOTHING;
				//choosemyaction = true;			
			}//else
				//Logger::Instance().WriteMsg("HeadBehavior",  " DONOTHING", Logger::Info);
			break;
		case (SCANFORBALL):
			//Logger::Instance().WriteMsg("HeadBehavior",  " SCANFORBALL", Logger::Info);
			scancompleted = false;
			if (bmsg != 0 && bmsg->radius() > 0) {
				MakeTrackBallAction();
				headaction = BALLTRACK;
				ballfound =1;
				ballLastSeen = microsec_clock::universal_time()+seconds(2);
				choosemyaction = true;
				hbmsg->set_ballfound(ballfound);
			//	cout << "ballfound " << ballfound << "HeadBehavior" << endl;
			} else if (allsm != 0) {
				//std::cout << "HEADBEHAVIOR SCANFORBALL" <<std::endl;
				HeadYaw= allsm->hjsm().sensordata(YAW);
				HeadPitch= allsm->hjsm().sensordata(PITCH);
				HeadScanStep();
			}
			break;
		case (SCANFORPOST):
			//Logger::Instance().WriteMsg("HeadBehavior",  " SCANFORPOST", Logger::Info);
			//std::cout << "HEADBEHAVIOR SCANFORPOST" <<std::endl;
			break;
		case (BALLTRACK):
			//Logger::Instance().WriteMsg("HeadBehavior",  " BALLTRACK", Logger::Info);
			//std::cout << "HEADBEHAVIOR BALLTRACK" <<std::endl;
			scancompleted=false;
			//scmsg->set_scancompleted(scancompleted);
			//_blk->publish_state(*scmsg, "behavior");
			ballfound=0;
			if (bmsg != 0) {
		//		Logger::Instance().WriteMsg("HeadBehavior", "BallTrackMessage", Logger::ExtraExtraInfo);
				if (bmsg->radius() > 0) { //This means that a ball was found
					MakeTrackBallAction();

					ballfound = 1;
					ballLastSeen = microsec_clock::universal_time()+seconds(1);

				} else {
					if (ballLastSeen <= microsec_clock::universal_time()){
						ballfound = 1;
					}else{
						ballfound = 0;
						headstartscan = true;
						lastturn=microsec_clock::universal_time()+seconds(4);
					}
				}
			}
		//	cout << "ballfound " << ballfound << "HeadBehavior" << endl;

			hbmsg->set_ballfound(ballfound);
			break;
	}
	oldheadaction = headaction;
	_blk->publish_state(*hbmsg, "behavior");
	return 0;
}

int HeadBehavior::MakeTrackBallAction() {

	hmot->set_command("setHead");
	hmot->set_parameter(0, bmsg->referenceyaw());
	hmot->set_parameter(1, bmsg->referencepitch());
	_blk->publish_signal(*hmot, "motion");
//	cout << "Track step" << endl;

	return 1;
}

void HeadBehavior::HeadScanStep() {
	
	static float s=(YAWMIN-YAWMAX)/(PITCHMIN-PITCHMAX);
	if (headstartscan) {
		//BE CAREFULL the max sign is according to sensors values (max maybe negative! :p)
		ysign=HeadYaw.sensorvalue()>0?1:-1; //Side
		//Crop to limits
		targetPitch=HeadPitch.sensorvalue();
		targetYaw=HeadYaw.sensorvalue();
		targetPitch=(targetPitch>=PITCHMAX)?PITCHMAX:targetPitch;
		targetPitch=(targetPitch<=PITCHMIN)?PITCHMIN:targetPitch;


		float yawlim=s*(targetPitch-PITCHMAX)+YAWMAX;
		//if(fabs(targetPitch)<PITCHSTEP) yawlim=YAWBACK;


		targetYaw+=ysign*YAWSTEP;
		targetYaw=fabs(targetYaw)>=yawlim?ysign*yawlim:targetYaw;

		if(fabs(targetYaw)>=yawlim)
		{
			ysign=-ysign;
		}
		psign=1;//Down
		hmot->set_command("setHead");
		hmot->set_parameter(0, targetYaw);
		hmot->set_parameter(1, targetPitch);
		_blk->publish_signal(*hmot, "motion");
		waiting=0;

		headstartscan=false;

	}
	waiting++;
	if( (fabs(targetPitch-HeadPitch.sensorvalue())<=OVERSH &&fabs(targetYaw -HeadYaw.sensorvalue())<=OVERSH )
		|| waiting>=WAITFOR)
	{

		waiting=0;

		float yawlim=s*(targetPitch-PITCHMAX)+YAWMAX;
		//if(fabs(targetPitch)<PITCHSTEP) yawlim=YAWBACK;


		if(fabs(fabs(targetYaw)-yawlim)<=OVERSH)
		{
			targetPitch+=psign*PITCHSTEP;
			targetPitch=(targetPitch>=PITCHMAX)?PITCHMAX:targetPitch;
			targetPitch=(targetPitch<=PITCHMIN)?PITCHMIN:targetPitch;
			if(targetPitch>=PITCHMAX)
				psign=-1;
			else if(targetPitch<=PITCHMIN)
				psign=1;


		}


		targetYaw+=ysign*YAWSTEP;
		targetYaw=fabs(targetYaw)>=yawlim?ysign*yawlim:targetYaw;
		if(fabs(targetYaw)>=yawlim)
		{
			ysign=-ysign;
		}


		hmot->set_command("setHead");
		hmot->set_parameter(0, targetYaw);
		hmot->set_parameter(1, targetPitch);
		_blk->publish_signal(*hmot, "motion");
	}
	if (microsec_clock::universal_time()>lastturn){
		lastturn=microsec_clock::universal_time()+seconds(4);
		scancompleted = true;
	//	cout << "scancompleted " << scancompleted << "HeadBehavior" << endl;
		scmsg->set_scancompleted(scancompleted);
		_blk->publish_signal(*scmsg, "behavior");
	}
	
}

void HeadBehavior::read_messages() {

	bhm = _blk->read_state<BToHeadMessage> ("BToHeadMessage");
	bmsg = _blk->read_signal<BallTrackMessage> ("BallTrackMessage");
//	hjsm = _blk->read_data<HeadJointSensorsMessage> ("HeadJointSensorsMessage");
allsm = _blk->read_data<AllSensorValues> ("AllSensorValues");
//	Logger::Instance().WriteMsg("HeadBehavior", "read_messages ", Logger::ExtraExtraInfo);
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

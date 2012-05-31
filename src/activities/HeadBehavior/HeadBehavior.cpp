#include "HeadBehavior.h"

#include "architecture/archConfig.h"
#include "tools/logger.h"
#include "tools/toString.h"
#include "messages/RoboCupGameControlData.h"

using namespace boost::posix_time;



ACTIVITY_REGISTER(HeadBehavior);

using namespace std;

void HeadBehavior::UserInit() {


	_blk.updateSubscription("vision", msgentry::SUBSCRIBE_ON_TOPIC);
	_blk.updateSubscription("sensors", msgentry::SUBSCRIBE_ON_TOPIC);
	_blk.updateSubscription("behavior", msgentry::SUBSCRIBE_ON_TOPIC);

	state =1;
	hmot = new MotionHeadMessage();
	hmot->add_parameter(0.0f);
	hmot->add_parameter(0.0f);
	hbmsg = new HeadToBMessage();
	scmsg = new ScanMessage();
	ballLastSeen =ballFirstSeen= microsec_clock::universal_time()-hours(5);

	GoalLastSeen =GoalFirstSeen= microsec_clock::universal_time()-hours(5);
	ysign = 1;
	headpos = 0.0;
	leftright = 1;
	prevaction = DONOTHING;
	curraction = DONOTHING;
	calibrated = 0;
	headaction = 0;
	lastbearing=obsmbearing=-1;

//	Logger::Instance().WriteMsg("HeadBehavior", "Initialized", Logger::Info);
}

int HeadBehavior::Execute() {
//Logger::Instance().WriteMsg("HeadBehavior", "start", Logger::Info);
	read_messages();
	if (bhm != 0){
		curraction = bhm->headaction();
		if(prevaction==CALIBRATE)
			headaction = DONOTHING;
		else
			headaction = curraction;
	}
	else{
		if(prevaction!=CALIBRATE)
			headaction = prevaction;
		else
			headaction = DONOTHING;
	}
	ptime now=microsec_clock::universal_time();
	newBearing=false;
	if(obsm&&obsm->regular_objects_size() > 0)
	{
		bool yellow = false, blue = false;
		::google::protobuf::RepeatedPtrField<const ::NamedObject>::const_iterator ptr = obsm->regular_objects().begin();
		obsmbearing=(*ptr).bearing();
		newBearing=true;
	}
	else
		obsmbearing=-1;
	if(headaction==SCANFORBALL)
	{
		if(ballLastSeen+seconds(1) >= now &&ballFirstSeen+seconds(1) < now  && obsm && obsm->has_ball() && obsm->ball().dist()>=0.2)
		{
			headaction=SCANFORPOST;
			if(obsmbearing==-1&&lastbearing!=-1) {obsmbearing=lastbearing;};
		}
		else if(!(bmsg != 0 &&bmsg->radius() > 0)&&lastgoodbmsg.get()) {
			bmsg=lastgoodbmsg;
			headaction = BALLTRACK;
		}

	}
	if (bmsg != 0 && bmsg->radius() > 0) { //This means that a ball was found
		startscan=false;
		if(ballLastSeen+seconds(1)<=now)
			ballFirstSeen=now;
		ballLastSeen = now;

		if(headaction==SCANFORBALL||headaction==BALLTRACK)
		{
			if(lastgoodbmsg!=bmsg)
				lastgoodbmsg=bmsg;
			else
				lastgoodbmsg.reset();

		}

		hbmsg->set_ballfound(1);

	} else {
			if (ballLastSeen+seconds(1.5) > now){ //Lost
				startscan=true;

				hbmsg->set_ballfound(0);
			}
	}
	if(obsmbearing!=-1)
	{

		if(GoalLastSeen+seconds(1)<=now)
			GoalFirstSeen=now;
		GoalLastSeen = now;
		if(headaction==SCANFORPOST)
		{
			if(newBearing)
				lastbearing=obsmbearing;
			else
				lastbearing=-1;
		}



	}

	//cout<<"-----action:"<<headaction<<endl;

	if(headaction==SCANFORBALL || headaction== SCANFIELD)
		;
	else
		startscan =true;
	//headaction = DONOTHING;


	switch (headaction) {

		case (DONOTHING):
			//std::cout << "HEADBEHAVIOR DONOTHING" <<std::endl;
		//	Logger::Instance().WriteMsg("HeadBehavior",  " DONOTHING", Logger::Info);
			hbmsg->set_ballfound(0);
			step =0;
			break;
		case (CALIBRATE):
			step =0;
			//std::cout << "HEADBEHAVIOR CALIBRATE" <<std::endl;
			//Logger::Instance().WriteMsg("HeadBehavior",  " CALIBRATE", Logger::Info);
			calibrate();
			hbmsg->set_calibrated(calibrated);
			hbmsg->set_ballfound(0);
			//Logger::Instance().WriteMsg("HeadBehavior",  " DONOTHING", Logger::Info);
			break;
		case (SCANFORBALL):
			step =0;
		//	Logger::Instance().WriteMsg("HeadBehavior",  " SCANFORBALL", Logger::Info);
			if (bmsg != 0 && bmsg->radius() > 0) {
				MakeTrackBallAction();
				//hbmsg->set_ballfound(1);
			//	cout << "ballfound " << ballfound << "HeadBehavior" << endl;
			} else if (asvm != 0&&ballLastSeen+milliseconds(500)<now) {
				//std::cout << "HEADBEHAVIOR SCANFORBALL" <<std::endl;
				HeadYaw= asvm->jointdata(KDeviceLists::HEAD+KDeviceLists::YAW);
				HeadPitch= asvm->jointdata(KDeviceLists::HEAD+KDeviceLists::PITCH);
				HeadScanStepSmart();
				//HeadScanStep();

			}
			break;
		case (HIGHSCANFORBALL):
			step =0;
			//Logger::Instance().WriteMsg("HeadBehavior",  " HIGHSCANFORBALL", Logger::Info);
			if (bmsg != 0 && bmsg->radius() > 0) {
				MakeTrackBallAction();
				//hbmsg->set_ballfound(1);
			//	cout << "ballfound " << ballfound << "HeadBehavior" << endl;
			} else{
				highheadscanstep(1.8);
			}
			break;

		case (SCANFORPOST):
			step =0;
			if (bmsg != 0 && bmsg->radius() > 0) {
				//MakeTrackBallAction();
			//	cout << "ballfound " << ballfound << "HeadBehavior" << endl;
			hbmsg->set_ballfound(1);
			}
			if(newBearing)
			{
				headmotion(-0.55,obsmbearing);
			}
			else if (asvm != 0 && GoalLastSeen+milliseconds(500)<now) {
				//std::cout << "HEADBEHAVIOR SCANFORBALL" <<std::endl;
				highheadscanstep(2.08);
			}

			//Logger::Instance().WriteMsg("HeadBehavior",  " SCANFORPOST", Logger::Info);
			//std::cout << "HEADBEHAVIOR SCANFORPOST" <<std::endl;
			break;
		case (BALLTRACK):
		//	Logger::Instance().WriteMsg("HeadBehavior",  " BALLTRACK", Logger::Info);
			MakeTrackBallAction();
			step =0;
			break;

		case (SCANFIELD):
			step =0;
			if (bmsg != 0 && bmsg->radius() > 0) {
				hbmsg->set_ballfound(1);
			}
			if(asvm != 0){
				HeadYaw= asvm->jointdata(KDeviceLists::HEAD+KDeviceLists::YAW);
				HeadPitch= asvm->jointdata(KDeviceLists::HEAD+KDeviceLists::PITCH);
			}
			HeadScanStepSmart();
			break;
		case (SCANAFTERPENALISED):
			if(step%2 ==0)
				headmotion( -0.504728, 0.972598);
			else
				headmotion( -0.504728, -0.972598);
				step++;
			break;
	}
	prevaction = curraction;
	_blk.publishState(*hbmsg, "behavior");
	//Logger::Instance().WriteMsg("HeadBehavior", "end", Logger::Info);
	return 0;
}

int HeadBehavior::MakeTrackBallAction() {


	if (bmsg != 0) {
//		Logger::Instance().WriteMsg("HeadBehavior", "BallTrackMessage", Logger::ExtraExtraInfo);
		if (bmsg->radius() > 0) { //This means that a ball was found
			headmotion(bmsg->referencepitch(), bmsg->referenceyaw());
		}
	}

	return 1;
}



void HeadBehavior::HeadScanStep() {
	static float s=(YAWMIN-YAWMAX)/(PITCHMIN-PITCHMAX);

	if (startscan) {
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
		headmotion(targetPitch, targetYaw);
		waiting=0;

		startscan=false;
		return;

	}

	if ((targetYaw>=YAWMAX || targetYaw<=YAWMIN) && (targetPitch>=PITCHMAX || targetPitch<=PITCHMIN)){
		scmsg->set_scancompleted(1);
		_blk.publishSignal(*scmsg, "behavior");
	}
	waiting++;
	if( (fabs(targetPitch-HeadPitch.sensorvalue())<=OVERSH &&fabs(targetYaw -HeadYaw.sensorvalue())<=OVERSH )
		|| waiting>=WAITFOR)
	{

		waiting=0;

		float yawlim=s*(targetPitch-PITCHMAX)+YAWMAX;

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
		else
		{
			targetYaw+=ysign*YAWSTEP;
			targetYaw=fabs(targetYaw)>=yawlim?ysign*yawlim:targetYaw;
			if(fabs(targetYaw)>=yawlim)
			{
				ysign=-ysign;
			}
		}

		headmotion(targetPitch, targetYaw);
	}
	return ;

}


void HeadBehavior::HeadScanStepSmart() {

	float  blue1y, blue1p, blue2y, blue2p;
	blue1y = +0.75;
	blue1p = +0.38;
	blue2y = +0.00;
	blue2p = -0.55;
	float green1y, green1p, green2y, green2p;
	green1y = +1.45;
	green1p = -0.42;
	green2y = +0.00;
	green2p = +0.35;
	float red1y, red1p, red2y, red2p;
	red1y = +1.80;
	red1p = -0.39;
	red2y = +0.00;
	red2p = -0.60;
	static enum {BLUE, RED, GREEN} state = BLUE;
	static enum {START, MIDDLE, END} phase = START;

//	HeadYaw = asvm->jointdata(KDeviceLists::HEAD+KDeviceLists::YAW);
//	HeadPitch = asvm->jointdata(KDeviceLists::HEAD+KDeviceLists::PITCH);

	if (startscan) {
		ysign = HeadYaw.sensorvalue() > 0 ? +1 : -1; //Side
		targetYaw = blue1y * ysign;
		targetPitch = blue1p;
		state = BLUE;
		phase = START;

		headmotion(targetPitch, targetYaw);
		waiting = 0;
		startscan = false;
		return;
	}

	waiting++;

	if ( ( (fabs(targetPitch - HeadPitch.sensorvalue()) <= OVERSH) && (fabs(targetYaw - HeadYaw.sensorvalue()) <= OVERSH) ) || (waiting >= WAITFOR) ) {
		waiting = 0;
		if (phase == START) {
			phase = MIDDLE;
			switch (state) {
			case BLUE:
				targetYaw = blue2y;
				targetPitch = blue2p;
				break;
			case GREEN:
				targetYaw = green2y;
				targetPitch = green2p;
				break;
			case RED:
				targetYaw = red2y;
				targetPitch = red2p;
				break;
			}
		}
		else if (phase == MIDDLE) {
			ysign = -ysign;
			phase = END;
			switch (state) {
			case BLUE:
				targetYaw = blue1y*ysign;
				targetPitch = blue1p;
				break;
			case GREEN:
				targetYaw = green1y * ysign;
				targetPitch = green1p;
				break;
			case RED:
				targetYaw = red1y*ysign;
				targetPitch = red1p;
				break;
			}
		}
		else {
			phase = START;
			switch (state) {
			case BLUE:
				state = GREEN;
				targetYaw = green1y * ysign;
				targetPitch = green1p;
				break;
			case GREEN:
				state = RED;
				targetYaw = red1y * ysign;
				targetPitch = red1p;
				break;
			case RED:
				state = BLUE;
				targetYaw = blue1y * ysign;
				targetPitch = blue1p;
				break;
			}
		}


		headmotion(targetPitch, targetYaw);
	}
	return;
}


void HeadBehavior::read_messages() {

	bhm = _blk.readSignal<BToHeadMessage> ("behavior");
	bmsg = _blk.readSignal<BallTrackMessage> ("vision");
	obsm = _blk.readSignal<ObservationMessage> ("vision");
	asvm = _blk.readData<AllSensorValuesMessage> ("sensors");
	boost::shared_ptr<const CalibrateCam> c = _blk.readState<CalibrateCam> ("vision");
	if (c != NULL) {
		if (c->status() == 1) {
			calibrated = 2;
			hbmsg->set_calibrated(2);
		}
	}
}

void HeadBehavior::calibrate() {
	CalibrateCam v;
	v.set_status(0);
	_blk.publishState(v, "vision");
	Logger::Instance().WriteMsg("HeadBehavior", "sendCalibrate ", Logger::Info);
	calibrated = 1;
}

void HeadBehavior::highheadscanstep(float limit_yaw){

	float pitchd;
	//hmot->set_command("setHead");
	if (fabs(headpos) > limit_yaw) // 1.8 h 2.08
		leftright *= -1;

	headpos += 0.1 * leftright;

	//hmot->set_parameter(0, headpos);	//yaw

	if(fabs(headpos)<1.57){
	//	Logger::Instance().WriteMsg("HeadBehavior",  " PITCH " + _toString((0.145 * fabs(headpos)) - 0.752), Logger::Info);
	//	hmot->set_parameter(1, (0.145 * fabs(headpos)) - 0.752);	//pitch
		pitchd =(0.145 * fabs(headpos)) - 0.752;
	}
	else{
	//	Logger::Instance().WriteMsg("HeadBehavior",  " PITCH " + _toString((-0.0698 * (fabs(headpos)-1.57)) - 0.52), Logger::Info);
		//hmot->set_parameter(1, (-0.0698 * (fabs(headpos)-1.57)) - 0.52);	//pitch
		pitchd = (-0.0698 * (fabs(headpos)-1.57)) - 0.52;
	}

	//_blk.publishSignal(*hmot, "motion");
	headmotion(pitchd, headpos);
	return;
}



void HeadBehavior::HeadScanStepFieldUntested(){
		float tpitch=0.0, tyaw=0.0;
		if(startscan){
				tpitch = PITCH1;
				tyaw = YAW1;
				state = 1;
				startscan = false;
				return;
			}

			if(state==1){
				tpitch = PITCH1;
				tyaw = HeadYaw.sensorvalue() + YAWSTEP1;
				if(tyaw>=YAW2){
					tyaw = YAW2;
					state = 2;
				}
			}else if(state==2){
				tpitch = PITCH2;
				tyaw = YAW3;
				state = 3;
			}else if(state ==3){
				tyaw = HeadYaw.sensorvalue() - YAWSTEP1;
				if(tyaw<=YAW4){
					tyaw = YAW4;
					state = 4;
				}
				tpitch =  0.182*tyaw - 0.67;
			}else if(state==4){
				tyaw = HeadYaw.sensorvalue() - YAWSTEP1;
				if(tyaw<=YAW5){
					tyaw = YAW5;
					state = 5;
				}
				tpitch =  -0.182*tyaw - 0.67;
			}else if(state ==5){
				tyaw = HeadYaw.sensorvalue() - YAWSTEP1;
				if(tyaw<=YAW1){
					tyaw = YAW1;
					startscan = true;
				}
				tpitch = -1.95*tyaw - 1.447;
			}
			headmotion(tpitch, tyaw);
	}

void HeadBehavior::headmotion(float pitch, float yaw){
	hmot->set_command("setHead");
	hmot->set_parameter(0, yaw);
	hmot->set_parameter(1, pitch);
	_blk.publishSignal(*hmot, "motion");
	//Logger::Instance().WriteMsg("HeadBehavior",  " YAW " + _toString(yaw), Logger::Info);
	//Logger::Instance().WriteMsg("HeadBehavior",  " PITCH " + _toString(pitch), Logger::Info);

}

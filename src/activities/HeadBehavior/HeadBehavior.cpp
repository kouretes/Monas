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

	field = true;
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
	float tpitch=0.0, tyaw=0.0;
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
		if(ballLastSeen+seconds(1) >= now &&ballFirstSeen+seconds(1) < now )
		{
			headaction=SCANFORPOST;
			if(obsmbearing==-1&&lastbearing!=-1) {obsmbearing=lastbearing;};
		}
		else if(!(bmsg != 0 &&bmsg->radius() > 0)&&lastgoodbmsg.get()) {bmsg=lastgoodbmsg;};

	}
	if (bmsg != 0 && bmsg->radius() > 0) { //This means that a ball was found
		startscan=false;
		if(ballLastSeen+seconds(1)<=now)
			ballFirstSeen=now;
		ballLastSeen = now;

		if(headaction==SCANFORBALL||headaction==HIGHSCANFORBALL||headaction==BALLTRACK)
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
				lastturn=now+seconds(4);
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
	//if(headaction!=CALIBRATE && headaction!=DONOTHING && (calibrated!=2 || calibrated!=1))
		//headaction = CALIBRATE;
	if(headaction!= SCANFIELD){
		field = true;
	}
	if(headaction!=SCANFORBALL)
		startscan =true;
	switch (headaction) {

		case (DONOTHING):
			//std::cout << "HEADBEHAVIOR DONOTHING" <<std::endl;
			Logger::Instance().WriteMsg("HeadBehavior",  " DONOTHING", Logger::Info);
			hbmsg->set_ballfound(0);
			//calibrated=0;
			break;
		case (CALIBRATE):

			//std::cout << "HEADBEHAVIOR CALIBRATE" <<std::endl;
			Logger::Instance().WriteMsg("HeadBehavior",  " CALIBRATE", Logger::Info);
			//if(calibrated!=1 && calibrated!=2)
			calibrate();
			//calibrated = 1;
			hbmsg->set_calibrated(calibrated);
			hbmsg->set_ballfound(0);
			//headaction = DONOTHING;
			//choosemyaction = true;

			//Logger::Instance().WriteMsg("HeadBehavior",  " DONOTHING", Logger::Info);
			break;
		case (SCANFORBALL):
			//calibrated=0;
			Logger::Instance().WriteMsg("HeadBehavior",  " SCANFORBALL", Logger::Info);
			if (bmsg != 0 && bmsg->radius() > 0) {
				MakeTrackBallAction();
			//	cout << "ballfound " << ballfound << "HeadBehavior" << endl;
			} else if (asvm != 0&&ballLastSeen+milliseconds(500)<now) {
				//std::cout << "HEADBEHAVIOR SCANFORBALL" <<std::endl;
				HeadYaw= asvm->jointdata(KDeviceLists::HEAD+KDeviceLists::YAW);
				HeadPitch= asvm->jointdata(KDeviceLists::HEAD+KDeviceLists::PITCH);
				HeadScanStep();

			}
			break;
		case (HIGHSCANFORBALL):
			Logger::Instance().WriteMsg("HeadBehavior",  " HIGHSCANFORBALL", Logger::Info);
			if (bmsg != 0 && bmsg->radius() > 0) {
				MakeTrackBallAction();
				hbmsg->set_ballfound(1);
			//	cout << "ballfound " << ballfound << "HeadBehavior" << endl;
			} else{
				highheadscanstep(1.8);
	
			}	
			break;
		
		case (SCANFORPOST):
			if (bmsg != 0 && bmsg->radius() > 0) {
				MakeTrackBallAction();
			//	cout << "ballfound " << ballfound << "HeadBehavior" << endl;
			hbmsg->set_ballfound(1);
			}else{
				if(newBearing)
				{
					headmotion(obsmbearing, -0.55);
				}
				else if (asvm != 0 && GoalLastSeen+milliseconds(500)<now) {
					//std::cout << "HEADBEHAVIOR SCANFORBALL" <<std::endl;
					highheadscanstep(2.08);
				}

			}
			
			Logger::Instance().WriteMsg("HeadBehavior",  " SCANFORPOST", Logger::Info);
			//std::cout << "HEADBEHAVIOR SCANFORPOST" <<std::endl;
			break;
		case (BALLTRACK):
			//calibrated=0;
			Logger::Instance().WriteMsg("HeadBehavior",  " BALLTRACK", Logger::Info);
			MakeTrackBallAction();
			break;
			
		case (SCANFIELD):
			if(asvm != 0){
				HeadYaw= asvm->jointdata(KDeviceLists::HEAD+KDeviceLists::YAW);
				HeadPitch= asvm->jointdata(KDeviceLists::HEAD+KDeviceLists::PITCH);
			}
			if(field){
				tpitch = PITCH1;
				tyaw = YAW1;
				state = 1;
				field = false;
			}else{
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
						field = true;
					}
					tpitch = -1.95*tyaw - 1.447;	
				}
			}	
			headmotion(tpitch, tyaw);		
			break;
	}
	prevaction = curraction;
	_blk->publishState(*hbmsg, "behavior");
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

	}
	
	if ((targetYaw>=YAWMAX || targetYaw<=YAWMIN) && (targetPitch>=PITCHMAX || targetPitch<=PITCHMIN)){
		lastturn=microsec_clock::universal_time()+seconds(4);
		scmsg->set_scancompleted(1);
		_blk->publishSignal(*scmsg, "behavior");
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

void HeadBehavior::read_messages() {

	bhm = _blk->readSignal<BToHeadMessage> ("behavior");
	bmsg = _blk->readSignal<BallTrackMessage> ("vision");
	obsm = _blk->readSignal<ObservationMessage> ("vision");
	asvm = _blk->readData<AllSensorValuesMessage> ("sensors");
	boost::shared_ptr<const CalibrateCam> c = _blk->readState<CalibrateCam> ("vision");
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
	_blk->publishState(v, "vision");
	Logger::Instance().WriteMsg("HeadBehavior", "sendCalibrate ", Logger::Info);
	calibrated = 1;
}

void HeadBehavior::highheadscanstep(float limit_yaw){
	
	
	hmot->set_command("setHead");
	if (fabs(headpos) > limit_yaw) // 1.8 h 2.08
		leftright *= -1;

	headpos += 0.2 * leftright;

	hmot->set_parameter(0, headpos);	//yaw

	if(fabs(headpos)<1.57){
	//	Logger::Instance().WriteMsg("HeadBehavior",  " PITCH " + _toString((0.145 * fabs(headpos)) - 0.752), Logger::Info);
		hmot->set_parameter(1, (0.145 * fabs(headpos)) - 0.752);	//pitch
	}
	else{
	//	Logger::Instance().WriteMsg("HeadBehavior",  " PITCH " + _toString((-0.0698 * (fabs(headpos)-1.57)) - 0.52), Logger::Info);
		hmot->set_parameter(1, (-0.0698 * (fabs(headpos)-1.57)) - 0.52);	//pitch
	}
	
	_blk->publishSignal(*hmot, "motion");
}
void HeadBehavior::headmotion(float pitch, float yaw){
	hmot->set_command("setHead");
	hmot->set_parameter(0, yaw);
	hmot->set_parameter(1, pitch);
	_blk->publishSignal(*hmot, "motion");
	
	
}


#include "Scan.h"


ACTIVITY_REGISTER(Scan);
int Scan::Execute()
{
	Logger::Instance().WriteMsg("Scan",  " execute", Logger::Info);
	hbm = _blk.readState<HeadToBMessage> ("behavior");
	msm = _blk.readState<MotionStateMessage> ("behavior");
	LedChangeMessage leds;
	LedValues* l = leds.add_leds();
	l->set_chain("r_ear");
	l->set_color( "off");
	_blk.publishSignal(leds, "leds");

	if( hbm.get() != 0 )
	{
		if( hbm->ballfound() > 0)
		{
			headaction = BALLTRACK;
		}
		else
		{
			//	if(msm.get()!=0 && ((msm->lastaction()).compare("InitPose.xar")!=0 || msm->detail().compare("walkTo")==0 || msm->detail().compare("setWalkTargetVelocity")==0)){
			amot->set_command("InitPose.xar");
			_blk.publishSignal(*amot, "motion");
			//}
			headaction = SCANFORBALL;
		}
	}

	bhmsg->set_headaction(headaction);
	_blk.publishSignal(*bhmsg, "behavior");
	_blk.publish_all();
	return 0;
}

void Scan::UserInit ()
{
	_blk.updateSubscription("behavior", msgentry::SUBSCRIBE_ON_TOPIC);
	_blk.updateSubscription("worldstate", msgentry::SUBSCRIBE_ON_TOPIC);
	headaction = SCANFORBALL;
	bhmsg = new BToHeadMessage();
	amot = new MotionActionMessage();
}

std::string Scan::GetName ()
{
	return "Scan";
}


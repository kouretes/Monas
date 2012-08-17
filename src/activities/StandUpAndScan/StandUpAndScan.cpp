
#include "StandUpAndScan.h"


ACTIVITY_REGISTER(StandUpAndScan);

int StandUpAndScan::Execute()
{
	Logger::Instance().WriteMsg(GetName(),  " execute", Logger::Info);
	hbm = _blk.readState<HeadToBMessage> ("behavior");
	msm = _blk.readState<MotionStateMessage>("worldstate");

	if( hbm.get() == 0 || hbm->ballfound() == 0)
	{
		headaction = SCANFORBALL;

		//	if(lastMove + boost::posix_time::seconds(4) <boost::posix_time::microsec_clock::universal_time()){
		//		lastMove = boost::posix_time::microsec_clock::universal_time();
		if(msm.get() != 0 && (msm->lastaction()).compare("StandUpX.xar") != 0)
		{
			Logger::Instance().WriteMsg(GetName(), msm->lastaction(), Logger::Info);
			Logger::Instance().WriteMsg(GetName(), _toString(msm->type()), Logger::Info);
			Logger::Instance().WriteMsg(GetName(),  " PUBLISH STAND", Logger::Info);
			amot->set_command("StandUpX.xar");
			_blk.publishSignal(*amot, "motion");
		}
	}
	else
		headaction = BALLTRACK;

	bhmsg->set_headaction(headaction);
	_blk.publishSignal(*bhmsg, "behavior");
	_blk.publish_all();
	return 0;
}
void StandUpAndScan::UserInit ()
{
	_blk.updateSubscription("behavior", msgentry::SUBSCRIBE_ON_TOPIC);
	_blk.updateSubscription("worldstate", msgentry::SUBSCRIBE_ON_TOPIC);
	headaction = SCANFORBALL;
	bhmsg = new BToHeadMessage();
	amot =  new MotionActionMessage();
	lastMove =  boost::posix_time::microsec_clock::universal_time();
}

std::string StandUpAndScan::GetName ()
{
	return "StandUpAndScan";
}


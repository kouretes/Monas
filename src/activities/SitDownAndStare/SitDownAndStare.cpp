
#include "SitDownAndStare.h"


ACTIVITY_REGISTER(SitDownAndStare);

int SitDownAndStare::Execute() {

	Logger::Instance().WriteMsg(GetName(),  " execute", Logger::Info);
	hbm = _blk.readState<HeadToBMessage> ("behavior");
		msm = _blk.readState<MotionStateMessage>("worldstate");
	if( hbm.get()!=0 ){
		if( hbm->ballfound()>0){
			headaction = BALLTRACK;
			//if(lastMove + boost::posix_time::seconds(4) <=boost::posix_time::microsec_clock::universal_time()){
			//	lastMove = boost::posix_time::microsec_clock::universal_time();
				if(msm.get()!=0 && (msm->lastaction()).compare("SitDownX.xar")!=0){
				amot->set_command("SitDownX.xar");
				_blk.publishSignal(*amot, "motion");
				Logger::Instance().WriteMsg(GetName(),  " PUBLISH SIT", Logger::Info);
			}
		}
		else{
			headaction = SCANFORBALL;
		}
	}
	else{
		headaction = SCANFORBALL;
	}


	bhmsg->set_headaction(headaction);
	_blk.publishSignal(*bhmsg, "behavior");

	_blk.publish_all();
	return 0;
}
void SitDownAndStare::UserInit () {
	_blk.updateSubscription("behavior", msgentry::SUBSCRIBE_ON_TOPIC);
	_blk.updateSubscription("worldstate", msgentry::SUBSCRIBE_ON_TOPIC);
	headaction = SCANFORBALL;
	bhmsg = new BToHeadMessage();
	amot=  new MotionActionMessage();

	lastMove =  boost::posix_time::microsec_clock::universal_time();
}
std::string SitDownAndStare::GetName () {
	return "SitDownAndStare";
}


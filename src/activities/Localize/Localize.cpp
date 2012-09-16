
#include "Localize.h"

ACTIVITY_REGISTER(Localize);

int Localize::Execute()
{
	Logger::Instance().WriteMsg(GetName(),  " Execute", Logger::Info);
	hbm = _blk.readState<HeadToBMessage>("behavior");

	if(boost::posix_time::microsec_clock::universal_time() - first > boost::posix_time::seconds(20))
	{
		Logger::Instance().WriteMsg(GetName(),  " CALIBRATE", Logger::Info);
		first = boost::posix_time::microsec_clock::universal_time();
		bhm->set_headaction(CALIBRATE);
		ld->set_moveon(false);
		_blk.publishState(*ld, "behavior");
		//amot->set_command("locScan.xar");
		//_blk.publishSignal(*amot, "motion");
		_blk.publishSignal(*locReset, "worldstate");
		_blk.publish_all();
		return 0;
	}

	if (boost::posix_time::microsec_clock::universal_time() - first > boost::posix_time::seconds(10) )
	{
		ld->set_moveon(true);
		_blk.publishState(*ld, "behavior");
		bhm->set_headaction(SCANFORPOST);
		Logger::Instance().WriteMsg(GetName(),  " MOVE ON", Logger::Info);
	}
	else if((hbm.get() != 0 && hbm->calibrated() != 2) && boost::posix_time::microsec_clock::universal_time() - first < boost::posix_time::seconds(3) )
	{
		ld->set_moveon(false);
		_blk.publishState(*ld, "behavior");
		bhm->set_headaction(DONOTHING);
		Logger::Instance().WriteMsg(GetName(),  " DONOTHING", Logger::Info);
	}
	else if ((hbm.get() != 0 && hbm->calibrated() == 2) && boost::posix_time::microsec_clock::universal_time() - first > boost::posix_time::seconds(3) && boost::posix_time::microsec_clock::universal_time() - first < boost::posix_time::seconds(10))
	{
		bhm->set_headaction(SCANFORPOST);
		Logger::Instance().WriteMsg(GetName(),  " SCANFORPOST", Logger::Info);
	}

	_blk.publishSignal(*bhm, "behavior");
	_blk.publish_all();
	return 0;
}
void Localize::UserInit ()
{
	_blk.updateSubscription("behavior", msgentry::SUBSCRIBE_ON_TOPIC);
	first = boost::posix_time::microsec_clock::universal_time() - boost::posix_time::hours(3);
	ld = new LocalizeDone();
	bhm = new BToHeadMessage();
	//amot = new MotionActionMessage();
	locReset = new LocalizationResetMessage();
	locReset->set_type(PLAYER_PENALISED);
}
std::string Localize::GetName ()
{
	return "Localize";
}


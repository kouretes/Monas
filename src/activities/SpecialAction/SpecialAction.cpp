
#include "SpecialAction.h"

namespace {
    ActivityRegistrar<SpecialAction>::Type temp("SpecialAction");
}

int SpecialAction::Execute() {
	
	Logger::Instance().WriteMsg("SpecialAction",  " execute" + to_simple_string(boost::posix_time::microsec_clock::universal_time()) , Logger::Info);
	obs = _blk->readData<DoubleObsInfo>("behavior");
	//Stare st;
	fm = _blk->readSignal<FallMessage>("behavior");
	//if (st.toFallOrNotToFall(obs) ==-1)
	if(fm!=0 && fm->fall()==1)
		amot->set_command("goalieLeftFootExtened.xar");
	else
		amot->set_command("goalieRightFootExtened.xar");
	_blk->publishSignal(*amot, "motion");
	
	bhm->set_headaction(BALLTRACK);
	_blk->publishSignal(*bhm, "behavior");
	
	sleep(2);
	return 0;
}

void SpecialAction::UserInit () {
	_blk->subscribeTo("behavior",0);
	amot = new MotionActionMessage();
	bhm = new BToHeadMessage();
}

std::string SpecialAction::GetName () {
	return "SpecialAction";
}
	


#include "SpecialAction.h"
#include "hal/robot/generic_nao/robot_consts.h"
#include "messages/Gamecontroller.pb.h"
namespace {
    ActivityRegistrar<SpecialAction>::Type temp("SpecialAction");
}

int SpecialAction::Execute() {
	
	Logger::Instance().WriteMsg("SpecialAction",  " execute" + to_simple_string(boost::posix_time::microsec_clock::universal_time()) , Logger::Info);
	obs = _blk->readData<DoubleObsInfo>("behavior");
	//Stare st;
	fm = _blk->readSignal<FallMessage>("behavior");
	LedChangeMessage leds;
	//if (st.toFallOrNotToFall(obs) ==-1)
	LedValues* l = leds.add_leds();

	if(fm!=0 && fm->fall()==1){
		l->set_chain("l_ear");
		l->set_color( "blue");
		amot->set_command("goalieLeftFootExtened.xar");
	}
	else{
		l->set_chain("r_ear");
		l->set_color( "blue");
		amot->set_command("goalieRightFootExtened.xar");
	}
	_blk->publishSignal(*amot, "motion");
	
	bhm->set_headaction(BALLTRACK);
	_blk->publishSignal(*bhm, "behavior");
	_blk->publishSignal(leds, "leds");
	//sleep(2);
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
	

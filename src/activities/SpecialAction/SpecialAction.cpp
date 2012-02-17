
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
		l->set_chain("r_ear");
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
	//rpm->set_goalietopos(true);
	//_blk->publishSignal(*rpm, "behavior");
	//sleep(2);
	_blk->publish_all();
	return 0;
}

void SpecialAction::UserInit () {
	_blk->updateSubscription("behavior", msgentry::SUBSCRIBE_ON_TOPIC);
	amot = new MotionActionMessage();
	bhm = new BToHeadMessage();
	rpm = new ReturnToPositionMessage();
}

std::string SpecialAction::GetName () {
	return "SpecialAction";
}


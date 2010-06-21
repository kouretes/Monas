#include "architecture/statechartEngine/ICondition.h"
#include "messages/timeout.pb.h"



class TrCond_0_2_4_2_2TO0_2_4_2_2 : public statechart_engine::ICondition {
  public:
    TrCond_0_2_4_2_2TO0_2_4_2_2() {
    }
    /* Motion_Timeout_Condition */
    void UserInit() {}

    bool Eval() {
      _blk->process_messages();
      TimeoutMsg* msg  = _blk->read_nb<TimeoutMsg>("TimeoutMsg","MotionControllerTimeout","localhost");
      //       cout<<"Msg: "<<msg<<endl;
      if ( msg == 0 )
        return true;
      std::string time = msg->wakeup();
      if ( time == "")
        return true;
//       std::cout<<"Condition time:"<<time<<std::endl;
      int timeout = (boost::posix_time::microsec_clock::local_time() - boost::posix_time::from_iso_string(time) ).total_microseconds();
//       std::cout<<"Condition timeout:"<<timeout<<std::endl;
      if ( timeout > 0)
        return true;
//       cout<<"Motion will FALSE!!!!!!!!!!"<<endl;
      return false;
    }
};



#include "architecture/statechartEngine/IAction.h"
#include "architecture/statechartEngine/TimoutAciton.h"

class TrAction_0_2_4_2_2TO0_2_4_2_2 : public statechart_engine::TimeoutAction {

  public:
    /* StartMotionTimeout */
    TrAction_0_2_4_2_2TO0_2_4_2_2() : statechart_engine::TimeoutAction("MotionControllerTimeout",100) {}

};

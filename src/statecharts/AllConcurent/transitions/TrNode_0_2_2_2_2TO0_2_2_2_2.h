
#include "architecture/statechartEngine/ICondition.h"

#include "architecture/statechartEngine/TimoutAciton.h"
using namespace statechart_engine;
class TrCond_0_2_2_2_2TO0_2_2_2_2 : public ICondition {
  public:
    TrCond_0_2_2_2_2TO0_2_2_2_2() {
      _var = "MotionControllerTimeout";
    }

    void UserInit() {}

    bool Eval() {
      TimeoutMsg* msg  = _blk->read_nb<TimeoutMsg>("TimeoutMsg",_var,"localhost");
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
      return false;
    }

  private:
    std::string _var;
};




#include "architecture/statechartEngine/IAction.h"

class TrAction_0_2_2_2_2TO0_2_2_2_2 : public statechart_engine::TimeoutAction {

public:

  TrAction_0_2_2_2_2TO0_2_2_2_2() : TimeoutAction("MotionControllerTimeout",100) {}

};


#include "architecture/statechartEngine/ICondition.h"

class TrCond_0_2_3_2_2_4_3_2_2_2_2TO0_2_3_2_2_4_3_2_2_2_2 : public statechart_engine::ICondition {
  public:
    TrCond_0_2_3_2_2_4_3_2_2_2_2TO0_2_3_2_2_4_3_2_2_2_2() : msg(0) {
    }

    void UserInit () {
      _com->get_message_queue()->add_subscriber(_blk);
      _com->get_message_queue()->subscribe("behavior",_blk,ON_TOPIC);
    }

    bool Eval() {
      _blk->process_messages();
      /* vision_loop_state_playing_VisionTimeout */
      GameStateMessage* newMsg = _blk->read_nb<GameStateMessage>("GameStateMessage","RobotController");
      if ( newMsg != 0 ) {
        delete msg;
        msg = newMsg;
      }
      return msg == 0 ? false
      : ( msg->player_state() == PLAYER_PLAYING && EvalTimeout() )  ? true : false;
    }

    bool EvalTimeout() {
      TimeoutMsg* msg  = _blk->read_nb<TimeoutMsg>("TimeoutMsg","VisionTimeout","localhost");
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
    GameStateMessage* msg;
};






#include "architecture/statechartEngine/IAction.h"
#include "architecture/statechartEngine/TimoutAciton.h"

class TrAction_0_2_3_2_2_4_3_2_2_2_2TO0_2_3_2_2_4_3_2_2_2_2 : public statechart_engine::TimeoutAction {

public:
  /* StartVisionTimeout */

  TrAction_0_2_3_2_2_4_3_2_2_2_2TO0_2_3_2_2_4_3_2_2_2_2() : statechart_engine::TimeoutAction("VisionTimeout",200) {}

};


#include "architecture/statechartEngine/ICondition.h"
#include "architecture/narukom/narukom_common.h"
#include "messages/Gamecontroller.pb.h"
#include "messages/RoboCupGameControlData.h"

class TrCond_0_2_3_2_1TO0_2_3_2_2 : public statechart_engine::ICondition {

  public:

    TrCond_0_2_3_2_1TO0_2_3_2_2() : msg(0) {
    }

    void UserInit () {
      cout<<"INIT OF play_start_state_not_finished"<<endl;
      _com->get_message_queue()->add_subscriber(_blk);
      _com->get_message_queue()->subscribe("behavior",_blk,ON_TOPIC);
    }

    bool Eval() {
      _blk->process_messages();
      /* play_start_state_not_finished */
      _blk->process_messages();
      GameStateMessage* newMsg = _blk->read_nb<GameStateMessage>("GameStateMessage","RobotController");
      if ( newMsg != 0 ) {
        delete msg;
        msg = newMsg;
      }
      bool ret_val = msg == 0 ? false
      :  msg->player_state() != PLAYER_FINISHED  ? true : false;
      cout<<"Eval: play_start gamestate not finished. Will return "<< ret_val<<"with msg=="<<msg<<endl;
      return msg == 0 ? false
      :  msg->player_state() != PLAYER_FINISHED  ? true : false;
    }

  private:

    GameStateMessage* msg;

};

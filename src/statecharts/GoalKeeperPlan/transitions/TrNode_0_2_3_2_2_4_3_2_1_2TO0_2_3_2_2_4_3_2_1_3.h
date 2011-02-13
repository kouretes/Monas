
#include "architecture/statechartEngine/ICondition.h"
#include "architecture/narukom/narukom_common.h"
#include "messages/Gamecontroller.pb.h"
#include "messages/RoboCupGameControlData.h"

class TrCond_0_2_3_2_2_4_3_2_1_2TO0_2_3_2_2_4_3_2_1_3 : public statechart_engine::ICondition {

public:

    TrCond_0_2_3_2_2_4_3_2_1_2TO0_2_3_2_2_4_3_2_1_3() : msg(0){
    }

    void UserInit () {
      _com->get_message_queue()->add_subscriber(_blk);
      _com->get_message_queue()->subscribe("behavior",_blk,ON_TOPIC);
    }

    bool Eval() {
      _blk->process_messages();
      /* state_not_playing */
      GameStateMessage* newMsg = _blk->read_nb<GameStateMessage>("GameStateMessage","RobotController");
      if ( newMsg != 0 ) {
        delete msg;
        msg = newMsg;
      }
      return msg == 0 ? false : msg->player_state() != PLAYER_PLAYING ? true : false;
    }

  private:

    GameStateMessage* msg;
};

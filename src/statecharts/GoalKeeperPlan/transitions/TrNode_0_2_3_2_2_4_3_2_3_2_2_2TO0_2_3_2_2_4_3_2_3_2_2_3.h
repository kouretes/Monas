
#include "architecture/statechartEngine/ICondition.h"
#include "architecture/narukom/narukom_common.h"
#include "messages/VisionObservations.pb.h"

class TrCond_0_2_3_2_2_4_3_2_3_2_2_2TO0_2_3_2_2_4_3_2_3_2_2_3 : public statechart_engine::ICondition {

  public:

    TrCond_0_2_3_2_2_4_3_2_3_2_2_2TO0_2_3_2_2_4_3_2_3_2_2_3() : _hasExecuted(false) {
    }

    void UserInit () {
      _com->get_message_queue()->add_subscriber(_blk);
      _com->get_message_queue()->subscribe("vision",_blk,ON_TOPIC);
    }

    bool Eval() {
      _blk->process_messages();
      /* to_search_ball_ball_not_found */
      BallTrackMessage* newMsg = _blk->read_nb<BallTrackMessage>("BallTrackMessage","Vision","localhost",&_new_time);
      if ( newMsg == 0 )
        return false;
      if ( ! _hasExecuted ){
        _old_time = _new_time;
        _hasExecuted = true;
        return newMsg->radius()<=0 ? true : false;
      }
      if ( _old_time <= _new_time )
        return false;
      _old_time = _new_time;
      return newMsg->radius()<=0 ? true : false;
    }

  private:

    bool _hasExecuted;
    boost::posix_time::ptime _old_time;
    boost::posix_time::ptime _new_time;
};


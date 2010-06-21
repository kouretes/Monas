
#include "architecture/statechartEngine/ICondition.h"
#include "architecture/narukom/narukom_common.h"
#include "messages/VisionObservations.pb.h"

#ifndef MONAD_TO_RAD
#define MONAD_TO_RAD 0.01745329f
#endif //MONAD_TO_RAD

class TrCond_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_2TO0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_3 : public statechart_engine::ICondition {

  public:

        void UserInit () {
          _com->get_message_queue()->add_subscriber(_blk);
          _com->get_message_queue()->subscribe("vision",_blk,ON_TOPIC);
        }

        bool Eval() {
          _blk->process_messages();
          /* to_right_fall */
          ObservationMessage* newMsg = _blk->read_nb<ObservationMessage>("ObservationMessage","Vision","localhost",&_new_time);
          if ( newMsg == 0 )
            return false;
          if ( ! _hasExecuted ){
            _old_time = _new_time;
            _hasExecuted = true;
            return newMsg->ball().bearing()<-80*MONAD_TO_RAD ? true : false;
          }
          if ( _old_time <= _new_time )
            return false;
          _old_time = _new_time;
          return newMsg->ball().bearing()<-80*MONAD_TO_RAD ? true : false;
        }


  private:
    bool _hasExecuted;
    boost::posix_time::ptime _old_time;
    boost::posix_time::ptime _new_time;

};
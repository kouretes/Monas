#include "TimoutAciton.h"
#include "Statechart.h"
#include "architecture/narukom/pub_sub/tuple.h"

#include "hal/syscall.h"


#include <boost/date_time/posix_time/posix_time.hpp>

namespace statechart_engine {

    TimeoutAction::TimeoutAction(std::string var, int timeout_ms)
      : _var(var), _timeout_ms(timeout_ms), _tpl(&_msg,"localhost",var,"timeout","",0x0fffffff) {
        ;
    }

    int TimeoutAction::Execute() {
      std::string time = boost::posix_time::to_iso_string(
            boost::posix_time::microsec_clock::local_time() +
           boost::posix_time::millisec(_timeout_ms));
      ((TimeoutMsg*)_tpl.get_msg_data())->set_wakeup(time);
//       cout<<"PRPRPRP "<<((TimeoutMsg*)_tpl.get_msg_data())->wakeup()<<endl;
      if ( ! _s->GetTimeoutThreadPool()->Enqueue( &_twork ) ) {
        throw "Timeout: can't enqueue timeout!";
      }
      return 0;
    }

    void TimeoutAction::UserInit() {
      _blk->add_tuple(&_tpl);
      _twork.Set ( _blk, _var, _s );
    }


    void TimeoutWorker::Set(Blackboard* blk, std::string& var, Statechart* s) {
      _blk = blk;
      _var = var;
      _s = s;
    }

    int TimeoutWorker::Execute() {
        TimeoutMsg* msg  = _blk->read_nb<TimeoutMsg>("TimeoutMsg",_var,"localhost");
        if ( msg == 0 )
          throw "TimeoutWorker msg null";
        std::string time = msg->wakeup();
        int timeout = (boost::posix_time::from_iso_string(time) - boost::posix_time::microsec_clock::local_time() ).total_microseconds();
//         cout<<"AAA: "<<time<<" timeout "<<timeout<<endl;
        if ( timeout > 0)
          SysCall::_usleep( timeout );
        _s->AtomicNotify();
//         cout<<"Statechart Notified!!!";
        return 0;
    }

}

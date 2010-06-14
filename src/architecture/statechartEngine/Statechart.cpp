#include "Statechart.h"

namespace statechart_engine {

    Statechart::Statechart ( Narukom* com )
    : OrState ( 0 ), _blackboard("StatechartBlakboard") { //FIXME
        _com = com;
        _blk = &_blackboard;
        _isRunning = new volatile int; //FIXME mem leak
        *_isRunning = 0;
    }

    Statechart::~Statechart () {
        ;
    }

    int Statechart::Activate () {

        _activeState = _startState;
        //_isRunning = true; //FIXME
        _isActive = true;

        return 0;
    }

    ThreadPool* Statechart::GetThreadPool () {
        return &_tp;
    }

    ThreadPool* Statechart::GetTimeoutThreadPool () {
      return &_timeoutpool;
    }

    void Statechart::Start () {
        Activate();
        StartThread();
    }

    void Statechart::Stop () {
        StopThread();
    }

    int Statechart::Execute () {
        while ( Step() )
          ;
        {
          boost::unique_lock<boost::mutex> lock(mut);
          if ( notified ) {
            notified = false;
            return 0;
          }
          cond.wait(lock);
        }
        return 0;
    }

    void Statechart::AtomicNotify()  {
      {
        boost::lock_guard<boost::mutex> lock(mut);
        notified = true;
      }
      cond.notify_all();
    }


}

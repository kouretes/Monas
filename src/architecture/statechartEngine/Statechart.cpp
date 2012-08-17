#include "Statechart.h"

namespace statechart_engine {

    Statechart::Statechart ( std::string name, Narukom* com )
    : OrState ( name, 0 ), _blackboard("StatechartBlakboard"), notified(false) { //FIXME
        _com = com;
        _blk = &_blackboard;
        _xml = &_xmlnode;
        _isRunning = new volatile int; //FIXME mem leak
        *_isRunning = 0;
		_blk->attachTo(*_com->get_message_queue());
		
		#ifdef RUN_ON_NAO
		_xmlnode = XmlNode(ArchConfig::Instance().GetConfigPrefix(),KRobotConfig::Instance().getConfig(KDeviceLists::Interpret::HEAD_ID)
															,KRobotConfig::Instance().getConfig(KDeviceLists::Interpret::BODY_ID));
		#else
		_xmlnode = XmlNode(ArchConfig::Instance().GetConfigPrefix(),"hi","bi");
		#endif
		_xmlnode.print("");
    }

    Statechart::~Statechart () {
        usleep(1000000);
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
        while ( Step() ) ;
        notified = false;
		{
          boost::unique_lock<boost::mutex> lock(mut);
          if ( notified ) {
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

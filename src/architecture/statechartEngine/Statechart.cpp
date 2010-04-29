#include "Statechart.h"

namespace statechart_engine {

    Statechart::Statechart ()
    : OrState ( 0 ), _blackboard("StatechartBlakboard") {
        _com = &_narukom;
        _blk = &_blackboard;
        _com->get_message_queue()->StartThread();
        // TODO Auto-generated constructor stub

    }

    Statechart::~Statechart () {
        _com->get_message_queue()->StopThread();
        usleep(200000); //TODO
//        std::cout<<"Vagvaz!"<<std::endl;// TODO
    }

    int Statechart::Activate () {

        _activeState = _startState;
        _isRunning = true;
        _isActive = true;

        return 0;
    }

    ThreadPool* Statechart::GetThreadPool () {
        return &_tp;
    }

}

#include "Statechart.h"

namespace statechart_engine {

    Statechart::Statechart ()
    : OrState ( 0 ), _blackboard("StatechartBlakboard") { //FIXME
        _com = &_narukom;
        _blk = &_blackboard;
        _com->get_message_queue()->StartThread(); //FIXME
	_isRunning = new volatile int; //FIXME mem leak
	*_isRunning = 0;
    }

    Statechart::~Statechart () {
        _com->get_message_queue()->StopThread();
        usleep(200000); //TODO
//        std::cout<<"Vagvaz!"<<std::endl;// TODO
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

}

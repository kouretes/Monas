#include "ActivityWrapper.h"

namespace statechart_engine
{

	ActivityWrapper::ActivityWrapper ()
		: _act(0), _running(0), _s(0)
	{
		;
	}

	void ActivityWrapper::Init(IActivity* act, volatile int* running, statechart_engine::Statechart* s)
	{
		_act = act;
		_running = running;
		_s = s;
	}

	int ActivityWrapper::Execute()
	{
		int ret = _act->Execute();
		*_running = 0;
		_s->AtomicNotify();
		return ret;
	}


}


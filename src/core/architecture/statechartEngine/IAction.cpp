#include "IAction.h"

namespace statechart_engine
{

	void IAction::Initialize ( MessageHub* com, Blackboard* blk, Statechart* s)
	{
		_com = com;
		_blk = blk;
		_s = s;
		this->UserInit();
	}

}

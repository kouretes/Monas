#include "ICondition.h"

namespace statechart_engine
{

	void ICondition::Initialize ( MessageHub* com, Blackboard* blk)
	{
		_com = com;
		_blk = blk;
		this->UserInit();
	}

}

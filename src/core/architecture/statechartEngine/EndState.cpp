#include "EndState.h"

namespace statechart_engine
{

	EndState::EndState ( std::string name, statechart_engine::State* parent )
		: State ( name, parent )
	{
		;
	}

	EndState::~EndState ()
	{
		// TODO Auto-generated destructor stub
	}

	bool EndState::Step ( IEvent* ev, IParameter* param )
	{
		return false;
	}

}

#include "StartState.h"
#include "OrState.h"

namespace statechart_engine
{

	StartState::StartState ( std::string name, statechart_engine::State* parent )
		: State ( name, parent )
	{
		OrState * parentState = dynamic_cast<OrState*> ( parent );
		//TODO if throw;
		parentState->SetStartState(this);
	}

	StartState::~StartState ()
	{
		// TODO Auto-generated destructor stub
	}

}

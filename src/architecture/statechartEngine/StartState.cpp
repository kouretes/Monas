#include "StartState.h"
#include "OrState.h"

namespace statechart_engine {

    StartState::StartState ( State* parent )
    : State ( parent ) {
        OrState * parentState = dynamic_cast<OrState*> ( parent );
        //TODO if throw;
        parentState->SetStartState(this);
    }

    StartState::~StartState () {
        // TODO Auto-generated destructor stub
    }

}

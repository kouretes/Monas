#include "EndState.h"

namespace statechart_engine {

    EndState::EndState ( State* parent )
    : State ( parent ) {
        ;
    }

    EndState::~EndState () {
        // TODO Auto-generated destructor stub
    }

    bool EndState::Step ( IEvent* ev, IParameter* param ) {
        return false;
    }

}

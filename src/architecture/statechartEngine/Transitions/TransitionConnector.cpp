#include "TransitionConnector.h"

namespace statechart_engine {

    TransitionConnector::TransitionConnector (State* parent)
    : State(parent)
    {
    }

    TransitionConnector::~TransitionConnector () {
        // TODO Auto-generated destructor stub
    }

    int TransitionConnector::Activate () {
        return 0;
    }

    int TransitionConnector::DeActivate () {
        return 0;
    }

    bool TransitionConnector::CanStep ( IEvent* ev ) const {
        for ( Containers::TransitionContainerIteratorConst i = _transitions.begin(); i != _transitions.end(); i++)
            if ( (*i)->CanExecute(ev) )
                return true;
        return false;

    }

}

#include "ConditionConnector.h"

namespace statechart_engine {

    ConditionConnector::ConditionConnector ( State* parent ) :
        TransitionConnector ( parent ) {
    }

    ConditionConnector::~ConditionConnector () {
        // TODO Auto-generated destructor stub
    }

    bool ConditionConnector::CanStep ( IEvent* ev,
            Containers::TransitionContainerIteratorConst& index ) {
        for ( Containers::TransitionContainerIteratorConst i =
                _transitions.begin (); i != _transitions.end (); i++ )
            if ( ( *i )->CanExecute ( ev ) ) {
                index = i;
                return true;
            }
        return false;
    }

    bool ConditionConnector::Step ( IEvent*ev, IParameter*param,
            Containers::TransitionContainerIteratorConst&index ) {
        if ( ( *index )->Execute ( ev, param ) ) //TODO should execute the action without a check
            return true;
        return false;
    }
}

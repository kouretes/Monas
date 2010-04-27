#include "BasicState.h"

#include "architecture/statechartEngine/Statechart.h"

namespace statechart_engine {

    BasicState::BasicState ( State* parent, IAction* entryAction, IAction* exitAction )
    : State(parent, entryAction, exitAction),
      _stateActivity(0) {
        ;
    }

    BasicState::BasicState ( State* parent, IActivity* activity, IAction* entryAction, IAction* exitAction )
    : State(parent, entryAction, exitAction),
      _stateActivity(activity) {

        State* p = parent;
        while ( p->GetParent() != 0 )
            p = p->GetParent();

        Statechart* stc = dynamic_cast<Statechart*>(p);
        if ( !stc )
            throw "Aaaaaaaaaaa"; //TODO

        _tp = stc->GetThreadPool();

        if ( _stateActivity )
            _stateActivity->Initialize ( _com, _blk );

    }

    int BasicState::Activate() {

        State::Activate();

        if ( _stateActivity ) {
            if ( !_tp->Enqueue( _stateActivity ) ) {
                throw "BasicState: can't enqueue activity!";
            }
        }
//            _stateActivity->Execute(); //TODO
        return 0;
    }

    BasicState::~BasicState () {
        // TODO Auto-generated destructor stub
    }



}

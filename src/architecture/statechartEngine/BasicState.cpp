#include "BasicState.h"

namespace statechart_engine {

    BasicState::BasicState ( std::string name, State* parent, IAction* entryAction, IAction* exitAction )
    : State(name, parent, entryAction, exitAction),
      _stateActivity(0), _st(0) {
        ;
    }

    BasicState::BasicState ( std::string name, statechart_engine::State* parent, IActivity* activity, statechart_engine::IAction* entryAction, statechart_engine::IAction* exitAction )
    : State(name, parent, entryAction, exitAction),
      _stateActivity(activity) {

        State* p = parent;
        while ( p->GetParent() != 0 )
            p = p->GetParent();

        Statechart* stc = dynamic_cast<Statechart*>(p);
        if ( !stc )
            throw "Aaaaaaaaaaa"; //TODO

        _st = stc;

        if ( _stateActivity ) {
            _actWrap.Init(_stateActivity,_isRunning,_st);
        }

    }

    int BasicState::Activate() {

        State::Activate();

        if ( _stateActivity ) {
             *_isRunning = 1;
            if ( !_st->GetThreadPool()->Enqueue( &_actWrap ) ) {
                *_isRunning = 0;
                throw "BasicState: can't enqueue activity!";
            }
        }
        return 0;
    }

    BasicState::~BasicState () {
        // TODO Auto-generated destructor stub
    }

}

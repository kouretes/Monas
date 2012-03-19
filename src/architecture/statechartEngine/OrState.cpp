#include "OrState.h"

namespace statechart_engine {

    OrState::OrState ( std::string name, statechart_engine::State* parent, statechart_engine::IAction* entryAction, statechart_engine::IAction* exitAction )
    : State ( name, parent, entryAction, exitAction ),
      _activeState(0),
      _startState(0) {
        ;
      }

    OrState::~OrState () {
        // TODO Auto-generated destructor stub
    }

    int OrState::Activate () {
        _activeState = _startState;
        return State::Activate();
    }

    int OrState::DeActivate () {
        _activeState->DeActivate();
        return State::DeActivate();
    }

    bool OrState::Step ( IEvent* ev, IParameter* param ) {
        if ( *_isRunning )
          return false;
        bool r= _activeState->Step(ev,param);

        if( *_isRunning )
            return r;

        return r ? true : State::Step(ev,param);
    }

    void OrState::SetActive ( State* aState) {
        _activeState = aState;
    }

    void OrState::SetStartState( State* sState ) {
        _startState = sState;
    }

    Blackboard* OrState::AddChild ( State* ) {
        return _blk;//TODO
    }

}

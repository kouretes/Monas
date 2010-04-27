#include "OrState.h"

namespace statechart_engine {

    OrState::OrState ( State* parent, IAction* entryAction, IAction* exitAction )
    : State ( parent, entryAction, exitAction ),
      _activeState(0) {
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
        return _activeState->Step(ev,param) ? true : State::Step(ev,param);
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

#include "AndState.h"

namespace statechart_engine {


    AndState::AndState ( State* parent, IAction* entryAction, IAction* exitAction )
    : State(parent, entryAction, exitAction) {
        ;
    }

    AndState::~AndState () {
        // TODO Auto-generated destructor stub
    }

    int AndState::Activate () {
	State::Activate();
        for ( OrNodeContIter it = _subStates.begin(); it != _subStates.end(); it++ )
            (*it)->Activate();
        return 0;
    }

    int AndState::DeActivate () {
        for ( OrNodeContIter it = _subStates.begin(); it != _subStates.end(); it++ )
            (*it)->DeActivate();
        return State::DeActivate();
    }

    bool AndState::Step ( IEvent* ev = 0, IParameter* param = 0) {
        bool stepTaken = false;
        for ( OrNodeContIter it = _subStates.begin(); it != _subStates.end(); it++ ) {
            if ( (*it)->Step(ev,param) )
                stepTaken = true;
        }
        return stepTaken ? true : isRunning() ? false : State::Step(ev,param);
    }

    Blackboard* AndState::AddChild ( State* subState) {
	cout<<"AndState: Producing blackboard!"<<endl; //TODO
        _subStates.push_back ( static_cast<OrState*>(subState) ); //FIXME dynamic_cast is not working!
        char s[]="aa"; //FIXME get name 
        return new Blackboard(s); //FIXME mem leak
    }

    bool AndState::isRunning () const {
        for ( OrNodeContIterConst it = _subStates.begin(); it != _subStates.end(); it++ ) {
            if ( (*it)->isRunning() )
                return true;
        }
        return false;
    }

    Blackboard* AndState::GetBlackboard () const {
        return State::GetBlackboard();
    }
    
    volatile int* AndState::GetIsRunningRef () const {
      cout<<"AndState: Producing int!"<<endl; //TODO
      volatile int* ret = new volatile int; //FIXME mem leak
      *ret = 0;
      return ret;
    }

}

#include "State.h"

#include <iostream>
#include <typeinfo>
//TODO

namespace statechart_engine {

    State::State (State* parent, IAction* entryAction, IAction* exitAction )
    : _entryAction(entryAction),
      _exitAction(exitAction),
      _parent(parent),
      _isActive(false),
      _isRunning(0)  {

        if ( _parent ) {

            _blk = _parent->AddChild ( this );
//            _blk = _parent->GetBlackboard ();//TODO const

	    _isRunning = _parent->GetIsRunningRef();
	    *_isRunning = 0;

            _com = _parent->GetCom();

            if ( _entryAction )
                _entryAction->Initialize ( _com, _blk );

            if ( _exitAction )
                _exitAction->Initialize ( _com, _blk );

        }
      }

    State::~State () {
        _entryAction = 0;
        _exitAction = 0;
        _parent = 0;
        _isActive = false;
    }

    bool State::isActive() const{
        return _isActive;
    }

    bool State::isRunning() const{
        return *_isRunning;
    }

    int State::Activate() {
        if ( _parent )
            _parent->SetActive(this);
//      _isRunning = true; //TODO
        _isActive = true;
        if ( _entryAction )
            _entryAction->Execute();
        return 0;
    }

    int State::DeActivate() {
        _isActive = false;
        if ( _exitAction )
            _exitAction->Execute();
        return 0;
    }

    bool State::Step ( IEvent* ev, IParameter* param ) {
        if ( *_isRunning ) {
            return false;
	}
        for ( TransitionsContIter it = _transitions.begin(); it != _transitions.end(); it++ ) {
            if ( (*it)->Execute(ev,param) )
                return true;
        }
        return false;
    }

    int State::AddTransition ( TransitionSegmentBase* transition ) {
        //TODO
//        std::cout<<"Adding Transition "<<transition<<" of type "<<typeid(transition).name()<<" to state "<<this<<" of type "<<typeid(this).name()<<std::endl;
        _transitions.push_back(transition);
        return 0;
    }

    void State::SetActive( State* aState ) {
        return;
    }

    State* State::GetParent () const {
        return _parent;
    }


    Blackboard* State::AddChild ( State* aState ) {
        //TODO
        //throw "you can't add a child node to this node";
        throw "up";
        return _blk;
    }

    Narukom* State::GetCom () const {
        return _com;
    }

    Blackboard* State::GetBlackboard () const { //TODO
        return _blk;
    }
    
    volatile int* State::GetIsRunningRef () const {
      return _isRunning;
    }

}

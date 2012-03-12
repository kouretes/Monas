#ifndef TRANSITIONSEGMENT_H_
#define TRANSITIONSEGMENT_H_ 1

#include "TransitionSegmentBase.h"

#include "../State.h"
#include "../IAction.h"
#include "../ICondition.h"
#include "../IEvent.h"
#include "../IParameter.h"

#include "ConditionConnector.h"

#include "architecture/narukom/narukom.h"
#include "architecture/narukom/pub_sub/blackboard.h"

#include "architecture/statechartEngine/Statechart.h"

#include "../Containers.h"
#include "tools/type2Type.h"

//TODO
#include <iostream>
#include <typeinfo>

namespace statechart_engine {

    class State;

    template< class TSource, class TTarget >
    class TransitionSegment : public statechart_engine::TransitionSegmentBase {

        public:

        TransitionSegment (TSource* source, TTarget* target, IEvent* event = 0 , ICondition* condition = 0 , IAction* action = 0 );
        TransitionSegment (TSource* source, TTarget* target, IAction* action);
        TransitionSegment (TSource* source, TTarget* target, IEvent* event, IAction* action);
        TransitionSegment (TSource* source, TTarget* target, ICondition* condition, IAction* action = 0 );

        virtual ~TransitionSegment ();

        bool CanExecute ( IEvent* ev ) const;
		bool has_Condition() const;
        bool Execute( IEvent* ev = 0, IParameter* param = 0);

        private:

        void Initialize();

        inline bool Execute( Type2Type<TSource> src, Type2Type<State> trg, IEvent* ev = 0, IParameter* param = 0);
        inline bool Execute( Type2Type<TSource> src, Type2Type<ConditionConnector> trg, IEvent* ev = 0, IParameter* param = 0);

        TSource* _source;
        TTarget* _target;

        IEvent* _event;
        ICondition* _condition;
        IAction* _action;

        Narukom* _com;
        Blackboard* _blk;

        typedef Containers::StateContainer StateCont;
        typedef Containers::StateContainerIterator StateContIter;

        StateCont _activateList;
        StateCont _deActivateList;

    };

    template< class TSource, class TTarget >
    TransitionSegment<TSource,TTarget>::TransitionSegment (TSource* source, TTarget* target, IAction* action)
    :_source(source),
     _target(target),
     _event(0),
     _condition(0),
     _action(action)
     {
            Initialize();
     }

    template< class TSource, class TTarget >
    TransitionSegment<TSource,TTarget>::TransitionSegment (TSource* source, TTarget* target, IEvent* event, IAction* action)
    :_source(source),
     _target(target),
     _event(event),
     _condition(0),
     _action(action)
     {
            Initialize();
     }

    template< class TSource, class TTarget >
    TransitionSegment<TSource,TTarget>::TransitionSegment (TSource* source, TTarget* target, ICondition* condition, IAction* action)
    :_source(source),
     _target(target),
     _event(0),
     _condition(condition),
     _action(action)
     {
            Initialize();
     }

    template< class TSource, class TTarget >
    TransitionSegment<TSource,TTarget>::TransitionSegment (TSource* source, TTarget* target, IEvent* event, ICondition* condition, IAction* action)
    :_source(source),
     _target(target),
     _event(event),
     _condition(condition),
     _action(action)
     {
            Initialize();
     }

    template< class TSource, class TTarget >
    TransitionSegment<TSource,TTarget>::~TransitionSegment ()
    {
            _source = 0;
            _target = 0;
            _event = 0;
            _condition = 0;
            _action = 0;
    }

    template< class TSource, class TTarget>
    bool TransitionSegment<TSource,TTarget>::CanExecute ( IEvent* ev ) const {

            if ( _event ) {
                if ( !ev || typeid(_event) != typeid(ev) )
                    return false;
            }
            else {
                if ( ev )
                    return false;
            }

            if ( _condition && !_condition->Eval() )
                return false;

            return true;
    }

 template< class TSource, class TTarget>
    bool TransitionSegment<TSource,TTarget>::has_Condition () const {

            if ( _condition ) {
                return true;
            }
           
            return false;
    }

    template< class TSource, class TTarget >
    void TransitionSegment<TSource,TTarget>::Initialize () {

            _source->AddTransition(this);

            StateCont srcAncesors;

            State* parent = _source->GetParent();
            srcAncesors.push_back(parent);

            Statechart* s = 0;

            while ( parent != 0 ) {
                if ( parent->GetParent() == 0 ) {
                  s = dynamic_cast<Statechart*>(parent);
                  if ( s == 0 )
                    throw "TRS: Can't find statechart";
                }
                parent=parent->GetParent();
                srcAncesors.push_back(parent);
            }

            StateCont trgAncesors;

            parent = _target->GetParent();
            trgAncesors.push_front(parent);

            while ( parent != 0 ) {
                parent=parent->GetParent();
                trgAncesors.push_front(parent);
            }


            for ( StateContIter srcIter = srcAncesors.begin(); srcIter != srcAncesors.end(); srcIter++ ) {

                for ( StateContIter trgIter = trgAncesors.begin(); trgIter != trgAncesors.end(); trgIter++ ) {
                    if ( (*srcIter) == (*trgIter) ) {
                        _deActivateList.assign( srcAncesors.begin(), srcIter );
                        _activateList.assign( ++trgIter, trgAncesors.end() );
                        _com = (*srcIter)->GetCom();
                        _blk = (*srcIter)->GetBlackboard();
                        if ( _action )
                            _action->Initialize ( _com, _blk, s );
                        if ( _condition )
                            _condition->Initialize ( _com, _blk );
//                        for ( StateContIter i = _deActivateList.begin(); i!=_deActivateList.end(); i++)
//                            std::cout<<"Deactivate: "<<(*i)<<std::endl;//TODO
//                        for ( StateContIter i = _activateList.begin(); i!=_activateList.end(); i++)
//                            std::cout<<"Activate: "<<(*i)<<std::endl;//TODO
                        return;
                    }
                }

            }

            std::cout<<"Nooooooooooo!!!"<<std::cout; //TODO throw
    }

    template< class TSource, class TTarget>
    bool TransitionSegment<TSource,TTarget>::Execute(IEvent* ev, IParameter* param) {
            return Execute ( Type2Type<TSource>(), Type2Type<TTarget>(), ev, param );
    }

    template< class TSource, class TTarget>
    bool TransitionSegment<TSource,TTarget>::Execute ( Type2Type<TSource>, Type2Type<State>, IEvent* ev, IParameter* param ) {

            if ( !CanExecute(ev) ){
                return false;
			}

            _source->DeActivate();

            for ( StateContIter i = _deActivateList.begin(); i != _deActivateList.end(); i++)
                (*i)->DeActivate();

            if ( _action )
                _action->Execute();

            for ( StateContIter i = _activateList.begin(); i != _activateList.end(); i++)
                (*i)->Activate();

            _target->Activate();

//            std::cout<<"TR:Hello from"<<typeid(_target).name()<<std::endl;//TODO

            return true;
    }

    template< class TSource, class TTarget>
    bool TransitionSegment<TSource,TTarget>::Execute ( Type2Type<TSource>, Type2Type<ConditionConnector>, IEvent* ev, IParameter* param ) {

            if ( !CanExecute(ev) )
                return false;

            Containers::TransitionContainerIteratorConst index;

            if ( !_target->CanStep(ev,index) )
                return false;

            _source->DeActivate();

            if ( _action )
                _action->Execute();

            _target->Step(ev,param,index);

//            std::cout<<"TR:Hello from"<<typeid(_target).name()<<std::endl;//TODO

            return true;
    }

}

#endif /* TRANSITIONSEGMENT_H_ */

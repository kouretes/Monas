#ifndef ANDSTATE_H_
#define ANDSTATE_H_ 1

#include "State.h"
#include "IAction.h"
#include "IEvent.h"
#include "IParameter.h"
#include "Transitions/TransitionSegment.h"
#include "OrState.h"

#include "Containers.h"


namespace statechart_engine {

    class AndState: public State {

        public:

            AndState ( std::string name, State* parent, IAction* entryAction = 0, IAction* exitAction = 0 );

            virtual ~AndState ();

            int Activate ();
            int DeActivate ();

            bool Step ( IEvent* ev , IParameter* param );

            Blackboard* AddChild ( State* );

            bool isRunning () const;

            Blackboard* GetBlackboard () const;

	    volatile int* GetIsRunningRef () const;

        private:

            typedef Containers::OrStateContainer OrNodeCont;
            typedef Containers::OrStateContainerIterator OrNodeContIter;
            typedef Containers::OrStateContainerIteratorConst OrNodeContIterConst;

            OrNodeCont _subStates;

    };

}

#endif /* ANDSTATE_H_ */

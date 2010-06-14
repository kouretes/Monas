#ifndef BASICSTATE_H_
#define BASICSTATE_H_ 1

#include "State.h"
#include "IAction.h"

#include "architecture/IActivity.h"
#include "ActivityWrapper.h"

#include "architecture/statechartEngine/Statechart.h"

namespace statechart_engine {

    class BasicState: public State {

        public:

            BasicState ( State* parent, IAction* entryAction = 0, IAction* exitAction = 0 );
            BasicState ( State* parent, IActivity* activity, IAction* entryAction = 0, IAction* exitAction = 0 );

            virtual ~BasicState ();

            int Activate ();

        private:

            IActivity* _stateActivity;

            ActivityWrapper _actWrap;

            Statechart* _st;
    };

}

#endif /* BASICSTATE_H_ */

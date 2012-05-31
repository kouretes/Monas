#ifndef BASICSTATE_H_
#define BASICSTATE_H_ 1

#include "State.h"
#include "IAction.h"

#include "architecture/executables/IActivity.h"
#include "ActivityWrapper.h"

#include "architecture/statechartEngine/Statechart.h"

namespace statechart_engine {

    class BasicState: public State {

        public:

            BasicState ( std::string name, State* parent, IAction* entryAction = 0, IAction* exitAction = 0 );
            BasicState ( std::string name, State* parent, IActivity* activity, IAction* entryAction = 0, IAction* exitAction = 0 );

            virtual ~BasicState ();

            int Activate ();

        private:

            IActivity* _stateActivity;

            ActivityWrapper _actWrap;

            Statechart* _st;
    };

}

#endif /* BASICSTATE_H_ */

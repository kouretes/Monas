#ifndef BASICSTATE_H_
#define BASICSTATE_H_ 1

#include "State.h"
#include "IAction.h"
#include "IActivity.h"

#include "hal/threadPool.h"

namespace statechart_engine {

    class BasicState: public State {

        public:

            BasicState ( State* parent, IAction* entryAction = 0, IAction* exitAction = 0 );
            BasicState ( State* parent, IActivity* activity, IAction* entryAction = 0, IAction* exitAction = 0 );

            virtual ~BasicState ();

            int Activate ();

        private:

            IActivity* _stateActivity;

            ThreadPool* _tp;  //TODO namespace fix
    };

}

#endif /* BASICSTATE_H_ */

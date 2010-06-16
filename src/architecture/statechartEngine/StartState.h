#ifndef STARTSTATE_H_
#define STARTSTATE_H_ 1

#include "State.h"

namespace statechart_engine {

    class StartState: public State {

        public:

            StartState ( std::string name, State* parent );

            virtual ~StartState ();
    };

}

#endif /* STARTSTATE_H_ */

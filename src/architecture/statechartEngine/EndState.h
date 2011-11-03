#ifndef ENDSTATE_H_
#define ENDSTATE_H_ 1

#include "State.h"

namespace statechart_engine {

    class EndState: public State {

        public:

            EndState ( std::string name, State* parent );

            virtual ~EndState ();

            bool Step ( IEvent* ev = 0, IParameter* param = 0);
    };

}

#endif /* ENDSTATE_H_ */

#ifndef TRANSITIONCONNECTOR_H_
#define TRANSITIONCONNECTOR_H_ 1

#include "../State.h"

namespace statechart_engine {

    class TransitionConnector : public State {

        public:

            TransitionConnector (State* parent);
            virtual ~TransitionConnector ();

            int Activate ();

            int DeActivate ();

            virtual bool CanStep ( IEvent* ev) const;
    };

}

#endif /* TRANSITIONCONNECTOR_H_ */

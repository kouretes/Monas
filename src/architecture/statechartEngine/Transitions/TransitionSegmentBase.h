#ifndef TRANSITIONSEGMENTBASE_H_
#define TRANSITIONSEGMENTBASE_H_ 1

#include "../IEvent.h"
#include "../IParameter.h"

namespace statechart_engine {

    class TransitionSegmentBase {
        public:
            TransitionSegmentBase () { }
            virtual ~TransitionSegmentBase () { }

            virtual bool Execute( IEvent* ev, IParameter* param) = 0;
            virtual bool CanExecute (IEvent* ev) const = 0;
    };

}

#endif /* TRANSITIONSEGMENTBASE_H_ */

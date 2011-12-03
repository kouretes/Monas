#ifndef ICONDITION_H_
#define ICONDITION_H_ 1

#include "architecture/narukom/narukom.h"
#include "architecture/narukom/pub_sub/blackboard.h"

namespace statechart_engine {

    class ICondition {

        public:

            virtual bool Eval() = 0;

            void Initialize ( Narukom*, Blackboard* );

            virtual void UserInit ()=0;

        protected:

            Narukom* _com;

            Blackboard* _blk;
    };

}

#endif /* ICONDITION_H_ */

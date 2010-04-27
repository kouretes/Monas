#ifndef IACTION_H_
#define IACTION_H_ 1

#include "architecture/narukom/narukom.h"
#include "architecture/narukom/pub_sub/blackboard.h"

#include "architecture/IExecutable.h"

namespace statechart_engine {

    class IAction: public IExecutable {

        public:

            void Initialize ( Narukom*, Blackboard* );

            virtual void UserInit ()=0;

        protected:

            Narukom* _com;

            Blackboard* _blk;

    };

}

#endif /* IACTION_H_ */

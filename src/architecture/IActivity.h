#ifndef IACTIVITY_H_
#define IACTIVITY_H_ 1

#include "architecture/IExecutable.h"

#include "architecture/narukom/narukom.h"
#include "architecture/narukom/pub_sub/blackboard.h"


class IActivity : public IExecutable {

    public:

        void Initialize ( Narukom*, Blackboard* );

        virtual void UserInit ()=0;

    protected:

        Narukom* _com;

        Blackboard* _blk;

};


#endif /* IACTIVITY_H_ */

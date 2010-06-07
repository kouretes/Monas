#ifndef _IACTIVITY_H_
#define _IACTIVITY_H_ 1

#include "architecture/IExecutable.h"

#include "architecture/narukom/narukom.h"
#include "architecture/narukom/pub_sub/blackboard.h"

#include "tools/genFactory.h"
#include "tools/genRegistrar.h"

#include <string>

class IActivity : public IExecutable {

    public:

        void Initialize ( Narukom*, Blackboard* );
	
	int ExecuteWrapper ( volatile int* running );

        virtual void UserInit ()=0;

        virtual std::string GetName ()=0;

    protected:

        Narukom* _com;

        Blackboard* _blk;

};

typedef GenericFactory < IActivity, std::string >  ActivityFactory;

template<class T>
struct ActivityRegistrar {
    typedef Registrar<ActivityFactory,IActivity,std::string,T> Type;
};

#endif /* IACTIVITY_H_ */

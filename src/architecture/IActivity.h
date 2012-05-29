#ifndef _IACTIVITY_H_
#define _IACTIVITY_H_ 1

#include "architecture/IExecutable.h"


#include "architecture/narukom/narukom.h"
#include "architecture/narukom/pub_sub/blackboard.h"

#include "tools/genFactory.h"
#include "tools/genRegistrar.h"

#include <string>
class ActivityBindings;
class IActivity : public IExecutable {

    public:
        IActivity  ( Blackboard & );
        virtual ~IActivity();

        virtual void UserInit ()=0;

        virtual std::string GetName ()=0;

    protected:

        Blackboard &  _blk;

};

typedef GenericFactory < IActivity, std::string ,IActivity* (*)(Blackboard&),Blackboard&>  ActivityFactory;

template<class T>
struct ActivityRegistrar {
    typedef Registrar<ActivityFactory,IActivity,std::string,T,Blackboard&> Type;
};


#ifdef __GNUC__
#define ACTIVITY_START _Pragma("GCC visibility push(hidden)" )
#define ACTIVITY_END _Pragma("GCC visibility pop")
#define ACTIVITY_VISIBLE __attribute__ ((visibility("default")))
#else
#define ACTIVITY_START
#define ACTIVITY_END
#define ACTIVITY_VISIBLE
#endif

#define ACTIVITY_CONSTRUCTOR(x)  x(Blackboard&b):IActivity(b){   };

#define ACTIVITY_REGISTER(x) namespace { 	ActivityRegistrar<x>::Type temp##x(#x);  };


#endif /* IACTIVITY_H_ */

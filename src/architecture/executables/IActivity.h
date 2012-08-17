#ifndef _IACTIVITY_H_
#define _IACTIVITY_H_ 1

#include "architecture/IExecutable.h"

#include "architecture/narukom/narukom.h"
#include "architecture/narukom/pub_sub/blackboard.h"

#include "architecture/XmlManager/XmlConfigurator.h"

#include "tools/genFactory.h"
#include "tools/genRegistrar.h"

#include <string>


#ifdef __GNUC__
#define ACTIVITY_START _Pragma("GCC visibility push(hidden)" )
#define ACTIVITY_END _Pragma("GCC visibility pop")
#define ACTIVITY_VISIBLE __attribute__ ((visibility("default")))
#else
#define ACTIVITY_START
#define ACTIVITY_END
#define ACTIVITY_VISIBLE
#endif


#define ACTIVITY_CONSTRUCTOR(x)  ACTIVITY_VISIBLE x(Blackboard&b,XmlNode&x): IActivity(b,x){   }

#define ACTIVITY_REGISTER(x) namespace { 	ACTIVITY_VISIBLE ActivityRegistrar<x>::Type temp##x(#x);  }


class IActivity : public IExecutable {

    public:
        IActivity  ( Blackboard &,  XmlNode & );
        virtual ~IActivity() {};

        virtual void UserInit () {};

        virtual std::string GetName ()=0;

    protected:

        Blackboard &  _blk;
        XmlNode &  _xml;

};

//typedef GenericFactory < IActivity, std::string ,IActivity* (*)(Blackboard&),Blackboard&>  ActivityFactory;
typedef GenericFactory < IActivity, std::string ,IActivity* (*)(Blackboard&, XmlNode &),Blackboard&, XmlNode &>  ActivityFactory;

template<class T>
struct ActivityRegistrar {
    typedef Registrar<ActivityFactory,IActivity,std::string,T,Blackboard&,XmlNode&> Type;
};




#endif /* IACTIVITY_H_ */

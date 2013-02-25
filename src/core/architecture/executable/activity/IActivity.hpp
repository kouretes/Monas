#ifndef _IACTIVITY_H_
#define _IACTIVITY_H_ 1

#include "../IExecutable.hpp"

#include "core/architecture/messaging/MessageHub.hpp"
#include "core/architecture/messaging/Blackboard.hpp"

#include "core/architecture/XmlManager/XmlManager.h"

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


#define ACTIVITY_CONSTRUCTOR(x)  ACTIVITY_VISIBLE x(Blackboard&b,XmlManager&x): IActivity(b,x){   }

#define ACTIVITY_REGISTER(x) namespace { 	ACTIVITY_VISIBLE ActivityRegistrar<x>::Type temp##x(#x);  }


class IActivity : public IExecutable
{

public:
	IActivity  ( Blackboard &,  XmlManager & );
	virtual ~IActivity() {};

	virtual void UserInit () {};
	virtual void Reset () {};

	virtual std::string GetName () = 0;

protected:

	Blackboard &  _blk;
	XmlManager &  _xml;

};

//typedef GenericFactory < IActivity, std::string ,IActivity* (*)(Blackboard&),Blackboard&>  ActivityFactory;
typedef GenericFactory < IActivity, std::string , IActivity* (*)(Blackboard&, XmlManager &), Blackboard&, XmlManager &>  ActivityFactory;

template<class T>
struct ActivityRegistrar
{
	typedef Registrar<ActivityFactory, IActivity, std::string, T, Blackboard&, XmlManager&> Type;
};




#endif /* IACTIVITY_H_ */

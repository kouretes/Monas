#ifndef IACTIVITY_HPP
#define IACTIVITY_HPP

#include "../IExecutable.hpp"

#include "core/architecture/messaging/MessageHub.hpp"
#include "core/architecture/messaging/Blackboard.hpp"

#include "core/include/Configurator.hpp"

#include "core/elements/factory/Factory.hpp"
#include "core/elements/factory/Registrar.hpp"

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


#define ACTIVITY_CONSTRUCTOR(x)  ACTIVITY_VISIBLE x(Blackboard&b): IActivity(b){   }

#define ACTIVITY_REGISTER(x) namespace { 	ACTIVITY_VISIBLE ActivityRegistrar<x>::Type temp##x(#x);  }

using Messaging::Blackboard;
class IActivity : public IExecutable
{

public:
	IActivity  ( Messaging::Blackboard &);
	virtual ~IActivity() {};

	virtual void UserInit () {};
	virtual void Reset () {};

	virtual std::string GetName () = 0;

protected:

	Messaging::Blackboard &  _blk;

};

typedef Factory < IActivity, std::string , IActivity* (*)(Messaging::Blackboard&), Messaging::Blackboard&>  ActivityFactory;

template<class T>
struct ActivityRegistrar
{
	typedef Registrar<ActivityFactory, IActivity, std::string, T, Messaging::Blackboard&> Type;
};




#endif /* IACTIVITY_H_ */

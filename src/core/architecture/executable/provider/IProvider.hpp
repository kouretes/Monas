#ifndef _IPROVIDER_H_
#define _IPROVIDER_H_ 1

#include "core/architecture/thread/PeriodicThread.hpp"

#include "core/architecture/messaging/MessageHub.hpp"
#include "core/architecture/messaging/EndPoint.hpp"


#include "core/elements/factory/Factory.hpp"
#include "core/elements/factory/Registrar.hpp"

#include <string>


#ifdef __GNUC__
#define PROVIDER_START _Pragma("GCC visibility push(hidden)" )
#define PROVIDER_END _Pragma("GCC visibility pop")
#define PROVIDER_VISIBLE __attribute__ ((visibility("default")))
#else
#define PROVIDER_START
#define PROVIDER_END
#define PROVIDER_VISIBLE
#endif

#define PROVIDER_CONSTRUCTOR(x)  PROVIDER_VISIBLE x(KSystem::ThreadConfig &c, Narukom&n): \
                                EndPoint(GetName()),IProvider(GetName(),c,n){ UserInit();  }

#define PROVIDER_REGISTER(x) namespace { 	PROVIDER_VISIBLE ProviderRegistrar<x>::Type temp##x(#x);  }



class IProvider : virtual public EndPoint , public KSystem::PeriodicThread
{

public:
	IProvider(const std::string &, KSystem::ThreadConfig &c, MessageHub&);
	virtual ~IProvider() {};
	virtual void UserInit () = 0;

	using   KSystem::PeriodicThread::GetName  ;

protected:
	void notify();

	MessageHub & _com;


};

typedef Factory < IProvider, std::string,
        IProvider* (*)(KSystem::ThreadConfig &, MessageHub&),
        KSystem::ThreadConfig &, MessageHub& >  ProviderFactory;

template<class T>
struct ProviderRegistrar
{
	typedef Registrar<ProviderFactory, IProvider, std::string, T, KSystem::ThreadConfig &, MessageHub&> Type;
};

#endif /* _IPROVIDER_H_ */

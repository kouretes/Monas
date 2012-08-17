#ifndef _IPROVIDER_H_
#define _IPROVIDER_H_ 1

#include "architecture/periodicthread.h"

#include "architecture/narukom/narukom.h"
#include "architecture/narukom/pub_sub/endpoint.h"


#include "tools/genFactory.h"
#include "tools/genRegistrar.h"

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
	IProvider(const std::string &, KSystem::ThreadConfig &c, Narukom&);
	virtual ~IProvider() {};
	virtual void UserInit () = 0;

	using   KSystem::PeriodicThread::GetName  ;

protected:
	void notify();

	Narukom & _com;


};

typedef GenericFactory < IProvider, std::string,
        IProvider* (*)(KSystem::ThreadConfig &, Narukom&),
        KSystem::ThreadConfig &, Narukom& >  ProviderFactory;

template<class T>
struct ProviderRegistrar
{
	typedef Registrar<ProviderFactory, IProvider, std::string, T, KSystem::ThreadConfig &, Narukom&> Type;
};

#endif /* _IPROVIDER_H_ */

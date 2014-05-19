#ifndef STATECHARTWRAPPER_H
#define STATECHARTWRAPPER_H

#include "core/architecture/statechartEngine/Statechart.h"
#include "core/elements/factory/Factory.hpp"
#include "core/elements/factory/Registrar.hpp"

class StatechartWrapper
{

public:

	StatechartWrapper ()
	{
		;
	}

	virtual void Start () = 0;

	virtual void Stop () = 0;

};

typedef Factory < StatechartWrapper, std::string, StatechartWrapper* (*)(Messaging::MessageHub*), Messaging::MessageHub* >  StatechartFactory;

template<class T>
struct StatechartRegistrar
{
	typedef Registrar<StatechartFactory, StatechartWrapper, std::string, T, Messaging::MessageHub*> Type;
};

#endif // STATECHARTWRAPPER_H

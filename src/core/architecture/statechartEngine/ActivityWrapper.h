#ifndef ACTIVITYWRAPPER_H
#define ACTIVITYWRAPPER_H 1

#include "core/architecture/executable/IExecutable.hpp"
#include "Statechart.h"
#include "core/include/IActivity.hpp"

namespace statechart_engine
{

	class ActivityWrapper : public IExecutable
	{

	public:

		ActivityWrapper ();

		void Init(IActivity* act, volatile int* running, Statechart* s);

		int IEX_DIRECTIVE_HOT Execute();

	private:

		IActivity* _act;

		volatile int* _running;

		Statechart* _s;
	};

}

#endif // ACTIVITYWRAPPER_H

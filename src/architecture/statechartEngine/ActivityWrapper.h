#ifndef ACTIVITYWRAPPER_H
#define ACTIVITYWRAPPER_H 1

#include "architecture/IExecutable.h"
#include "Statechart.h"
#include "architecture/executables/IActivity.h"

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

#ifndef _talws_h_
#define _talws_h_ 1

#include "core/architecture/thread/agent/Agent.hpp"
#include "core/architecture/executable/provider/IProvider.hpp"
#include "hal/Thread.hpp"

#include "core/architecture/statechartWrapper.h"

#include <vector>

class Talws
{

public:

	Talws ();

	~Talws();

	void Start();

	void Stop();

private:

	std::vector<Agent*> Agents;
	std::vector<IProvider*> Providers;

	std::vector<StatechartWrapper*> StatechartPlans;


	MessageHub com;

};

#endif // _talws_h_

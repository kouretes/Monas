#ifndef TALWS_H
#define TALWS_H

#include "core/architecture/thread/agent/Agent.hpp"
#include "core/architecture/executable/provider/IProvider.hpp"
#include "core/include/Configurator.hpp"


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

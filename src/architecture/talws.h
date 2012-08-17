#ifndef _talws_h_
#define _talws_h_ 1

#include "architecture/executables/agent.h"
#include "architecture/executables/IProvider.h"
#include "hal/thread.h"

#include "architecture/statechartWrapper.h"

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


	Narukom com;

};

#endif // _talws_h_

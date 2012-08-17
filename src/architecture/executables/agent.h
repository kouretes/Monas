#ifndef _agent_h_
#define _agent_h_ 1

#include <vector>
#include <string>

#include "tools/logger.h"

#include "architecture/narukom/narukom.h"
#include "architecture/narukom/pub_sub/blackboard.h"

#include "architecture/XmlManager/XmlConfigurator.h"

#include "architecture/executables/IActivity.h"

#include "architecture/periodicthread.h"

#ifdef RUN_ON_NAO
#include "hal/robot/generic_nao/robot_consts.h"
#endif

#include "agentTiming.h"
#include "tools/toString.h"


class Agent : public KSystem::PeriodicThread
{

public:

	Agent ( std::string name, KSystem::ThreadConfig cfg, int stats, Narukom& com, std::vector<std::string> activities ) ;

	virtual ~Agent ();

	int Execute () ;
	typedef std::vector<std::string> ActivityNameList;

	const std::string GetName() const
	{
		return _name;
	};
private:

	void PrintStatistics();

	std::string _name;


	Narukom & _com;
	Blackboard _blk;
	XmlNode _xml;

	typedef std::vector<IActivity*> ActivList;

	ActivList _activities; //in execution order

	AgentTiming agentStats;

	unsigned int _statsCycle;

	unsigned int _executions;

};

#endif // _agent_h_

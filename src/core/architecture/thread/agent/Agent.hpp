#ifndef _agent_h_
#define _agent_h_ 1

#include <vector>
#include <string>

#include "tools/logger.h"

#include "core/architecture/messaging/MessageHub.hpp"
#include "core/architecture/messaging/Blackboard.hpp"
#include "core/include/IActivity.hpp"
#include "core/architecture/thread/PeriodicThread.hpp"

#include "messages/GUICommunication.pb.h"

#ifdef NAOQI
#include "hal/robot/generic_nao/robot_consts.h"
#endif

#include "AgentTiming.hpp"
#include "tools/toString.h"


class Agent : public KSystem::PeriodicThread
{

public:

	Agent ( std::string name, KSystem::ThreadConfig cfg, int stats, MessageHub& com, std::vector<std::string> activities ) ;

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


	MessageHub & _com;
	Blackboard _blk;

	typedef std::vector<IActivity*> ActivList;

	ActivList _activities; //in execution order

	AgentTiming agentStats;

	unsigned int _statsCycle;

	unsigned int _executions;

	//Reset Messages
	boost::shared_ptr<const ResetMessage> rmsg;
	boost::shared_ptr<const UpdateMessage> umsg;

};

#endif // _agent_h_

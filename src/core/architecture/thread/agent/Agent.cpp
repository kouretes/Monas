#include "Agent.hpp"
#include "core/include/Logger.hpp"
Agent::Agent( std::string name, KSystem::ThreadConfig cfg, int stats, MessageHub& com, std::vector<std::string> activities ) :
	PeriodicThread(cfg),
	_statsCycle(stats),
	_name(name),
	_com(com),
	_blk(name),
	_executions(0)
{
	_blk.attachTo(com);

	for ( ActivityNameList::const_iterator it = activities.begin(); it != activities.end(); it++ )
		//_activities.push_back( ActivityFactory::Instance()->CreateObject( (*it),_blk ) );
		_activities.push_back( ActivityFactory::Instance()->CreateObject( (*it), _blk) );

	for ( ActivList::iterator it = _activities.begin(); it != _activities.end(); ++it )
		(*it)->UserInit();

	_blk.updateSubscription("architecture", msgentry::SUBSCRIBE_ON_TOPIC);
};

Agent::~Agent ()
{
	PrintStatistics();

	for ( ActivList::iterator it = _activities.begin(); it != _activities.end(); ++it )
		delete (*it);
};

int Agent:: Execute ()
{
	_executions++;
	agentStats.StartAgentTiming();
	_blk.process_messages();

	rmsg = _blk.readSignal<ResetMessage> ("architecture");

	if(rmsg != 0){
		for ( ActivList::iterator it = _activities.begin(); it != _activities.end(); it++ )
		{
			std::string activityName = (*it)->GetName();
			for(int i=0; i < rmsg->resetactivities_size(); i++){
				if(activityName.compare(rmsg->resetactivities(i)) == 0){
					(*it)->Reset();
					break;
				}
			}
		}
	}
	for ( ActivList::iterator it = _activities.begin(); it != _activities.end(); it++ )
	{
		agentStats.StartActivityTiming(*it);
		(*it)->Execute();
		agentStats.StopActivityTiming(*it);
	}

	_blk.publish_all();
	agentStats.StopAgentTiming();

	if ( ! (_executions % _statsCycle) )
	{
		PrintStatistics();
	}

	return 0;
};

void Agent::PrintStatistics()
{
	LogEntry m(LogLevel::Info,_name);
	m<<"Avg: "<<agentStats.GetAgentAvgExecTime()<<" Var: "<<agentStats.GetAgentVarExecTime()<<std::endl;

	for (ActivList::const_iterator it = _activities.begin(); it != _activities.end(); it++ )
	{
		m<< "Activity: " << (*it)->GetName() ;
		m<<" Avg: " << agentStats.GetActivityAvgExecTime( (*it) );
		m<<  "Var: "<< agentStats.GetActivityVarExecTime( (*it) )<<std::endl;
	}
};


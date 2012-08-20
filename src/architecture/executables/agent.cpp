#include "agent.h"
Agent::Agent( std::string name, KSystem::ThreadConfig cfg, int stats, Narukom& com, std::vector<std::string> activities ) :
	PeriodicThread(cfg),
	_statsCycle(stats),
	_name(name),
	_com(com),
	_blk(name),
	_executions(0)
{
#ifdef RUN_ON_NAO
	_xml = XmlNode(ArchConfig::Instance().GetConfigPrefix(), KRobotConfig::Instance().getConfig(KDeviceLists::Interpret::HEAD_ID)
	               , KRobotConfig::Instance().getConfig(KDeviceLists::Interpret::BODY_ID));
#else
	_xml = XmlNode(ArchConfig::Instance().GetConfigPrefix(), "hi", "bi");
#endif
	_blk.attachTo(*com.get_message_queue());

	for ( ActivityNameList::const_iterator it = activities.begin(); it != activities.end(); it++ )
		//_activities.push_back( ActivityFactory::Instance()->CreateObject( (*it),_blk ) );
		_activities.push_back( ActivityFactory::Instance()->CreateObject( (*it), _blk, _xml ) );

	for ( ActivList::iterator it = _activities.begin(); it != _activities.end(); ++it )
		(*it)->UserInit();
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
	std::string StatMessage( "Avg: " + _toString(agentStats.GetAgentAvgExecTime()) + " Var: " + _toString(agentStats.GetAgentVarExecTime()) + "\n"  );

	for (ActivList::const_iterator it = _activities.begin(); it != _activities.end(); it++ )
	{
		StatMessage += "Activity: " + (*it)->GetName() ;
		StatMessage += " Avg: " + _toString( agentStats.GetActivityAvgExecTime( (*it) ));
		StatMessage += " Var: " + _toString( agentStats.GetActivityVarExecTime( (*it) )) + "\n";
	}

	Logger::Instance().WriteMsg( _name, StatMessage, Logger::Info );
};


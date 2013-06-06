#include "talws.h"

#include "core/architecture/messaging/MessageHub.hpp"
#include "core/architecture/thread/agent/AgentConfig.hpp"

#include "tools/logger.h"

#include "activities/activityRegistry.h"
#include "providers/providerRegistry.h"

Talws::Talws ()
{
	Logger::Instance();
	com.StartThread();

	std::string agentFile = Configurator::Instance().findValueForKey("monasConfig.AgentConfigurationFile");
	if ( Configurator::Instance().numberOfNodesForKey(agentFile) != 1)
	{
		std::string msg("Can't read or parse agent configuration file @ ");
		msg += Configurator::Instance().getDirectoryPath();
		msg += Configurator::Instance().findValueForKey("monasConfig.AgentConfigurationFile");
		Logger::Instance().WriteMsg(std::string("Talws"), msg, Logger::FatalError );
		SysCall::_exit(1);
	}

	//======================= Start Agents  ===================================
	int numOfAgents = Configurator::Instance().numberOfNodesForKey(agentFile + ".agent");
	Logger::Instance().WriteMsg("Talws", "Found " + _toString(numOfAgents) + " agent(s)", Logger::Info );

	for (int i=0;i<numOfAgents;i++)
	{
		if(atoi(Configurator::Instance().findValueForKey(agentFile + ".agent~" + _toString(i)+".$Enable").c_str()) == 1){
			std::string AgentName = Configurator::Instance().findValueForKey(agentFile + ".agent~" + _toString(i) + ".name");

			int numOfActivities = Configurator::Instance().numberOfNodesForKey(agentFile + ".agent~" + _toString(i) + ".activity");
			std::vector<std::string> activities;

			for (int j = 0; j < numOfActivities; j++ )
			{
				if(atoi(Configurator::Instance().findValueForKey(agentFile + ".agent~" + _toString(i) + ".activity~" + _toString(j) + ".$Enable").c_str()) == 1){
					activities.push_back( Configurator::Instance().findValueForKey(agentFile + ".agent~" + _toString(i) + ".activity~" + _toString(j)) );
					Logger::Instance().WriteMsg("Talws", "Agent: " + AgentName + " Registering module: " + activities.back(), Logger::ExtraInfo );
				}
			}

			KSystem::ThreadConfig tcfg;
			tcfg.IsRealTime = atoi(Configurator::Instance().findValueForKey(agentFile + ".agent~" + _toString(i)+".$IsRealTime").c_str()) == 0 ? false : true;
			tcfg.Priority = atoi(Configurator::Instance().findValueForKey(agentFile + ".agent~" + _toString(i)+".$Priority").c_str());
			float threadFreq = atof(Configurator::Instance().findValueForKey(agentFile + ".agent~" + _toString(i)+".$ThreadFrequency").c_str());
			tcfg.ThreadPeriod = threadFreq > 0 ? 1 / threadFreq  : 0;
			int StatsCycle = atoi(Configurator::Instance().findValueForKey(agentFile + ".agent~" + _toString(i)+".$StatsCycle").c_str());
			Agent *a = new Agent(AgentName, tcfg, StatsCycle, com, activities);
			Agents.push_back( a );
			std::ostringstream AgentInfo;
			AgentInfo << AgentName
				      << " Attrb: IsRealTime=" << tcfg.IsRealTime
				      << " Priority=" << tcfg.Priority
				      << " ThreadFrequency=" << tcfg.ThreadPeriod
				      << " StatsCycle=" << StatsCycle << std::endl;
			Logger::Instance().WriteMsg("Talws", AgentInfo.str(), Logger::ExtraInfo);
		}
	}

	//======================= Start StateCharts  ===================================
	int numOfStatecharts = Configurator::Instance().numberOfNodesForKey(agentFile + ".statechart");
	Logger::Instance().WriteMsg("Talws", "Found " + _toString(numOfStatecharts) + " statechart plan(s)", Logger::Info );

	for (int i=0;i<numOfStatecharts;i++){
		if(atoi(Configurator::Instance().findValueForKey(agentFile + ".statechart~" + _toString(i)+".$Enable").c_str()) == 1){
			std::string statechart = Configurator::Instance().findValueForKey(agentFile + ".statechart~" + _toString(i));
			StatechartPlans.push_back( StatechartFactory::Instance()->CreateObject( statechart, &com ) );
			Logger::Instance().WriteMsg("Talws", statechart, Logger::ExtraInfo);
		}
	}

	//======================= Start Providers  ===================================
	int numOfProviders = Configurator::Instance().numberOfNodesForKey(agentFile + ".provider");
	Logger::Instance().WriteMsg("Talws", "Found " + _toString(numOfProviders) + " provider(s)", Logger::Info );

	for (int i=0;i<numOfProviders;i++)
	{
		if(atoi(Configurator::Instance().findValueForKey(agentFile + ".provider~" + _toString(i)+".$Enable").c_str()) == 1){
			std::string provider = Configurator::Instance().findValueForKey(agentFile + ".provider~" + _toString(i));
			KSystem::ThreadConfig tcfg;
			tcfg.IsRealTime = atoi(Configurator::Instance().findValueForKey(agentFile + ".provider~" + _toString(i)+".$IsRealTime").c_str()) == 0 ? false : true;
			tcfg.Priority = atoi(Configurator::Instance().findValueForKey(agentFile + ".provider~" + _toString(i)+".$Priority").c_str());
			float threadFreq = atof(Configurator::Instance().findValueForKey(agentFile + ".provider~" + _toString(i)+".$ThreadFrequency").c_str());
			tcfg.ThreadPeriod = threadFreq > 0 ? 1 / threadFreq  : 0;
			Providers.push_back( ProviderFactory::Instance()->CreateObject(provider ,  tcfg, com ) );
			Logger::Instance().WriteMsg("Talws", provider, Logger::ExtraInfo);
		}
	}
}

Talws::~Talws()
{
	Stop();

	for ( std::vector<Agent*>::const_iterator it = Agents.begin(); it != Agents.end(); it++ )
		delete (*it);

	for ( std::vector<StatechartWrapper*>::const_iterator it = StatechartPlans.begin(); it != StatechartPlans.end(); it++ )
		delete (*it);

	for ( std::vector<IProvider*>::const_iterator it = Providers.begin(); it != Providers.end(); it++ )
		delete (*it);
}

void Talws::Start()
{
	std::cout << "Talws: Starting..." << std::endl; //TODO

	for ( std::vector<Agent*>::const_iterator it = Agents.begin(); it != Agents.end(); it++ )
		(*it)->StartThread();

	for ( std::vector<StatechartWrapper*>::const_iterator it = StatechartPlans.begin(); it != StatechartPlans.end(); it++ )
		(*it)->Start();

	for ( std::vector<IProvider*>::const_iterator it = Providers.begin(); it != Providers.end(); it++ )
		(*it)->StartThread();
}

void Talws::Stop()
{
	std::cout << "Talws: Stoping..." << std::endl; //TODO

	for ( std::vector<Agent*>::const_iterator it = Agents.begin(); it != Agents.end(); it++ )
		(*it)->StopThread();

	for ( std::vector<IProvider*>::const_iterator it = Providers.begin(); it != Providers.end(); it++ )
		(*it)->StopThread();

	SysCall::_usleep(100000);

	//TODO stop somehow narukom
	for ( std::vector<Agent*>::const_iterator it = Agents.begin(); it != Agents.end(); it++ )
		(*it)->JoinThread();

	for ( std::vector<IProvider*>::const_iterator it = Providers.begin(); it != Providers.end(); it++ )
		(*it)->JoinThread();

	//com.get_message_queue()->JoinThread();
}


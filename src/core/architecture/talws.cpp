#include "talws.h"

#include "core/architecture/messaging/MessageHub.hpp"
#include "core/architecture/thread/agent/AgentConfig.hpp"

#include "core/include/Logger.hpp"

#include "activities/activityRegistry.h"
#include "providers/providerRegistry.h"

Talws::Talws ()
{
	LoggerSingleton::Instance();
	com.StartThread();

	std::string agentFile = Configurator::Instance().findValueForKey("monasConfig.AgentConfigurationFile");
	if ( Configurator::Instance().numberOfNodesForKey(agentFile) != 1)
	{
		{ //Create different scope to ensure that m gets destructed and dispatched
			LogEntry m(LogLevel::FatalError,"Talws");
			m<<"Can't read or parse agent configuration file @ "
			 << Configurator::Instance().getDirectoryPath()
			 << Configurator::Instance().findValueForKey("monasConfig.AgentConfigurationFile");
		}
		SysCall::_exit(1);
	}

	//======================= Start Agents  ===================================
	int numOfAgents = Configurator::Instance().numberOfNodesForKey(agentFile + ".agent");
	LogEntry(LogLevel::Info,"Talws")<<"Found "<< (numOfAgents) <<" agent(s)";
	for (int i=0;i<numOfAgents;i++)
	{
		if(atoi(Configurator::Instance().findValueForKey(agentFile + ".agent~" + _toString(i)+".$Enable").c_str()) == 1){
			std::string AgentName = Configurator::Instance().findValueForKey(agentFile + ".agent~" + _toString(i) + ".name");
			//Clear naming conflicts



            for(unsigned k=0;k<Agents.size();k++)
            {
                if(AgentName.compare(Agents[k]->GetName())==0)
                {
                    LogEntry(LogLevel::Warning,"Talws")  << "Agent name: " << AgentName << " exists! mangling name to resolve conflict";
                    AgentName+="_mangling";
                    k=0;
                }
            }
			int numOfActivities = Configurator::Instance().numberOfNodesForKey(agentFile + ".agent~" + _toString(i) + ".activity");
			std::vector<std::string> activities;

			for (int j = 0; j < numOfActivities; j++ )
			{
				if(atoi(Configurator::Instance().findValueForKey(agentFile + ".agent~" + _toString(i) + ".activity~" + _toString(j) + ".$Enable").c_str()) == 1){
					activities.push_back( Configurator::Instance().findValueForKey(agentFile + ".agent~" + _toString(i) + ".activity~" + _toString(j)) );
					LogEntry(LogLevel::ExtraInfo,"Talws")<<"Agent: " << AgentName << " Registering module: " + activities.back();
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

			LogEntry m(LogLevel::ExtraInfo,"Talws");
			m		  << AgentName
				      << " Attrb: IsRealTime=" << tcfg.IsRealTime
				      << " Priority=" << tcfg.Priority
				      << " ThreadFrequency=" << tcfg.ThreadPeriod
				      << " StatsCycle=" << StatsCycle ;
		}
	}
	//======================= Start StateCharts  ===================================
	int numOfStatecharts = Configurator::Instance().numberOfNodesForKey(agentFile + ".statechart");
	LogEntry(LogLevel::Info,"Talws") << "Found " <<(numOfStatecharts) + " statechart plan(s)";

	for (int i=0;i<numOfStatecharts;i++){
		if(atoi(Configurator::Instance().findValueForKey(agentFile + ".statechart~" + _toString(i)+".$Enable").c_str()) == 1){
			std::string statechart = Configurator::Instance().findValueForKey(agentFile + ".statechart~" + _toString(i));
			StatechartPlans.push_back( StatechartFactory::Instance()->CreateObject( statechart, &com ) );
			LogEntry(LogLevel::ExtraInfo,"Talws")<< statechart;
		}
	}

	//======================= Start Providers  ===================================
	int numOfProviders = Configurator::Instance().numberOfNodesForKey(agentFile + ".provider");
	LogEntry(LogLevel::Info,"Talws") << "Found " <<(numOfProviders) + " providers(s)";

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
			LogEntry(LogLevel::ExtraInfo,"Talws")<< provider;
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
	std::cout << "Talws: Stoping Agents...";
	std::cout.flush();
	for ( std::vector<Agent*>::const_iterator it = Agents.begin(); it != Agents.end(); it++ )
		(*it)->StopThread();
    std::cout << "Done!"<<std::endl;
    std::cout << "Talws: Stoping Providers...";
    std::cout.flush();
	for ( std::vector<IProvider*>::const_iterator it = Providers.begin(); it != Providers.end(); it++ )
		(*it)->StopThread();
    std::cout << "Done!"<<std::endl;
    std::cout << "Talws: Joining Agents...";
    std::cout.flush();
	for ( std::vector<Agent*>::const_iterator it = Agents.begin(); it != Agents.end(); it++ )
		(*it)->JoinThread();
    std::cout << "Done!"<<std::endl;
    std::cout << "Talws: Joining Providers...";
    std::cout.flush();
	for ( std::vector<IProvider*>::const_iterator it = Providers.begin(); it != Providers.end(); it++ )
		(*it)->JoinThread();
    std::cout << "Done!"<<std::endl;
    std::cout << "Talws: Terminating StateCharts...";
    std::cout.flush();
	for ( std::vector<StatechartWrapper*>::const_iterator it = StatechartPlans.begin(); it != StatechartPlans.end(); it++ )
		(*it)->Stop();
    std::cout << "Done!"<<std::endl;
    std::cout << "Talws: Stopping MessageHub...";
    std::cout.flush();
	com.StopThread();
	std::cout << "Done!"<<std::endl;
	std::cout << "Talws: Wake MessageHub one last time...";
	std::cout.flush();
	com.requestMailMan(NULL); //Wake him
	std::cout << "Done!"<<std::endl;
	std::cout << "Talws: Joining MessageHub...";
	std::cout.flush();
	com.JoinThread();
	std::cout << "Done!"<<std::endl;
	std::cout<<"Bye Bye!"<<std::endl;
	std::cout.flush();
}


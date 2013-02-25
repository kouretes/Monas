#include "talws.h"

#include "core/architecture/messaging/MessageHub.hpp"

#include "core/architecture/thread/agent/AgentConfig.hpp"
#include "tools/XML.h"
#include "tools/XMLConfig.h"
#include "tools/logger.h"
#include "config.h"
#include "core/architecture/archConfig.h"

#include "activities/activityRegistry.h"
#include "providers/providerRegistry.h"

Talws::Talws ()
{
	ArchConfig::Instance();
	Logger::Instance();
	XML AgentXmlFile( ArchConfig::Instance().GetConfigPrefix() + ArchConfig::Instance().GetAgentCfgFile() );

	if ( ! AgentXmlFile.IsLoadedSuccessfully() )
	{
		std::string msg("Can't read or parse agent configuration file @ ");
		msg += ArchConfig::Instance().GetAgentCfgFile();
		Logger::Instance().WriteMsg(std::string("Talws"), msg, Logger::FatalError );
		SysCall::_exit(1);
	}

	//======================= Start Agents  ===================================
	typedef std::vector<XMLNode<std::string, float, std::string> > NodeCont;
	NodeCont AgentNodes = AgentXmlFile.QueryElement<std::string, float, std::string>( "agent" );
	std::ostringstream AgentNum;
	AgentNum << "Found " << AgentNodes.size() << " agent(s)";
	Logger::Instance().WriteMsg("Talws", AgentNum.str(), Logger::Info );

	for ( NodeCont::iterator it = AgentNodes.begin(); it != AgentNodes.end(); it++ )
	{
		if(it->attrb["Enable"] == 1){
			NodeCont AgentNameNode = AgentXmlFile.QueryElement<std::string, float, std::string>("name", &(*it) );
			std::string AgentName = AgentNameNode[0].value;
			NodeCont ActivityNodes = AgentXmlFile.QueryElement<std::string, float, std::string>("activity", &(*it) );
			std::vector<std::string> activities;

			for ( unsigned int i = 0; i < ActivityNodes.size(); i++ )
			{
				if(ActivityNodes[i].attrb["Enable"] == 1){
					activities.push_back( ActivityNodes[i].value );
					Logger::Instance().WriteMsg("Talws", "Agent: " + AgentName + " Registering module: " + activities.back(), Logger::ExtraInfo );
				}
			}

			KSystem::ThreadConfig tcfg;
			tcfg.IsRealTime = it->attrb["IsRealTime"] == 0 ? false : true;
			tcfg.Priority = it->attrb["Priority"];
			tcfg.ThreadPeriod = it->attrb["ThreadFrequency"] > 0 ? 1 / it->attrb["ThreadFrequency"] : 0;
			int StatsCycle = it->attrb["StatsCycle"];
			Agent *a = new Agent(AgentName, tcfg, StatsCycle, com, activities);
			Agents.push_back( a );
			std::ostringstream AgentInfo;
			AgentInfo << AgentName
				      << " Attrb: IsRealTime=" << (it->attrb["IsRealTime"])
				      << " Priority=" << (it->attrb["Priority"])
				      << " ThreadFrequency=" << (it->attrb["ThreadFrequency"])
				      << " StatsCycle=" << (it->attrb["StatsCycle"]) << std::endl;
			Logger::Instance().WriteMsg("Talws", AgentInfo.str(), Logger::ExtraInfo);
		}
	}

	//======================= Start StateCharts  ===================================
	NodeCont StatechartNodes = AgentXmlFile.QueryElement<std::string, float, std::string>( "statechart" );
	Logger::Instance().WriteMsg("Talws", "Found " + _toString(StatechartNodes.size()) + " statechart plan(s)", Logger::Info );

	for ( NodeCont::iterator it = StatechartNodes.begin(); it != StatechartNodes.end(); it++ ){
		if(it->attrb["Enable"] == 1){
			StatechartPlans.push_back( StatechartFactory::Instance()->CreateObject( (*it).value , &com ) );
		}
	}

	//======================= Start Providers  ===================================
	NodeCont ProviderNodes = AgentXmlFile.QueryElement<std::string, float, std::string>( "provider" );
	Logger::Instance().WriteMsg("Talws", "Found " + _toString(ProviderNodes.size()) + " provider(s)", Logger::Info );

	for ( NodeCont::iterator it = ProviderNodes.begin(); it != ProviderNodes.end(); it++ )
	{
		if(it->attrb["Enable"] == 1){
			KSystem::ThreadConfig tcfg;
			tcfg.IsRealTime = it->attrb["IsRealTime"] == 0 ? false : true;
			tcfg.Priority = it->attrb["Priority"];
			tcfg.ThreadPeriod = it->attrb["ThreadFrequency"] > 0 ? 1 / it->attrb["ThreadFrequency"] : 0;
			Providers.push_back( ProviderFactory::Instance()->CreateObject( (*it).value ,  tcfg, com ) );
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


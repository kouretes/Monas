#include "talws.h"

#include "architecture/narukom/narukom.h"

#include "architecture/agentConfig.h"
#include "tools/XML.h"
#include "tools/XMLConfig.h"
#include "tools/logger.h"
#include "../config/config.h"
#include "architecture/archConfig.h"

#ifdef DLIB_FUNCTIONALITY
#include "hal/dlib_fnc.h"
#endif // DLIB_FUNCTIONALITY

#include "activities/activityRegistry.h"

Talws::Talws () {

    ArchConfig::Instance();
    Logger::Instance();

    XML AgentXmlFile( ArchConfig::Instance().GetConfigPrefix()+ArchConfig::Instance().GetAgentCfgFile() );

    if ( ! AgentXmlFile.IsLoadedSuccessfully() ) {
        std::string msg("Can't read or parse agent configuration file @ ");
        msg += ArchConfig::Instance().GetAgentCfgFile();
        Logger::Instance().WriteMsg(std::string("Talws"),msg, Logger::FatalError );
        SysCall::_exit(1);
    }


    typedef std::vector<XMLNode<std::string, int, std::string> > NodeCont;

    NodeCont AgentNodes = AgentXmlFile.QueryElement<std::string, int, std::string>( "agent" );

    std::ostringstream AgentNum;
    AgentNum<<"Found "<<AgentNodes.size()<<" agent(s)";

    Logger::Instance().WriteMsg("Talws",AgentNum.str(), Logger::Info );

    for ( NodeCont::iterator it = AgentNodes.begin(); it != AgentNodes.end(); it++ ) {

        NodeCont AgentNameNode = AgentXmlFile.QueryElement<std::string, int, std::string>("name", &(*it) );

        std::string AgentName = AgentNameNode[0].value;

        NodeCont ActivityNodes = AgentXmlFile.QueryElement<std::string, int, std::string>("activity", &(*it) );

        std::vector<std::string> activities;
        for ( unsigned int i=0; i<ActivityNodes.size(); i++ ) {
#ifdef DLIB_FUNCTIONALITY
            void* dlib_handler = DLibFnc::_open( ("lib"+ActivityNodes[i].value+".so").c_str());
            if ( ! dlib_handler ) {
                Logger::Instance().WriteMsg("Talws", DLibFnc::_error(), Logger::Info );
            }
#endif //DLIB_FUNCTIONALITY
            activities.push_back( ActivityNodes[i].value );
            Logger::Instance().WriteMsg("Talws", "Agent: "+AgentName+" Registering module: "+activities[i], Logger::ExtraInfo );
        }

        AgentConfig tcfg;
        tcfg.IsRealTime = it->attrb["IsRealTime"] == 0 ? false : true;
        tcfg.Priority = it->attrb["Priority"];
        tcfg.ThreadFrequency = it->attrb["ThreadFrequency"];
        tcfg.StatsCycle = it->attrb["StatsCycle"];

        Agent *a = new Agent(AgentName,tcfg,&com,activities);

        Agents.push_back( a );

        std::ostringstream AgentInfo;
        AgentInfo<<AgentName<<" Attrb: IsRealTime="<<tcfg.IsRealTime<<" Priority="<<tcfg.Priority
            <<" ThreadFrequency="<<tcfg.ThreadFrequency
            <<" StatsCycle="<<tcfg.StatsCycle<<std::endl;
        Logger::Instance().WriteMsg("Talws", AgentInfo.str(), Logger::ExtraInfo);

        NodeCont StatechartNodes = AgentXmlFile.QueryElement<std::string, int, std::string>( "statechart" );

        Logger::Instance().WriteMsg("Talws","Found "+_toString(StatechartNodes.size())+" statechart plan(s)", Logger::Info );

        for ( NodeCont::iterator it = StatechartNodes.begin(); it != StatechartNodes.end(); it++ )
          StatechartPlans.push_back( StatechartFactory::Instance()->CreateObject( (*it).value , &com ) );

    }

}

Talws::~Talws() {
    Stop();
    for ( std::vector<Agent*>::const_iterator it = Agents.begin(); it != Agents.end(); it++ )
        delete (*it);
    for ( std::vector<StatechartWrapper*>::const_iterator it = StatechartPlans.begin(); it != StatechartPlans.end(); it++ )
      delete (*it);
}

void Talws::Start() {
    std::cout<<"Talws: Starting..."<<std::endl; //TODO
    for ( std::vector<Agent*>::const_iterator it = Agents.begin(); it != Agents.end(); it++ )
        (*it)->StartThread();
    for ( std::vector<StatechartWrapper*>::const_iterator it = StatechartPlans.begin(); it != StatechartPlans.end(); it++ )
        (*it)->Start();
}

void Talws::Stop() {
    std::cout<<"Talws: Stoping..."<<std::endl; //TODO
    for ( std::vector<Agent*>::const_iterator it = Agents.begin(); it != Agents.end(); it++ )
        (*it)->StopThread();
    for ( std::vector<StatechartWrapper*>::const_iterator it = StatechartPlans.begin(); it != StatechartPlans.end(); it++ )
      (*it)->Stop();
    SysCall::_usleep(100000);
    //TODO stop somehow narukom
    for ( std::vector<Agent*>::const_iterator it = Agents.begin(); it != Agents.end(); it++ )
        (*it)->JoinThread();
    //com.get_message_queue()->JoinThread();
}


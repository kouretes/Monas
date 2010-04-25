#ifndef _talws_h_
#define _talws_h_ 1

#include "architecture/agent.h"
#include "hal/thread.h"
#include "hal/threadconfig.h"
#include "tools/XML.h"
#include "tools/XMLConfig.h"
#include "tools/logger.h"
#include "../config/config.h"
#include "architecture/archConfig.h"

#include <vector>

#ifdef DLIB_FUNCTIONALITY
#include "hal/dlib_fnc.h"
#endif // DLIB_FUNCTIONALITY

#include "provAndRep/providerRegistry.h"
//FIXME add thread registry

class Talws {

    public:

        Talws () {

            ArchConfig::Instance();
            Logger::Instance();

            XML AgentXmlFile( ArchConfig::Instance()->GetAgentCfgFile() );

            if ( ! AgentXmlFile.IsLoadedSuccessfully() ) {
                std::string msg("Can't read or parse agent configuration file @ ");
                msg += ArchConfig::Instance()->GetAgentCfgFile();
                Logger::Instance()->WriteMsg(std::string("Talws"),msg, Logger::FatalError );
                SysCall::_exit(1);
            }
            
            typedef std::vector<XMLNode<std::string, int, std::string> > NodeCont;
            
            NodeCont AgentNodes = AgentXmlFile.QueryElement<std::string, int, std::string>( "agent" );

            std::ostringstream AgentNum;
            AgentNum<<"Found "<<AgentNodes.size()<<" agent(s)";

            Logger::Instance()->WriteMsg("Talws",AgentNum.str(), Logger::Info );

            for ( NodeCont::iterator it = AgentNodes.begin(); it != AgentNodes.end(); it++ ) {

                NodeCont AgentNameNode = AgentXmlFile.QueryElement<std::string, int, std::string>("name", &(*it) );

                std::string AgentName = AgentNameNode[0].value;

                NodeCont ProviderNodes = AgentXmlFile.QueryElement<std::string, int, std::string>("provider", &(*it) );

                std::vector<std::string> providers;
                for ( unsigned int i=0; i<ProviderNodes.size(); i++ ) {
#ifdef DLIB_FUNCTIONALITY
                    void* dlib_handler = DLibFnc::_open( ("lib"+ProviderNodes[i].value+".so").c_str());
                    if ( ! dlib_handler ) {
                        Logger::Instance()->WriteMsg("Talws", DLibFnc::_error(), Logger::Info );
                    }
#endif //DLIB_FUNCTIONALITY
                    providers.push_back( ProviderNodes[i].value );
                    Logger::Instance()->WriteMsg("Talws", "Agent: "+AgentName+" Registering module: "+providers[i], Logger::ExtraInfo );
                }

                Agent *a = new Agent(providers);

                ThreadConfig tcfg;
                tcfg.IsRealTime = it->attrb["IsRealTime"] == 0 ? false : true;
                tcfg.Priority = it->attrb["Priority"];
                tcfg.ThreadFrequency = it->attrb["ThreadFrequency"];
                tcfg.MultipleThreads = it->attrb["MultipleThreads"] == 0 ? false : true;
                
                Thread *t =  ThreadFactory::Instance()->CreateObject(ArchConfig::Instance()->GetThreadType(), a, tcfg );

                Agents.push_back( a );
                Threads.push_back( t );

                std::ostringstream AgentInfo;
                AgentInfo<<AgentName<<" Attrb: IsRealTime="<<tcfg.IsRealTime<<" Priority="<<tcfg.Priority
                    <<" ThreadFrequency="<<tcfg.ThreadFrequency<<" MultipleThreads="<<tcfg.MultipleThreads<<std::endl;
                Logger::Instance()->WriteMsg("Talws", AgentInfo.str(), Logger::ExtraInfo);


            }

        }

        void Start() {
            std::cout<<"Talws: Starting..."<<std::endl;
            for ( std::vector<Thread*>::const_iterator it = Threads.begin(); it != Threads.end(); it++ )
                (*it)->StartThread();
        }

        void Stop() {
            std::cout<<"Talws: Stoping..."<<std::endl;
            for ( std::vector<Thread*>::const_iterator it = Threads.begin(); it != Threads.end(); it++ )
                (*it)->StopThread();
        }

        ~Talws() {
            for ( std::vector<Agent*>::const_iterator it = Agents.begin(); it != Agents.end(); it++ )
                delete (*it);
        }


    private:

        std::vector<Agent*> Agents;

        std::vector<Thread*> Threads;

};

#endif // _talws_h_

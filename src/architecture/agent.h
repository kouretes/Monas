#ifndef _agent_h_
#define _agent_h_ 1

#include <vector>
#include <string>

#include "agentConfig.h"
#include "tools/logger.h"

#include "architecture/narukom/narukom.h"
#include "architecture/narukom/pub_sub/blackboard.h"

#include "architecture/IActivity.h"

#include "hal/threadable.h"
#include "hal/syscall.h"
#include "tools/agentTiming.h"


class Agent : public Thread {

    public:

        Agent ( std::string name, AgentConfig cfg, Narukom* com, std::vector<std::string> activities ) :
            _name(name),
            _cfg(cfg),
            _com(com),
            _blk(name) {

            for ( ActivityNameList::const_iterator it = activities.begin();
                    it != activities.end(); it++ ) 
                _activities.push_back( ActivityFactory::Instance()->CreateObject( (*it) ) );

            for ( ActivList::iterator it = _activities.begin(); 
                    it != _activities.end(); it++ )
                (*it)->Initialize(_com,&_blk);
            
            Freq2Time = (1/(double) _cfg.ThreadFrequency)*1000000;

        }

        virtual ~Agent () {
            std::cout<<"AgentTimings: Avg \t\t\t Var"<<std::endl; //TODO
            std::cout<<"Agent       : "<<agentStats.GetAgentAvgExecTime()<<"\t\t "<<
                agentStats.GetAgentVarExecTime()<<std::endl; //TODO

            //for ( ActivList::iterator it=_activities.begin(); it != _activities.end(); ++it ) 
            //  std::cout<<(*it)->GetName()<<"\t"<<agentStats.GetActivityAvgExecTime(*it)<<
            //      "\t\t"<<agentStats.GetActivityVarExecTime(*it)<<std::endl;

            //TODO delete...
        }

        int Execute () {
            
            unsigned long start = SysCall::_GetCurrentTimeInUSec();
            
            agentStats.StartAgentTiming();

            for ( ActivList::iterator it=_activities.begin(); it != _activities.end(); it++ ) {
                agentStats.StartActivityTiming(*it);
                (*it)->Execute();
                agentStats.StopActivityTiming(*it);
            }

            agentStats.StopAgentTiming();
            
            unsigned long ExecInterval = SysCall::_GetCurrentTimeInUSec() - start;

            if ( ExecInterval > Freq2Time )
                Logger::Instance()->WriteMsg(_name, "Decrease Freq!!!", Logger::ExtraInfo );
            else
                SysCall::_usleep( Freq2Time - ExecInterval );

            return 0;
        }

        typedef std::vector<std::string> ActivityNameList;


    private:

        std::string _name;

        AgentConfig _cfg;

        Narukom* _com;
        Blackboard _blk;

        typedef std::vector<IActivity*> ActivList;

        ActivList _activities; //in execution order 

        AgentTiming agentStats;

        double Freq2Time;

};

#endif // _agent_h_

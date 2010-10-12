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
#include "tools/toString.h"


class Agent : public Thread {

    public:

        Agent ( std::string name, AgentConfig cfg, Narukom* com, std::vector<std::string> activities ) :
            _name(name),
            _cfg(cfg),
            _com(com),
            _blk(name),
            _executions(0) {

            for ( ActivityNameList::const_iterator it = activities.begin();
                    it != activities.end(); it++ )
                _activities.push_back( ActivityFactory::Instance()->CreateObject( (*it) ) );

            for ( ActivList::iterator it = _activities.begin();
                    it != _activities.end(); it++ )
                (*it)->Initialize(_com,&_blk);
            Freq2Time = (1/(double) _cfg.ThreadFrequency)*1000000;
            com->get_message_queue()->add_subscriber(&_blk);
            com->get_message_queue()->add_publisher(&_blk);

        }

        virtual ~Agent () {
           PrintStatistics();

            //for ( ActivList::iterator it=_activities.begin(); it != _activities.end(); ++it )
            //  std::cout<<(*it)->GetName()<<"\t"<<agentStats.GetActivityAvgExecTime(*it)<<
            //      "\t\t"<<agentStats.GetActivityVarExecTime(*it)<<std::endl;

            //TODO delete...
        }

        int Execute () {

            _executions++;

            unsigned long start = SysCall::_GetCurrentTimeInUSec();


            agentStats.StartAgentTiming();

            for ( ActivList::iterator it=_activities.begin(); it != _activities.end(); it++ ) {
                agentStats.StartActivityTiming(*it);
                _blk.process_messages();
                (*it)->Execute();
                _blk.publish_all();
                agentStats.StopActivityTiming(*it);
            }

            agentStats.StopAgentTiming();
            unsigned long ExecInterval = SysCall::_GetCurrentTimeInUSec() - start;

            if ( ExecInterval > Freq2Time )
                Logger::Instance().WriteMsg(_name, "Decrease Freq!!!", Logger::ExtraInfo );
            else
                SysCall::_usleep( Freq2Time - ExecInterval );

            if ( ! (_executions % _cfg.StatsCycle) ){
                PrintStatistics();
            }

            return 0;
        }

        typedef std::vector<std::string> ActivityNameList;


    private:

        void PrintStatistics() {
            std::string StatMessage( "Avg: "+_toString(agentStats.GetAgentAvgExecTime())
                    +" Var: "+_toString(agentStats.GetAgentVarExecTime())+"\n"  );
            for ( ActivList::const_iterator it = _activities.begin();
                    it != _activities.end(); it++ ) {
                StatMessage += "Activity: " + (*it)->GetName() ;
                StatMessage += " Avg: " + _toString( agentStats.GetActivityAvgExecTime( (*it) ));
                StatMessage += " Var: " + _toString( agentStats.GetActivityVarExecTime( (*it) ))+ "\n";
            }
            Logger::Instance().WriteMsg( _name, StatMessage, Logger::Info );
        }

        std::string _name;

        AgentConfig _cfg;

        Narukom* _com;
        Blackboard _blk;

        typedef std::vector<IActivity*> ActivList;

        ActivList _activities; //in execution order

        AgentTiming agentStats;

        double Freq2Time;

        unsigned int _executions;

};

#endif // _agent_h_

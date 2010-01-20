#ifndef _agent_h_
#define _agent_h_ 1

#include <vector>
#include <string>

#include "architecture/blackboard.h"
#include "architecture/provider.h"
#include "architecture/providerHandler.h"

#include "hal/threadable.h"
#include "hal/syscall.h"
#include "tools/agentTiming.h"



class Agent : public Threadable  {

    public:

        Agent ( std::vector<std::string> providerList ) {
            blk = new Blackboard;
            providers = hand.LoadProviders ( blk , providerList );
        }

        virtual ~Agent () {
            std::cout<<"AgentTimings: Avg \t\t\t Var"<<std::endl;
            std::cout<<"Agent       : "<<agentStats.GetAgentAvgExecTime()<<"\t\t "<<
                agentStats.GetAgentVarExecTime()<<std::endl;

            for ( ProvList::iterator it=providers.begin(); it != providers.end(); ++it ) 
              std::cout<<(*it)->GetName()<<"\t"<<agentStats.GetProviderAvgExecTime(*it)<<
                  "\t\t"<<agentStats.GetProviderVarExecTime(*it)<<std::endl;
            hand.UnloadProviders (blk); //,providers );
        }

        int ThreadMain () {
            //Update frame
            
            
            agentStats.StartAgentTiming();
            

            for ( ProvList::iterator it=providers.begin(); it != providers.end(); ++it ) {
                agentStats.StartProviderTiming(*it);
                (*it)->Update();
                agentStats.StopProviderTiming(*it);
            }

            agentStats.StopAgentTiming();


            return 0;
        }


    private:

        ProviderHandler hand;

        Blackboard * blk;

        typedef std::vector<Provider *> ProvList;

        ProvList providers; //in execution order 

        AgentTiming agentStats;

};

#endif // _agent_h_

#ifndef  _agentTiming_h_
#define  _agentTiming_h_ 1


#include "hal/syscall.h"
#include "architecture/provider.h"

#include <string>
#include <map>

#include "tools/statMovingAverage.h"

#include "tools/stopWatch.h"


class AgentTiming 
{
    public:

        AgentTiming () { 
            // Get Alpha from XML

        }

        void StartAgentTiming () {
            agentStats.StartTiming();
        }

        double  StopAgentTiming() {
            return agentStats.StopTiming();
        }


        double GetAgentAvgExecTime () {
            return agentStats.GetAvg();
        }

        double GetAgentVarExecTime () {
            return agentStats.GetVar();
        }

        void StartProviderTiming ( Provider * a ) {
            providerStats[a].StartTiming();
        }

        double StopProviderTiming ( Provider * a ) {
            return providerStats[a].StopTiming();
        }

        double GetProviderAvgExecTime ( Provider * a ) {
            return providerStats[a].GetAvg();
        }

        double GetProviderVarExecTime ( Provider * a ) {
            return providerStats[a].GetVar();
        }

    private:

        StopWatch<> agentStats;

        std::map<Provider*,StopWatch<> > providerStats;

}; 



#endif   // _agentTiming_h_  

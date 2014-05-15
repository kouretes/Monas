#ifndef  AGENTTIMING_H
#define  AGENTTIMING_H


#include "hal/syscall.h"

#include <string>
#include <map>

#include "core/elements/math/stat/MovingAverage.hpp"

#include "core/architecture/time/StopWatch.hpp"

class IActivity;
namespace KSystem
{
    class AgentTiming
    {
    public:

        AgentTiming ()
        {
            //TODO Get Alpha from XML
        }

        void StartAgentTiming ()
        {
            agentStats.StartTiming();
        }

        double  StopAgentTiming()
        {
            return agentStats.StopTiming();
        }


        double GetAgentAvgExecTime ()
        {
            return agentStats.GetAvg();
        }

        double GetAgentVarExecTime ()
        {
            return agentStats.GetVar();
        }

        void StartActivityTiming ( IActivity * a )
        {
            activityStats[a].StartTiming();
        }

        double StopActivityTiming ( IActivity * a )
        {
            return activityStats[a].StopTiming();
        }

        double GetActivityAvgExecTime ( IActivity * a )
        {
            return activityStats[a].GetAvg();
        }

        double GetActivityVarExecTime ( IActivity * a )
        {
            return activityStats[a].GetVar();
        }

    private:

        Time::StopWatch<> agentStats;

        std::map<IActivity*, Time::StopWatch<> > activityStats;

    };
}



#endif   // _agentTiming_h_

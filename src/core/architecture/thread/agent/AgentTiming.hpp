#ifndef  _agentTiming_h_
#define  _agentTiming_h_ 1


#include "core/hal/syscall.h"

#include <string>
#include <map>

#include "tools/statMovingAverage.h"

#include "tools/stopWatch.h"

class IActivity;

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

	StopWatch<> agentStats;

	std::map<IActivity*, StopWatch<> > activityStats;

};



#endif   // _agentTiming_h_

#ifndef FLOODER_H
#define FLOODER_H
#include "core/include/IActivity.hpp"
#include "tools/profiler.hpp"

ACTIVITY_START

class Pipe: public IActivity
{
public:
	Pipe(Blackboard&b, XmlManager&x) : IActivity(b, x), p("Pipe") {};
	int ACTIVITY_VISIBLE IEX_DIRECTIVE_HOT Execute();
	KProfiling::profiler p;

	void ACTIVITY_VISIBLE UserInit();
	void ACTIVITY_VISIBLE Reset();
	std::string ACTIVITY_VISIBLE GetName()
	{
		return "Pipe";
	}
};
ACTIVITY_END

ACTIVITY_START
class Drain: public IActivity
{
public:
	Drain(Blackboard&b, XmlManager&x) : IActivity(b, x), p("Drain") {}
	int ACTIVITY_VISIBLE IEX_DIRECTIVE_HOT Execute();
	KProfiling::profiler p;
	void ACTIVITY_VISIBLE UserInit();
	void ACTIVITY_VISIBLE Reset();
	std::string  ACTIVITY_VISIBLE GetName()
	{
		return "Drain";
	}


};
ACTIVITY_END

#endif


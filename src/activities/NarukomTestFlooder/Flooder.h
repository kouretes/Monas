#ifndef FLOODER_H
#define FLOODER_H
#include "architecture/executables/IActivity.h"
#include "tools/profiler.hpp"

ACTIVITY_START

class Pipe: public IActivity
{
public:
	Pipe(Blackboard&b, XmlNode&x) : IActivity(b, x), p("Pipe") {};
	int ACTIVITY_VISIBLE IEX_DIRECTIVE_HOT Execute();
	KProfiling::profiler p;

	void ACTIVITY_VISIBLE UserInit();
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
	Drain(Blackboard&b, XmlNode&x) : IActivity(b, x), p("Drain") {}
	int ACTIVITY_VISIBLE IEX_DIRECTIVE_HOT Execute();
	KProfiling::profiler p;
	void ACTIVITY_VISIBLE UserInit();
	std::string  ACTIVITY_VISIBLE GetName()
	{
		return "Drain";
	}


};
ACTIVITY_END

#endif


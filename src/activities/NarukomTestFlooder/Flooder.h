#ifndef FLOODER_H
#define FLOODER_H
#include "architecture/IActivity.h"
#include "tools/profiler.hpp"

//#define NUMBER_OF_SENSORS 46//TODO Please check the number devices
// Use DCM proxy

//#define USE_POINTERS


class Pipe: public IActivity
{
	public:
		Pipe() :p("Pipe") {}
		int Execute();
		KProfiling::profiler p;

		void UserInit();
		std::string GetName()
		{
			return "Pipe";
		}
};

class Drain: public IActivity
{
	public:
		Drain() : p("Drain"){}
		int Execute();
		KProfiling::profiler p;
		void UserInit();
		std::string GetName()
		{
			return "Drain";
		}


};

#endif


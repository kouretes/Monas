//Based on profiler by: Copyright (c) 2005 Christopher Diggins

#ifndef __PROFILER_HPP__
#define __PROFILER_HPP__






#ifdef DEBUG
#define KPROFILING_ENABLED

#endif

#ifdef UNUSED
#elif defined(__GNUC__)
# define UNUSED(x) UNUSED_ ## x __attribute__((unused))
#elif defined(__LCLINT__)
# define UNUSED(x) /*@unused@*/ x
#else
# define UNUSED(x) x
#endif




namespace KProfiling {

  using namespace std;

  struct empty_logging_policy
  {
    static void on_start(string UNUSED(name) ) { };
    static void on_stop(string UNUSED(name), double UNUSED(sec), bool UNUSED(underflow)) { };
  };
   class empty_stats_policy
  {
  	protected:
    static void on_stop(string UNUSED(name), double UNUSED(sec), bool UNUSED(underflow)) { }
    static void on_report() { }
    static void on_clear() { }
  };
#ifdef KPROFILING_ENABLED

#include <vector>
#include "smart_timer.hpp"
#include "tools/logger.h"
#include "toString.h"
#include <sstream>

  struct cerr_logging_policy
  {
    static void on_start(string name) {
		Logger::Instance().WriteMsg("KProf:", "starting profile "+name, Logger::ExtraInfo);
    }
    static void on_stop(string name, double sec, bool underflow) {
      Logger::Instance().WriteMsg("KProf:", "stopping profile "+name, Logger::ExtraInfo);
      Logger::Instance().WriteMsg("KProf:", "time profile "+_toString(sec), Logger::ExtraInfo);

      if (underflow) Logger::Instance().WriteMsg("KProf:", "Underflow Occured", Logger::Warning);
    }
  };





  struct counted_sum : pair<int, double> {
    counted_sum() : pair<int, double>(0, 0) { }
    counted_sum(int x, double y) : pair<int, double>(x, y) { }
    void operator+=(double x) {
      first++;
      second += x;
    }
  };





  class default_stats_policy
  {
  	typedef map<string, counted_sum> stats_map;
    stats_map stats;
	protected:
    void on_stop(string name, double sec, bool UNUSED(underflow)) {
      // underflow and overflow are sticky.
        stats[name] += sec;
    }

    void on_report()const {
    	stringstream s;
		s <<endl
		<<"        profile name"
        <<"     total elapsed"
        <<"       entry count"
        <<"           average" << endl;

      for (stats_map::const_iterator i=stats.begin(); i != stats.end(); i++)
      {
        string sName = i->first;
        int nCount = i->second.first;
        double dTotal = i->second.second;
        double dAvg = dTotal / nCount;
        string Tunits, AUnits;
        Tunits="sec"; AUnits="sec";
        if(dTotal<1){
			 dTotal*=1000;
			 Tunits="msec";
        }
        if(dTotal<1){
			 dTotal*=1000;
			 Tunits="usec";
        }
		if(dAvg<1){
			dAvg*=1000;
			AUnits="msec";
        }
        if(dAvg<1){
			 dAvg*=1000;
			 AUnits="usec";
        }

        s
          << fixed
          << setw(20) <<sName
          << setw(14) << dTotal <<setw(4)<< Tunits
          << setw(18) << nCount
          << setw(14) <<dAvg << setw(4) <<AUnits << endl;
      }
      Logger::Instance().WriteMsg("KProf:", s.str(), Logger::ExtraInfo);
    }
    void on_clear() {stats.clear();}
  };

    template<typename logging_policy, typename stats_policy, typename timer_t>
    class basic_profiler : protected stats_policy {
	  private:
	  	struct frame {
			string name;
			timer_t t;
			double residual;
	  	};
	  	const string name;
	  	vector<frame> callstack;
	  	int repcount;
      public:
        basic_profiler(char const* s) : name(s) { repcount=0; };
        ~basic_profiler() {
        	while(callstack.size()>0)
			{
				pop();
			}
        }
        void push(char const *s){
        	frame f;
        	f.name=s;
        	f.residual=0;
        	f.t.restart();
        	callstack.push_back(f);
        }
        void pop( ){
        	frame &f=callstack.back();

        	double time= f.t.elasped()-f.residual;
        	double res=f.residual;
        	stats_policy::on_stop(f.name,time,time<f.t.elasped_min());
        	logging_policy::on_stop(f.name,time,time<f.t.elasped_min());

        	callstack.pop_back();
        	if(callstack.size()>0)
				callstack.back().residual+=res;
        }

		void generate_report(int every=1000) {
			repcount++;
			if(every>repcount)
				return;
			repcount=0;
			Logger::Instance().WriteMsg("KProf:","-- Profiler Report: "+name+" --", Logger::ExtraInfo);
			stats_policy::on_report();
			clear();
        }
        void clear() { stats_policy::on_clear();}
    };
	template<typename _profiler>
	class basic_profiling_scope
    {
	private:
		_profiler &pr;
	public:
    	basic_profiling_scope(_profiler &p, char const *s) : pr(p) { pr.push(s); }
    	~basic_profiling_scope() { pr.pop(); }
    	typedef basic_profiler<empty_logging_policy, default_stats_policy, smart_timer> profiler;
		typedef basic_profiling_scope<profiler> profiling_scope;
    };

  #else
	struct nulltimer {};
    template<typename logging_policy, typename stats_policy, typename timer_t>
    class basic_profiler {
      public:
        basic_profiler(char const* UNUSED(s) ) { }
        static void push(char const* UNUSED(s)) { }
        static void pop() { }
        static void generate_report(int UNUSED(i)=0) { }
        static void clear() {}
    };

	template<typename _profiler>
	class basic_profiling_scope
    {
	public:
    	basic_profiling_scope(_profiler & UNUSED(p), char const * UNUSED(s)) {};
    	~basic_profiling_scope() { }

    };
    typedef basic_profiler<empty_logging_policy, empty_stats_policy, nulltimer> profiler;
	typedef basic_profiling_scope<profiler> profiling_scope;

  #endif





} // namespace prof

#ifdef KPROFILING_ENABLED
  #define KPROF_EXECUTE(prof,TKN) { KProfiling::profiling_scope _kprofscope_(prof,#TKN); TKN; }
  #define KPROF_SCOPE(prof,TKN) KProfiling::profiling_scope _kprofscope_(prof,#TKN);
#else
  #define KPROF_EXECUTE(prof,TKN) TKN;
  #define KPROF_SCOPE(prof,TKN)
#endif


#endif // #ifndef __PROFILER_HPP__

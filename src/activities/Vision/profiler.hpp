
// Copyright (c) 2005 Christopher Diggins
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// Disclaimer: Not a Boost library

#ifndef BOOST_PROFILER_HPP_INCLUDED
#define BOOST_PROFILER_HPP_INCLUDED

#include <iostream>
#include "high_resolution_timer.hpp"

namespace boost {
namespace prof {

  using namespace std;

  struct empty_logging_policy
  {
    static void on_start(string name) { };
    static void on_resume(string name) { };
    static void on_pause(string name, double sec) { };
    static void on_stop(string name, double sec, bool underflow, bool overflow) { };
  };

  struct default_logging_policy
  {
    static void on_start(string name) {
      cerr << "starting profile " << name << endl;
    }
    static void on_resume(string name) {
		cerr << "resuming profile " << name << endl;
    }
    static void on_pause(string name) {
      cerr << "pausing profile " << name << endl;
    }
    static void on_stop(string name, double sec, bool underflow, bool overflow) {
      cerr << "stopping profile " << name;
      cerr << " time elapsed = " << sec;
      if (underflow) cerr << " underflow occurred";
      if (overflow) cerr << " overflow occurred";
      cerr << endl;
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

  typedef map<string, counted_sum> stats_map;

  struct empty_stats_policy
  {
    static void on_stop(string name, double sec, bool underflow, bool overflow) { }
    static void on_report() { }
  };

  struct default_stats_policy
  {
    static stats_map stats;

    static void on_stop(string name, double sec, bool underflow, bool overflow) {
      // underflow and overflow are sticky.
        stats[name] += sec;
    }
    static void on_report() {
      cerr
        << "profile name" << '\t'
        << "total elapsed" << '\t'
        << "entry count" << '\t'
        << "average" << endl;

      for (stats_map::iterator i=stats.begin(); i != stats.end(); i++)
      {
        string sName = i->first;
        int nCount = i->second.first;
        double dTotal = i->second.second;
        double dAvg = dTotal / nCount;
        cerr
          << fixed
          << sName << '\t'
          << dTotal << '\t'
          << nCount << '\t'
          << dAvg << endl;
      }
    }
  };

  stats_map default_stats_policy::stats;

  #ifndef PROFILING_OFF
    template<typename logging_policy, typename stats_policy, typename timer_t>
    class basic_profiler {
      public:
        basic_profiler(char const* s = "")
          : name(s), timing(true), elapsed(0.0), underflow(false), overflow(false)
        {
          logging_policy::on_start(name);
          timer.restart();
        }
        ~basic_profiler() {
          if (timing) {
            stop();
          }
        }
        void stop() {
          assert(timing);
          double tmp = timer.elapsed();
          if (tmp <= timer.elapsed_min()) {
            underflow = true;
          }
          if (tmp >= timer.elapsed_max()) {
            overflow = true;
          }
          tmp += elapsed;
          elapsed = 0.0;
          timing = false;
          logging_policy::on_stop(name, tmp, underflow, overflow);
          stats_policy::on_stop(name, tmp, underflow, overflow);
        }
        void restart() {
          timing = true;
          elapsed = 0.0;
          logging_policy::on_restart(name);
          timer.restart();
        }
        void resume() {
          timing = true;
          logging_policy::on_resume(name);
          timer.restart();
        }
        void pause() {
          double tmp = timer.elapsed();
          if (tmp <= timer.elapsed_min()) {
            underflow = true;
          }
          if (tmp >= timer.elapsed_max()) {
            overflow = true;
          }
          elapsed += tmp;
          timing = false;
          logging_policy::on_pause(name);
          timer.pause();
        }
        static void generate_report() {
          stats_policy::on_report();
        }
      private:
        bool underflow;
        bool overflow;
        bool timing;
        string name;
        double elapsed;
        timer_t timer;
    };
  #else
    template<typename logging_policy, typename stats_policy, typename timer_t>
    class basic_profiler {
      public:
        basic_profiler(char const* s = "") { }
        void stop() { }
        void restart() { }
        void resume() { }
        void pause() { }
        static void generate_report() { }
    };
  #endif


  typedef basic_profiler<empty_logging_policy, default_stats_policy, high_resolution_timer> profiler;

} // namespace prof
} // namespace boost

#ifndef BOOST_PROFILING_OFF
  #define BOOST_PROFILE(TKN) { boost::prof::profiler p_(#TKN); TKN; }
  #define BOOST_PROFILER(TKN) boost::prof::profiler p_(#TKN);
#else
  #define BOOST_PROFILE(TKN) TKN;
  #define BOOST_PROFILER(TKN)
#endif


#endif // #ifndef BOOST_PROFILER_HPP_INCLUDED

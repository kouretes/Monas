#ifndef __SMART_TIMER_HPP__
#define __SMART_TIMER_HPP__


#if defined(__GNUC__ )  && (defined(linux) || defined(__linux) || defined(__linux__))

#include <time.h>

#endif

#if defined(__timespec_defined) //GNUC Timespec available ! YEAH :)

//============================================
#if defined(CLOCK_THREAD_CPUTIME_ID)

#define SMART_TIMER_TIME_ID CLOCK_THREAD_CPUTIME_ID

#elif defined(SMART_TIMER_TIME_ID)

#define SMART_TIMER_TIME_ID CLOCK_PROCESS_CPUTIME_ID

#else

#define SMART_TIMER_TIME_ID 0

#endif

#include <limits>
namespace KSystem {
class smart_timer
{
  private:
    typedef struct timespec timertype;
    timertype t;
    static const double NSECPERSEC = 1000000000.0;
    static double resultify(timertype const& t) { return t.tv_sec+ t.tv_nsec/NSECPERSEC;};
    static timertype timertypediff(timertype const& start, timertype  const& end)
    {
      timertype temp;
      if ((end.tv_nsec-start.tv_nsec)<0) {
        temp.tv_sec = end.tv_sec-start.tv_sec-1;
        temp.tv_nsec = NSECPERSEC+end.tv_nsec-start.tv_nsec;
      } else {
        temp.tv_sec = end.tv_sec-start.tv_sec;
        temp.tv_nsec = end.tv_nsec-start.tv_nsec;
      }
      return temp;
    }
  public:

    smart_timer() { restart();};

    void restart()
    {
      clock_gettime(SMART_TIMER_TIME_ID,&t);
    }
    //Maximum value for elasped
    static double elapsed_max()
    {
        timertype temp;
        temp.tv_sec=std::numeric_limits<time_t>::max();
        temp.tv_nsec=std::numeric_limits<long>::max();
        return resultify(temp);
    }
     // return elapsed time in seconds
    double elapsed() const {
      timertype temp;
      clock_gettime(SMART_TIMER_TIME_ID,&temp);

      return resultify(timertypediff(t,temp));
    }

    // return minimum value for elapsed()
    static double elapsed_min()
    {
       timertype temp;
       clock_getres(SMART_TIMER_TIME_ID,&temp);
       return resultify(temp);
    }


};
}

#else


#include <boost/config.hpp>
#include <boost/timer.hpp>

#if defined(BOOST_WINDOWS)

#include <stdexcept>
#include <limits>
#include <windows.h>


///////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Hartmut Kaiser
// Copyright (c) 2005 Christopher Diggins
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

//  high_resolution_timer
//      A timer object measures elapsed time.
//      CAUTION: Windows only!
//
///////////////////////////////////////////////////////////////////////////////
namespace KSystem
{
class smart_timer
{
public:
    // ctor
    smart_timer()
    {
        start_time.QuadPart = 0;
        frequency.QuadPart = 0;

        if (!QueryPerformanceFrequency(&frequency))
            throw std::runtime_error("Couldn't acquire frequency");

        restart();
    }

    // restart timer
    void restart()
    {
        t.restart();
        if (!QueryPerformanceCounter(&start_time))
            throw std::runtime_error("Couldn't initialize start_time");
    }

    // return elapsed time in seconds
    double elapsed() const
    {
        LARGE_INTEGER now;
        if (!QueryPerformanceCounter(&now))
            throw std::runtime_error("Couldn't get current time");

        // QueryPerformanceCounter() workaround
        // http://support.microsoft.com/default.aspx?scid=kb;EN-US;q274323
        double d1 = double(now.QuadPart - start_time.QuadPart) / frequency.QuadPart;
        double d2 = t.elapsed();
        return ((d1 - d2) > 0.5) ? d2 : d1;
    }

    // return estimated maximum value for elapsed()
    static double elapsed_max()
    {
        return (double((std::numeric_limits<LONGLONG>::max)())
            - double(start_time.QuadPart)) / double(frequency.QuadPart);
    }

    // return minimum value for elapsed()
    static double elapsed_min()
    {
        return 1.0 / frequency.QuadPart;
    }

private:
    boost::timer t; // backup in case of QueryPerformanceCounter() bug
    LARGE_INTEGER start_time;
    LARGE_INTEGER frequency;
};
}

#else  // defined(BOOST_WINDOWS)
namespace KSystem{

  typedef boost:timer smart_timer;
}
#endif

#endif

#endif  //__SMART_TIMER_HPP__

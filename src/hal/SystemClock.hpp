#ifndef SYSTEMCLOCK_HPP
#define SYSTEMCLOCK_HPP
#include "core/architecture/time/IClock.hpp"

#if defined(__GNUC__ )  && (defined(linux) || defined(__linux) || defined(__linux__))

#include <time.h>

#endif

#if defined(__timespec_defined) && defined(__clockid_t_defined) //GNUC Timespec available ! YEAH :)

namespace KSystem
{
	namespace Time
	{
        namespace _SystemClockGeneric
        {

            class SystemClockGeneric : public IClock
            {
                private:
                typedef struct timespec Syclktype;
                static const long NSECPERSEC = 1000000000L;
                clockid_t clk_id;
                public:

                SystemClockGeneric(clockid_t id ) {clk_id=id;};
                virtual ClockValue  now()
                {
                    ClockValue cv;
                    Syclktype clk;
                    clock_gettime(clk_id, &clk);

                    //std::cout<<"ClockNow:"<<clk_id<<" "<<(clk_id==CLOCK_THREAD_CPUTIME_ID)<<std::endl;
                    //std::cout<<"ClockNowSec:"<<clk.tv_sec<<std::endl;
                    //std::cout<<"ClockNowNSec:"<<clk.tv_nsec<<std::endl;
                    cv.p=clk.tv_sec*ClockValue::TPS;
                    //std::cout<<"SecondsOnly:"<<cv.p<<std::endl;
                    if(ClockValue::TPS>NSECPERSEC )
                        cv.p+=clk.tv_nsec*(ClockValue::TPS/NSECPERSEC);
                    else if(ClockValue::TPS<NSECPERSEC )
                        cv.p+=clk.tv_nsec/(NSECPERSEC/ClockValue::TPS);
                    else
                        cv.p+=clk.tv_nsec;
                    //std::cout<<"Full:"<<cv.p<<std::endl;
                    return cv;
                }
                IClock * Clone() const { return new SystemClockGeneric(*this);};


            };
		}
	}

}
#if !defined(CLOCK_MONOTONIC) && defined( CLOCK_REALTIME)
namespace KSystem
{
    namespace Time
    {
        class SystemClock :  public _SystemClockGeneric::SystemClockGeneric
        {
            public:

            SystemThreadClock() : _SystemClockGeneric::SystemClockGeneric(CLOCK_REALTIME) {};

        };
    }
}
#elif defined(CLOCK_MONOTONIC)
namespace KSystem
{
    namespace Time
    {
        class SystemClock :  public _SystemClockGeneric::SystemClockGeneric
        {
            public:
            SystemClock() : _SystemClockGeneric::SystemClockGeneric(CLOCK_MONOTONIC) {};
        };
    }
}
#endif




#if defined(CLOCK_PROCESS_CPUTIME_ID)

namespace KSystem
{
    namespace Time
    {
        class SystemProcessClock :  public _SystemClockGeneric::SystemClockGeneric
        {
            public:
            SystemProcessClock() : _SystemClockGeneric::SystemClockGeneric(CLOCK_PROCESS_CPUTIME_ID) {};

        };
    }
}
#else
    typedef KSystem::Time::SystemClock SystemProcessClock;

#endif


#if defined(CLOCK_THREAD_CPUTIME_ID)

namespace KSystem
{
    namespace Time
    {
        class SystemThreadClock : public _SystemClockGeneric::SystemClockGeneric
        {
            public:
            SystemThreadClock() : _SystemClockGeneric::SystemClockGeneric(CLOCK_THREAD_CPUTIME_ID) {};

        };
    }
}
#else
    typedef KSystem::Time::SystemProcessClock SystemThreadClock;

#endif




#endif




#endif //CLOCK_HPP

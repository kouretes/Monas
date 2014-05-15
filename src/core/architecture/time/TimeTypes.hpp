#ifndef TIMETYPES_HPP
#define TIMETYPES_HPP
#include <stdint.h>
#include "boost/date_time/posix_time/posix_time_duration.hpp"
#include "boost/date_time/posix_time/posix_time_io.hpp"
#include "boost/date_time/posix_time/time_parsers.hpp"
#include "boost/date_time/posix_time/time_formatters.hpp"
namespace KSystem
{

    namespace Time
    {
        using namespace boost::posix_time;
        typedef boost::posix_time::ptime TimeAbsolute;
        typedef boost::posix_time::time_duration TimeDuration;

        class ClockValue {
            public:
            typedef int64_t clockvalue_type;
            enum cconcsts{ CVTPS=1000000000LL}; //Ticks per second, nanosecond resolution
            clockvalue_type p;


            operator TimeDuration() const
            {
                clockvalue_type ttls;
                long m,s,rest;
                ttls=p/CVTPS;
                s=ttls % 60;
                m=ttls/60;
                rest=ttls % CVTPS;
                if(TimeDuration::ticks_per_second() > CVTPS)
                    rest=rest*(TimeDuration::ticks_per_second()/CVTPS);
                else if(TimeDuration::ticks_per_second() < CVTPS)
                    rest=rest/(CVTPS/TimeDuration::ticks_per_second() );
                return TimeDuration(0,m,s,rest);
            }
            ClockValue operator -(ClockValue const& rhs) const
            {
                ClockValue res;
                res.p=p;
                res.p-=rhs.p;
                return res;
            }
             ClockValue operator +(ClockValue const& rhs) const
            {
                ClockValue res;
                res.p=p;
                res.p+=rhs.p;
                return res;
            }

            float toFloat() const
            {
                float r=p;
                //std::cout<<"Tofloat:"<<p<<std::endl;
                r/=CVTPS;
                return r;

            }



        };
    }

}



#endif // TIMETYPES_HPP

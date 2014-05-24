#ifndef TIMETYPES_HPP
#define TIMETYPES_HPP
#include <stdint.h>
#include "BasicTimeStamp.hpp"
//#include "boost/date_time/posix_time/posix_time_duration.hpp"
//#include "boost/date_time/posix_time/posix_time_io.hpp"
//#include "boost/date_time/posix_time/time_parsers.hpp"
//#include "boost/date_time/posix_time/time_formatters.hpp"
namespace KSystem
{

    namespace Time
    {
        typedef BasicTimeStamp<int32_t,1000000L> TimeStamp;
        typedef BasicTimeStamp<int64_t,1000000L> TimeAbsolute;
        typedef TimeAbsolute TimeDuration;
        typedef BasicTimeStamp<int64_t,1000000000L> ClockValue;
        template<typename C,typename R>
        R wrap(C const& k){        return k.wrapTo();       };
    }

}



#endif // TIMETYPES_HPP

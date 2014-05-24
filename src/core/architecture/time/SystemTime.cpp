#include "SystemTime.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#ifdef BOOST_DATE_TIME_HAS_HIGH_PRECISION_CLOCK
#define BOOSTGETSYSTEMTIMENOW boost::posix_time::microsec_clock::universal_time()
#else
#define BOOSTGETSYSTEMTIMENOW boost::posix_time::second_clock::universal_time()
#endif

namespace KSystem
{

	namespace Time
	{

		namespace SystemTime
		{

			TimeAbsolute now() {
			    static boost::posix_time::ptime epoch=BOOSTGETSYSTEMTIMENOW;
			    TimeAbsolute a;
			    boost::posix_time::time_duration d= BOOSTGETSYSTEMTIMENOW-epoch;
			    if(d.ticks_per_second()<TimeAbsolute::TPS)
			        a.p=d.ticks()*(TimeAbsolute::TPS/d.ticks_per_second());
                else if (d.ticks_per_second()>TimeAbsolute::TPS)
			        a.p=d.ticks()*(d.ticks_per_second()/TimeAbsolute::TPS);
                else
                    a.p=d.ticks();
                return a;
            }
		}

	}

}


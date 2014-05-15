#include "SystemTime.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>

namespace KSystem
{

	namespace Time
	{

		namespace SystemTime
		{
#ifdef BOOST_DATE_TIME_HAS_HIGH_PRECISION_CLOCK
			TimeAbsolute now() {return  boost::posix_time::microsec_clock::universal_time(); };
#else
            TimeAbsolute now() {return  boost::posix_time::second_clock::universal_time(); };
#endif // BOOST_DATE_TIME_HAS_HIGH_PRECISION_CLOCK
		}

	}

}


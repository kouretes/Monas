#ifndef SYSTEMTIME_HPP
#define SYSTEMTIME_HPP
#include "TimeTypes.hpp"

namespace KSystem
{

	namespace Time
	{

		namespace SystemTime
		{

			TimeAbsolute now();
			//TimeStamp timestamp(){return SystemTime::now().wrapTo<TimeStamp>();};
			template<typename T>
            TimeAbsolute unwrap(T const& a)
            {
                return now().unwrap(a);
            }
		}

	}

}

#endif //SYSTEMTIME_HPP

#ifndef SYSTEMCLOCKPROVIDER_HPP
#define SYSTEMCLOCKPROVIDER_HPP
#include "IClock.hpp"
namespace KSystem
{
	namespace Time
	{
		namespace SystemClockProvider
		{
			IClock* getSystemClock(); 
			IClock* getProcessClock(); 
			IClock* getThreadClock();
		}
	}


}

#endif //SYSTEMCLOCKPROVIDER_HPP

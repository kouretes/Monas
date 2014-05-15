#ifndef SYSTEMCLOCKPROVIDER_HPP
#define SYSTEMCLOCKPROVIDER_HPP
#include "SystemClockProvider.hpp"
#include "hal/SystemClock.hpp"
namespace KSystem
{
	namespace Time
	{
		namespace SystemClockProvider
		{
			IClock* getSystemClock() {return new SystemClock();} ;
			IClock* getProcessClock() {return new SystemProcessClock();} ;
			IClock* getThreadClock() {return new SystemThreadClock();} ;
		}
	}


}

#endif //SYSTEMCLOCKPROVIDER_HPP

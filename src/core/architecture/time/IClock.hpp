#ifndef ICLOCK_HPP
#define ICLOCK_HPP
#include "TimeTypes.hpp"
#include "core/elements/IClonableCRTP.hpp"
namespace KSystem
{
	namespace Time
	{

		class IClock
		{
			public:

			virtual ClockValue now()=0;
			virtual IClock * Clone() const =0;
			virtual ~IClock() {};
		};

	}


}

#endif //ICLOCK_HPP

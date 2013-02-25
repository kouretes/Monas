#ifndef _THREADCONFIG_HPP_
#define  _THREADCONFIG_HPP_ 1
namespace KSystem
{

	class ThreadConfig
	{

	public:

		bool IsRealTime;

		int Priority;

		float ThreadPeriod;

	};
}




#endif  _THREADCONFIG_HPP_

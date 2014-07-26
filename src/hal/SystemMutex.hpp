#ifndef SYSTEMMUTEX_HPP
#define SYSTEMMUTEX_HPP

#include <boost/thread/mutex.hpp>
namespace KSystem
{

	//class SystemMutex  : public boost::mutex { };
	typedef boost::mutex SystemMutex;

};

#endif // SYSTEMMUTEX_HPP

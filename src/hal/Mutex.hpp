#ifndef MUTEX_HPP
#define MUTEX_HPP

#include <boost/thread/mutex.hpp>
namespace KSystem
{

	class Mutex  : public boost::mutex { };

};

#endif // _mutex_h_

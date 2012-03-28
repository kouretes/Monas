#ifndef _mutex_h_
#define _mutex_h_ 1

#include <boost/thread/mutex.hpp>
namespace KSystem {

class Mutex  : public boost::mutex{ };

};

#endif // _mutex_h_

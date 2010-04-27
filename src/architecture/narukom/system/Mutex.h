#ifndef MUTEX_H
#define MUTEX_H
#include <boost/thread/mutex.hpp>

/** Mutex container class, used by Lock. 
	\ingroup threading */
class Mutex 
{
public:
	Mutex();
	~Mutex();

	virtual void Lock() ;
	virtual void Unlock() ;

private:
  boost::mutex mx;
};



#endif // _SOCKETS_Mutex_H


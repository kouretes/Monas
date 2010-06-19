
#include "Mutex.h"


Mutex::Mutex()
{
 mx = new boost::mutex;
 mx->unlock();
 
}


Mutex::~Mutex()
{
 mx->unlock();
}

bool Mutex::try_lock()
{
	return mx->try_lock();
}
void Mutex::lock()
{
mx->lock();
}


void Mutex::unlock()
{
mx->unlock();
}



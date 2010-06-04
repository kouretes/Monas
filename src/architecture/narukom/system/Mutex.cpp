
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

void Mutex::Lock() 
{
mx->lock();
}


void Mutex::Unlock() 
{
mx->unlock();
}



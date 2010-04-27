
#include "Mutex.h"


Mutex::Mutex()
{
 mx.unlock();
}


Mutex::~Mutex()
{

}


void Mutex::Lock() 
{
mx.lock();
}


void Mutex::Unlock() 
{
mx.unlock();
}



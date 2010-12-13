#ifndef _mutex_h_
#define _mutex_h_ 1

#include <pthread.h>

class Mutex  {

    public:

        Mutex(){    pthread_mutex_init(&mtex, NULL);};

	virtual ~Mutex() {    pthread_mutex_destroy(&mtex);};

	virtual void lock() const {    pthread_mutex_lock(&mtex);};

	virtual void unlock() const {    pthread_mutex_unlock(&mtex);};

	virtual bool try_lock() const { return pthread_mutex_trylock(&mtex)==0;};


    private:

        mutable pthread_mutex_t mtex;
};

#endif // _mutex_h_

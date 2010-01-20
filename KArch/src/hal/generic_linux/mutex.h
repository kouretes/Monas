#ifndef _mutex_h_
#define _mutex_h_ 1

#include <pthread.h>

class Mutex {

    public:

        Mutex();

        virtual ~Mutex();

        virtual void lock() const;

        virtual void release() const;

    private:

        mutable pthread_mutex_t mtex;
};

#endif // _mutex_h_

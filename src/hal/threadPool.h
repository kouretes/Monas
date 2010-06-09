#ifndef THREADPOOL_H_
#define THREADPOOL_H_ 1

#include "architecture/IExecutable.h"
#include "architecture/IActivity.h"

#include "../external/threadpool-0_2_5-src/threadpool/boost/threadpool.hpp"

class ThreadPool {

    public:

        //            ThreadPool ( int numOfThreads ); //TODO add thread config

        ThreadPool () : _pool(10) {
            ;
        }

        bool Enqueue ( IExecutable* job ) {
            return _pool.schedule ( boost::bind ( &IExecutable::Execute, job ) );
        }
        
        bool Enqueue ( IActivity* job, volatile int* running ) {
            return _pool.schedule ( boost::bind ( &IActivity::ExecuteWrapper, job, running ) );
        }

    private:

        boost::threadpool::fifo_pool _pool;

        int _numOfThreads;

};


#endif /* THREADPOOL_H_ */

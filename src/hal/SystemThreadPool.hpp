#ifndef SYSTEMTHREADPOOL_HPP
#define SYSTEMTHREADPOOL_HPP

#include "core/architecture/executable/IExecutable.hpp"


#include "../external/threadpool-0_2_5-src/threadpool/boost/threadpool.hpp"

class SystemThreadPool
{

public:

	//            ThreadPool ( int numOfThreads ); //TODO add thread config

	SystemThreadPool () : _pool(10)
	{
		;
	}

	bool Enqueue ( IExecutable* job )
	{
		return _pool.schedule ( boost::bind ( &IExecutable::Execute, job ) );
	}

private:

	boost::threadpool::fifo_pool _pool;

	int _numOfThreads;

};


#endif /* SYSTEMTHREADPOOL_HPP */

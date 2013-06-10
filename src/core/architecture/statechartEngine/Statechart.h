#ifndef STATECHART_H_
#define STATECHART_H_ 1

#include "OrState.h"
#include "core/include/MessageHub.hpp"
#include "core/include/Blackboard.hpp"
#include "hal/Thread.hpp"
#include "hal/threadPool.h"

#include <boost/thread/condition_variable.hpp>

namespace statechart_engine
{

	class Statechart:  public statechart_engine::OrState, private KSystem::Thread
	{

	public:

		Statechart ( std::string name, MessageHub* com );

		virtual ~Statechart ();

		int Activate();

		ThreadPool* GetThreadPool ();

		ThreadPool* GetTimeoutThreadPool ();

		void Start ();

		void Stop ();

		int IEX_DIRECTIVE_HOT Execute();

		void AtomicNotify ();

	private:

		MessageHub* _narukom;

		Blackboard _blackboard;

		ThreadPool _tp;

		ThreadPool _timeoutpool;

		boost::condition_variable cond;
		KSystem::Mutex mut;
		bool notified;


	};

}

#endif /* STATECHART_H_ */

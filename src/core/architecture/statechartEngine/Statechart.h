#ifndef STATECHART_H_
#define STATECHART_H_ 1

#include "OrState.h"
#include "core/include/MessageHub.hpp"
#include "core/include/Blackboard.hpp"
#include "hal/SystemThread.hpp"
#include "hal/SystemThreadPool.hpp"

#include <boost/thread/condition_variable.hpp>

namespace statechart_engine
{

	class Statechart:  public statechart_engine::OrState, private KSystem::SystemThread
	{

	public:

		Statechart ( std::string name, Messaging::MessageHub* com );

		virtual ~Statechart ();

		int Activate();

		SystemThreadPool* GetThreadPool ();

		SystemThreadPool* GetTimeoutThreadPool ();

		void Start ();

		void Stop ();

		int IEX_DIRECTIVE_HOT Execute();

		void AtomicNotify ();

	private:

		Messaging::MessageHub* _narukom;

		Messaging::Blackboard _blackboard;

		SystemThreadPool _tp;

		SystemThreadPool _timeoutpool;

		boost::condition_variable cond;
		KSystem::SystemMutex mut;
		bool notified;


	};

}

#endif /* STATECHART_H_ */

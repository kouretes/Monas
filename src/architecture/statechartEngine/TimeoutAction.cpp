#include "TimeoutAction.h"
#include "Statechart.h"
#include "architecture/narukom/pub_sub/msg.h"

#include "hal/syscall.h"


#include <boost/date_time/posix_time/posix_time.hpp>

namespace statechart_engine
{

	TimeoutAction::TimeoutAction(std::string var, int timeout_ms)
		: _var(var), _timeout_ms(timeout_ms)
	{
		;
	}

	int TimeoutAction::Execute()
	{
		std::string time = boost::posix_time::to_iso_string(
		                       boost::posix_time::microsec_clock::local_time() +
		                       boost::posix_time::millisec(_timeout_ms));
		//((TimeoutMsg*)_tpl.get_msg_data())->set_wakeup(time);
		//       cout<<"PRPRPRP "<<((TimeoutMsg*)_tpl.get_msg_data())->wakeup()<<endl;
		_msg->set_wakeup(time);
		_blk->publishState(*_msg, _var);

		if ( ! _s->GetTimeoutThreadPool()->Enqueue( &_twork ) )
		{
			throw "Timeout: can't enqueue timeout!";
		}

		return 0;
	}

	void TimeoutAction::UserInit()
	{
		_msg.reset(new TimeoutMsg());
		_blk->publishState(*_msg, _var);
		_twork.Set ( _blk, _var, _s );
	}


	void TimeoutWorker::Set(Blackboard* blk, std::string& var, Statechart* s)
	{
		_blk = blk;
		_var = var;
		_s = s;
	}

	int TimeoutWorker::Execute()
	{
		boost::shared_ptr<const TimeoutMsg> msg  = _blk->readState<TimeoutMsg>(_var);

		if ( msg == 0 )
			throw "TimeoutWorker msg null";

		std::string time = msg->wakeup();
		int timeout = (boost::posix_time::from_iso_string(time) - boost::posix_time::microsec_clock::local_time() ).total_microseconds();

		if ( timeout > 0)
			SysCall::_usleep( timeout );

		_s->AtomicNotify();
		return 0;
	}

}

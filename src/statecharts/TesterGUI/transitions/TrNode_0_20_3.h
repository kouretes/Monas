
#include "architecture/statechartEngine/IAction.h"
#include "architecture/statechartEngine/TimoutAciton.h"
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>	
#include "messages/AllMessagesHeader.h"		

// InitTO_open_group_play_or_NoPlay_close_group__one_or_more_times_
class TrAction_0_20_3 : public statechart_engine::IAction {
			
public:
	void UserInit () { }

	int Execute() {
    _blk->process_messages();
  }
};
		

		

		

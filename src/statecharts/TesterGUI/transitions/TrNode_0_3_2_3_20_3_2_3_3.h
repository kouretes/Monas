
#include "architecture/statechartEngine/IAction.h"
#include "architecture/statechartEngine/TimoutAciton.h"
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>	
#include "messages/AllMessagesHeader.h"		

// ChangeRobotPositionTO_open_group_GoToPosition_one_or_more_times__close_group_
class TrAction_0_3_2_3_20_3_2_3_3 :  public statechart_engine::IAction {
			
public:

	void UserInit () { }

	int Execute() {
    _blk->process_messages();
  }
};
		

		

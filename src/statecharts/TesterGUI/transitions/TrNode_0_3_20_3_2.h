
#include "architecture/statechartEngine/IAction.h"
#include "architecture/statechartEngine/TimoutAciton.h"
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>	
#include "messages/AllMessagesHeader.h"		
// _open_group_play_or_NoPlay_close_group_TO_open_group_play_or_NoPlay_close_group_
class TrAction_0_3_20_3_2 :  public statechart_engine::IAction {
			
public:

  	void UserInit () { }

	int Execute() {
    _blk->process_messages();
  }
};
		

		

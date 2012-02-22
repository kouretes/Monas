
#include "architecture/statechartEngine/IAction.h"
#include "messages/AllMessagesHeader.h"
// InitTO_open_group_playing_or_nonPlaying_close_group__one_or_more_times_
class TrAction_Goalie0_20_3 : public statechart_engine::
				IAction {
		/* process_messages */
	public:
	void UserInit () { }

	int Execute() {


		_blk->process_messages();

		return 0;
    }
};

		
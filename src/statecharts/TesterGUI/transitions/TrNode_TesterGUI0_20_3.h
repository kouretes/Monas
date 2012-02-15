
#include "architecture/statechartEngine/IAction.h"
#include "messages/AllMessagesHeader.h"
// Init_TO__open_group_play_or_NoPlay_close_group__one_or_more_times_
class TrAction_TesterGUI0_20_3 : public statechart_engine::
				IAction {
		/* process_messages */
	public:
	void UserInit () { }

	int Execute() {


		_blk->process_messages();

		return 0;
    }
};

		
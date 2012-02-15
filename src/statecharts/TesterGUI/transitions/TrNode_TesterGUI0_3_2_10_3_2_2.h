
#include "architecture/statechartEngine/IAction.h"
#include "messages/AllMessagesHeader.h"
// 0.3.2.1_TO_0.3.2.2
class TrAction_TesterGUI0_3_2_10_3_2_2 : public statechart_engine::
				IAction {
		/* process_messages */
	public:
	void UserInit () { }

	int Execute() {


		_blk->process_messages();

		return 0;
    }
};

		
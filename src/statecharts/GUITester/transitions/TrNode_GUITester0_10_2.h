
#include "architecture/statechartEngine/IAction.h"
#include "architecture/statechartEngine/TimoutAciton.h"
#include "messages/AllMessagesHeader.h"
// 0.1TO_open_group_play_or_NoPlay_close_group_
class TrAction_GUITester0_10_2 : public statechart_engine::
	TimeoutAction
{
	/* TimeoutAction.behavior.250 */
public:
	TrAction_GUITester0_10_2() : statechart_engine::TimeoutAction( "behavior", 250 )
	{
		;
	}
};


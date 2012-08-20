
#include "architecture/statechartEngine/IAction.h"
#include "architecture/statechartEngine/TimoutAciton.h"
#include "messages/AllMessagesHeader.h"
// 0.2.3.3.3.1TOGoToPosition
class TrAction_GoalieRoboCup20120_2_3_3_3_10_2_3_3_3_2 : public statechart_engine::
	TimeoutAction
{
	/* TimeoutAction.behavior.250 */
public:
	TrAction_GoalieRoboCup20120_2_3_3_3_10_2_3_3_3_2() : statechart_engine::TimeoutAction( "behavior", 250 )
	{
		;
	}
};


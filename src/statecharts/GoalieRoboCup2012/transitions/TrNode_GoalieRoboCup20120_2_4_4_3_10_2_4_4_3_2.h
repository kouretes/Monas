
#include "architecture/statechartEngine/IAction.h"
#include "architecture/statechartEngine/TimoutAciton.h"
#include "messages/AllMessagesHeader.h"
// 0.2.4.4.3.1TOScan
class TrAction_GoalieRoboCup20120_2_4_4_3_10_2_4_4_3_2 : public statechart_engine::
	TimeoutAction
{
	/* TimeoutAction.behavior.250 */
public:
	TrAction_GoalieRoboCup20120_2_4_4_3_10_2_4_4_3_2() : statechart_engine::TimeoutAction( "behavior", 250 )
	{
		;
	}
};


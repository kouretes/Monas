
#include "architecture/statechartEngine/IAction.h"
#include "architecture/statechartEngine/TimoutAciton.h"
#include "messages/AllMessagesHeader.h"
// 0.2.4.4.4.1TOApproachBall
class TrAction_GoalieRoboCup20120_2_4_4_4_10_2_4_4_4_2 : public statechart_engine::
	TimeoutAction
{
	/* TimeoutAction.behavior.250 */
public:
	TrAction_GoalieRoboCup20120_2_4_4_4_10_2_4_4_4_2() : statechart_engine::TimeoutAction( "behavior", 250 )
	{
		;
	}
};


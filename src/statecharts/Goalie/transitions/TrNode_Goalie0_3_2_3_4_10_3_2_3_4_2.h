
#include "architecture/statechartEngine/IAction.h"
#include "architecture/statechartEngine/TimoutAciton.h"
#include "messages/AllMessagesHeader.h"
// 0.3.2.3.4.1TOApproachBall
class TrAction_Goalie0_3_2_3_4_10_3_2_3_4_2 : public statechart_engine::
	TimeoutAction
{
	/* TimeoutAction.behavior.250 */
public:
	TrAction_Goalie0_3_2_3_4_10_3_2_3_4_2() : statechart_engine::TimeoutAction( "behavior", 250 )
	{
		;
	}
};


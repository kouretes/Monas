
#include "architecture/statechartEngine/IAction.h"
#include "architecture/statechartEngine/TimoutAciton.h"
#include "messages/AllMessagesHeader.h"
// 0.3.2.3.6.2.1TOStare
class TrAction_Goalie0_3_2_3_6_2_10_3_2_3_6_2_2 : public statechart_engine::
	TimeoutAction
{
	/* TimeoutAction.behavior.250 */
public:
	TrAction_Goalie0_3_2_3_6_2_10_3_2_3_6_2_2() : statechart_engine::TimeoutAction( "behavior", 250 )
	{
		;
	}
};


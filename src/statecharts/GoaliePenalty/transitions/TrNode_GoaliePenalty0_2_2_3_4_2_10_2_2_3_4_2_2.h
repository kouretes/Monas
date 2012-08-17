
#include "architecture/statechartEngine/IAction.h"
#include "architecture/statechartEngine/TimoutAciton.h"
#include "messages/AllMessagesHeader.h"
// 0.2.2.3.6.2.1_TO_Stare
class TrAction_GoaliePenalty0_2_2_3_4_2_10_2_2_3_4_2_2 : public statechart_engine::
	TimeoutAction
{
	/* TimeoutAction.behavior.250 */
public:
	TrAction_GoaliePenalty0_2_2_3_4_2_10_2_2_3_4_2_2() : statechart_engine::TimeoutAction( "behavior", 250 )
	{
		;
	}
};


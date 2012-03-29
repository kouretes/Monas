
#include "architecture/statechartEngine/IAction.h"
#include "architecture/statechartEngine/TimoutAciton.h"
#include "messages/AllMessagesHeader.h"
// 0.2.2.3.4.1_TO_ApproachBall
class TrAction_GoalieYellow0_2_2_3_4_10_2_2_3_4_2 : public statechart_engine::
				TimeoutAction {
		/* TimeoutAction.behavior.250 */
	public:	TrAction_GoalieYellow0_2_2_3_4_10_2_2_3_4_2() : statechart_engine::TimeoutAction( "behavior", 250 ) { 
		;
	 }
};

		
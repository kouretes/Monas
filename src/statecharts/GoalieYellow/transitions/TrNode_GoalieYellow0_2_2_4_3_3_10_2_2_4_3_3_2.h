
#include "architecture/statechartEngine/IAction.h"
#include "architecture/statechartEngine/TimoutAciton.h"
#include "messages/AllMessagesHeader.h"
// 0.2.2.4.3.3.1_TO_GoToPosition
class TrAction_GoalieYellow0_2_2_4_3_3_10_2_2_4_3_3_2 : public statechart_engine::
				TimeoutAction {
		/* TimeoutAction.behavior.250 */
	public:	TrAction_GoalieYellow0_2_2_4_3_3_10_2_2_4_3_3_2() : statechart_engine::TimeoutAction( "behavior", 250 ) { 
		;
	 }
};

		
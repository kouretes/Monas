
#include "architecture/statechartEngine/IAction.h"
#include "architecture/statechartEngine/TimoutAciton.h"
#include "messages/AllMessagesHeader.h"
// 0.1TO_open_group_NonPlaying_or_Playing_close_group_
class TrAction_GoalieRoboCup20120_10_2 : public statechart_engine::
				TimeoutAction {
		/* TimeoutAction.behavior.300 */
	public:	TrAction_GoalieRoboCup20120_10_2() : statechart_engine::TimeoutAction( "behavior", 300 ) { 
		;
	 }
};

		
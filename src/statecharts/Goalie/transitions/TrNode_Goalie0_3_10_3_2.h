
#include "architecture/statechartEngine/IAction.h"
#include "architecture/statechartEngine/TimoutAciton.h"
#include "messages/AllMessagesHeader.h"
// 0.3.1TO_open_group_playing_or_nonPlaying_close_group_
class TrAction_Goalie0_3_10_3_2 : public statechart_engine::
				TimeoutAction {
		/* TimeoutAction.behavior.250 */
	public:	TrAction_Goalie0_3_10_3_2() : statechart_engine::TimeoutAction( "behavior", 250 ) { 
		;
	 }
};

		
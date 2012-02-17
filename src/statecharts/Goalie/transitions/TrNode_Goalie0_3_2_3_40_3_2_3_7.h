
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
// ApproachBall_one_or_more_times_TO0.3.2.3.7
class TrCond_Goalie0_3_2_3_40_3_2_3_7 : public statechart_engine::ICondition {
			
public:

	void UserInit () {
		_blk->updateSubscription("behavior",msgentry::SUBSCRIBE_ON_TOPIC);

	}

	bool Eval() {
		/* behavior.State.HeadToBMessage.ballfound()==0 || behavior.State.GameStateMessage.player_state()!=PLAYER_PLAYING || behavior.State.GameStateMessage==NULL || ballAway(behavior.Data.WorldInfo) || readyToKick(behavior.Data.WorldInfo) */
		
		boost::shared_ptr<const HeadToBMessage> var_1901744185 = _blk->readState<HeadToBMessage> ("behavior" );
		boost::shared_ptr<const GameStateMessage> var_621149599 = _blk->readState<GameStateMessage> ("behavior" );
		boost::shared_ptr<const WorldInfo> var_1071592760 = _blk->readData<WorldInfo> ("behavior" );

		
		if ( var_621149599.get() == 0 )
			 return true;
 		if ( var_1901744185.get() != 0  && var_1071592760.get() != 0 ){	
				return ( var_1901744185->ballfound()==0 || var_621149599->player_state()!=PLAYER_PLAYING  || ballAway(var_1071592760) || readyToKick(var_1071592760) );
		}return false;

		
    }
};
		
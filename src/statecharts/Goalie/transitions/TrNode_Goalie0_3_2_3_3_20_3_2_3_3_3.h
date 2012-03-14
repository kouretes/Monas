
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
// ScanTO0.3.2.3.3.3
class TrCond_Goalie0_3_2_3_3_20_3_2_3_3_3 : public statechart_engine::ICondition {		
public:
	void UserInit () {
		_blk->updateSubscription("behavior",msgentry::SUBSCRIBE_ON_TOPIC);

	}
	bool Eval() {
		/* behavior.State.HeadToBMessage.ballfound()>0 ||
 behavior.State.GameStateMessage.player_state()!=PLAYER_PLAYING || behavior.State.GameStateMessage==NULL */
		boost::shared_ptr<const HeadToBMessage> var_1901744185 = _blk->readState<HeadToBMessage> ("behavior" );
		boost::shared_ptr<const GameStateMessage> var_621149599 = _blk->readState<GameStateMessage> ("behavior" );
	
				return ( (var_1901744185.get()!=0 && var_1901744185->ballfound()>0) ||  (var_621149599.get()!=0 && var_621149599->player_state()!=PLAYER_PLAYING) || var_621149599.get()==0  );	
    }
};
		
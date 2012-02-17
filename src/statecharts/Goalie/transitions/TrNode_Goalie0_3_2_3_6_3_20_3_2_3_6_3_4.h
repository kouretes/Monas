
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
// 0.3.2.3.6.3.2TO0.3.2.3.6.3.4
class TrCond_Goalie0_3_2_3_6_3_20_3_2_3_6_3_4 : public statechart_engine::ICondition {
			
public:

	void UserInit () {
		_blk->updateSubscription("behavior",msgentry::SUBSCRIBE_ON_TOPIC);

	}

	bool Eval() {
		/* behavior.State.GameStateMessage==NULL || behavior.State.GameStateMessage.player_state()!=PLAYER_PLAYING ||behavior.Signal.FallMessage==NULL || behavior.Signal.FallMessage.fall()==0 */
		
		boost::shared_ptr<const GameStateMessage> var_621149599 = _blk->readState<GameStateMessage> ("behavior" );
		boost::shared_ptr<const FallMessage> var_734099742 = _blk->readSignal<FallMessage> ("behavior" );

		
		if ( var_621149599.get() == 0 )
			 return true;
 		if ( var_734099742.get() == 0 )
			 return true;
 	
				return (  var_621149599->player_state()!=PLAYER_PLAYING  || var_734099742->fall()==0 );
		return false;

		
    }
};
		
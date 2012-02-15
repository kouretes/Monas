
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
#include "tools/logger.h"
#include "tools/toString.h"
// Scan_one_or_more_times_TO0.2.3.3.5
class TrCond_GoalieTest0_2_3_3_30_2_3_3_5 : public statechart_engine::ICondition {
			
public:

	void UserInit () {
		_blk->updateSubscription("behavior",msgentry::SUBSCRIBE_ON_TOPIC);

	}

	bool Eval() {
		/* _behavior.State.GameStateMessage.player_state()!=PLAYER_PLAYING || _behavior.State.HeadToBMessage.ballfound()!=0 */
		
		boost::shared_ptr<const GameStateMessage> var_1692513826 = _blk->readState<GameStateMessage> ("behavior" );
		boost::shared_ptr<const HeadToBMessage> var_1318191046 = _blk->readState<HeadToBMessage> ("behavior" );

		
		if ( var_1692513826.get() == 0 ){
			Logger::Instance().WriteMsg("Condition", "true",  Logger::Info);
			return true;
		}
		if ( var_1318191046.get() == 0 ){
			Logger::Instance().WriteMsg("Condition", "true",  Logger::Info);
			return true;
		}

		
		Logger::Instance().WriteMsg("var_1692513826->player_state()!=PLAYER_PLAYING || var_1318191046->ballfound()!=0", _toString(var_1692513826->player_state()!=PLAYER_PLAYING || var_1318191046->ballfound()!=0),  Logger::Info);
		return ( var_1692513826->player_state()!=PLAYER_PLAYING || var_1318191046->ballfound()!=0 ); 

		
    }
};
		

#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
#include "tools/logger.h"
#include "tools/toString.h"
// Stare_one_or_more_times_TO_open_option_SpecialAction_close_option_
class TrCond_GoalieTest0_2_3_6_20_2_3_6_3 : public statechart_engine::ICondition {
			
public:

	void UserInit () {
		_blk->updateSubscription("behavior",msgentry::SUBSCRIBE_ON_TOPIC);

	}

	bool Eval() {
		/* _behavior.State.GameStateMessage.player_state()!=PLAYER_PLAYING || _behavior.State.HeadToBMessage.ballfound()==0 || !ballAway(behavior.Data.WorldInfo) || behavior.Signal.FallMessage.fall()!=0 */
		
		boost::shared_ptr<const GameStateMessage> var_1692513826 = _blk->readState<GameStateMessage> ("behavior" );
		boost::shared_ptr<const HeadToBMessage> var_1318191046 = _blk->readState<HeadToBMessage> ("behavior" );
		boost::shared_ptr<const WorldInfo> var_1071592760 = _blk->readData<WorldInfo> ("behavior" );
		boost::shared_ptr<const FallMessage> var_734099742 = _blk->readSignal<FallMessage> ("behavior" );

		
		if ( var_1692513826.get() == 0 ){
			Logger::Instance().WriteMsg("Condition", "true",  Logger::Info);
			return true;
		}
		if ( var_1318191046.get() == 0 ){
			Logger::Instance().WriteMsg("Condition", "true",  Logger::Info);
			return true;
		}
		if ( var_1071592760.get() == 0 ){
			Logger::Instance().WriteMsg("Condition", "false",  Logger::Info);
			return false;
		}
		if ( var_734099742.get() == 0 ){
			Logger::Instance().WriteMsg("Condition", "false",  Logger::Info);
			return false;
		}

		
		Logger::Instance().WriteMsg("var_1692513826->player_state()!=PLAYER_PLAYING || var_1318191046->ballfound()==0 || !ballAway(var_1071592760) || var_734099742->fall()!=0", _toString(var_1692513826->player_state()!=PLAYER_PLAYING || var_1318191046->ballfound()==0 || !ballAway(var_1071592760) || var_734099742->fall()!=0),  Logger::Info);
		return ( var_1692513826->player_state()!=PLAYER_PLAYING || var_1318191046->ballfound()==0 || !ballAway(var_1071592760) || var_734099742->fall()!=0 ); 

		
    }
};
		
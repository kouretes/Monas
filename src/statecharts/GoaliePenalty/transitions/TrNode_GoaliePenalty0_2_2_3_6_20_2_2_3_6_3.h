
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
#include "tools/logger.h"
#include "tools/toString.h"
// Stare_one_or_more_times__TO__open_option_SpecialAction_close_option_
class TrCond_GoaliePenalty0_2_2_3_6_20_2_2_3_6_3 : public statechart_engine::ICondition {		
public:
	void UserInit () {
		_blk->updateSubscription("worldstate",msgentry::SUBSCRIBE_ON_TOPIC);
		_blk->updateSubscription("behavior",msgentry::SUBSCRIBE_ON_TOPIC);

	}

	bool Eval() {
		/* worldstate.State.GameStateMessage==NULL|| 
worldstate.State.GameStateMessage.player_state()!=PLAYER_PLAYING 
|| worldstate.State.GameStateMessage.kickoff() 
|| behavior.State.HeadToBMessage.ballfound()==0 ||  behavior.State.FallMessage.fall()!=0 */
		boost::shared_ptr<const GameStateMessage> var_1263977940 = _blk->readState<GameStateMessage> ("worldstate" );
		boost::shared_ptr<const HeadToBMessage> var_1901744185 = _blk->readState<HeadToBMessage> ("behavior" );
		boost::shared_ptr<const FallMessage> var_1561961811 = _blk->readState<FallMessage> ("behavior" );

		Logger::Instance().WriteMsg("Stare_one_or_more_times__TO__open_option_SpecialAction_close_option_, worldstate.State.GameStateMessage==NULL||  worldstate.State.GameStateMessage.player_state()!=PLAYER_PLAYING  || worldstate.State.GameStateMessage.kickoff()  || behavior.State.HeadToBMessage.ballfound()==0 ||  behavior.State.FallMessage.fall()!=0" ,_toString(var_1263977940.get()==0 ||   (var_1263977940.get()!=0 && var_1263977940->player_state()!=PLAYER_PLAYING)  || (var_1263977940.get()!=0 && var_1263977940->kickoff()  )|| (var_1901744185.get()!=0 && var_1901744185->ballfound()==0) ||  (var_1561961811.get()!=0 && var_1561961811->fall()!=0)),  Logger::Info);
		return ( var_1263977940.get()==0 ||   (var_1263977940.get()!=0 && var_1263977940->player_state()!=PLAYER_PLAYING)  || (var_1263977940.get()!=0 && var_1263977940->kickoff()  )|| (var_1901744185.get()!=0 && var_1901744185->ballfound()==0) ||  (var_1561961811.get()!=0 && var_1561961811->fall()!=0) );
    }
};
		
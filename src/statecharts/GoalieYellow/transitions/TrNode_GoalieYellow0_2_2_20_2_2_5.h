
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
#include "tools/logger.h"
#include "tools/toString.h"
// 0.3.2.2_TO__open_option_Localize_one_or_more_times__close_option_
class TrCond_GoalieYellow0_2_2_20_2_2_5 : public statechart_engine::ICondition {		
public:
	void UserInit () {
		_blk->updateSubscription("worldstate",msgentry::SUBSCRIBE_ON_TOPIC);
		_blk->updateSubscription("behavior",msgentry::SUBSCRIBE_ON_TOPIC);

	}

	bool Eval() {
		/* worldstate.State.GameStateMessage.player_state()==PLAYER_PLAYING 
&&worldstate.State.GameStateMessage.previous_player_state()==PLAYER_PENALISED
&& (behavior.State.ReturnToPositionMessage==NULL || !behavior.State.ReturnToPositionMessage.goalietopos()) */
		boost::shared_ptr<const GameStateMessage> var_1263977940 = _blk->readState<GameStateMessage> ("worldstate" );
		boost::shared_ptr<const ReturnToPositionMessage> var_969154074 = _blk->readState<ReturnToPositionMessage> ("behavior" );

		Logger::Instance().WriteMsg("0.3.2.2_TO__open_option_Localize_one_or_more_times__close_option_, worldstate.State.GameStateMessage.player_state()==PLAYER_PLAYING  &&worldstate.State.GameStateMessage.previous_player_state()==PLAYER_PENALISED && (behavior.State.ReturnToPositionMessage==NULL || !behavior.State.ReturnToPositionMessage.goalietopos())" ,_toString((var_1263977940.get()!=0 && var_1263977940->player_state()==PLAYER_PLAYING)  &&(var_1263977940.get()!=0 && var_1263977940->previous_player_state()==PLAYER_PENALISED) && (var_969154074.get()==0 ||  (var_969154074.get()!=0 && !var_969154074->goalietopos()))),  Logger::Info);
		return ( (var_1263977940.get()!=0 && var_1263977940->player_state()==PLAYER_PLAYING)  &&(var_1263977940.get()!=0 && var_1263977940->previous_player_state()==PLAYER_PENALISED) && (var_969154074.get()==0 ||  (var_969154074.get()!=0 && !var_969154074->goalietopos())) );
    }
};
		

#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
// _open_option_GoToPosition_one_or_more_times__close_option_TOMakeAction
class TrCond_GoalieRoboCup20120_2_4_30_2_4_4 : public statechart_engine::ICondition {		
public:
	void UserInit () {
		_blk->updateSubscription("worldstate",msgentry::SUBSCRIBE_ON_TOPIC);
		_blk->updateSubscription("behavior",msgentry::SUBSCRIBE_ON_TOPIC);

	}
	bool Eval() {
		/* worldstate.State.GameStateMessage==NULL 
|| worldstate.State.GameStateMessage.player_state()!=PLAYER_PLAYING 
||(behavior.State.LocalizeDone.moveon() && behavior.State.ReturnToPositionMessage.goalietopos())
|| robotInPosition(worldstate.Data.WorldInfo, behavior.State.PositionMessage) || worldstate.State.GameStateMessage.previous_player_state()!=PLAYER_PENALISED
 */
		boost::shared_ptr<const GameStateMessage> var_1263977940 = _blk->readState<GameStateMessage> ("worldstate" );
		boost::shared_ptr<const LocalizeDone> var_373152964 = _blk->readState<LocalizeDone> ("behavior" );
		boost::shared_ptr<const ReturnToPositionMessage> var_969154074 = _blk->readState<ReturnToPositionMessage> ("behavior" );
		boost::shared_ptr<const WorldInfo> var_1111221333 = _blk->readData<WorldInfo> ("worldstate" );
		boost::shared_ptr<const PositionMessage> var_1868674971 = _blk->readState<PositionMessage> ("behavior" );
	
				return ( var_1263977940.get()==0 ||  (var_1263977940.get()!=0 && var_1263977940->player_state()!=PLAYER_PLAYING)  ||((var_373152964.get()!=0 && var_373152964->moveon() )&& (var_969154074.get()!=0 && var_969154074->goalietopos())) || robotInPosition(var_1111221333, var_1868674971) || (var_1263977940.get()!=0 && var_1263977940->previous_player_state()!=PLAYER_PENALISED)  );	
    }
};
		
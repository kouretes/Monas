
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
#include "tools/logger.h"
#include "tools/toString.h"
// ApproachBallTO0.2.3.4.3
class TrCond_GoalieTest0_2_3_4_20_2_3_4_3 : public statechart_engine::ICondition {
			
public:

	void UserInit () {
		_blk->updateSubscription("behavior",msgentry::SUBSCRIBE_ON_TOPIC);

	}

	bool Eval() {
		/* _behavior.State.GameStateMessage.player_state()!=PLAYER_PLAYING || _behavior.State.HeadToBMessage.ballfound()==0 || ballAway(behavior.Data.WorldInfo) || readyToKick(behavior.Data.WorldInfo) */
		
		boost::shared_ptr<const GameStateMessage> var_1692513826 = _blk->readState<GameStateMessage> ("behavior" );
		boost::shared_ptr<const HeadToBMessage> var_1318191046 = _blk->readState<HeadToBMessage> ("behavior" );
		boost::shared_ptr<const WorldInfo> var_1071592760 = _blk->readData<WorldInfo> ("behavior" );

		
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

		
		Logger::Instance().WriteMsg("var_1692513826->player_state()!=PLAYER_PLAYING || var_1318191046->ballfound()==0 || ballAway(var_1071592760) || readyToKick(var_1071592760)", _toString(var_1692513826->player_state()!=PLAYER_PLAYING || var_1318191046->ballfound()==0 || ballAway(var_1071592760) || readyToKick(var_1071592760)),  Logger::Info);
		return ( var_1692513826->player_state()!=PLAYER_PLAYING || var_1318191046->ballfound()==0 || ballAway(var_1071592760) || readyToKick(var_1071592760) ); 

		
    }
};
		
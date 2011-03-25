
#include "architecture/statechartEngine/ICondition.h"
#include "architecture/archConfig.h"
#include "tools/logger.h"
#include "tools/toString.h"
#include "messages/BehaviorMessages.pb.h"	
		
class TrCond_0_3_2_3_2TOcenter : public statechart_engine::ICondition {
			
public:

	void UserInit () {
		_blk->subscribeTo("behavior", 0);
 }

	bool Eval() {
		boost::shared_ptr<const PlayerNumberMessage> pnm = _blk->readState<PlayerNumberMessage> ("behavior");
		if ( pnm.get() == 0 ){
			//Logger::Instance().WriteMsg("TrCond_0_3_2_3_2TOcenter",  " FALSE NO PNM", Logger::Info);
			return true;
		}
		if( pnm->player_number()==2){
			//Logger::Instance().WriteMsg("TrCond_0_3_2_3_2TOcenter",  " TRUE", Logger::Info);
			return true;
		}
		//Logger::Instance().WriteMsg("TrCond_0_3_2_3_2TOcenter",  " FALSE", Logger::Info);
		return false;
    }
};
		

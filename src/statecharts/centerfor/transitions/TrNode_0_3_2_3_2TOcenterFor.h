
#include "architecture/statechartEngine/ICondition.h"
#include "architecture/archConfig.h"
#include "tools/logger.h"
#include "tools/toString.h"
#include "messages/BehaviorMessages.pb.h"	
			
class TrCond_0_3_2_3_2TOcenterFor : public statechart_engine::ICondition {
			
public:	

void UserInit () {
		_blk->subscribeTo("behavior", 0);
 }

	bool Eval() {

		 boost::shared_ptr<const PlayerNumberMessage> pnm = _blk->read_state<PlayerNumberMessage> ("PlayerNumberMessage");
     if ( pnm.get() == 0 ){		 
		// Logger::Instance().WriteMsg("TrCond_0_3_2_3_2TOcenterFor",  " FALSE NO PNM", Logger::Info);
		 return false;
		 }
     if( pnm->player_number()!=2){		
		Logger::Instance().WriteMsg("TrCond_0_3_2_3_2TOcenterFor",  " TRUE " + _toString(pnm->player_number()) , Logger::Info);
        return true;
	}else{		 
		 Logger::Instance().WriteMsg("TrCond_0_3_2_3_2TOcenterFor",  " FALSE", Logger::Info);
		 return false;
	 }
    }
    
 };
		

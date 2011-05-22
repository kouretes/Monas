
#include "architecture/statechartEngine/ICondition.h"
#include "activities/Stare/Stare.h"			
#include "messages/AllMessagesHeader.h"		
#include "tools/logger.h"
#include "tools/toString.h"		
			
class TrCond_0_3_2_3_4_2_3_2TOSpecialAction : public statechart_engine::ICondition {
			
public:

	void UserInit () {
		_blk->subscribeTo("behavior", 0);
	}

	bool Eval() {
		/* check ballspeed Fall */
	//	Logger::Instance().WriteMsg("TrCond_0_3_2_3_4_2_3_2TOSpecialAction",  " enter", Logger::Info);
		Stare st;
		boost::shared_ptr<const DoubleObsInfo> doi = _blk->readData<DoubleObsInfo>("behavior");
		 
		int fall;
		fall = st.toFallOrNotToFall(doi);
		boost::shared_ptr<const GameStateMessage> gsm = _blk->readState<GameStateMessage>("behavior");

		if(gsm.get()!=0 && gsm->player_state()==PLAYER_PLAYING){

			if(fall==0)
				return false;
			else
				return true;
		}
    }
};
		

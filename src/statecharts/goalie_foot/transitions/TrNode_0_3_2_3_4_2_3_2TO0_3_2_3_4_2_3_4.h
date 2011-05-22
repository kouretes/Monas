
#include "architecture/statechartEngine/ICondition.h"
#include "activities/Stare/Stare.h"			
#include "messages/AllMessagesHeader.h"		
#include "tools/logger.h"
#include "tools/toString.h"		

class TrCond_0_3_2_3_4_2_3_2TO0_3_2_3_4_2_3_4 : public statechart_engine::ICondition {
			
public:

	void UserInit () { 
		_blk->subscribeTo("behavior", 0);
		}

	bool Eval() {
		/* checkballspeed Do not fall*/
		//Logger::Instance().WriteMsg("TrCond_0_3_2_3_4_2_3_2TO0_3_2_3_4_2_3_4",  " enter", Logger::Info);
		Stare st;
		boost::shared_ptr<const DoubleObsInfo> doi = _blk->readData<DoubleObsInfo>("behavior");
		boost::shared_ptr<const GameStateMessage> gsm = _blk->readState<GameStateMessage>("behavior");

		if(gsm.get()!=0 && gsm->player_state()!=PLAYER_PLAYING)
			return true;
		return (st.toFallOrNotToFall(doi)==0);
    }
};
		

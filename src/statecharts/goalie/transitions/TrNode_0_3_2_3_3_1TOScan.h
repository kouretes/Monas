
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"			
#include "tools/toString.h"
#include "tools/logger.h"
			
class TrCond_0_3_2_3_3_1TOScan : public statechart_engine::ICondition {
			
public:

	void UserInit () {
	}

	bool Eval() {
		//Logger::Instance().WriteMsg("TrCond_0_3_2_3_3_1TOScan", " publish velocitywalk ", Logger::Info);
		//MotionWalkMessage* wmot = new MotionWalkMessage();
		//wmot->set_command("setWalkTargetVelocity");
		//wmot->set_parameter(0, 0.0f);
		//wmot->set_parameter(1, 0.0f);
		//wmot->set_parameter(2, 0.0f);
		//wmot->set_parameter(3, 1.0f);
		//_blk->publishSignal(*wmot, "motion");
		return true;
    }
};
		

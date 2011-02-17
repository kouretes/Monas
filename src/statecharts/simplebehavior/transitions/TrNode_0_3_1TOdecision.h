
#include "architecture/statechartEngine/ICondition.h"
			
class TrCond_0_3_1TOdecision : public statechart_engine::ICondition {
			
public:

	void UserInit () { }

	bool Eval() {
		/* TRUE */
		std::cout<<"*************Transition TrCond_0_3_1TOdecision TRUE*****************"<<std::endl;
		return true;
    }
};
		

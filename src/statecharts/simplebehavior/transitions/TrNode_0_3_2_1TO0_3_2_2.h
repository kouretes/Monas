
#include "architecture/statechartEngine/ICondition.h"
			
class TrCond_0_3_2_1TO0_3_2_2 : public statechart_engine::ICondition {
			
public:

	void UserInit () { }

	bool Eval() {
		/* TRUE */
		std::cout<<"*************Transition TrCond_0_3_2_1TO0_3_2_2 TRUE*****************"<<std::endl;
		return true;
    }
};
		

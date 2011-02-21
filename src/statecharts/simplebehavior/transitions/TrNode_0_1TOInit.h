
#include "architecture/statechartEngine/ICondition.h"
			
class TrCond_0_1TOInit : public statechart_engine::ICondition {
			
public:

	void UserInit () { }

	bool Eval() {
		/* TRUE */
		//std::cout<<"*************Transition TrCond_0_1TOInit TRUE*****************"<<std::endl;
		return true;
    }
};
		

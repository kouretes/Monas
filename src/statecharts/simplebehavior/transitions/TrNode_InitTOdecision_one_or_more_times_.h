
#include "architecture/statechartEngine/ICondition.h"
			
class TrCond_InitTOdecision_one_or_more_times_ : public statechart_engine::ICondition {
			
public:

	void UserInit () { }

	bool Eval() {
		/* TRUE */
		std::cout<<"*************Transition TrCond_InitTOdecision_one_or_more_times_ TRUE*****************"<<std::endl;
		return true;
    }
};
		

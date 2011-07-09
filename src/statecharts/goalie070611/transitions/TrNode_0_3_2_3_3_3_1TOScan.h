
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"			
#include "tools/toString.h"
#include "tools/logger.h"	
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
	
class TrCond_0_3_2_3_3_3_1TOScan : public statechart_engine::ICondition {
			
public:

	void UserInit () { }

	bool Eval() {
		Logger::Instance().WriteMsg("TOScan","enter", Logger::Info);
		return true;
    }
};
		

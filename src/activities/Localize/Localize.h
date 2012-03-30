
#ifndef _Localize_h_
#define _Localize_h_ 1

#include "architecture/IActivity.h"
#include "messages/AllMessagesHeader.h"
#include "tools/logger.h"
#include "tools/toString.h"
#include "tools/BehaviorConst.h"
#include "tools/MathFunctions.h"
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

class Localize : public IActivity {
			
public:
	
	int Execute ();
	
	void UserInit ();
	
	std::string GetName ();
	
private:
	 LocalizeDone* ld;
	 boost::posix_time::ptime first;
	 boost::shared_ptr< const HeadToBMessage> hbm;
	 BToHeadMessage* bhm;
	 MotionActionMessage* amot;
	 LocalizationResetMessage* locReset;
	
};

#endif // _Localize_h_
	

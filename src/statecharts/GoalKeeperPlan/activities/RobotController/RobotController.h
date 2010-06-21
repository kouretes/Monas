
#ifndef _RobotController_h_
#define _RobotController_h_ 1

#include "architecture/IActivity.h"
			
class RobotController : public IActivity {
			
public:
	
	int Execute ();
	
	void UserInit ();
	
	std::string GetName ();
	
};

#endif // _RobotController_h_
	
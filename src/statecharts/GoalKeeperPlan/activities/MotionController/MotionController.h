
#ifndef _MotionController_h_
#define _MotionController_h_ 1

#include "architecture/IActivity.h"
			
class MotionController : public IActivity {
			
public:
	
	int Execute ();
	
	void UserInit ();
	
	std::string GetName ();
	
};

#endif // _MotionController_h_
	
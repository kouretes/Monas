
#ifndef _MyRightKick_h_
#define _MyRightKick_h_ 1

#include "architecture/IActivity.h"
			
class MyRightKick : public IActivity {
			
public:
	
	int Execute ();
	
	void UserInit ();
	
	std::string GetName ();
	
};

#endif // _MyRightKick_h_
	
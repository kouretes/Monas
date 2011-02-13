
#ifndef _LedHandler_h_
#define _LedHandler_h_ 1

#include "architecture/IActivity.h"
			
class LedHandler : public IActivity {
			
public:
	
	int Execute ();
	
	void UserInit ();
	
	std::string GetName ();
	
};

#endif // _LedHandler_h_
	
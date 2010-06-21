
#ifndef _Stand_h_
#define _Stand_h_ 1

#include "architecture/IActivity.h"
			
class Stand : public IActivity {
			
public:
	
	int Execute ();
	
	void UserInit ();
	
	std::string GetName ();
	
};

#endif // _Stand_h_
	
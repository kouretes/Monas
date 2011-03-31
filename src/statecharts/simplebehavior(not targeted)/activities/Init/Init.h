
#ifndef _Init_h_
#define _Init_h_ 1

#include "architecture/IActivity.h"
			
class Init : public IActivity {
			
public:
	
	int Execute ();
	
	void UserInit ();
	
	std::string GetName ();
	
};

#endif // _Init_h_
	
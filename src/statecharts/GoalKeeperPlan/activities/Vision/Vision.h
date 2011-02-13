
#ifndef _Vision_h_
#define _Vision_h_ 1

#include "architecture/IActivity.h"
			
class Vision : public IActivity {
			
public:
	
	int Execute ();
	
	void UserInit ();
	
	std::string GetName ();
	
};

#endif // _Vision_h_
	

#ifndef _RightFall_h_
#define _RightFall_h_ 1

#include "architecture/IActivity.h"
			
class RightFall : public IActivity {
			
public:
	
	int Execute ();
	
	void UserInit ();
	
	std::string GetName ();
	
};

#endif // _RightFall_h_
	
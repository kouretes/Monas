
#ifndef _LeftFall_h_
#define _LeftFall_h_ 1

#include "architecture/IActivity.h"
			
class LeftFall : public IActivity {
			
public:
	
	int Execute ();
	
	void UserInit ();
	
	std::string GetName ();
	
};

#endif // _LeftFall_h_
	
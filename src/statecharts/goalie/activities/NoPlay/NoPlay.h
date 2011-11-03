
#ifndef _NoPlay_h_
#define _NoPlay_h_ 1

#include "architecture/IActivity.h"
			
class NoPlay : public IActivity {
			
public:
	
	int Execute ();
	
	void UserInit ();
	
	std::string GetName ();
	
};

#endif // _NoPlay_h_
	
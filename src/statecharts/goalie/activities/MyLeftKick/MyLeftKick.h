
#ifndef _MyLeftKick_h_
#define _MyLeftKick_h_ 1

#include "architecture/IActivity.h"
			
class MyLeftKick : public IActivity {
			
public:
	
	int Execute ();
	
	void UserInit ();
	
	std::string GetName ();
	
};

#endif // _MyLeftKick_h_
	
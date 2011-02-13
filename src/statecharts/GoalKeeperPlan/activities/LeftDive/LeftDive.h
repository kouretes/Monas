
#ifndef _LeftDive_h_
#define _LeftDive_h_ 1

#include "architecture/IActivity.h"
			
class LeftDive : public IActivity {
			
public:
	
	int Execute ();
	
	void UserInit ();
	
	std::string GetName ();
	
};

#endif // _LeftDive_h_
	
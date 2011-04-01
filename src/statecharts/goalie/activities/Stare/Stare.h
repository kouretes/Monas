
#ifndef _Stare_h_
#define _Stare_h_ 1

#include "architecture/IActivity.h"
			
class Stare : public IActivity {
			
public:
	
	int Execute ();
	
	void UserInit ();
	
	std::string GetName ();
	
};

#endif // _Stare_h_
	
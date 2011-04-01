
#ifndef _Scan_h_
#define _Scan_h_ 1

#include "architecture/IActivity.h"
			
class Scan : public IActivity {
			
public:
	
	int Execute ();
	
	void UserInit ();
	
	std::string GetName ();
	
};

#endif // _Scan_h_
	
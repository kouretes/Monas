
#ifndef _Sensors_h_
#define _Sensors_h_ 1

#include "architecture/IActivity.h"
			
class Sensors : public IActivity {
			
public:
	
	int Execute ();
	
	void UserInit ();
	
	std::string GetName ();
	
};

#endif // _Sensors_h_
	
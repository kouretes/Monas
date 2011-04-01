
#ifndef _ApproachBall_h_
#define _ApproachBall_h_ 1

#include "architecture/IActivity.h"
			
class ApproachBall : public IActivity {
			
public:
	
	int Execute ();
	
	void UserInit ();
	
	std::string GetName ();
	
};

#endif // _ApproachBall_h_
	
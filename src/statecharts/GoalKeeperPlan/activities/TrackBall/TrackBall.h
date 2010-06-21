
#ifndef _TrackBall_h_
#define _TrackBall_h_ 1

#include "architecture/IActivity.h"
			
class TrackBall : public IActivity {
			
public:
	
	int Execute ();
	
	void UserInit ();
	
	std::string GetName ();
	
};

#endif // _TrackBall_h_
	
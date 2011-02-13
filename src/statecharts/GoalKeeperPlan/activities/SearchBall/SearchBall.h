
#ifndef _SearchBall_h_
#define _SearchBall_h_ 1

#include "architecture/IActivity.h"
			
class SearchBall : public IActivity {
			
public:
	
	int Execute ();
	
	void UserInit ();
	
	std::string GetName ();
	
};

#endif // _SearchBall_h_
	

#ifndef _ChangeRobotPosition_h_
#define _ChangeRobotPosition_h_ 1

#include "architecture/IActivity.h"
#include "messages/BehaviorMessages.pb.h"		
#include "tools/logger.h"
#include "tools/toString.h"	
class ChangeRobotPosition : public IActivity {
			
public:
	
	int Execute ();
	
	void UserInit ();
	
	std::string GetName ();
	
private:
	int positionChooser;
	PositionMessage* pm;
};



#endif // _ChangeRobotPosition_h_
	

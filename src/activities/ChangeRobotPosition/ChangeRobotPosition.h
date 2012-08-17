
#ifndef _ChangeRobotPosition_h_
#define _ChangeRobotPosition_h_ 1

#include "architecture/executables/IActivity.h"
#include "messages/BehaviorMessages.pb.h"
#include "tools/logger.h"
#include "tools/toString.h"

ACTIVITY_START
class ChangeRobotPosition : public IActivity
{

public:
	ACTIVITY_CONSTRUCTOR(ChangeRobotPosition);
	int ACTIVITY_VISIBLE Execute ();

	void ACTIVITY_VISIBLE UserInit ();

	std::string ACTIVITY_VISIBLE GetName ();

private:
	int positionChooser;
	PositionMessage* pm;
};

ACTIVITY_END


#endif // _ChangeRobotPosition_h_



#include "ChangeRobotPosition.h"


ACTIVITY_REGISTER(ChangeRobotPosition);
///namespace nsChangeRobotPosition {

int ChangeRobotPosition::Execute()
{
	if(positionChooser % 3 == 0)
	{
		pm->set_posx(-1.5);
		pm->set_posy(1.0);
		pm->set_theta(0.0);
		positionChooser++;
	}
	else if(positionChooser % 3 == 1)
	{
		pm->set_posx(-1.5);
		pm->set_posy(-1.0);
		pm->set_theta(0.0);
		positionChooser++;
	}
	else if(positionChooser % 3 == 2)
	{
		pm->set_posx(-1.2);
		pm->set_posy(0.0);
		pm->set_theta(0.0);
		positionChooser++;
	}

	Logger::Instance().WriteMsg(GetName(),  " Position " + _toString(positionChooser % 3), Logger::Info);
	_blk.publishSignal(*pm, "behavior");
	_blk.publish_all();
	return 0;
}

void ChangeRobotPosition::UserInit ()
{
	_blk.updateSubscription("behavior", msgentry::SUBSCRIBE_ON_TOPIC);
	positionChooser = 0;
	pm = new PositionMessage();
}

std::string ChangeRobotPosition::GetName ()
{
	return "ChangeRobotPosition";
}
//}


#ifndef SHAREDWORLDMODEL_H
#define SHAREDWORLDMODEL_H

#include "core/include/IActivity.hpp"

#include "messages/WorldInfo.pb.h"
#include "messages/Gamecontroller.pb.h"
#include "messages/Network.pb.h"

ACTIVITY_START

class SharedWorldModel: public IActivity
{

public:
	ACTIVITY_CONSTRUCTOR(SharedWorldModel);
	std::string ACTIVITY_VISIBLE GetName()
	{
		return "SharedWorldModel";
	}
	int ACTIVITY_VISIBLE IEX_DIRECTIVE_HOT Execute();
	void ACTIVITY_VISIBLE UserInit();
	void ACTIVITY_VISIBLE Reset();
	boost::shared_ptr<const WorldInfo>  wim;
	boost::shared_ptr<const GameStateMessage>  gsm;
	boost::shared_ptr<const KnownHosts> h;
	static const int numOfRobots = 5;
private:
	int findClosestRobot();

	SharedWorldInfo swi;

	int idx, count;
	float robot_x[numOfRobots], robot_y[numOfRobots], robot_phi[numOfRobots], bd[numOfRobots];
	float bx, by;

};

ACTIVITY_END
#endif


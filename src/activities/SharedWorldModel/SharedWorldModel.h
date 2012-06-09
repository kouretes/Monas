#ifndef SHAREDWORLDMODEL_H
#define SHAREDWORLDMODEL_H

#include "architecture/executables/IActivity.h"

#include "messages/WorldInfo.pb.h"
#include "messages/Network.pb.h"

#define numOfRobots 5

ACTIVITY_START

class SharedWorldModel: public IActivity {

	public:
        ACTIVITY_CONSTRUCTOR(SharedWorldModel);
		std::string ACTIVITY_VISIBLE GetName() {
			return "SharedWorldModel";
		}
        void ACTIVITY_VISIBLE  UserInit();
		int ACTIVITY_VISIBLE IEX_DIRECTIVE_HOT Execute();
		boost::shared_ptr<const WorldInfo>  wim;
		boost::shared_ptr<const KnownHosts> h;

	private:
        int findClosestRobot();

        SharedWorldInfo swi;

        int idx, count;
        float robot_x[numOfRobots], robot_y[numOfRobots], bd[numOfRobots];
        float bx, by;

};

ACTIVITY_END
#endif


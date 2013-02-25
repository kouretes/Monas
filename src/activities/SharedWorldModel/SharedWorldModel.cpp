#include "SharedWorldModel.h"
#include "tools/logger.h"
#include "tools/toString.h"
#include "core/hal/robot/generic_nao/robot_consts.h"

#define INIT_VALUE -111.0f
#define INF 100000000.0f

ACTIVITY_REGISTER(SharedWorldModel);
using namespace std;

void SharedWorldModel::UserInit()
{
	_blk.updateSubscription("worldstate", msgentry::SUBSCRIBE_ON_TOPIC, msgentry::HOST_ID_ANY_HOST);
	_blk.updateSubscription("communication", msgentry::SUBSCRIBE_ON_TOPIC);
	idx = -1;

	for(int i = 0; i < numOfRobots; i++)
	{
		bd[i] = INIT_VALUE;
	}

	Logger::Instance().WriteMsg("SharedWorldModel", "Initialized", Logger::Info);
}

void SharedWorldModel::Reset(){

}

int SharedWorldModel::Execute()
{
	h = _blk.readState<KnownHosts>("communication");

	for(int i = 0; i < numOfRobots; i++)
	{
		bd[i] = INIT_VALUE;
		robot_x[i] = INIT_VALUE;
		robot_y[i] = INIT_VALUE;
	}
    swi.Clear();
    swi.add_teammateposition(); //add position for self

	//Other robots' WorldInfo and ball distances
	if(!h.get() || (h && h->entrylist_size() == 0))
	{
		//		Logger::Instance().WriteMsg("SharedWorldModel", "No info from other robots", Logger::Info);
	}
	else
	{
		const ::google::protobuf::RepeatedPtrField< HostEntry >& rf = h->entrylist();
		::google::protobuf::RepeatedPtrField< HostEntry >::const_iterator fit;
		count = 1;

		for(fit = rf.begin(); fit != rf.end(); ++fit)
		{
		    gsm  = _blk.readState<GameStateMessage> ("worldstate", (*fit).hostid());
			wim  = _blk.readData<WorldInfo> ("worldstate", (*fit).hostid());

			if(wim != 0)
				if(wim.get() != 0)
				{
					//                   Logger::Instance().WriteMsg("SharedWorldModel", "Host Name: " + _toString((*fit).hostname()) + " \tRobot x: " + _toString(robot_x[count]) + " Robot y: " + _toString(robot_y[count]), Logger::Info);
					robot_x[count] = wim->myposition().x();
                    robot_y[count] = wim->myposition().y();
                    robot_phi[count] = wim->myposition().phi();

                    RobotPose rPose;
                    rPose.set_x(robot_x[count]);
                    rPose.set_y(robot_y[count]);
                    rPose.set_phi(robot_phi[count]);

                    TeammatePose tPose;
                    tPose.mutable_pose()->CopyFrom(rPose);

                    if(gsm!=0)
                        tPose.set_robotid(gsm->player_number());

                    swi.add_teammateposition();
                    swi.mutable_teammateposition(count)->CopyFrom(tPose);

                    if (wim->balls_size() > 0)
					{
						bx = wim->balls(0).relativex() + wim->balls(0).relativexspeed() * 0.200;
						by = wim->balls(0).relativey() + wim->balls(0).relativeyspeed() * 0.200;
						bd[count] = sqrt(pow(bx, 2) + pow(by, 2));
						count++;
					}
				}
		}

		//		Logger::Instance().WriteMsg("SharedWorldModel", "--------------------", Logger::Info);
	}

	//Local WorldInfo and ball distance
	wim  = _blk.readData<WorldInfo> ("worldstate");

	if(wim != 0){
		if(wim.get() != 0)
		{
			robot_x[0] = wim->myposition().x();
			robot_y[0] = wim->myposition().y();
			robot_phi[0] = wim->myposition().phi();

			RobotPose rPose;
            rPose.set_x(robot_x[0]);
            rPose.set_y(robot_y[0]);
            rPose.set_phi(robot_phi[0]);

            TeammatePose tPose;
            tPose.mutable_pose()->CopyFrom(rPose);
            tPose.set_robotid(444); //test

            swi.mutable_teammateposition(0)->CopyFrom(tPose);

			//            Logger::Instance().WriteMsg("SharedWorldModel", "Local World info: Robot x: " + _toString(robot_x[0]) + " Robot y: " + _toString(robot_y[0]), Logger::Info);
			if (wim->balls_size() > 0)
			{
				bx = wim->balls(0).relativex() + wim->balls(0).relativexspeed() * 0.200;
				by = wim->balls(0).relativey() + wim->balls(0).relativeyspeed() * 0.200;
				bd[0] = sqrt(pow(bx, 2) + pow(by, 2));
				//                Logger::Instance().WriteMsg("SharedWorldModel", "My bd " + _toString(bd[0]) + " bx " + _toString(bx) + " by " + _toString(by), Logger::Info);
			}

			//Find the robot which is closer to the ball and publish the corresponding message
			idx = findClosestRobot();
			//           Logger::Instance().WriteMsg("SharedWorldModel", "Idx " + _toString(idx), Logger::Info);
			swi.mutable_playerclosesttoball()->set_x(robot_x[idx]);
			swi.mutable_playerclosesttoball()->set_y(robot_y[idx]);
			_blk.publishData(swi, "worldstate");
		}
	}
}

int SharedWorldModel::findClosestRobot()
{
	float min = INF;
	int index = 0;

	for(int i = 0; i < numOfRobots; i++)
	{
		if(bd[i] < min && bd[i] != INIT_VALUE)
		{
			min = bd[i];
			index = i;
		}
	}

	return index;
}

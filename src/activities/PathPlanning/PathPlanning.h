#ifndef PATHPLANNING_H
#define PATHPLANNING_H

#include <iomanip>
#include <string>
#include <cstring>
#include <fstream>
#include <math.h>
#include <vector>
#include <map>
#include <cstdlib>
#include <list>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/pool/object_pool.hpp>

#include "core/include/IActivity.hpp"

#include "messages/PathPlanningMessages.pb.h"
#include "messages/SensorsMessage.pb.h"
#include "messages/Motion.pb.h"

#include "hal/robot/generic_nao/robot_consts.h"


//#define KPROFILING_ENABLED
#include "tools/profiler.hpp"

#include "core/elements/math/Common.hpp"
#include "core/elements/customTypes/K3DArray.hpp"
#include "core/elements/customTypes/ReservaplePrioryQueue.hpp"
#include "core/architecture/configurator/Configurator.hpp"

#include "GridMap.h"

ACTIVITY_START
class PathPlanning: public IActivity {

public:

	ACTIVITY_VISIBLE PathPlanning(Blackboard& b);

	int ACTIVITY_VISIBLE IEX_DIRECTIVE_HOT Execute();
	void ACTIVITY_VISIBLE UserInit();
	void ACTIVITY_VISIBLE Reset();

	~PathPlanning(){
		delete leftSonars;
		delete rightSonars;
	}
	std::string ACTIVITY_VISIBLE GetName() {
		return "PathPlanning";
	}

	struct coords {
		int x, y, z;
	};

	struct mapKey {
		int x, y, z;
		bool operator< (const mapKey &mapKey1) const {
			return (x << 16) + (y << 8) + z < (mapKey1.x << 16) + (mapKey1.y << 8) + mapKey1.z;
		}
	};

	struct node {
		int l, x, y, z;
		float g, h;
	};
	class mycomparison
	{
	public:
		mycomparison(){}
		bool operator() (const node* lhs, const node* rhs) const
		{
			return lhs->h > rhs->h;
		}
	};
	
private:

	GridMap pathMap;
	GridMap smallPathMap;
	
	GridInfo gridInfoMessage;
	MotionWalkMessage wmot;
	
	boost::shared_ptr<const AllSensorValuesMessage> allsm;
	boost::shared_ptr<const RobotPositionMessage> rpm;
	boost::shared_ptr<const PathPlanningRequestMessage> pprm;

	float* leftSonars;
	float* rightSonars;

	void processOdometryData();
	void publishGridInfo();
	void printSonarValues();
	void commitMovement();
	void velocityWalk(float ix, float iy, float it, float f);
		
	mutable KProfiling::profiler vprof;
	bool gameMode;
	bool firstTime;
	//Obstacles
	bool updateObstacles;
	bool checkForTargetInObstacle;
	//Sonars preferences
	float SonarsMinDist;
	float SonarsMaxDist;
	float SonarsConeInDegs;

	//AStar Functions
	void aStar ();
	bool targetReachable (int x, int y);
	inline void setupNode (node *n, int x, int y, int z, float g);
	inline void updateH (node *n);
	inline void updateG (node *n, int oldX, int oldY, int oldZ, int newX, int newY, int newZ);
	inline bool lessThan (const node* a, const node* b) const{
		//KPROF_SCOPE(vprof, "lessThan");
		return a->h > b->h;
	}
	//aStar global values
	boost::object_pool<node> pool;
	std::vector<coords> pathFromAStar;

	//std::vector<temp> openNodes;
	reservable_priority_queue<node *,std::vector<node*>, mycomparison > openNodes;
	K3DArray<float> currentValues;
	K3DArray<int> directions;

	bool hasTarget;
	int aStarTargetR;
	int aStarTargetC;
	int aStarTargetZ;
	bool aStarUseSmallGrid;
	float aStarRealTargetX;
	float aStarRealTargetY;
	int aStarDirections;
	float aStarTransformation;
	int aStarCircleCells;
	int aStarRadiusCells;
	int counter;

	//Odometry global values
	float newOdometryX,newOdometryY,newOdometryZ;
	float oldOdometryX,oldOdometryY,oldOdometryZ;
	bool firstOdometryData;

	boost::posix_time::ptime currentTime;

	/*std::vector<temp>::iterator returnMin(){
		float min = 1000000.0f;
		std::vector<temp>::iterator minIter;
		for(std::vector<temp>::iterator iter=openNodes.begin(); iter!=openNodes.end(); iter++){
			//std::cout << iter->checked << " ";
			if(iter->checked == false && iter->h < min){
				min = iter->h;
				minIter = iter;
			}
		}
		//std::cout << std::endl;
		return minIter;
	}*/
};



ACTIVITY_END

#endif


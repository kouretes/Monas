#ifndef OBSTACLEAVOIDANCE_H
#define OBSTACLEAVOIDANCE_H

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

#include "messages/TestMessage.pb.h"
#include "messages/SensorsMessage.pb.h"
#include "messages/BehaviorMessages.pb.h"
#include "messages/motion.pb.h"
#include "messages/ObstacleAvoidanceMessage.pb.h"
#include "hal/robot/generic_nao/robot_consts.h"

//#include "architecture/archConfig.h"
#include "core/include/IActivity.hpp"

#include "core/elements/math/Common.hpp"
#include "tools/obstacleConst.h"


//IF SOME DEFINE CHANGE, YOU MUST CHANGE THE DEFINE IN /GUITools/KMonitos/KMapScene.h
#define M 			10
#define N 			18
#define TotalRings	(1+M+1)
#define InnerRing	(0)
#define OuterRing	(1+M)
#define NEIGHBOURS 	8

#define RingDistance 	( (MapRadius) / (M) )
#define SectorAngleDeg 	(360.0/N) // deg
#define SectorAngleRad 	(TO_RAD(SectorAngleDeg))
#define SectorShiftRad	( ( (N%4) == 0 ) ? 0.5*SectorAngleRad : 0.0 )

#define FRONT 			(int(N/4))
#define SIDE_LEFT 		(int(N/2))
#define SIDE_RIGHT 		( ( (N%4) == 0 ) ? 0 :N-1 )


#define SonarAngleShiftRad	(TO_RAD(SonarAngleShiftDeg))
#define SonarAngleRangeRad 	(TO_RAD(SonarAngleRangeDeg))

#define RotationAngle 		(360.0/(N))
#define RotationAngleRad 	(TO_RAD(RotationAngle))

#define RIGHT 			(int(FRONT - (SonarAngleShiftDeg/RotationAngle)))
#define LEFT 			(int(FRONT + (SonarAngleShiftDeg/RotationAngle)))



#define NumOfTargetCoordinates 3



using namespace  std;

ACTIVITY_START
class ObstacleAvoidance: public IActivity
{

public:
	ACTIVITY_CONSTRUCTOR(ObstacleAvoidance)


	int ACTIVITY_VISIBLE IEX_DIRECTIVE_HOT Execute();
	void ACTIVITY_VISIBLE UserInit();
	void ACTIVITY_VISIBLE Reset();
	void read_messages();
	struct OpenListNode
	{
		int ring;
		int sector;
		int orientation;
		double angle;
		double gn;
		double hn;
		double fn;
	} ;
	std::string ACTIVITY_VISIBLE GetName()
	{
		return "ObstacleAvoidance";
	}


private:
	double AgeFactor ;
	double ageTimerSeconds;

	double PolarGrid[2][TotalRings][N];
	int present, past;

	boost::posix_time::ptime lastwalk;
	boost::posix_time::ptime lastAge;

	/*********** Robot coordinates - Odometry ************/
	double robotX, robotY, robotA;
	double originX, originY, originA;
	double odometryX, odometryY, odometryA;
	bool initializeOdometry ;
	double cellCenterX[TotalRings][N],
	       cellCenterY[TotalRings][N];

	/*********** Map Update ***********/

	double Right[KDeviceLists::US_SIZE], Left[KDeviceLists::US_SIZE], empty[KDeviceLists::US_SIZE];
	double changed[TotalRings][N];
	int SonarFailCounter;
	int countLeft, countRight;
	double OccupancyDecreaseFactor ;
	double OccupancyIncreaseFactor ;
	double MinOccupancy 	;
	double MaxOccupancy 	;
	double SonarDistanceRange;
	double SonarMinReading;
	double CloseObstacleRadius;
	double CloseObstacleCenter;

	/*********** Path Planning ***********/
	double targetX, targetY, targetA;
	double targetX_0, targetY_0, targetA_0;
	double targetR, targetS, targetO;

	int whatList[TotalRings][N][NEIGHBOURS];
	int parentM[TotalRings][N][NEIGHBOURS],
	    parentN[TotalRings][N][NEIGHBOURS],
	    parentO[TotalRings][N][NEIGHBOURS];

	int neighbourRing[NEIGHBOURS + 1],
	    neighbourSector[NEIGHBOURS + 1];

	double euclidean[TotalRings][N][TotalRings][N];
	int pathR[PathLength], pathS[PathLength], pathO[PathLength];
	int pathCounter;
	int testPathX, testPathY, testPathA;
	int relativeMode;
	double ObstacleThreshold ;
	double ObstacleCostFactor;


	/***** Walk *******/
	double GoalDistanceTolerance;
	double GoalAngleTolerance;
	double FrontDeviation, SideDeviation;
	double RotationCostFactor;
	double SideStepCostFactor;
	double SemiSideCostFactor;
	double RotationMoveFactor;
	float cX, cY, ct;
	float bd;

	/*******variables used in messages***********/
	SensorData RightValue[KDeviceLists::US_SIZE], LeftValue[KDeviceLists::US_SIZE];
	SensorData PosX, PosY, Angle;
	bool frontObstacle, rightObstacle, leftObstacle ;
	double frontDist, rightDist, leftDist;
	double frontCert, rightCert, leftCert;

	/* Incoming Messages */
	boost::shared_ptr<const  MotionWalkMessage> wm;
	boost::shared_ptr<const AllSensorValuesMessage> asvm;
	boost::shared_ptr<const RobotPositionMessage> rpm;
	boost::shared_ptr<const ObstacleMessage> DataFromVision;
	boost::shared_ptr<const PathPlanningRequestMessage> pprm;

	/* Outgoing Messages */
	ObstacleMessageArray obavm;
	GridInfo gridInfoMessage;
	MotionWalkMessage* wmot;
	PathPlanningRequestMessage* pprm_rel;


	/********** Debugging ***********/
	int debugModeCout;
	int debugRelativeMode;
	int debugCounter;

	/*********** OpenCV Drawing ***********/
	//void cvDrawGrid();
	void printSonarValues();

	/*********** Initialization ***********/
	void initGrid();
	void initCoordinates();
	void initEuclidean();
	void initPaths();
	void initPolarGrid();
	void initChanged();
	void initPossibilities();
	void initIndexes(int mm, int nn);
	void Initialize();

	/*********** Map Update ***********/
	void mapObstacle(double distance, double theta, double radius);
	void mapFreeSpace(double distance, double theta);
	void updateGrid(double* left, double* right);
	void chooseCloserObstacle();

	void ageGrid();

	/*********** Map Transformation ***********/
	void rotateGrid(double angle);
	void translateGrid(double deltaX, double deltaY);
	void moveRobot();

	/*********** Path Planning ***********/
	void initAstar();
	void insertInOpenList(OpenListNode anode, list<OpenListNode> &openList);
	void generateNode(OpenListNode next, int pr, int ps, int po, list<OpenListNode> &openList);
	void findNeighbours(int r, int s);
	void astar13Neighbours(int goalm, int goaln, int goalo);
	void reconstructPath(int ring, int sector, int orientation);

	double myAngleDiff(double a1, double a2);
	double myRound(double value);

	/*********** Messages *************/
	void gridInformationMessage();
	void publishGridInfo();
	void publishObstacleMessage();
	void pathPlanningRequestRelative(float target_x, float target_y, float target_phi);


	/*********** Motion ***********/
	void velocityWalk(double ix, double iy, double it, double f);
	void callVelocityWalk(double walkToX, double walkToY, double walkToT, double distance2Goal);
	void motionController(double distance2Goal);

	//Only for internal use
	int DtoR(double d);
	int TtoS(double theta);
	double RtoD(int r);
	double StoT(int s);
	int XYtoR(double x, double y);
	int XYtoS(double x, double y);
	int wrapTo(int n, int MAXN);

};

ACTIVITY_END

#endif


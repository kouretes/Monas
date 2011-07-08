#ifndef OBSTACLEAVOIDANCE_H
#define OBSTACLEAVOIDANCE_H
#include "hal/robot/generic_nao/robot_consts.h"
#include "messages/TestMessage.pb.h"
#include "messages/SensorsMessage.pb.h"
#include "messages/motion.pb.h"
#include "messages/ObstacleAvoidanceMessage.pb.h"
#include <vector>
#include <map>
#include <list>
#include <string>
#include "architecture/IActivity.h"

#include <opencv/cv.h>
#include <opencv/highgui.h>

//#include "alptr.h"

//~ 
#define M 7
#define N 36
#define colorMax 255
#define EMPTY 0.0
#define PathChosen 2.0
#define RotationAngle (360.0/(N)) //10 degrees
#define FRONT ((N/4)-1)	// ((N/4)-1) //8 //17
//#define RIGHT (FRONT+4) //21	//12
#define RIGHT (FRONT-SonarCellRange)//5 //21	
#define LEFT (FRONT+SonarCellRange+1)//12 //13		
//#define LEFT (FRONT-4) //13		//4
#define RotationAngleRad ((RotationAngle)*PI/180)
#define SonarCellRange  int(SonarAngleRange/(RotationAngle))
#define ToDegrees (180/PI)
#define TooFar (TooClose+(M*0.1))
#define distance (((SonarDistanceRange)-30)/(M))  //10

#define SOnARsNum KDeviceLists::US_SIZE
#define PI 3.14159f
#define NEIGHBOURS 8
#define GridCm 200
#define BodyCm 20
#define cmPerCell (GridCm/CELLS)
#define SonarDistanceRange 100 //in cm
#define NoKnowledge 0.5

#define WALK_MAX_STEP_X  0.04;
#define WALK_MAX_STEP_Y  0.04;
#define WALK_MAX_STEP_THETA  20;

#define ToRad (PI/180)


using namespace  std;

class ObstacleAvoidance: public IActivity {

	public:
		ObstacleAvoidance();
		int grid[3][3];
		int Execute();
		void UserInit();
		void read_messages();
		void initGrid();
        std::string GetName() {
            return "ObstacleAvoidance";
        }

	private:
		
		int SonarAngleRange;
		int RobotDirections;
		int shiftConsole, ToCm;
		int angleDegrees;
		double TOLERANCE;
		double FrontDeviation, SideDeviation;

		double TooClose;
		double MinimumDistanceValue, MinimumValidValue, MinimumValidSonarValue;
		
		int ageTimerSeconds;
		int obstacleFound;
		int velosityShift;
		int current;
		int leftCounter, rightCounter, SonarFailCounter, firstTimeInitializeOdometry;
		int countLeft, countRight, countPos, initializeOdometry;
		int robotCells;

		double PolarGrid[2][M+3][N];
		double IterationGrid[M+3][N];
		double RobotPosition[3];
		double Right[10], Left[10], empty[10];
		double resX, resY, resAngle;
		double rightArray[10], leftArray[10];
		int cellCenterX[(M+3)*N], cellCenterY[(M+3)*N];
		int x[(M+4)*N], y[(M+4)*N];
		double changed[M*N];
		int index[M*N], indey[M*N];
		
		typedef map<int, int> mapType;
		mapType MoveGrid;
		
		//for path planning
		int discount;
		double reward, targetX, targetY;
		int goal, obstacle, goalX, goalY, goalOrientation, targetOrientation;
		int ITERATIONS, CELLS, WATCHDIRECTION;
		double possibilities[NEIGHBOURS+1], value[NEIGHBOURS+1] ;
		int indexx[NEIGHBOURS+1], indexy[NEIGHBOURS+1] ;
		int  orient[(M+3)*N][NEIGHBOURS];
		int parent[(M+3)*N][NEIGHBOURS];
		int whatList[(M+3)*N][NEIGHBOURS];
		double euclidean[(M+4)*N][(M+4)*N];
		double euclideanOrientation[(M+4)*N][(M+4)*N];
		int obstacleCost;
		struct node {
			double gn;
			double hn;
			double fn;
			int cell;
			int parentcell;
			int orientation;
		};
		list<node> openList, closedList;
		int nextM[2], nextN[2], nextOr[2], counterPath;
		
		//for velocityWalk
		float cX, cY, ct;

		//debuging
		int debugModeCout;
		int debugInstantResponce;
		int debugWithMovement;
		int debugPathPlannignDirect;
		int debugFullPathPlanning;
		int debugModePublishMotion;
		int debugCounter;
		boost::posix_time::ptime debugTimer, lastwalk;
		boost::posix_time::ptime lastAge;

		//possibilities
		double PossibilityDiscount, PossibilityDiscountNoObst;
		double UsePossibilityDown, UsePossibilityUp;
		double MinPossibility, MaxPossibility;
		double ObstaclePossibility, HighObstaclePossibility;
		double agePossibility;
		
		//gui variables
		int cvHighgui;
		IplImage *img;
		CvScalar white ;
		CvScalar lightgrey ;
		CvScalar grey;
		CvScalar darkgrey ;
		CvScalar black;
		CvScalar blue ;
		CvScalar  paintColor;
		CvPoint curve1[4];
		int colorValue;
		int nCurvePts, nCurves, isCurveClosed, lineWidth;

		//*******variables used in messages
		//sonar message
		SensorData RightValue[10], LeftValue[10];
		SensorData PosX, PosY, Angle;
		//obstacle existance msg
		bool mprosta, dexia, aristera ;
		double mprostaDist, dexiaDist, aristeraDist;
		double mprostaCert, dexiaCert, aristeraCert;
		//motion message
		float velocityParam1, velocityParam2, velocityParam3, velocityParam4, velocityParam5;
		float xDirection, yDirection, thetaDirection;
		float xDistance, yDistance;
		float velocityAngle, velocityDistance;
		
		//messages
		boost::shared_ptr<const  MotionWalkMessage> wm;
		//boost::shared_ptr<const ResetMessage> resetMsg;
		boost::shared_ptr<const AllSensorValuesMessage> asvm;
		boost::shared_ptr<const RobotPositionMessage> rpm;
		//boost::shared_ptr<const SonarsData> sonarData;
		//boost::shared_ptr<const beam> beamVals;
		boost::shared_ptr<const ObstacleMessage> DataFromVision;
		boost::shared_ptr<const PathPlanningRequestMessage> pprm;
		ObstacleMessageArray obavm;
		//beam BeamMessage;
		//SonarsData SDM;
		
		std::vector<float> tempBeam;
		MotionWalkMessage* wmot;
		PathPlanningResultMessage* ppresm;
		PathPlanningRequestMessage* ppreqm;
		
		
			 double angle;  
    CvPoint center, toP, fromP, leftP, rightP;  
    CvScalar color;  
	CvRect comp_rect;  
		
		
		void publishObstacleMessage();
		void publishMotionMessage(int initn);
		
		void cvDrawGrid();
		void drawIterationGrid();		
		void printSonarValues();
		
		void updateGrid(double (&left)[SOnARsNum], double (&right)[SOnARsNum]);
		void updateFront();
		
		void ageRestGrid();
		void ageGrid();
		void ageSpecGrid(int x, int y);
		void findNewPosition();
		void straightMove(int distanceFront, int distanceSide);
		void straightMoveGrid(double deltaX,double deltaY, double deltatheta);
		void rotateGrid(double angle);
		void findCoordinates();
		
		void iterationGridDir(int mm, int nn);
		void bestPath(int goalx, int goaly);
		void fillIterationGrid(int goalx, int goaly);

		void initPolarGrid();
		void initMoveGrid();
		void initChanged();
		void initPathPlanning(int goalx, int goaly);
		void initPossibilities();
		void initIndexes(int mm, int nn);
		void Initialize();
		
		void reset();
		double angleDiff(double a1, double a2);
		double wrapToPi(double angle);
		double angleDiff2(double a1, double a2);
		double myRound(double value);
		int checkForObstacle(float checkAngle, float checkDistance);
		void examplePathPlanningInit();
		void gridInformationMessage();
		
		void EuclideanLookup();
		void EuclideanOrientationLookup();
		void reconstructPathNew(int currentcell, int orientt);
		void insertInOpenList(node anode);
		void astar3Neighbours(int goalx, int goaly, int goalorient);
		void astarPlain(int goalx, int goaly);
		void velocityWalk(double ix, double iy, double it, double f);
		void reconstructPath(int currentcell);
		void callVelocityWalk(double bx, double by, double bb);

		void testPath(int m, int n, int o);

};

#endif


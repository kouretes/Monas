#ifndef OBSTACLEAVOIDANCE_H
#define OBSTACLEAVOIDANCE_H
#include "messages/TestMessage.pb.h"
#include "messages/SensorsMessage.pb.h"
#include "messages/ObstacleAvoidanceMessage.pb.h"
#include <vector>
#include <map>
#include <list>
#include <string>
#include "architecture/IActivity.h"

#include <opencv/cv.h>
#include <opencv/highgui.h>

//#include "alptr.h"


#define N 36
#define M 7
#define RotationAngle (360/N)  //10
#define FRONT ((N/2)-1)
#define SonarAngleRange 30
#define RobotDirections 3
#define RotationAngleRad ((RotationAngle)*PI/180)
#define SonarCellRange (SonarAngleRange/(RotationAngle))
#define ToDegrees (180/PI)

#define HighObstaclePossibility 0.8

#define TooClose 0.2
#define TooFar (TooClose+(M*0.1))

#define NoKnowledge 0.5
#define distance 10
#define SOnARsNum 10
#define PI 3.14159f

#define discount 1
#define reward -0.05
#define goal 1
#define obstacle -1
#define NEIGHBOURS 8
#define ITERATIONS 7

#define CELLS 100
#define UsePossibilityDown 0.9
#define UsePossibilityUp 1.1

#define PossibilityDiscount 0.4
#define PossibilityDiscountNoObst 0.8

#define GridCm 200
#define BodyCm 20
#define cmPerCell (GridCm/CELLS)

using namespace  std;

//namespace AL {
//	class ALMotionProxy;
//	class ALMemoryProxy;
//}

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
		bool cvHighgui;
		IplImage *img;
		CvScalar white ;
		CvScalar lightgrey ;
		CvScalar grey;
		CvScalar darkgrey ;
		CvScalar black;
		CvScalar blue ;
		CvScalar paintColor;
		CvPoint curve1[4];
		int nCurvePts;
		int nCurves;
		int isCurveClosed;
		int lineWidth;
		
		
//		AL::ALPtr<AL::ALMemoryProxy> memory;
		double PolarGrid[M][N];
				
		typedef map<int, int> mapType;
		mapType MoveGrid;
		//int MoveGrid[100][100];
		int goalX, goalY;
		int leftCounter, rightCounter;
		SensorPair RightValue[10];
		SensorPair LeftValue[10];
		SensorPair PosX;
		SensorPair PosY;
		SensorPair Angle;
		//double grid[M][N];
		double RobotPosition[3];
		double IterationGrid[M][N];
		double resX, resY, resAngle;
		double rightArray[10], leftArray[10];
		int countLeft, countRight, countPos, firstTimeInitializeOdometry;
		int x[(M+1)*N];
		int y[(M+1)*N];
		

		bool mprosta ;
		double mprostaDist;
		double mprostaCert;
		bool dexia ;
		double dexiaDist;
		double dexiaCert;
		bool aristera;
		double aristeraCert;
		double aristeraDist;
		
		
		
		double changed[M*N];
		int shiftConsole, index[M*N], indey[M*N];
		double possibilities[NEIGHBOURS+1], value[NEIGHBOURS+1] ;
		int indexx[NEIGHBOURS+1], indexy[NEIGHBOURS+1] ;//gia path planning
		
		boost::shared_ptr<const UltaSoundSensorsMessage> ussm;
		boost::shared_ptr<const RobotPositionSensorMessage> rpsm;
		ObstacleMessageArray obavm;
		boost::shared_ptr<const ObstacleMessage> DataFromVision;
		PathPlanningResultMessage* ppresm;
		PathPlanningRequestMessage* ppreqm;
		
		double Right[10], Left[10], empty[10];
		int countAge;
		int countValid;
		void publishObstacleMessage();
		void printIterationGrid();
		void smoothGrid(int smooth);
		//void updateGrid(double (&newValues1)[10], double (&newValues2)[10]);
		void updateGrid(double (&left)[SOnARsNum], double (&right)[SOnARsNum]);
		void updateFront();
		void ageRestGrid();
		
		void ageGrid();
		void findNewPosition();
		void straightMove(int distanceFront, int distanceSide);
		void rotateGrid(double angle);
		void findCoordinates();
		
		void initPossibilities();
		void initIndexes(int mm, int nn);
		void iterationGridDir(int mm, int nn);
		void bestPath(int goalx, int goaly);
		void fillIterationGrid();
		void pathPlanningInit(int goalx, int goaly);
		void printSonarValues();
		void drawGrid();
		void cvDrawGrid();
		void initPolarGrid();
		void drawIterationGrid();
		void ageSpecGrid(int x, int y);
		
};

#endif  


#ifndef OBSTACLEAVOIDANCE_H
#define OBSTACLEAVOIDANCE_H
#include "architecture/narukom/pub_sub/publisher.h"
#include "architecture/narukom/pub_sub/publisher.h"
#include "messages/TestMessage.pb.h"
#include "messages/SensorsMessage.pb.h"

#include <vector>
#include <map>
#include <string>
#include "architecture/IActivity.h"
#include "architecture/narukom/pub_sub/publisher.h"

#include "alptr.h"


#define N 36
#define M 7
#define PI 3.14159f
#define RotAngle 360/N
#define RotAngleRad RotAngle*PI/180
#define distance 10
#define discount 1
#define reward -0.05
#define goal 10
#define obstacle -50
#define NEIGHBOURS 8
#define ITERATIONS 7


namespace AL {
	class ALMotionProxy;
	class ALMemoryProxy;
}

class ObstacleAvoidance: public IActivity, public Publisher {
	
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
		AL::ALPtr<AL::ALMemoryProxy> memory;
		int flag;
		double PolarGrid[M][N];
		int MoveGrid[100][100];
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
		
		int x[(M+1)*N];
		int y[(M+1)*N];
		double changed[M*N];
		int shift, index[M*N], indey[M*N];
		UltaSoundSensorsMessage * ussm;
		RobotPositionSensorMessage * rpsm;
		int countAge, countPos;
		double countLeft;
		double countRight;
		
		void smoothGrid(int smooth);
		void updateGrid(double newValues2, double newValues1);
		void ageGrid();
		void findNewPosition();
		void straightMove(int distanceFront, int distanceSide);
		void rotateGrid(double angle);
		void findCoordinates();
		void initIndexes(int mm, int nn);
		double iterationGridDir(int num, int mm, int nn);
		void bestPath(int goalx, int goaly);
		void fillIterationGrid();
		void pathPlanningInit(int goalx, int goaly);

		void drawGrid();
};

#endif  


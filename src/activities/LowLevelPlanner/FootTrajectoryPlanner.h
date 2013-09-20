//
//  FootTrajectoryPlanner.h
//  Kouretes Walk Engine
//
//  Created by Stelios Piperakis on 8/14/13.
//  Copyright (c) 2013 SP. All rights reserved.
//
/* Foot Trajectory Planner */
#ifndef _FOOTTRAJECTORYPLANNER_
#define _FOOTTRAJECTORYPLANNER_
#include "Stepplanner.h"


#include "KWalkMat.h"
class FootTrajectoryPlanner
{
	private:
		PlanePoint startL, startR, targetL, targetR;
		KWalkMat KWalkMath;
		bool RIGHTLEGSUPPORT;
		Stepplanner* Planner;
		RobotParameters Robot;
		int fillcounter[4][2];
	public:
		std::vector<float> Xl, Yl, Zl, Thl, Xr, Yr, Zr, Thr;
		FootTrajectoryPlanner();

		void FootTrajectoryInitialize(Stepplanner*, RobotParameters, KWalkMat);
		void FootTrajectoryInitStep();
		int FootTrajectoryInitStep(float Buffer[3][2][MAX_TRAJECTORY_LENGTH]);
		//void FootTrajectoryInterpolate();
		int FootTrajectoryInterpolate(float Buffer[3][2][MAX_TRAJECTORY_LENGTH],int append);
		void NextTrajectory();
		//void FootTrajectoryClear();

};

#endif


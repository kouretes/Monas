//
//  ZMPTrajectoryPlanner.cpp
//  Kouretes Walk Engine
//
//  Created by Stelios Piperakis on 8/14/13.
//  Copyright (c) 2013 SP. All rights reserved.
//

#include "ZMPTrajectoryPlanner.h"
ZMPTrajectoryPlanner::ZMPTrajectoryPlanner()
{
	Planner=NULL;
       
}
void ZMPTrajectoryPlanner::ZMPTrajectoryInitialize(Stepplanner* xPlanner,RobotParameters xRobot,KWalkMat xMath)
{
    Planner=xPlanner;
    Robot=xRobot;
    KWalkMath=xMath;
    fillcounter[0]=0;fillcounter[1]=0;
}

void ZMPTrajectoryPlanner::NextTrajectory()
{
	start=target;
	target=Planner->ZmpQ.front();
	Planner->ZmpQ.pop();
}

int ZMPTrajectoryPlanner::ZMPTrajectoryInterpolate(float Buffer[2][MAX_TRAJECTORY_LENGTH], int append)
{
	if(!append){
		fillcounter[0]=0;fillcounter[1]=0;
	}
	//Interpolating the DS Phase
	KWalkMath.LinearInterpolation(Buffer[X],fillcounter[X],start.x, target.x ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tds));
	KWalkMath.LinearInterpolation(Buffer[Y],fillcounter[Y],start.y, target.y ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tds));
	//Interpolating the SS Phase
	KWalkMath.LinearInterpolation(Buffer[X],fillcounter[X],target.x, target.x ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tss));
	KWalkMath.LinearInterpolation(Buffer[Y],fillcounter[Y],target.y, target.y ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tss));

//	//Interpolating the DS Phase
//	KWalkMath.LinearInterpolation(ZMPX,start.x, target.x ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tds));
//	KWalkMath.LinearInterpolation(ZMPY,start.y, target.y ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tds));
//	//Interpolating the SS Phase
//	KWalkMath.LinearInterpolation(ZMPX,target.x, target.x ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tss));
//	KWalkMath.LinearInterpolation(ZMPY,target.y, target.y ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tss));

	return fillcounter[0];
}

//void ZMPTrajectoryPlanner::ZMPTrajectoryInterpolate()
//{
//	//Interpolating the DS Phase
//	KWalkMath.LinearInterpolation(ZMPX,start.x, target.x ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tds));
//	KWalkMath.LinearInterpolation(ZMPY,start.y, target.y ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tds));
//	//Interpolating the SS Phase
//	KWalkMath.LinearInterpolation(ZMPX,target.x, target.x ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tss));
//	KWalkMath.LinearInterpolation(ZMPY,target.y, target.y ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tss));
//}

int ZMPTrajectoryPlanner::ZMPTrajectoryInitStep(float Buffer[2][MAX_TRAJECTORY_LENGTH])
{
	start=Planner->ZmpQ.front();
	target=start;
	Planner->ZmpQ.pop();

	fillcounter[0]=0;
	fillcounter[1]=0;
	KWalkMath.LinearInterpolation(Buffer[X],fillcounter[X],start.x, target.x ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tstep));
	KWalkMath.LinearInterpolation(Buffer[Y],fillcounter[Y],start.y, target.y ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tstep));

//	KWalkMath.LinearInterpolationINIT(ZMPX,start.x, target.x ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tstep));
//	KWalkMath.LinearInterpolationINIT(ZMPY,start.y, target.y ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tstep));

//	for(int i = 0; i < ZMPX.size(); i++){
//		std::cout << " Buffer[X] " << Buffer[X][i] << "  Zmpx " <<  ZMPX[i] << std::endl;
//	}

	return fillcounter[0];
}

//void ZMPTrajectoryPlanner::ZMPTrajectoryInitStep()
//{
//	start=Planner->ZmpQ.front();
//	target=start;
//	Planner->ZmpQ.pop();
//
//    KWalkMath.LinearInterpolationINIT(ZMPX,start.x, target.x ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tstep));
//    KWalkMath.LinearInterpolationINIT(ZMPY,start.y, target.y ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tstep));
//}

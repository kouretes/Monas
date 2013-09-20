//
//  FootTrajectoryPlanner.cpp
//  Kouretes Walk Engine
//
//  Created by Stelios Piperakis on 8/14/13.
//  Copyright (c) 2013 SP. All rights reserved.
//
#include "FootTrajectoryPlanner.h"


FootTrajectoryPlanner::FootTrajectoryPlanner(){
	RIGHTLEGSUPPORT=1;
	Planner=NULL;
}
void FootTrajectoryPlanner::FootTrajectoryInitialize(Stepplanner* xPlanner,RobotParameters xRobot,KWalkMat xMath)
{
    Planner=xPlanner;
    Robot=xRobot;
    KWalkMath=xMath;
    memset(fillcounter,0,4*2*sizeof(int));
}
void FootTrajectoryPlanner::NextTrajectory()
{
    if(RIGHTLEGSUPPORT)
    {
        startL=targetL;
        targetL=Planner->FootQ.front();
        startR=targetR;
    }
    else
    {
        startR=targetR;
        targetR=Planner->FootQ.front();
        startL=targetL;
    }
    Planner->FootQ.pop();
}
//Foot Trajectory Interpolation using simple multidimensional array to save results, returns the size of the trajectory points created
int FootTrajectoryPlanner::FootTrajectoryInterpolate(float Buffer[3][2][MAX_TRAJECTORY_LENGTH], int append )
{
	 //std::cout << "fillcounter[X][LEFT] " << fillcounter[X][LEFT] << " append " << append << std::endl;
	Xl.clear();
	Zl.clear();
	Yl.clear();
	Xr.clear();
	Zr.clear();
	Yr.clear();

	if(!append)
		memset(fillcounter,0,4*2*sizeof(int));

	int counterstart= fillcounter[X][LEFT];
	//std::cout << " Init counter" <<  counterstart << std::endl;
	// std::cout << "fillcounter[X][LEFT] " << fillcounter[X][LEFT] << std::endl;
	if (RIGHTLEGSUPPORT)
	{
	    RIGHTLEGSUPPORT=0;
	    //std::cout << " Ts " << Robot.getWalkParameter(Ts) << " Tds " << Robot.getWalkParameter(Tds) << std::endl;
	    std::cout << " startL.x" << startL.x << "  startL.y  " << startL.y  << " Target.x" << targetL.x << "  targetL.y  " << targetL.y << std::endl;

	    KWalkMath.LinearInterpolation(Buffer[X][LEFT], fillcounter[X][LEFT], startL.x, startL.x ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tds));
	    KWalkMath.LinearInterpolation(Buffer[Y][LEFT], fillcounter[Y][LEFT], startL.y, startL.y ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tds));
	    KWalkMath.LinearInterpolation(Buffer[Z][LEFT], fillcounter[Z][LEFT], 0.000, 0.000 ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tds));
	    KWalkMath.LinearInterpolation(Buffer[Theta][LEFT], fillcounter[Theta][LEFT], startL.theta, startL.theta ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tds));
	 //  std::cout << "1fillcounter[X][LEFT] " << fillcounter[X][LEFT] << std::endl;
	    KWalkMath.LinearInterpolation(Buffer[X][RIGHT], fillcounter[X][RIGHT], startR.x, startR.x ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tds));
	    KWalkMath.LinearInterpolation(Buffer[Y][RIGHT], fillcounter[Y][RIGHT], startR.y, startR.y ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tds));
	    KWalkMath.LinearInterpolation(Buffer[Z][RIGHT], fillcounter[Z][RIGHT], 0.000, 0.000 ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tds));
	    KWalkMath.LinearInterpolation(Buffer[Theta][RIGHT], fillcounter[Theta][RIGHT], startR.theta, startR.theta ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tds));
	   // std::cout << "2fillcounter[X][LEFT] " << fillcounter[X][LEFT] << std::endl;
	    KWalkMath.LinearInterpolation(Buffer[X][LEFT], fillcounter[X][LEFT], startL.x, targetL.x ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tss));
	    KWalkMath.LinearInterpolation(Buffer[Y][LEFT], fillcounter[Y][LEFT], startL.y, targetL.y ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tss));
	    KWalkMath.CubicBezierInterpolation(Buffer[Z][LEFT], fillcounter[Z][LEFT], 0.000, 0.000 ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tss));
	    KWalkMath.LinearInterpolation(Buffer[Theta][LEFT],fillcounter[Theta][LEFT],startL.theta, targetL.theta ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tss));
	    //std::cout << "3fillcounter[X][LEFT] " << fillcounter[X][LEFT] << std::endl;
	    KWalkMath.LinearInterpolation(Buffer[X][RIGHT], fillcounter[X][RIGHT], startR.x, startR.x ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tss));
	    KWalkMath.LinearInterpolation(Buffer[Y][RIGHT], fillcounter[Y][RIGHT], startR.y, startR.y ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tss));
	    KWalkMath.LinearInterpolation(Buffer[Z][RIGHT], fillcounter[Z][RIGHT], 0.000, 0.000 ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tss));
	    KWalkMath.LinearInterpolation(Buffer[Theta][RIGHT], fillcounter[Theta][RIGHT], startR.theta, startR.theta ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tss));
	    //std::cout << "4fillcounter[X][LEFT] " << fillcounter[X][LEFT] << std::endl;



		KWalkMath.LinearInterpolation(Xl,startL.x, startL.x ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tds));
		KWalkMath.LinearInterpolation(Yl,startL.y, startL.y ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tds));
		KWalkMath.LinearInterpolation(Zl,0.000, 0.000 ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tds));
		KWalkMath.LinearInterpolation(Thl,startL.theta, startL.theta ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tds));

		KWalkMath.LinearInterpolation(Xr,startR.x, startR.x ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tds));
		KWalkMath.LinearInterpolation(Yr,startR.y, startR.y ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tds));
		KWalkMath.LinearInterpolation(Zr,0.000, 0.000 ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tds));
		KWalkMath.LinearInterpolation(Thr,startR.theta, startR.theta ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tds));


		KWalkMath.LinearInterpolation(Xl,startL.x, targetL.x ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tss));
		KWalkMath.LinearInterpolation(Yl,startL.y, targetL.y ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tss));
		KWalkMath.CubicBezierInterpolation(Zl,0.000, 0.000 ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tss));
		KWalkMath.LinearInterpolation(Thl,startL.theta, targetL.theta ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tss));

		KWalkMath.LinearInterpolation(Xr,startR.x, startR.x ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tss));
		KWalkMath.LinearInterpolation(Yr,startR.y, startR.y ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tss));
		KWalkMath.LinearInterpolation(Zr,0.000, 0.000 ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tss));
		KWalkMath.LinearInterpolation(Thr,startR.theta, startR.theta ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tss));

	}
	else
	{
	    RIGHTLEGSUPPORT=1;
	    //std::cout << "fillcounter[X][LEFT] " << fillcounter[X][LEFT] << std::endl;
	    KWalkMath.LinearInterpolation(Buffer[X][LEFT], fillcounter[X][LEFT], targetL.x, targetL.x ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tds));
	    KWalkMath.LinearInterpolation(Buffer[Y][LEFT], fillcounter[Y][LEFT], targetL.y, targetL.y ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tds));
	    KWalkMath.LinearInterpolation(Buffer[Z][LEFT], fillcounter[Z][LEFT], 0.000, 0.000 ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tds));
	    KWalkMath.LinearInterpolation(Buffer[Theta][LEFT],fillcounter[Theta][LEFT], targetL.theta, targetL.theta ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tds));

	    KWalkMath.LinearInterpolation(Buffer[X][RIGHT], fillcounter[X][RIGHT], startR.x, startR.x ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tds));
	    KWalkMath.LinearInterpolation(Buffer[Y][RIGHT], fillcounter[Y][RIGHT], startR.y, startR.y ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tds));
	    KWalkMath.LinearInterpolation(Buffer[Z][RIGHT], fillcounter[Z][RIGHT], 0.000, 0.000 ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tds));
	    KWalkMath.LinearInterpolation(Buffer[Theta][RIGHT], fillcounter[Theta][RIGHT], startR.theta, startR.theta ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tds));

	    KWalkMath.LinearInterpolation(Buffer[X][LEFT], fillcounter[X][LEFT], targetL.x, targetL.x ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tss));
	    KWalkMath.LinearInterpolation(Buffer[Y][LEFT], fillcounter[Y][LEFT], targetL.y, targetL.y ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tss));
	    KWalkMath.LinearInterpolation(Buffer[Z][LEFT], fillcounter[Z][LEFT], 0.000, 0.000 ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tss));
	    KWalkMath.LinearInterpolation(Buffer[Theta][LEFT],fillcounter[Theta][LEFT], targetL.theta, targetL.theta ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tss));

	    KWalkMath.LinearInterpolation(Buffer[X][RIGHT], fillcounter[X][RIGHT], startR.x, targetR.x ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tss));
	    KWalkMath.LinearInterpolation(Buffer[Y][RIGHT], fillcounter[Y][RIGHT], startR.y, targetR.y ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tss));
	    KWalkMath.CubicBezierInterpolation(Buffer[Z][RIGHT], fillcounter[Z][RIGHT], 0.000, 0.000 ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tss));
	    KWalkMath.LinearInterpolation(Buffer[Theta][RIGHT], fillcounter[Theta][RIGHT], startR.theta, targetR.theta ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tss));

	    KWalkMath.LinearInterpolation(Xl,targetL.x, targetL.x ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tds));
		KWalkMath.LinearInterpolation(Yl,targetL.y, targetL.y ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tds));
		KWalkMath.LinearInterpolation(Zl,0.000, 0.000 ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tds));
		KWalkMath.LinearInterpolation(Thl,targetL.theta, targetL.theta ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tds));

		KWalkMath.LinearInterpolation(Xr,startR.x, startR.x ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tds));
		KWalkMath.LinearInterpolation(Yr,startR.y, startR.y ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tds));
		KWalkMath.LinearInterpolation(Zr,0.000, 0.000 ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tds));
		KWalkMath.LinearInterpolation(Thr,startR.theta, startR.theta ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tds));

		KWalkMath.LinearInterpolation(Xl,targetL.x, targetL.x ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tss));
		KWalkMath.LinearInterpolation(Yl,targetL.y, targetL.y ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tss));
		KWalkMath.LinearInterpolation(Zl,0.000, 0.000 ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tss));
		KWalkMath.LinearInterpolation(Thl,targetL.theta, targetL.theta ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tss));

		KWalkMath.LinearInterpolation(Xr,startR.x, targetR.x ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tss));
		KWalkMath.LinearInterpolation(Yr,startR.y, targetR.y ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tss));
		KWalkMath.CubicBezierInterpolation(Zr,0.000, 0.000 ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tss));
		KWalkMath.LinearInterpolation(Thr,startR.theta, targetR.theta ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tss));
	}
	std::cout << "fillcounter[X][LEFT] " << fillcounter[X][LEFT] << std::endl;
//
//    for(int c=0; c< Xl.size(); c++){
//    	std::cout << Xl.at(c) << " " << Buffer[X][LEFT][c+counterstart] << std::endl;
//
//    }
	return  fillcounter[X][LEFT];
}

//void FootTrajectoryPlanner::FootTrajectoryInterpolate()
//{
//
//	if (RIGHTLEGSUPPORT)
//	{
//		RIGHTLEGSUPPORT=0;
//
//
//		KWalkMath.LinearInterpolation(Xl,startL.x, startL.x ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tds));
//		KWalkMath.LinearInterpolation(Yl,startL.y, startL.y ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tds));
//		KWalkMath.LinearInterpolation(Zl,0.000, 0.000 ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tds));
//		KWalkMath.LinearInterpolation(Thl,startL.theta, startL.theta ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tds));
//
//		KWalkMath.LinearInterpolation(Xr,startR.x, startR.x ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tds));
//		KWalkMath.LinearInterpolation(Yr,startR.y, startR.y ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tds));
//		KWalkMath.LinearInterpolation(Zr,0.000, 0.000 ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tds));
//		KWalkMath.LinearInterpolation(Thr,startR.theta, startR.theta ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tds));
//
//
//		KWalkMath.LinearInterpolation(Xl,startL.x, targetL.x ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tss));
//		KWalkMath.LinearInterpolation(Yl,startL.y, targetL.y ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tss));
//		KWalkMath.CubicBezierInterpolation(Zl,0.000, 0.000 ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tss));
//		KWalkMath.LinearInterpolation(Thl,startL.theta, targetL.theta ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tss));
//
//		KWalkMath.LinearInterpolation(Xr,startR.x, startR.x ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tss));
//		KWalkMath.LinearInterpolation(Yr,startR.y, startR.y ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tss));
//		KWalkMath.LinearInterpolation(Zr,0.000, 0.000 ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tss));
//		KWalkMath.LinearInterpolation(Thr,startR.theta, startR.theta ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tss));
//	}
//	else
//	{
//		RIGHTLEGSUPPORT=1;
//
//		KWalkMath.LinearInterpolation(Xl,targetL.x, targetL.x ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tds));
//		KWalkMath.LinearInterpolation(Yl,targetL.y, targetL.y ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tds));
//		KWalkMath.LinearInterpolation(Zl,0.000, 0.000 ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tds));
//		KWalkMath.LinearInterpolation(Thl,targetL.theta, targetL.theta ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tds));
//
//		KWalkMath.LinearInterpolation(Xr,startR.x, startR.x ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tds));
//		KWalkMath.LinearInterpolation(Yr,startR.y, startR.y ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tds));
//		KWalkMath.LinearInterpolation(Zr,0.000, 0.000 ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tds));
//		KWalkMath.LinearInterpolation(Thr,startR.theta, startR.theta ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tds));
//
//		KWalkMath.LinearInterpolation(Xl,targetL.x, targetL.x ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tss));
//		KWalkMath.LinearInterpolation(Yl,targetL.y, targetL.y ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tss));
//		KWalkMath.LinearInterpolation(Zl,0.000, 0.000 ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tss));
//		KWalkMath.LinearInterpolation(Thl,targetL.theta, targetL.theta ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tss));
//
//		KWalkMath.LinearInterpolation(Xr,startR.x, targetR.x ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tss));
//		KWalkMath.LinearInterpolation(Yr,startR.y, targetR.y ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tss));
//		KWalkMath.CubicBezierInterpolation(Zr,0.000, 0.000 ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tss));
//		KWalkMath.LinearInterpolation(Thr,startR.theta, targetR.theta ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tss));
//
//	}
//}
//
//void FootTrajectoryPlanner::FootTrajectoryClear()
//{
//    Xl.clear();
//    Yl.clear();
//    Zl.clear();
//    Thl.clear();
//    Xr.clear();
//    Yr.clear();
//    Zr.clear();
//    Thr.clear();
//}

//Calculates the trajectory of the initial Step of the feet, returns the number of trajectory points created
int FootTrajectoryPlanner::FootTrajectoryInitStep(float Buffer[3][2][MAX_TRAJECTORY_LENGTH])
{
	std::cout << " Init Step " << std::endl;
	RIGHTLEGSUPPORT=1;
	startL=Planner->FootQ.front();
	targetL=startL;
	Planner->FootQ.pop();

	startR=Planner->FootQ.front();

	targetR=startR;
	Planner->FootQ.pop();

	memset(fillcounter,0,4*2*sizeof(int));

	KWalkMath.LinearInterpolation(Buffer[X][LEFT], fillcounter[X][LEFT], startL.x, startL.x ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tstep));
	KWalkMath.LinearInterpolation(Buffer[Y][LEFT], fillcounter[Y][LEFT], startL.y, startL.y ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tstep));
	KWalkMath.LinearInterpolation(Buffer[Z][LEFT], fillcounter[Z][LEFT], 0.000, 0.000 ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tstep));
	KWalkMath.LinearInterpolation(Buffer[Theta][LEFT],fillcounter[Theta][LEFT],startL.theta, startL.theta ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tstep));


	KWalkMath.LinearInterpolation(Buffer[X][RIGHT], fillcounter[X][RIGHT], startR.x, startR.x ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tstep));
	KWalkMath.LinearInterpolation(Buffer[Y][RIGHT], fillcounter[Y][RIGHT], startR.y, startR.y ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tstep));
	KWalkMath.LinearInterpolation(Buffer[Z][RIGHT], fillcounter[Z][RIGHT], 0.000, 0.000 ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tstep));
	KWalkMath.LinearInterpolation(Buffer[Theta][RIGHT], fillcounter[Theta][RIGHT], startR.theta, startR.theta ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tstep));


    KWalkMath.LinearInterpolationINIT(Xl,startL.x, startL.x ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tstep));
    KWalkMath.LinearInterpolationINIT(Yl,startL.y, startL.y ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tstep));
    KWalkMath.LinearInterpolationINIT(Zl,0.000, 0.000 ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tstep));
    KWalkMath.LinearInterpolationINIT(Thl,startL.theta, startL.theta ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tstep));


    KWalkMath.LinearInterpolationINIT(Xr,startR.x, startR.x ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tstep));
    KWalkMath.LinearInterpolationINIT(Yr,startR.y, startR.y ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tstep));
    KWalkMath.LinearInterpolationINIT(Zr,0.000, 0.000 ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tstep));
    KWalkMath.LinearInterpolationINIT(Thr,startR.theta, startR.theta ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tstep));

    for(int i = 0; i < Xl.size(); i++){

    	std::cout << " Xl[" << i << "]= " << Xl[i] << " Buffer[X][LEFT]" << Buffer[X][LEFT][i] << std::endl;
    }

	std::cout << "fillcounter[0][0] " << fillcounter[0][0] << std::endl;
	return  fillcounter[0][0];
}
void FootTrajectoryPlanner::FootTrajectoryInitStep()
{
    RIGHTLEGSUPPORT=1;
    startL=Planner->FootQ.front();
    targetL=startL;
    Planner->FootQ.pop();

    startR=Planner->FootQ.front();

    targetR=startR;
    Planner->FootQ.pop();

    KWalkMath.LinearInterpolationINIT(Xl,startL.x, startL.x ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tstep));
    KWalkMath.LinearInterpolationINIT(Yl,startL.y, startL.y ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tstep));
    KWalkMath.LinearInterpolationINIT(Zl,0.000, 0.000 ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tstep));
    KWalkMath.LinearInterpolationINIT(Thl,startL.theta, startL.theta ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tstep));


    KWalkMath.LinearInterpolationINIT(Xr,startR.x, startR.x ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tstep));
    KWalkMath.LinearInterpolationINIT(Yr,startR.y, startR.y ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tstep));
    KWalkMath.LinearInterpolationINIT(Zr,0.000, 0.000 ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tstep));
    KWalkMath.LinearInterpolationINIT(Thr,startR.theta, startR.theta ,Robot.getWalkParameter(Ts),Robot.getWalkParameter(Tstep));
}

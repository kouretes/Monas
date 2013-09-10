//
//  FootTrajectoryPlanner.cpp
//  Kouretes Walk Engine
//
//  Created by Stelios Piperakis on 8/14/13.
//  Copyright (c) 2013 SP. All rights reserved.
//
#include "FootTrajectoryPlanner.h"

FootTrajectoryPlanner::FootTrajectoryPlanner(){
    
    ;
    
}
void FootTrajectoryPlanner::FootTrajectoryInitialize(Stepplanner* xPlanner,RobotParameters xRobot,KWalkMat xMath)
{
    Planner=xPlanner;
    Robot=xRobot;
    KWalkMath=xMath;
    RIGHTLEGSUPPORT=1;
    startL=Planner->FootQ.front();
    targetL=startL;
    Planner->FootQ.pop();

    startR=Planner->FootQ.front();

    targetR=startR;
    Planner->FootQ.pop();

    KWalkMath.LinearInterpolationINIT(Xl,startL.x, startL.x ,Robot.getRobotParameter("Ts"),Robot.getRobotParameter("Tstep"));
    KWalkMath.LinearInterpolationINIT(Yl,startL.y, startL.y ,Robot.getRobotParameter("Ts"),Robot.getRobotParameter("Tstep"));
    KWalkMath.LinearInterpolationINIT(Zl,0.000, 0.000 ,Robot.getRobotParameter("Ts"),Robot.getRobotParameter("Tstep"));
    KWalkMath.LinearInterpolationINIT(Thl,startL.theta, startL.theta ,Robot.getRobotParameter("Ts"),Robot.getRobotParameter("Tstep"));
    
    
    KWalkMath.LinearInterpolationINIT(Xr,startR.x, startR.x ,Robot.getRobotParameter("Ts"),Robot.getRobotParameter("Tstep"));
    KWalkMath.LinearInterpolationINIT(Yr,startR.y, startR.y ,Robot.getRobotParameter("Ts"),Robot.getRobotParameter("Tstep"));
    KWalkMath.LinearInterpolationINIT(Zr,0.000, 0.000 ,Robot.getRobotParameter("Ts"),Robot.getRobotParameter("Tstep"));
    KWalkMath.LinearInterpolationINIT(Thr,startR.theta, startR.theta ,Robot.getRobotParameter("Ts"),Robot.getRobotParameter("Tstep"));
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
void FootTrajectoryPlanner::FootTrajectoryInterpolate()
{

if (RIGHTLEGSUPPORT)
{
    RIGHTLEGSUPPORT=0;
    
    
    KWalkMath.LinearInterpolation(Xl,startL.x, startL.x ,Robot.getRobotParameter("Ts"),Robot.getRobotParameter("Tds"));
    KWalkMath.LinearInterpolation(Yl,startL.y, startL.y ,Robot.getRobotParameter("Ts"),Robot.getRobotParameter("Tds"));
    KWalkMath.LinearInterpolation(Zl,0.000, 0.000 ,Robot.getRobotParameter("Ts"),Robot.getRobotParameter("Tds"));
    KWalkMath.LinearInterpolation(Thl,startL.theta, startL.theta ,Robot.getRobotParameter("Ts"),Robot.getRobotParameter("Tds"));

    KWalkMath.LinearInterpolation(Xr,startR.x, startR.x ,Robot.getRobotParameter("Ts"),Robot.getRobotParameter("Tds"));
    KWalkMath.LinearInterpolation(Yr,startR.y, startR.y ,Robot.getRobotParameter("Ts"),Robot.getRobotParameter("Tds"));
    KWalkMath.LinearInterpolation(Zr,0.000, 0.000 ,Robot.getRobotParameter("Ts"),Robot.getRobotParameter("Tds"));
    KWalkMath.LinearInterpolation(Thr,startR.theta, startR.theta ,Robot.getRobotParameter("Ts"),Robot.getRobotParameter("Tds"));

    
    KWalkMath.LinearInterpolation(Xl,startL.x, targetL.x ,Robot.getRobotParameter("Ts"),Robot.getRobotParameter("Tss"));
    KWalkMath.LinearInterpolation(Yl,startL.y, targetL.y ,Robot.getRobotParameter("Ts"),Robot.getRobotParameter("Tss"));
    KWalkMath.CubicBezierInterpolation(Zl,0.000, 0.000 ,Robot.getRobotParameter("Ts"),Robot.getRobotParameter("Tss"));
    KWalkMath.LinearInterpolation(Thl,startL.theta, targetL.theta ,Robot.getRobotParameter("Ts"),Robot.getRobotParameter("Tss"));

    KWalkMath.LinearInterpolation(Xr,startR.x, startR.x ,Robot.getRobotParameter("Ts"),Robot.getRobotParameter("Tss"));
    KWalkMath.LinearInterpolation(Yr,startR.y, startR.y ,Robot.getRobotParameter("Ts"),Robot.getRobotParameter("Tss"));
    KWalkMath.LinearInterpolation(Zr,0.000, 0.000 ,Robot.getRobotParameter("Ts"),Robot.getRobotParameter("Tss"));
    KWalkMath.LinearInterpolation(Thr,startR.theta, startR.theta ,Robot.getRobotParameter("Ts"),Robot.getRobotParameter("Tss"));
}
else
{
    RIGHTLEGSUPPORT=1;
    
    KWalkMath.LinearInterpolation(Xl,targetL.x, targetL.x ,Robot.getRobotParameter("Ts"),Robot.getRobotParameter("Tds"));
    KWalkMath.LinearInterpolation(Yl,targetL.y, targetL.y ,Robot.getRobotParameter("Ts"),Robot.getRobotParameter("Tds"));
    KWalkMath.LinearInterpolation(Zl,0.000, 0.000 ,Robot.getRobotParameter("Ts"),Robot.getRobotParameter("Tds"));
    KWalkMath.LinearInterpolation(Thl,targetL.theta, targetL.theta ,Robot.getRobotParameter("Ts"),Robot.getRobotParameter("Tds"));
    
    KWalkMath.LinearInterpolation(Xr,startR.x, startR.x ,Robot.getRobotParameter("Ts"),Robot.getRobotParameter("Tds"));
    KWalkMath.LinearInterpolation(Yr,startR.y, startR.y ,Robot.getRobotParameter("Ts"),Robot.getRobotParameter("Tds"));
    KWalkMath.LinearInterpolation(Zr,0.000, 0.000 ,Robot.getRobotParameter("Ts"),Robot.getRobotParameter("Tds"));
    KWalkMath.LinearInterpolation(Thr,startR.theta, startR.theta ,Robot.getRobotParameter("Ts"),Robot.getRobotParameter("Tds"));
    
    KWalkMath.LinearInterpolation(Xl,targetL.x, targetL.x ,Robot.getRobotParameter("Ts"),Robot.getRobotParameter("Tss"));
    KWalkMath.LinearInterpolation(Yl,targetL.y, targetL.y ,Robot.getRobotParameter("Ts"),Robot.getRobotParameter("Tss"));
    KWalkMath.LinearInterpolation(Zl,0.000, 0.000 ,Robot.getRobotParameter("Ts"),Robot.getRobotParameter("Tss"));
    KWalkMath.LinearInterpolation(Thl,targetL.theta, targetL.theta ,Robot.getRobotParameter("Ts"),Robot.getRobotParameter("Tss"));

    KWalkMath.LinearInterpolation(Xr,startR.x, targetR.x ,Robot.getRobotParameter("Ts"),Robot.getRobotParameter("Tss"));
    KWalkMath.LinearInterpolation(Yr,startR.y, targetR.y ,Robot.getRobotParameter("Ts"),Robot.getRobotParameter("Tss"));
    KWalkMath.CubicBezierInterpolation(Zr,0.000, 0.000 ,Robot.getRobotParameter("Ts"),Robot.getRobotParameter("Tss"));
    KWalkMath.LinearInterpolation(Thr,startR.theta, targetR.theta ,Robot.getRobotParameter("Ts"),Robot.getRobotParameter("Tss"));

}
   

}
void FootTrajectoryPlanner::FootTrajectoryUpdate(Stepplanner *xPlanner)
{
    
    Planner=xPlanner;
}
void FootTrajectoryPlanner::FootTrajectoryClear()
{
    Xl.clear();
    Yl.clear();
    Zl.clear();
    Thl.clear();
    Xr.clear();
    Yr.clear();
    Zr.clear();
    Thr.clear();
    
}

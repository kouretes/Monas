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
    ;
       
}
void ZMPTrajectoryPlanner::ZMPTrajectoryInitialize(Stepplanner* xPlanner,RobotParameters xRobot,KWalkMat xMath)
{
    Planner=xPlanner;
    Robot=xRobot;
    KWalkMath=xMath;

    start=Planner->ZmpQ.front();
    target=start;
    Planner->ZmpQ.pop();
    
    
    KWalkMath.LinearInterpolationINIT(ZMPX,start.x, target.x ,Robot.getRobotParameter("Ts"),Robot.getRobotParameter("Tstep"));
    KWalkMath.LinearInterpolationINIT(ZMPY,start.y, target.y ,Robot.getRobotParameter("Ts"),Robot.getRobotParameter("Tstep"));
       
}

void ZMPTrajectoryPlanner::NextTrajectory()
{
    
        start=target;
        target=Planner->ZmpQ.front();
        Planner->ZmpQ.pop();
    
    
}
void ZMPTrajectoryPlanner::ZMPTrajectoryInterpolate()
{
    
   
        //Interpolating the DS Phase
        KWalkMath.LinearInterpolation(ZMPX,start.x, target.x ,Robot.getRobotParameter("Ts"),Robot.getRobotParameter("Tds"));
        KWalkMath.LinearInterpolation(ZMPY,start.y, target.y ,Robot.getRobotParameter("Ts"),Robot.getRobotParameter("Tds"));
        //Interpolating the SS Phase
        KWalkMath.LinearInterpolation(ZMPX,target.x, target.x ,Robot.getRobotParameter("Ts"),Robot.getRobotParameter("Tss"));
        KWalkMath.LinearInterpolation(ZMPY,target.y, target.y ,Robot.getRobotParameter("Ts"),Robot.getRobotParameter("Tss"));
       
    
        
}
void ZMPTrajectoryPlanner::ZMPTrajectoryUpdate(Stepplanner* xPlanner)
{
    
        Planner=xPlanner;
    
    
}
void ZMPTrajectoryPlanner::ZMPTrajectoryClear()
{
    ZMPX.clear();
    ZMPY.clear();
}




//
//  ZMPTrajectoryPlanner.h
//  Kouretes Walk Engine
//
//  Created by Stelios Piperakis on 8/14/13.
//  Copyright (c) 2013 SP. All rights reserved.
//

#ifndef Kouretes_Walk_Engine_ZMPTrajectoryPlanner_h
#define Kouretes_Walk_Engine_ZMPTrajectoryPlanner_h
#include "Stepplanner.h"
#include "KWalkMat.h"
class ZMPTrajectoryPlanner
{
    private :
            GroundPoint start,target;
            KWalkMat KWalkMath;
            Stepplanner* Planner;
            RobotParameters Robot;
            int fillcounter[2];
    public :
            //std::vector <float> ZMPX,ZMPY;
            ZMPTrajectoryPlanner();
            void ZMPTrajectoryInitialize(Stepplanner* ,RobotParameters , KWalkMat );
            int ZMPTrajectoryInterpolate(float Buffer[3][MAX_TRAJECTORY_LENGTH], int append);

            int ZMPTrajectoryInitStep(float Buffer[3][MAX_TRAJECTORY_LENGTH]);
          //  void ZMPTrajectoryInterpolate();
            void NextTrajectory();
          //  void ZMPTrajectoryInitStep();
};


#endif

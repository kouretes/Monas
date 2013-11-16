
set ( ActivityName LowLevelPlanner )

set ( ActivitySrcs  LowLevelPlanner.cpp LowLevelPlanner.h FootTrajectoryPlanner.cpp FootTrajectoryPlanner.h RobotParameters.cpp RobotParameters.h ZMPTrajectoryPlanner.cpp ZMPTrajectoryPlanner.h Stepplanner.cpp Stepplanner.h ControlThread.cpp ControlThread.h KWalkMat.h)

include ( ../activTemplate.cmake )

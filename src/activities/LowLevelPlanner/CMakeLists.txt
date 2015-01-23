
set ( ActivityName LowLevelPlanner )
set(CMAKE_BUILD_TYPE Debug)
set ( ActivitySrcs	LowLevelPlanner.cpp 
			LowLevelPlanner.h
			RobotParameters.cpp 
			RobotParameters.h 
			Stepplanner.cpp 
			Stepplanner.h 
			ControlThread.cpp 
			ControlThread.h 
			KWalkMat.h
			CircularBuffer.hpp
			WalkEngine.hpp
			WalkEngine.cpp
			Kalman.h
			Kalman.cpp
			Dynamics.h
			Dynamics.cpp
			matlog.cpp
			cem.cpp
			cem.h
			)

include ( ../activTemplate.cmake )

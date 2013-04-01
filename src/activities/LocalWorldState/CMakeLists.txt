include_directories( ../../../external/PracticalSocket)

set ( ActivityName LocalWorldState )

set ( ActivitySrcs  LocalWorldState.h LocalWorldState.cpp BallFilter.h BallFilter.cpp
KLocalization.h KLocalization.cpp ../../../external/PracticalSocket/PracticalSocket.cpp EKFLocalization.h EKFLocalization.cpp KalmanModel.h KalmanModel.cpp LocalizationStructs.h
)

include ( ../activTemplate.cmake )


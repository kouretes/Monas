set ( ActivityName LocalWorldState )

set ( ActivitySrcs  LocalWorldState.h LocalWorldState.cpp BallFilter.h BallFilter.cpp
KLocalization.h KLocalization.cpp  EKFLocalization.h EKFLocalization.cpp KalmanModel.h KalmanModel.cpp LocalizationStructs.h
)

include ( ../activTemplate.cmake )


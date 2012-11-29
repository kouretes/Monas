set ( ActivityName KalmanWorldState )

set ( ActivitySrcs  KalmanWorldState.h KalmanWorldState.cpp BallFilter.h BallFilter.cpp
KKalmanLocalization.h KKalmanLocalization.cpp KalmanParticle.h
)

include ( ../activTemplate.cmake )

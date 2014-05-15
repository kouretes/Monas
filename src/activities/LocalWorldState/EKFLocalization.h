#ifndef EKFLOCALIZATION_H
#define EKFLOCALIZATION_H

#include <string>
#include <vector>
#include "core/elements/math/Common.hpp"
#include "core/elements/math/KMat.hpp"
#include "core/architecture/time/TimeTypes.hpp"
#include "KLocalization.h"
#include <math.h>
#include "KalmanModel.h"
#include <algorithm>
#include "LocalizationStructs.h"
using namespace std;
using namespace KMath;
class EKFLocalization{

public :

    float e1,e2,e3;

    KMath::KMat::GenMatrix<float,6,6> var;
    vector<KalmanModel> kalmanModels;

    void Initialize();
    Localization::blf LocalizationStep(Localization::KMotionModel & MotionModel, vector<Localization::KObservationModel>& Observations, vector<Localization::KObservationModel>& AmbiguousObservations);

    void InitializeHypothesis(int resetType, bool kickOff, float inX, float inY, float inPhi);
    void IncreaseUncertaintyAfterFall();
    void Reset();
    void hypothesisMerging(float thres);

    Localization::blf agentPosition;
    float actionOdError;
    int numberOfModels;

    // structs to store xml data
    Localization::LocConfig* locConfig;

    KSystem::Time::TimeAbsolute lastPrint;
	KSystem::Time::TimeAbsolute now;
};


#endif /* EKFLocalization_H_*/

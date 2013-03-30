#ifndef KALMANMODEL_H_
#define KALMANMODEL_H_

#include "core/elements/math/Common.hpp"
#include "core/elements/math/KMat.hpp"
#include "KLocalization.h"
#include <boost/circular_buffer.hpp>

using namespace KMath;

class KalmanModel{

private:
    
    typedef KMath::KMat::GenMatrix<float,6,1> matrix6_1;
    typedef KMath::KMat::GenMatrix<float,6,6> matrix6_6;
    typedef KMath::KMat::GenMatrix<float,2,2> matrix2_2;
    typedef KMath::KMat::GenMatrix<float,2,6> matrix2_6;
    typedef KMath::KMat::GenMatrix<float,6,2> matrix6_2;
    typedef KMath::KMat::GenMatrix<float,2,1> matrix2_1;
    typedef KMath::KMat::GenMatrix<float,6,3> matrix6_3;
    typedef KMath::KMat::GenMatrix<float,3,3> matrix3_3;

public :

    matrix6_6 var;
    matrix6_1 state ;
    float mWeight;
    bool active;
    boost::circular_buffer<int> associationHistory;

    KalmanModel();
    void Initialize(float x, float y, float phi, float e1, float e2, float e3, float w , bool a);
    void Predict(Localization::KMotionModel & MotionModel);
    void Update(float featureX,float featureY,float distanceVal,float bearingVal,float distanceDev,float bearingDev);

    bool operator < (const KalmanModel& m2) const
    {
        return (mWeight > m2.mWeight);
    }
};

#endif 

/**
 * BallFilter.h
 *
 *  Created on: Mar 30, 2011
 *      Author: trs
 */

#ifndef BALLFILTER_H
#define BALLFILTER_H
#include "messages/WorldInfo.pb.h"
#include "core/elements/math/KMat.hpp"
#include "KLocalization.h"
#include "LocalizationStructs.h"
/*
 *
 */
class BallFilter
{
	private:

        typedef KMath::KMat::GenMatrix<float,4,1> matrix4_1;
        typedef KMath::KMat::GenMatrix<float,4,2> matrix4_2;
        typedef KMath::KMat::GenMatrix<float,4,4> matrix4_4;
        typedef KMath::KMat::GenMatrix<float,2,1> matrix2_1;
        typedef KMath::KMat::GenMatrix<float,2,2> matrix2_2;
        typedef KMath::KMat::GenMatrix<float,2,4> matrix2_4;

	public:

        // x,y,x',y'
        matrix4_1 state;
        matrix4_4 var;

		BallFilter();
		virtual ~BallFilter();

		void update(float new_dist, float dist_variance, float new_dir, float dir_variance);
		void predict(float dt,Localization::KMotionModel const & MM);
		void reset(float dist, float distVariance, float bearing, float bearVariance);
};

#endif /* BALLFILTER_H_ */

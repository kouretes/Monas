/**
 * BallFilter.h
 *
 *  Created on: Mar 30, 2011
 *      Author: trs
 */

#ifndef BALLFILTER_H_
#define BALLFILTER_H_
#include "messages/WorldInfo.pb.h"
#include "tools/stat/kalman.h"
#include "tools/KMat.h"
#include "KLocalization.h"

/*
 *
 */
class BallFilter
{
	private:
		Ball filtered_ball;
//		Kalman1D<float> dist_filter;
//		Kalman1D<float> dir_filter;

		Kalman1D<float> x_filter;
		Kalman1D<float> y_filter;


	public:
		BallFilter();
		virtual ~BallFilter();

		Ball get_updated_ball_estimate(float new_dist, float dist_variance, float new_dir, float dir_variance);
		Ball get_predicted_ball_estimate(float dt, KMotionModel const & MM);
		void reset(float new_dist, float dist_variance, float new_dir, float dif_variance);
		float get_filter_variance() const;

};

#endif /* BALLFILTER_H_ */

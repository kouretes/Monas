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

/*
 *
 */
class BallFilter
{
	private:
		Ball filtered_ball;
		Kalman1D<float> dist_filter;
		Kalman1D<float> dir_filter;

	public:
		BallFilter();
		virtual ~BallFilter();

		Ball get_updated_ball_estimate(float new_dist, float dist_confidence, float new_dir, float dir_confidence, float dt); //dt milliseconds
		Ball get_predicted_ball_estimate(float dt);
		void reset(float new_dist, float dist_confidence, float new_dir, float dif_confidence);

};

#endif /* BALLFILTER_H_ */

/**
 * BallFilter.cpp
 *
 *  Created on: Mar 30, 2011
 *      Author: trs
 */

#include "BallFilter.h"

BallFilter::BallFilter()
{
	// TODO Auto-generated constructor stub
	dist_filter.init(1 / 1000000.0f);
	dist_filter.reset(3, 10);
	dir_filter.init(KMat::transformations::PI / 2);
	dir_filter.reset(0, KMat::transformations::PI);
	filtered_ball.set_relativex(3);
	filtered_ball.set_relativey(0);
}

BallFilter::~BallFilter()
{
	// TODO Auto-generated destructor stub
}

//Update the position
Ball BallFilter::get_updated_ball_estimate(float new_dist, float dist_confidence, float new_dir, float dir_confidence, float dt)
{ //dt milliseconds
	Kalman1D<float>::Xbar dist = dist_filter.update(new_dist, dist_confidence, dt);
	Kalman1D<float>::Xbar dir = dir_filter.update(new_dir, dir_confidence, dt);

	filtered_ball.set_relativex(dist(0) * cos(dir(0)));
	filtered_ball.set_relativey(dist(0) * sin(dir(0)));

	filtered_ball.set_relativexspeed((dist(0) + dist(1)) * cos(dir(0) + dir(1)) - filtered_ball.relativex());
	filtered_ball.set_relativeyspeed((dist(0) + dist(1)) * sin(dir(0) + dir(1)) - filtered_ball.relativey());

	return filtered_ball;
}

//Predict the position without observations
Ball BallFilter::get_predicted_ball_estimate(float dt)
{
	Kalman1D<float>::Xbar dist = dist_filter.predict(dt);
	Kalman1D<float>::Xbar dir = dir_filter.predict(dt);

	filtered_ball.set_relativex(dist(0) * cos(dir(0)));
	filtered_ball.set_relativey(dist(0) * sin(dir(0)));

	filtered_ball.set_relativexspeed((dist(0) + dist(1)) * cos(dir(0) + dir(1)) - filtered_ball.relativex());
	filtered_ball.set_relativeyspeed((dist(0) + dist(1)) * sin(dir(0) + dir(1)) - filtered_ball.relativey());

	return filtered_ball;
}

void BallFilter::reset(float new_dist, float dist_confidence, float new_dir, float dir_confidence)
{
	dist_filter.reset(new_dist, 10);
	dir_filter.reset(new_dir, KMat::transformations::PI);
}

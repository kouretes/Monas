/**
 * BallFilter.cpp
 *
 *  Created on: Mar 30, 2011
 *      Author: trs
 */

#include "BallFilter.h"

BallFilter::BallFilter()
{
//	dist_filter.init(1.0f);
//	dist_filter.reset(3, 10);
//	dir_filter.init(KMat::transformations::PI);
//	dir_filter.reset(0, KMat::transformations::PI);

	x_filter.init(10.0f);
	y_filter.init(10.0f);
	x_filter.reset(3,4);
	y_filter.reset(0,4);


	filtered_ball.set_relativex(3);
	filtered_ball.set_relativey(0);
}

BallFilter::~BallFilter()
{

}

//Update the position
Ball BallFilter::get_updated_ball_estimate(float new_dist, float dist_variance, float new_dir, float dir_variance)
{ 	//dt seconds

	float new_x = new_dist * cos(new_dir);
	float new_y = new_dist * sin(new_dir);

	KMath::Kalman1D<float>::Xbar x_dist = x_filter.update(new_x, dist_variance);
	KMath::Kalman1D<float>::Xbar y_dist = y_filter.update(new_y, dist_variance);



//	Kalman1D<float>::Xbar dist = dist_filter.update(new_dist, dist_variance, dt);
//	Kalman1D<float>::Xbar dir = dir_filter.update(new_dir, dir_variance, dt);

//	filtered_ball.set_relativex(dist(0) * cos(dir(0)));
//	filtered_ball.set_relativey(dist(0) * sin(dir(0)));
//
//	filtered_ball.set_relativexspeed((dist(0) + dist(1)) * cos(dir(0) + dir(1)) - filtered_ball.relativex());
//	filtered_ball.set_relativeyspeed((dist(0) + dist(1)) * sin(dir(0) + dir(1)) - filtered_ball.relativey());

	filtered_ball.set_relativex(x_dist(0));
	filtered_ball.set_relativey(y_dist(0));

	filtered_ball.set_relativexspeed(x_dist(1));
	filtered_ball.set_relativeyspeed(y_dist(1));

	return filtered_ball;
}

//Predict the position without observations and robots Motion Model
Ball BallFilter::get_predicted_ball_estimate(float dt, KKalmanLocalization::KMotionModel const & MM)
{

	double tmpDist, tmpDir, tmpRot, speedX, speedY, newx, newy, newx2, newy2;

	tmpDist = MM.Distance.val * (MM.Distance.ratiomean);
	tmpDist/=1000;
	tmpDir = MM.Direction.val + MM.Direction.Emean;
	tmpRot = - (MM.Rotation.val + MM.Rotation.Emean); //We turn on the other way the robot turns

	KMath::Kalman1D<float>::Xbar x_dist = x_filter.read();
	KMath::Kalman1D<float>::Xbar y_dist = y_filter.read();

	//Translation
	newx = x_dist(0) - cos(tmpDir)*tmpDist;
	newy = y_dist(0) - sin(tmpDir)*tmpDist;

	//Rotate point
	newx2 = newx*cos(tmpRot) - newy*sin(tmpRot);
	newy2 = newx*sin(tmpRot) + newy*cos(tmpRot);

	//Rotate speed vector
	speedX = x_dist(1)*cos(tmpRot) - y_dist(1)*sin(tmpRot);
	speedY = x_dist(1)*sin(tmpRot) + y_dist(1)*cos(tmpRot);

	if(fabs(speedX)<0.02) speedX=0;//2cm per second is the threshold according to RAUL ROJAS, MARK SIMON
    if(fabs(speedY)<0.02) speedY=0;//2cm per second is the threshold according to RAUL ROJAS, MARK SIMON


	x_filter.set(newx2, speedX);
	y_filter.set(newy2, speedY);
    //Again -0.35m/s is the deceleration found by RAUL ROJAS, MARK SIMON
	x_dist = x_filter.predict_with_decel(dt,0.35);
	y_dist = y_filter.predict_with_decel(dt,0.35);


//	Kalman1D<float>::Xbar dist = dist_filter.predict(dt);
//	Kalman1D<float>::Xbar dir = dir_filter.predict(dt);
//
//	filtered_ball.set_relativex(dist(0) * cos(dir(0)));
//	filtered_ball.set_relativey(dist(0) * sin(dir(0)));
//
//	filtered_ball.set_relativexspeed((dist(0) + dist(1)) * cos(dir(0) + dir(1)) - filtered_ball.relativex());
//	filtered_ball.set_relativeyspeed((dist(0) + dist(1)) * sin(dir(0) + dir(1)) - filtered_ball.relativey());

	filtered_ball.set_relativex(x_dist(0));
	filtered_ball.set_relativey(y_dist(0));

	filtered_ball.set_relativexspeed(x_dist(1));
	filtered_ball.set_relativeyspeed(y_dist(1));

	return filtered_ball;
}

//Reseting the ball to new position
void BallFilter::reset(float new_dist, float dist_variance, float new_dir, float dir_variance)
{

	float new_x = new_dist * cos(new_dir);
	float new_y = new_dist * sin(new_dir);
	x_filter.reset(new_x,10);
	y_filter.reset(new_y,10);

//	dist_filter.reset(new_dist, 10); //dist , speed variance in (m/s)^2
//	dir_filter.reset(new_dir, KMat::transformations::PI);
}

float BallFilter::get_filter_variance() const
{
	return max(x_filter.readVariance()(0),y_filter.readVariance()(0));
}

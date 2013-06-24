/**
 * BallFilter.cpp
 *
 *  Created on: Mar 30, 2011
 *      Author: trs
 */

#include "BallFilter.h"

BallFilter::BallFilter()
{
    state.zero();
    var.zero();
}

BallFilter::~BallFilter()
{

}

//Update the position
void BallFilter::update(float distance, float distVariance, float bearing, float bearVariance)
{ 	
    matrix2_1 obs,diff;
    matrix2_2 qt,st,stInv;
    matrix4_2 kt;
    matrix4_4 identityM;
    matrix2_4 ht;

    ht.zero();
    obs.zero();
    qt.zero();
    st.zero();
    diff.zero();

    qt(0,0) = 0.2;
    qt(1,1) = 0.1;

    obs(0) = distance * cos(bearing);
    obs(1) = distance * sin(bearing);

    ht(0,0) = 1;
    ht(0,1) = 0;
    ht(0,2) = 0;
    ht(0,3) = 0;
  
    ht(1,0) = 0;
    ht(1,1) = 1;
    ht(1,2) = 0;
    ht(1,3) = 0;

    st = ((ht.slow_mult(var)).slow_mult(ht.transp())).add(qt);

    stInv = st;
    invert_square_matrix(stInv);

    diff(0,0) = obs(0,0) - state(0,0);
    diff(1,0) = obs(1,0) - state(1,0);
   
    //diff.prettyPrint();
    kt = (var.slow_mult(ht.transp())).slow_mult(stInv);
    //kt.prettyPrint();
    state = state.add(kt.slow_mult(diff));
    var = ((identityM.identity().sub(kt.slow_mult(ht)))).slow_mult(var);

    //state.prettyPrint();
}

//Predict the position without observations and robots Motion Model
void BallFilter::predict(float dt, Localization::KMotionModel const & MM)
{

	double tmpDist, tmpDir, tmpRot;
    double acc = -0.15,vel;

	tmpDist = MM.Distance.val;
	tmpDir = MM.Direction.val;
	tmpRot = MM.Rotation.val;

    matrix4_4 gt;
    matrix4_4 fict;
    gt.zero();
    fict.zero();

    fict(0,0) = 0.1 * dt;
    fict(1,1) = 0.1 * dt;
    fict(2,2) = 0.01 * dt;
    fict(3,3) = 0.01 * dt;

    //Velocity_t-1
    vel = sqrt(pow(state(2,0),2) + pow(state(3,0),2));

    gt(0,0) = cos(tmpRot);
    gt(0,1) = sin(tmpRot);
    gt(0,2) = dt * cos(tmpRot);
    gt(0,3) = dt * sin(tmpRot);

    gt(1,0) = -sin(tmpRot);
    gt(1,1) = cos(tmpRot);
    gt(1,2) = -dt*sin(tmpRot);
    gt(1,3) = dt*cos(tmpRot);


    if (fabs(vel) > fabs(acc * dt)){
        gt(2,2) = (1+acc*dt/vel)*cos(tmpRot) - (acc*dt*state(2,0)*(cos(tmpRot) * state(2,0) + sin(tmpRot)*state(3,0)))/pow(vel,3);
        gt(2,3) = (1+acc*dt/vel)*sin(tmpRot) - (acc*dt*state(3,0)*(cos(tmpRot) * state(2,0) + sin(tmpRot)*state(3,0)))/pow(vel,3);
    }else{
        gt(2,2) = cos(tmpRot);
        gt(2,3) = sin(tmpRot);
    }

    if (fabs(vel)  > fabs(acc * dt)){
        gt(3,2) = -(1+acc*dt/vel)*sin(tmpRot) - (acc*dt*state(2,0)*(-sin(tmpRot) * state(2,0) + cos(tmpRot)*state(3,0)))/pow(vel,3);
        gt(3,3) = (1+acc*dt/vel)*cos(tmpRot) - (acc*dt*state(3,0)*(-sin(tmpRot) * state(2,0) + cos(tmpRot)*state(3,0)))/pow(vel,3);
    }else{
        gt(3,2) = -sin(tmpRot);
        gt(3,3) = cos(tmpRot);;
    }

	//Translation
	state(0,0) = state(0,0) - cos(tmpDir)*tmpDist;
	state(1,0) = state(1,0) - sin(tmpDir)*tmpDist;

	//Rotate position
	state(0,0) = state(0,0)*cos(tmpRot) + state(1,0)*sin(tmpRot);
	state(1,0) = -state(0,0)*sin(tmpRot) + state(1,0)*cos(tmpRot);

	//Rotate speed 
	state(2,0) = state(2,0)*cos(tmpRot) + state(3,0)*sin(tmpRot);
	state(3,0) = -state(2,0)*sin(tmpRot) + state(3,0)*cos(tmpRot);

    state(0,0) =  state(0,0) + state(2,0) * dt;
    state(1,0) =  state(1,0) + state(3,0) * dt;


    if (fabs(state(2,0))  > fabs(acc * state(2,0)/vel *dt)){
        state(2,0) =  state(2,0) + acc * state(2,0)/vel *dt;
    }
    else{
        state(2,0) =  0;
    }

    if (fabs(state(3,0))  > fabs(acc * state(3,0)/vel *dt)){
        state(3,0) =  state(3,0) + acc * state(3,0)/vel *dt;
    }
    else{
        state(3,0) =  0;
    }

    var =((gt.slow_mult(var)).slow_mult(gt.transp())).add(fict);

}

//Reseting the ball to new position
void BallFilter::reset(float distance, float distVariance, float bearing, float bearVariance)
{
    state.zero();
    state(0,0) = distance * cos(bearing);
    state(1,0) = distance * sin(bearing);

    var.zero();
    var(0,0) = 0.5;
    var(1,1) = 0.5;
    var(2,2) = 0.1;
    var(3,3) = 0.1;	
}

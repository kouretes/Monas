//
//  Stepplanner.h
//  Kouretes Walk Engine
//
//  Created by Stelios Piperakis on 8/14/13.
//  Copyright (c) 2013 SP. All rights reserved.
//
#include "Stepplanner.h"

Stepplanner::Stepplanner()
{
    ;

}

void Stepplanner::initialize(RobotParameters OurRobot)
{
	lastvelocity.zero();
	Robot = OurRobot;
	WalkInstruction i;
	i.targetSupport=KDeviceLists::SUPPORT_LEG_LEFT;
	i.targetZMP=KDeviceLists::SUPPORT_LEG_LEFT;
	i.target(0)=Robot.getWalkParameter(HX);
	i.target(1)=-Robot.getWalkParameter(H0);
	i.target(2)=0;
	i.steps=Robot.getWalkParameter(Tss)/Robot.getWalkParameter(Ts);
	ankler=i.target;
	inst.push(i);
	i.targetSupport=KDeviceLists::SUPPORT_LEG_RIGHT;
	i.targetZMP=KDeviceLists::SUPPORT_LEG_RIGHT;
	i.target(0)=Robot.getWalkParameter(HX);
	i.target(1)=Robot.getWalkParameter(H0);
	anklel=i.target;
	inst.push(i);


	i.targetSupport=KDeviceLists::SUPPORT_LEG_RIGHT;
	i.targetZMP=KDeviceLists::SUPPORT_LEG_BOTH;
	inst.push(i);
	Pelvis.zero();//Init



}


void Stepplanner::oneStep(std::vector<float> v)
{    /**     If Walk Engine just Started this procedure determines which leg will move first
     **/
	WalkInstruction i;
    KMath::KMat::transformations::makeRotation(RotPelvisZ,(float)Pelvis(2));
    if( (v[1]>0 ||v[2]>0)&&support==KDeviceLists::SUPPORT_LEG_RIGHT)
	{
		std::vector<float> speed;
		speed.push_back(0);
		speed.push_back(0);
		speed.push_back(0);
		oneStep(speed);
		return oneStep(v);
	}
	else if( (v[1]<0||v[2]<0)&&support==KDeviceLists::SUPPORT_LEG_LEFT)
	{
		std::vector<float> speed;
		speed.push_back(0);
		speed.push_back(0);
		speed.push_back(0);
		oneStep(speed);
		return oneStep(v);
	}


    //Switch Support for next step
    if(support==KDeviceLists::SUPPORT_LEG_LEFT)
		support=KDeviceLists::SUPPORT_LEG_RIGHT;
	else if(support==KDeviceLists::SUPPORT_LEG_RIGHT)
		support=KDeviceLists::SUPPORT_LEG_LEFT;
	else//BOTH! choose where to put the support
	{

		if( v[1]>0||v[2]>0)
		{
			support=KDeviceLists::SUPPORT_LEG_RIGHT;
		}
		else
		{
			support=KDeviceLists::SUPPORT_LEG_LEFT;
		}

	}

	KVecFloat2 h=KVecFloat2(Robot.getWalkParameter(HX),Robot.getWalkParameter(H0));
	if(support==KDeviceLists::SUPPORT_LEG_RIGHT)
		h(1)=h(1);//duh
	else
		h(1)=-h(1);//duh
    KVecFloat3 velocity;


	KVecFloat2 a(v[0],v[1]);
	a=RotPelvisZ*a;
	velocity(0)= a(0) ;//* Robot.getWalkParameter(Tstep)*Robot.getWalkParameter(MaxStepX);
	velocity(1)= a(1) ;// Robot.getWalkParameter(Tstep)*Robot.getWalkParameter(MaxStepY);
    velocity(2) = v[2] ;// Robot.getWalkParameter(Tstep)*Robot.getWalkParameter(MaxStepTheta);

	float tstep=Robot.getWalkParameter(Tstep)/(1.0+sqrt(velocity.norm2())*0.05);
	std::cout<<tstep<<std::endl;
	velocity(0)= a(0) * tstep*Robot.getWalkParameter(MaxStepX);
	velocity(1)= a(1) * tstep*Robot.getWalkParameter(MaxStepY);
    velocity(2) = v[2] * tstep*Robot.getWalkParameter(MaxStepTheta);
    if(velocity.norm2()==0)
	    lastvelocity.zero();

	velocity=velocity*0.1+lastvelocity*0.9;

	lastvelocity=velocity;
    Pelvis+=velocity;
    KMath::KMat::transformations::makeRotation(RotPelvisZ,(float)Pelvis(2));
    h=RotPelvisZ*h;
	i.target=Pelvis;
	i.target(0)+=h(0);

	i.target(1)+=h(1);
	i.targetSupport=support;
	i.targetZMP=support;
	i.target=i.target+velocity;
	//i.target.prettyPrint();
	if(support==KDeviceLists::SUPPORT_LEG_LEFT)
		ankler=i.target;
	else
		anklel=i.target;
	i.steps=ceil(Robot.getWalkParameter(Tss)*tstep/Robot.getWalkParameter(Ts));
	inst.push(i);


}


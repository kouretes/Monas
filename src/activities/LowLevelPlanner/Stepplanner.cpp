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
   Time=0;
   stepcounter=0;
}

void Stepplanner::initialize(RobotParameters OurRobot)
{
	Robot = OurRobot;
    Pelvis.x=0;
	Pelvis.y=0;
    Pelvis.theta=0;
	ZMP.x=0;
	ZMP.y=0;
	ZmpQ.push(ZMP); //Initialize the ZMP Assuming Standing Position with Both Feet
	for (stepcounter = 1; stepcounter < 3; stepcounter++)
	{
		Step.x=0;
		Step.y= pow(-1, (stepcounter + 1)) * Robot.getWalkParameter(H0);
		Step.theta=0;
		FootQ.push(Step);
        
	}
	Time = 0; //initialize time sequence
    stepcounter--;

}

void Stepplanner::oneStep(std::vector<float> v)
{
	float temp0,temp1;


	Time = Time + Robot.getWalkParameter(Tstep);
	//Predicted Pelvis Position
	Pelvis.x = Pelvis.x + v[0] * Robot.getWalkParameter(Tstep)* Robot.getWalkParameter(MaxStepX);
	Pelvis.y = Pelvis.y + v[1] * Robot.getWalkParameter(Tstep)* Robot.getWalkParameter(MaxStepY);
    Pelvis.theta = Pelvis.theta + 0.5* v[2] * Robot.getWalkParameter(Tstep)* Robot.getWalkParameter(MaxStepTheta);
	
    ZMP.x=Step.x;
	ZMP.y=Step.y;
	ZmpQ.push(ZMP);
    
    //Predicted Foot Position
	temp0=Step.x;
	temp1=Step.y;
    stepcounter = stepcounter + 1;
	Step.x = Pelvis.x  + 0.5 * v[0] * Robot.getWalkParameter(Tstep) * Robot.getWalkParameter(MaxStepX);
	Step.y = Pelvis.y  + 0.5 * v[1] * Robot.getWalkParameter(Tstep) * Robot.getWalkParameter(MaxStepY)+pow(-1, stepcounter + 1) * Robot.getWalkParameter(H0);
	Step.theta = Pelvis.theta  + 0.5* v[2] * Robot.getWalkParameter(Tstep) * Robot.getWalkParameter(MaxStepTheta);
    FootQ.push(Step);

	

}
void Stepplanner::afterStep()
{
    //Next Zmp
    ZMP.x=Step.x;
	ZMP.y=Step.y;
	ZmpQ.push(ZMP);
	//Next Foot
    float temp;
    temp=Step.y;
	Time = Time + Robot.getWalkParameter(Tstep);
	stepcounter = stepcounter + 1;
	Step.y =Step.y+2*pow(-1, stepcounter + 1) * Robot.getWalkParameter(H0);
    FootQ.push(Step);
    Pelvis.y=Step.y+pow(-1,stepcounter+1)*Robot.getWalkParameter(H0);
    Pelvis.x=Step.x;
    Pelvis.theta=Step.theta;
    
    
}
void Stepplanner::finalStep()
{

    
    
	float temp;
    temp=Step.y;
	Time = Time + Robot.getWalkParameter(Tstep);
	stepcounter = stepcounter + 1;
	Step.y =Step.y+2*pow(-1, stepcounter + 1) * Robot.getWalkParameter(H0);
    FootQ.push(Step);
    Pelvis.y=Step.y+pow(-1,stepcounter+1)*Robot.getWalkParameter(H0);
    Pelvis.x=Step.x;
    Pelvis.theta=Step.theta;
    
   
    ZMP.x=Step.x;
    ZMP.y=temp+pow(-1,stepcounter+1)* fabs(Step.y-temp)/2;
    ZmpQ.push(ZMP);
}

PlanePoint Stepplanner::showPelvis()
{
	return Pelvis;
}


int Stepplanner::getStepcounter()
{
	return stepcounter;
}

void Stepplanner::clear()
{
    while(!ZmpQ.empty())
    {
        ZmpQ.pop();
    }
    while(!FootQ.empty())
    {
        FootQ.pop();
    }
    
}

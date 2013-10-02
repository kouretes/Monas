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
	Robot = OurRobot;
    Pelvis.x=-0.03;
	Pelvis.y=0;
    Pelvis.theta=0;
    /** Initialize the ZMP Assuming Standing Position with Both Feet **/
	ZMP.x=0;
	ZMP.y=0;
	ZmpQ.push(ZMP);
    /** Initialize the Ankle Positions wrt inertial frame **/
	for (stepcounter = 1; stepcounter < 3; stepcounter++)
	{
		Step.x=0;
		Step.y= pow(-1, (stepcounter + 1)) * Robot.getWalkParameter(H0);
        Step.theta=0;
        if (stepcounter==2)
        {
            PrevAnkle.x=Ankle.x;
            PrevAnkle.y=Ankle.y;
            PrevAnkle.theta=Ankle.theta;
        }
        Ankle.x=-0.03;
        Ankle.y=Step.y;
        Ankle.theta=Step.theta;
        FootQ.push(Ankle);

	}

    /** Initialize time sequence  **/
	Time = 0;
    stepcounter--;
    firstRun=true;
    LEG=1;

}

void Stepplanner::oneStep(std::vector<float> v)
{
    /**
     If Walk Engine just Started this procedure determines which leg will move first
     **/

    KMath::KMat::transformations::makeRotation(RotPelvisZ,Pelvis.theta);
    KMath::KMat::transformations::makeRotation(RotFootZ,PrevAnkle.theta);
	firstRun=false;
    if (firstRun==true)
    {
        firstRun=false;
        if (v[1]!=0)
            {
                if( v[1]>0)
                {
                    LEG=1;
                    RIGHTSUPPORT=true;
                }
                else
                {
                    LEG=-1;
                    RIGHTSUPPORT=false;
                }
            }
        else if (v[2]!=0)
        {
                if(v[2]>0)
                {
                    LEG=1;
                    RIGHTSUPPORT=true;

                }
                else
                {
                    LEG=-1;
                    RIGHTSUPPORT=false;
                }
        }
        else
        {
                LEG=1;
                RIGHTSUPPORT=true;
        }
    }
   // else
    {
		ZMP.x=Step.x;
		ZMP.y=Step.y;
		ZmpQ.push(ZMP);
		std::cout<<"------"<<ZMP.x<<" "<<ZMP.y<<std::endl;
    }
	Time = Time + Robot.getWalkParameter(Tstep);


	/** Predicted Pelvis Position and Foot Position in Local frame **/
	dx= v[0] * Robot.getWalkParameter(Tstep);
	dy= v[1] * Robot.getWalkParameter(Tstep);
    dtheta = v[2] * Robot.getWalkParameter(Tstep);
    PrevAnkle.x=Ankle.x;
    PrevAnkle.y=Ankle.y;
    PrevAnkle.theta=Ankle.theta;
    Ankle.x=1.5*dx*Robot.getWalkParameter(MaxStepX);
    Ankle.y=1.5*dy*Robot.getWalkParameter(MaxStepY)+LEG*Robot.getWalkParameter(H0);
    Ankle.theta=dtheta*Robot.getWalkParameter(MaxStepTheta);
    Step.x=Ankle.x+0.03;
    Step.y=Ankle.y;
    Step.theta=Ankle.theta;
    if(fabs(dy*Robot.getWalkParameter(MaxStepY)-Ankle.y)
			<Robot.getWalkParameter(H0))
    {
        Ankle.y=Robot.getWalkParameter(H0)*LEG;
        Step.y=Ankle.y;
        dy=0;


    }
    /** Forming the Step in the Local Frame  **/
    Foot(0,0)=Step.x+Robot.getWalkParameter(StepX)/2;
    Foot(0,1)=Foot(0,0);
    Foot(0,2)=Step.x-Robot.getWalkParameter(StepX)/2;
    Foot(0,3)=Foot(0,2);

    Foot(1,0)=Step.y+Robot.getWalkParameter(StepY)/2;
    Foot(1,1)=Step.y-Robot.getWalkParameter(StepY)/2;
    Foot(1,2)=Foot(1,0);
    Foot(1,3)=Foot(1,1);

    Foot(0,4)=Step.x;//-Ankle.x;
    Foot(1,4)=Step.y;//-Ankle.y;

    /** Rotate the Step by the ankle position **/

    KMath::KMat::transformations::makeRotation(RotStepZ,Ankle.theta);

    Foot(1,0)-=Ankle.y;
    Foot(1,1)-=Ankle.y;
    Foot(1,2)-=Ankle.y;
    Foot(1,3)-=Ankle.y;
    Foot(1,4)-=Ankle.y;

    Foot(0,0)-=Ankle.x;
    Foot(0,1)-=Ankle.x;
    Foot(0,2)-=Ankle.x;
    Foot(0,3)-=Ankle.x;
    Foot(0,4)-=Ankle.x;

    Foot=RotStepZ*Foot;

    Foot(1,0)+=Ankle.y;
    Foot(1,1)+=Ankle.y;
    Foot(1,2)+=Ankle.y;
    Foot(1,3)+=Ankle.y;
    Step.y=Foot(1,4)+Ankle.y;

    Foot(0,0)+=Ankle.x;
    Foot(0,1)+=Ankle.x;
    Foot(0,2)+=Ankle.x;
    Foot(0,3)+=Ankle.x;
    Step.x=Foot(0,4)+Ankle.x;

    /** Bounding
    for(unsigned index=0;index<4;index++)
    {
        float MinDist=0;
        if (LEG*Foot(1,index)<MinDist)
        {
            float displace=1.5*(Foot(1,index)-MinDist);
            Foot(1,index)-=displace;
            FootY-=displace;
            dy-=displace;

        }
    }
     **/

    /** To inertial frame **/
    tempVec(0)=Step.x;
    tempVec(1)=Step.y;
    tempVec=RotFootZ*tempVec;
    Step.x=Pelvis.x+tempVec(0);
    Step.y=Pelvis.y+tempVec(1);
    Step.theta=Pelvis.theta+Step.theta;

    tempVec(0)=Ankle.x;
    tempVec(1)=Ankle.y;
    tempVec=RotFootZ*tempVec;
    Ankle.x=Pelvis.x+tempVec(0);
    Ankle.y=Pelvis.y+tempVec(1);
    Ankle.theta=Pelvis.theta+Ankle.theta;

    tempVec(0)=dx*Robot.getWalkParameter(MaxStepX);
    tempVec(1)=dy*Robot.getWalkParameter(MaxStepY);

    tempVec=RotPelvisZ*tempVec;

    Pelvis.x+=tempVec(0);
    Pelvis.y+=tempVec(1);
    Pelvis.theta+=0.5*dtheta*Robot.getWalkParameter(MaxStepTheta);


    /** Updating Data **/

    stepcounter++;
    FootQ.push(Ankle);

	/** Change Leg for next Step Procedure **/
    LEG*=-1;
}
void Stepplanner::afterStep()
{
    /** Next Zmp **/
    ZMP.x=Step.x;
	ZMP.y=Step.y;
	ZmpQ.push(ZMP);


	Time = Time + Robot.getWalkParameter(Tstep);
	stepcounter++;

    /** Next Foot Position and orientation wrt Inertial Frame  **/
    tempVec(0)=Ankle.x;
	Step.x+=-2*pow(-1, stepcounter + 1) * Robot.getWalkParameter(H0)*sin(Step.theta);
    Ankle.x+=-2*pow(-1, stepcounter + 1) * Robot.getWalkParameter(H0)*sin(Ankle.theta);
    tempVec(1)=Ankle.y;
    Step.y+=2*pow(-1, stepcounter + 1) * Robot.getWalkParameter(H0)*cos(Step.theta);
    Ankle.y+=2*pow(-1, stepcounter + 1) * Robot.getWalkParameter(H0)*cos(Ankle.theta);

    FootQ.push(Ankle);

    /** Next Pelvis Position and orientation wrt Inertial Frame **/

    Pelvis.y=Ankle.y+(Ankle.y-tempVec(1))/2;
    Pelvis.x=Ankle.x+(Ankle.x-tempVec(0))/2;
    Pelvis.theta=Ankle.theta;

    /** Change Leg for next Step Procedure **/

    LEG*=-1;


}
void Stepplanner::finalStep()
{



	float temp;
    temp=Step.y;
	Time+=Robot.getWalkParameter(Tstep);
	stepcounter++;
	/** Next Foot Position and orientation wrt Inertial Frame  **/

    tempVec(0)=Ankle.x;
	Step.x+=-2*pow(-1, stepcounter + 1) * Robot.getWalkParameter(H0)*sin(Step.theta);
    Ankle.x+=-2*pow(-1, stepcounter + 1) * Robot.getWalkParameter(H0)*sin(Ankle.theta);
    tempVec(1)=Ankle.y;
    Step.y+=2*pow(-1, stepcounter + 1) * Robot.getWalkParameter(H0)*cos(Step.theta);
    Ankle.y+=2*pow(-1, stepcounter + 1) * Robot.getWalkParameter(H0)*cos(Ankle.theta);

    FootQ.push(Ankle);

    /** Next Pelvis Position and orientation wrt Inertial Frame **/

    Pelvis.y=Ankle.y+(Ankle.y-tempVec(1))/2;
    Pelvis.x=Ankle.x+(Ankle.x-tempVec(0))/2;
    Pelvis.theta=Ankle.theta;


    ZMP.x=Step.x;
    ZMP.y=temp+pow(-1,stepcounter+1)* fabs(Step.y-temp)/2;
    ZmpQ.push(ZMP);
}

PlanePoint Stepplanner::getPelvis()
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
bool Stepplanner::IfFirstRightSupportleg()
{
    return RIGHTSUPPORT;
}

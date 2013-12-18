//
//  ControlThread.cpp
//  Kouretes Walk Engine
//
//  Created by Stelios Piperakis on 8/14/13.
//  Copyright (c) 2013 SP. All rights reserved.
//

#include "ControlThread.h"
#include <iostream>
LIPMPreviewController::LIPMPreviewController(RobotParameters robot)
{
    OurRobot=robot;
    //Defining the System Dynamics Augmented with the Observer Structure
    Ad(0,0)=1.0000;
    Ad(0,1)=OurRobot.getWalkParameter(Ts);
    Ad(0,2)=OurRobot.getWalkParameter(Ts)*OurRobot.getWalkParameter(Ts)/2;
    Ad(1,0)=0.0000;
    Ad(1,1)=1.0000;
    Ad(1,2)=Ad(0,1);
    Ad(2,0)=0.0000;
    Ad(2,1)=0.0000;
    Ad(2,2)=1.0000;
/*----------------------*/
    Ad(0,3)=0.0000;
    Ad(1,3)=0.0000;
    Ad(2,3)=0.0000;
    Ad(3,0)=0.0000;
    Ad(3,1)=0.0000;
    Ad(3,2)=0.0000;
    Ad(3,3)=1.0000;


    Bd(0)=OurRobot.getWalkParameter(Ts)*OurRobot.getWalkParameter(Ts)*OurRobot.getWalkParameter(Ts)/6.0000;
    Bd(1)=OurRobot.getWalkParameter(Ts)*OurRobot.getWalkParameter(Ts)/2.0000;
    Bd(2)=OurRobot.getWalkParameter(Ts);
    Bd(3)=0;
    Cd(0)=1.0000;
    Cd(1)=0.0000;
    Cd(2)=-OurRobot.getWalkParameter(ComZ)/OurRobot.getWalkParameter(g);

  //Defining the Optimal Gains for the Preview Controller
    //Integral Feedback Gain
    Gi=858.9552;
    //State Feedback Gain
    Gx(0,0)=1.0e+04 * 3.0043 ;
    Gx(0,1)=1.0e+04 * 0.5404;
    Gx(0,2)=1.0e+04 * 0.0090;
    //Predicted Reference Gain
    Gd(0)=-0.8590;
    Gd(1)=-1.0544;
    Gd(2)=-1.2600;
    Gd(3)=-1.3975;
    Gd(4)=-1.4522;
    Gd(5)=-1.4390;
    Gd(6)=-1.3819;
    Gd(7)=-1.3027;
    Gd(8)=-1.2168;
    Gd(9)=-1.1332;
    Gd(10)=-1.0562;
    Gd(11)=-0.9867;
    Gd(12)=-0.9241;
    Gd(13)=-0.8674;
    Gd(14)=-0.8153;
    Gd(15)=-0.7670;
    Gd(16)=-0.7217;
    Gd(17)=-0.6791;
    Gd(18)=-0.6390;
    Gd(19)=-0.6011;
    Gd(20)=-0.5654;
    Gd(21)=-0.5317;
    Gd(22)=-0.5000;
    Gd(23)=-0.4702;
    Gd(24)=-0.4422;
    Gd(25)=-0.4158;
    Gd(26)=-0.3910;
    Gd(27)=-0.3677;
    Gd(28)=-0.3458;
    Gd(29)=-0.3252;
    Gd(30)=-0.3059;
    Gd(31)=-0.2876;
    Gd(32)=-0.2705;
    Gd(33)=-0.2544;
    Gd(34)=-0.2392;
    Gd(35)=-0.2250;
    Gd(36)=-0.2116;
    Gd(37)=-0.1990;
    Gd(38)=-0.1871;
    Gd(39)=-0.1760;
    Gd(40)=-0.1655;
    Gd(41)=-0.1556;
    Gd(42)=-0.1463;
    Gd(43)=-0.1376;
    Gd(44)=-0.1294;
    Gd(45)=-0.1217;
    Gd(46)=-0.1145;
    Gd(47)=-0.1076;
    Gd(48)=-0.1012;
    Gd(49)=-0.0952;
    Gd.scalar_mult(1.0e+03);
    //Defining the Optimal Observer Gain L
    //L(0)=0.0360;;
    //L(1)=-0.3445;
    //L(2)=-0.5056;
    L(0,0)=0.1128;
    L(0,1)=0.0606;
    L(1,0)=-0.5004;
    L(1,1)=-0.2043;
    L(2,0)=-12.8674;
    L(2,1)=-22.9822;
    L(3,0)=-0.1403;
    L(3,1)=0.0821;

/*
    L(0,0)=0.1644;
    L(0,1)=0.0209;
    L(1,0)=-0.9022;
    L(1,1)=-0.1728;
    L(2,0)=-68.2951;
    L(2,1)=18.3944;
    L(3,0)= -0.1947;
    L(3,1)=0.0334;*/


// 0.1061 0.0531 -0.3630 -0.3406 -42.2434 -10.8218




	State.zero();
    //Initializing Variables
    Integrationfb=0;
    Statefb=0;
    Predictionfb=0;
    /*Akalman(0,0)=1.000;
    Akalman(0,1)=0;//-OurRobot.getWalkParameter(Ts);
    Akalman(1,0)=0.000;
    Akalman(1,1)=1.000;*/
    Bkalman.zero();
    Bkalman(0)=1.000;//OurRobot.getWalkParameter(Ts);
    //Bkalman(1)=0.000;
    StateKalman.zero();
    ProcessNoise.zero();
	ProcessNoise(0,0)=1e-8;
	P.zero();
	P(0,0)=1e-20;
	MeasurementNoise.identity();
	MeasurementNoise(0,0)=0.001;
	MeasurementNoise(1,1)=0.01;
	Ckalman.zero();
	Ckalman(0,0)=1.000;
	Ckalman(1,0)=1;

	//MeasurementNoise.scalar_mult(0.1);
	uBuffer.push(0);




}



void LIPMPreviewController::LIPMComPredictor(CircularBuffer<float> & ZmpBuffer,float CoMMeasured,float ZMPMeasured)
{
    //Update
		/*Kalman filter for the ZMP in the corresponding axis taking into account the bias of the FSRs */
        //Predict

		StateKalman=StateKalman+Bkalman*uBuffer.front();


		float ct1=0,ct2=0;
		float zmpfromcom=0;
		bool doup=false;
		if(fabs(ZMPMeasured)>1e-15)
		{
			P=(P)+ProcessNoise;
			doup=true;

			if(combuffer.size()==2)
			{


				ct2=combuffer.front();
				combuffer.pop();
				ct1=combuffer.front();
				combuffer.pop();
				combuffer.push(ct1);

				//std::cout<<"CoMMeasured:"<<CoMMeasured<<std::endl;
				//std::cout<<"ct1:"<<ct1<<std::endl;
				//std::cout<<"ct2:"<<ct2<<std::endl;
				float comddot=(CoMMeasured-2*ct1+ct2)/(OurRobot.getWalkParameter(Ts)*OurRobot.getWalkParameter(Ts) );
				//std::cout<<"comddot:"<<comddot<<std::endl;
				zmpfromcom=CoMMeasured-(OurRobot.getWalkParameter(ComZ)/OurRobot.getWalkParameter(g))*comddot;
				//std::cout<<"zmpfromcom:"<<zmpfromcom<<std::endl;
			}

		}




		combuffer.push(CoMMeasured);

		if(doup)
		{
			ykalman=KVecFloat2(ZMPMeasured,zmpfromcom);
			ykalman.prettyPrint();
			ykalman+=(Ckalman*(StateKalman)).scalar_mult(-1.0);//innovation value
			s=Ckalman*P*Ckalman.transp()+MeasurementNoise;
			s.fast_invert();
			Kgain=(P*Ckalman.transp())*s;
			StateKalman+=Kgain*ykalman;
		}

        //StateKalman.prettyPrint();

		Kgain.scalar_mult(-1);
		P+=Kgain*Ckalman*P;
		if(combuffer.size()>2)
			combuffer.pop();


		if(uBuffer.size()>5)
			uBuffer.pop();



	StatePredict=StateKalman;
	unsigned bufsize=uBuffer.size();
	for(unsigned i=0;i<bufsize;i++)
	{
		StatePredict=StatePredict+Bkalman*uBuffer.front();
		uBuffer.push(uBuffer.front());
		uBuffer.pop();
	}
	    //Setting the Reference Signal
	unsigned int l = 0;

	for (unsigned int i = 0; i < PreviewWindow; i++, l++)
	{
		if (i < ZmpBuffer.size())
			ZMPReference(l) = ZmpBuffer[i];
		else
			ZMPReference(l) = ZmpBuffer[ZmpBuffer.size() - 1];
	}



    //State Feedback Computation
    Statefb=0;
    Statefb=Gx(0,0)*State(0)+Gx(0,1)*State(1)+Gx(0,2)*State(2);

    //Updating the Integral Feedback
    Integrationfb+=Gi*((Cd(0)*State(0)+Cd(2)*State(2))-ZMPReference(0));
    //Predicted Feedback Computation
   //Predicted Feedback Computation
    Predictionfb=0;
    unsigned  int z=0;
    for (unsigned  int i=0;i<PreviewWindow-1;i++)
    {
        Predictionfb+=Gd(z)*ZMPReference(i+1);
        z++;
    }
    //Optimal Preview Control
    u=-Statefb-Integrationfb-Predictionfb;


    //Updating the Dynamics

    KVecFloat2 error=KVecFloat2(CoMMeasured,StatePredict(0));
	float zmpstate=(Cd(0)*State(0)+Cd(2)*State(2));
	error-=KVecFloat2(State(0),(Cd(0)*State(0)+Cd(2)*State(2)+State(3)));//StatePredict(0));
	error(0)*=0.5;
	//error.scalar_mult(1.0);

    State=Ad*State;

    temp=Bd;
    temp.scalar_mult(u);
    State+=temp;

    State+=L*error;
    State.prettyPrint();

    uBuffer.push((Cd(0)*State(0)+Cd(2)*State(2))-zmpstate);//(ZMPReference(1)-ZMPReference(0)));


    //Estimated COM position
    Com=State(0);//+0.6*(State(1)+1/2*State(2)*OurRobot.getWalkParameter(Ts))*OurRobot.getWalkParameter(Ts);
}

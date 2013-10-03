//
//  ControlThread.cpp
//  Kouretes Walk Engine
//
//  Created by Stelios Piperakis on 8/14/13.
//  Copyright (c) 2013 SP. All rights reserved.
//

#include "ControlThread.h"
#include <iostream>
#include  <math.h>

LIPMPreviewController::LIPMPreviewController(RobotParameters robot)
{

OurRobot=robot;
    //Defining the System Dynamics Augmented with the Observer Structure
    Ad(0,0)=1.0000;
    Ad(0,1)=OurRobot.getWalkParameter(Ts);
    Ad(0,2)=0;
    Ad(1,0)=(OurRobot.getWalkParameter(g)/OurRobot.getWalkParameter(ComZ))*OurRobot.getWalkParameter(Ts);
    Ad(1,1)=1.0000;
    Ad(1,2)=-Ad(1,0);
    Ad(2,0)=0.0000;
    Ad(2,1)=0.0000;
    Ad(2,2)=1.0000;

    Aobs(0,0)=0.8839;
    Aobs(0,1)=OurRobot.getWalkParameter(Ts);
    Aobs(0,2)=-0.0301;
    Aobs(1,0)= 0.5174;
    Aobs(1,1)=1.0000;
    Aobs(1,2)=-0.0513;
    Aobs(2,0)= -2.3890 ;
    Aobs(2,1)=0;
    Aobs(2,2)=1.5792;

   /* Aobs(0,0)=1;
    Aobs(0,1)=OurRobot.getWalkParameter(Ts);
    Aobs(0,2)=-0.0359;
    Aobs(1,0)= 0.3773;
    Aobs(1,1)=1.0000;
    Aobs(1,2)=-0.0335;
    Aobs(2,0)= 0 ;
    Aobs(2,1)=0;
    Aobs(2,2)=1.5050;*/
    Bd(0)=0;
    Bd(1)=0;
    Bd(2)=OurRobot.getWalkParameter(Ts);
    Cd(0)=0;
    Cd(1)=0;
    Cd(2)=1;


    //Defining the Optimal Gains for the Preview Controller for Preview Horizon 50 and ComZ=0.268
    //Integral Feedback Gain
    Gi=-56.7626;
    //State Feedback Gain
    Gx(0,0)=1.0e3*(-2.2852);
    Gx(0,1)=1.0e3*(-0.3821);
    Gx(0,2)=1.0e3*(0.2320);
    //Predicted Reference Gain
    Gd(0)= 56.7626;
    Gd(1)= 88.9825;
    Gd(2)=  96.9711;
    Gd(3)=  96.4123;
    Gd(4)=  92.9489;
    Gd(5)=  88.6059;
    Gd(6)=  84.1047;
    Gd(7)=  79.6976;
    Gd(8)=  75.4681;
    Gd(9)=  71.4391;
    Gd(10)=  67.6120 ;
    Gd(11)= 63.9811 ;
    Gd(12)= 60.5380 ;
    Gd(13)= 57.2738 ;
    Gd(14)= 54.1799 ;
    Gd(15)= 51.2477 ;
    Gd(16)= 48.4692 ;
    Gd(17)= 45.8366 ;
    Gd(18)= 43.3424 ;
    Gd(19)= 40.9798 ;
    Gd(20)= 38.7420 ;
    Gd(21)= 36.6227 ;
    Gd(22)= 34.6158 ;
    Gd(23)= 32.7155;
    Gd(24)=  30.9165 ;
    Gd(25)= 29.2134 ;
    Gd(26)= 27.6014;
    Gd(27)=  26.0757;
    Gd(28)=  24.6318;
    Gd(29)=  23.2656;
    Gd(30)=  21.9729;
    Gd(31)=  20.7501;
    Gd(32)=  19.5933;
    Gd(33)=  18.4991;
    Gd(34)=  17.4644;
    Gd(35)=  16.4858 ;
    Gd(36)= 15.5606 ;
    Gd(37)= 14.6858 ;
    Gd(38)= 13.8588;
    Gd(39)=  13.0771;
    Gd(40)=  12.3382;
    Gd(41)=  11.6399;
    Gd(42)=  10.9801;
    Gd(43)=  10.3566;
    Gd(44)=  9.7676 ;
    Gd(45)= 9.2111;
    Gd(46)=  8.6855;
    Gd(47)=  8.1890;
    Gd(48)=  7.7201;
    Gd(49)=  7.2774;

    //Defining the Optimal Observer Gain L KALMAN BUCY
    /*L(0)=0.0359;
    L(1)=-0.3438;
    L(2)=-0.5050;*/
    L(0,0)=0.1155;
    L(0,1)=-0.0052;
    L(1,0)=-0.7272;
    L(1,1)=-0.0730 ;
    L(2,0)=-0.0052;
    L(2,1)=0.0083;

    State(0)=0.000;
    State(1)=0.000;
    State(2)=0.000;
    //Initializing Variables
    Integrationfb=0.000;
    Statefb=0.000;
    Predictionfb=0.000;
    //bias=0;
    //P=1;
    //StateKalman=0;
    Akalman(0,0)=1.000;
    Akalman(0,1)=0;//-OurRobot.getWalkParameter(Ts);
    Akalman(1,0)=0.000;
    Akalman(1,1)=1.000;
    Bkalman(0)=1.000;//OurRobot.getWalkParameter(Ts);
    Bkalman(1)=0.000;
	StateKalman(0)=0.000;
	StateKalman(1)=0.000;
	ProcessNoise.zero();
	ProcessNoise(0,0)=1e-4;
	ProcessNoise(1,1)=1e-7;
	P.zero();
	P(0,0)=0.0001;
	P(1,1)=0.0000005;
	MeasurementNoise=0.001;
	uBuffer.push(0);
	Ckalman(0,0)=1.000;
	Ckalman(0,1)=1.000;
}

void LIPMPreviewController::LIPMComPredictor(CircularBuffer<float> & ZmpBuffer,float CoMmeasured,float ZMPMeasured/*, std::ve,tor<float> ZMP,bool print*/)
{

	//Update
	//if(fabs(ZMPMeasured)>1e-10)

		/*Kalman filter for the ZMP in the corresponding axis taking into account the bias of the FSRs */
	//Predict
		StateKalman=Akalman*StateKalman+Bkalman*uBuffer.front();
		P=(Akalman*P*Akalman.transp())+ProcessNoise;
		if(fabs(ZMPMeasured)>1e-15)
		{

			float y=ZMPMeasured-Ckalman*StateKalman; //innovation value
			s=Ckalman*P*Ckalman.transp()+MeasurementNoise;
			Kgain=(Akalman*P*Ckalman.transp());
			Kgain.scalar_mult(1/s);
			StateKalman+=Kgain*y;
			tempg=Kgain;
			tempg.scalar_mult(-1);
			P+=tempg*Ckalman*P;
		}
	   /*
		float y=ZMPMeasured-StateKalman; //innovation value


		float s=P+0.01;
       //fuck bias for a moment
		float prob=pow(2.71,(-(y-bias)*(y-bias))/(2*s));///sqrt(2*3.1415*s);
		std::cout<<"prob:"<<prob<<std::endl;

		float k=P/s;

		if(prob>0.6)
		{
			StateKalman+=k*(y);//-bias);
			bias=bias*0.99+0.01*y;
			P-=k*P;
		}

	}
	//Predict
	StateKalman=Akalman*StateKalman+Bkalman*uBuffer.front();

	P+=ProcessNoise;
	*/
	if(uBuffer.size()>3)
		uBuffer.pop();



	StatePredict=StateKalman;
	unsigned bufsize=uBuffer.size();
	for(unsigned i=0;i<bufsize;i++)
	{
		StatePredict=Akalman*StatePredict+Bkalman*uBuffer.front();
		uBuffer.push(uBuffer.front());
		uBuffer.pop();
	}
	//StatePredict(0)=0;
	//std::cout<<"StatePredict:"<<std::endl;
	//StatePredict.prettyPrint();

	//Setting the Reference Signal
	unsigned int l = 0;

	for (unsigned int i = 0; i < PreviewWindow; i++, l++)
	{
		if (i < ZmpBuffer.size())
			ZMPReference(l) = ZmpBuffer[i];
		else
			ZMPReference(l) = ZmpBuffer[ZmpBuffer.size() - 1];
	}

	//CoMmeasured =State(0)+1.0e-2;
	//CoMmeasured=State(0);
	//State(2)=0.5*State(2)+0.5*ZMPMeasured;

    //State Feedback Computation
    Statefb=0;
    Statefb=Gx*State;

    //Updating the Integral Feedback
    Integrationfb+=Gi*(State(2)-ZMPReference(0));
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

	uBuffer.push((ZMPReference(1)-ZMPReference(0)));
    //Updating the Dynamics
	KVecFloat2 error=KVecFloat2(CoMmeasured,StatePredict(0));
	error-=KVecFloat2(State(0),State(2));
	error.scalar_mult(0.4);
    State=Ad*State;

    temp=Bd;
    temp.scalar_mult(u);

    State+=temp;
    State+=Ad*L*error;
    //std::cout<<"State:"<<std::endl;
    //State.prettyPrint();
    //temp=L;
    //temp.scalar_mult(ZMPMeasured);


   	//Estimated COM position
	Com = State(0)+State(1)*uBuffer.size()*OurRobot.getWalkParameter(Ts)/2;
	//d=State(1)*OurRobot.getWalkParameter(Ts);
	//std::cout<<"ddd"<<d<<std::endl;
}



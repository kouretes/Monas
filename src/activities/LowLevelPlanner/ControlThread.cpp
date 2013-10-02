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


    //Defining the Optimal Gains for the Preview Controller
    //Integral Feedback Gain
    Gi=-70.3550;
    //State Feedback Gain
    Gx(0,0)=1.0e3*(-2.7219);
    Gx(0,1)=1.0e3*(-0.4450);
    Gx(0,2)=1.0e3*(0.2566);
    //Predicted Reference Gain
    Gd(0)=70.3550;
    Gd(1)=119.8532;
    Gd(2)=126.6650;
    Gd(3)=121.9978;
    Gd(4)=115.5007;
    Gd(5)=109.0384;
    Gd(6)=102.8914;
    Gd(7)=97.0818;
    Gd(8)=91.5958;
    Gd(9)=86.4160;
    Gd(10)=81.5256;
    Gd(11)=76.9088;
    Gd(12)=72.5503;
    Gd(13)=68.4359;
    Gd(14)=64.5521;
    Gd(15)=60.8862;
    Gd(16)=57.4260;
    Gd(17)=54.1603;
    Gd(18)=51.0781;
    Gd(19)=48.1694;
    Gd(20)=45.4244;
    Gd(21)=42.8341;
    Gd(22)=40.3899;
    Gd(23)=38.0836;
    Gd(24)=35.9075;
    Gd(25)=33.8544;
    Gd(26)=31.9174;
    Gd(27)=30.0900;
    Gd(28)=28.3661;
    Gd(29)=26.7399;
    Gd(30)=25.2059;
    Gd(31)=23.7590;
    Gd(32)=22.3942;
    Gd(33)=21.1070;
    Gd(34)=19.8930;
    Gd(35)=18.7481;
    Gd(36)=17.6684;
    Gd(37)=16.6502;
    Gd(38)=15.6901;
    Gd(39)=14.7848;
    Gd(40)=13.9312;
    Gd(41)=13.1263;
    Gd(42)=12.3675;
    Gd(43)=11.6520;
    Gd(44)=10.9776;
    Gd(45)=10.3418;
    Gd(46)=9.7424;
    Gd(47)=9.1774;
    Gd(48)=8.6449;
    Gd(49)=8.1429;

    //Defining the Optimal Observer Gain L
    /*L(0)=0.0359;
    L(1)=-0.3438;
    L(2)=-0.5050;*/
    L(0,0)=0.1161;
    L(0,1)=0.0301;
    L(1,0)=-0.1401;
    L(1,1)=-0.3260 ;
    L(2,0)=2.3890;
    L(2,1)=-0.5792;

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
    State+=L*error;
    //std::cout<<"State:"<<std::endl;
    //State.prettyPrint();
    //temp=L;
    //temp.scalar_mult(ZMPMeasured);


   	//Estimated COM position
	Com = State(0)+State(1)*2*OurRobot.getWalkParameter(Ts);///2;
	//d=State(1)*OurRobot.getWalkParameter(Ts);
	//std::cout<<"ddd"<<d<<std::endl;
}



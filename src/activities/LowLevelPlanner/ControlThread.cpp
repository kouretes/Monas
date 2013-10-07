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
    /*Aobs(0,0)=1.0000;
    Aobs(0,1)=OurRobot.getWalkParameter(Ts);
    Aobs(0,2)=-0.0360;
    Aobs(1,0)=0.3773;cd ..
    Aobs(1,1)=1.0000;
    Aobs(1,2)=-0.0328;
    Aobs(2,0)=0;
    Aobs(2,1)=0;
    Aobs(2,2)=1.5056; */

    Bd(0)=OurRobot.getWalkParameter(Ts)*OurRobot.getWalkParameter(Ts)*OurRobot.getWalkParameter(Ts)/6.0000;
    Bd(1)=OurRobot.getWalkParameter(Ts)*OurRobot.getWalkParameter(Ts)/2.0000;
    Bd(2)=OurRobot.getWalkParameter(Ts);
    Cd(0)=1.0000;
    Cd(1)=0.0000;
    Cd(2)=-OurRobot.getWalkParameter(ComZ)/OurRobot.getWalkParameter(g);


    //Defining the Optimal Gains for the Preview Controller
    //Integral Feedback Gain
    Gi=3.5355e+03;
    //State Feedback Gain
    Gx(0,0)=1.0e+05 *1.1868;
    Gx(0,1)=1.0e+05 * 0.2051 ;
    Gx(0,2)=1.0e+05*0.0020;
    //Predicted Reference Gain
    Gd(0)=-3.5355;
    Gd(1)=-6.8463;
    Gd(2)=-6.4579;
    Gd(3)=-6.0682;
    Gd(4)=-5.7066;
    Gd(5)=-5.3666;
    Gd(6)=-5.0468;
    Gd(7)=-4.7461;
    Gd(8)=-4.4633;
    Gd(9)=-4.1973;
    Gd(10)=-3.9472;
    Gd(11)=-3.7120;
    Gd(12)=-3.4908;
    Gd(13)=-3.2828;
    Gd(14)=-3.0872;
    Gd(15)=-2.9033;
    Gd(16)=-2.7303;
    Gd(17)=-2.5676;
    Gd(18)=-2.4146;
    Gd(19)=-2.2707;
    Gd(20)=-2.1354;
    Gd(21)=-2.0082;
    Gd(22)=-1.8885;
    Gd(23)=-1.7760;
    Gd(24)=-1.6702;
    Gd(25)=-1.5706;
    Gd(26)=-1.4771;
    Gd(27)=-1.3890;
    Gd(28)=-1.3063;
    Gd(29)=-1.2284;
    Gd(30)=-1.1552;
    Gd(31)=-1.0864;
    Gd(32)=-1.0217;
    Gd(33)=-0.9608;
    Gd(34)=-0.9035;
    Gd(35)=-0.8497;
    Gd(36)=-0.7991;
    Gd(37)=-0.7515;
    Gd(38)=-0.7067;
    Gd(39)=-0.6646;
    Gd(40)=-0.6250;
    Gd(41)=-0.5877;
    Gd(42)=-0.5527;
    Gd(43)=-0.5198;
    Gd(44)=-0.4888;
    Gd(45)=-0.4597;
    Gd(46)=-0.4323;
    Gd(47)=-0.4065;
    Gd(48)=-0.3823;
    Gd(49)=-0.3595;

    Gd.scalar_mult(1.0e+03);
    //Defining the Optimal Observer Gain L
    L(0,0)=0.1095;
    L(0,1)=0.0220;
    L(1,0)=-0.6647;
    L(1,1)=-0.1383;
    L(2,0)=-103.8237;
    L(2,1)=-39.6108;

    State(0)=0;
    State(1)=0;
    State(2)=0;
    //Initializing Variables
    Integrationfb=0;
    Statefb=0;
    Predictionfb=0;
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



void LIPMPreviewController::LIPMComPredictor(CircularBuffer<float> & ZmpBuffer,float CoMMeasured,float ZMPMeasured)
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
  //Setting the Reference Signal
	unsigned int l = 0;

	for (unsigned int i = 0; i < PreviewWindow; i++, l++)
	{
		if (i < ZmpBuffer.size())
			ZMPReference(l) = ZmpBuffer[i];
		else
			ZMPReference(l) = ZmpBuffer[ZmpBuffer.size() - 1];
	}


    }
    //State Feedback Computation
    Statefb=0;
    Statefb=Gx*State;

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

	uBuffer.push((ZMPReference(1)-ZMPReference(0)));
    //Updating the Dynamics

    KVecFloat2 error=KVecFloat2(CoMMeasured,ZMPMeasured);
	error-=KVecFloat2(State(0),StatePredict(0));
	error.scalar_mult(0.5);

    State=Ad*State;

    temp=Bd;
    temp.scalar_mult(u);
    State+=temp;

    State+=L*error;


    //Estimated COM position
    Com=State(0);

    Com = State(0)+State(1)*uBuffer.size()*OurRobot.getWalkParameter(Ts)/2;
}

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
    Gi=652.5882;
    //State Feedback Gain
    Gx(0,0)=1.0e+04 * 2.3094;
    Gx(0,1)=1.0e+04 * 0.4202;
    Gx(0,2)=1.0e+04 * 0.0078;
    //Predicted Reference Gain
    Gd(0)=-0.6526;
    Gd(1)=-0.7654;
    Gd(2)=-0.8979;
    Gd(3)=-1.0025;
    Gd(4)=-1.0622;
    Gd(5)=-1.0777;
    Gd(6)=-1.0583;
    Gd(7)=-1.0154;
    Gd(8)=-0.9595;
    Gd(9)=-0.8986;
    Gd(10)=-0.8380;
    Gd(11)=-0.7807;
    Gd(12)=-0.7280;
    Gd(13)=-0.6802;
    Gd(14)=-0.6369;
    Gd(15)=-0.5974;
    Gd(16)=-0.5613;
    Gd(17)=-0.5278;
    Gd(18)=-0.4966;
    Gd(19)=-0.4673;
    Gd(20)=-0.4398;
    Gd(21)=-0.4138;
    Gd(22)=-0.3893;
    Gd(23)=-0.3662;
    Gd(24)=-0.3445;
    Gd(25)=-0.3239;
    Gd(26)=-0.3046;
    Gd(27)=-0.2865;
    Gd(28)=-0.2694;
    Gd(29)=-0.2533;
    Gd(30)=-0.2382;
    Gd(31)=-0.2240;
    Gd(32)=-0.2107;
    Gd(33)=-0.1981;
    Gd(34)=-0.1863;
    Gd(35)=-0.1752;
    Gd(36)=-0.1648;
    Gd(37)=-0.1550;
    Gd(38)=-0.1458;
    Gd(39)=-0.1371;
    Gd(40)=-0.1289;
    Gd(41)=-0.1212;
    Gd(42)=-0.1140;
    Gd(43)=-0.1072;
    Gd(44)=-0.1008;
    Gd(45)=-0.0948;
    Gd(46)=-0.0892;
    Gd(47)=-0.0839;
    Gd(48)=-0.0789;
    Gd(49)=-0.0742;
    Gd.scalar_mult(1.0e+03);
    //Defining the O    ptimal Observer Gain L
    //L(0)=0.0360;
    //L(1)=-0.3445;
    //L(2)=-0.5056;
    L(0,0)=0.1051;
    L(0,1)=0.1063;
    L(1,0)=-0.2455;
    L(1,1)=-0.3425;
    L(2,0)=-18.2483;
    L(2,1)=-27.8822;

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
	ProcessNoise(0,0)=1e-5;
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

	//uBuffer.push((ZMPReference(1)-ZMPReference(0)));
    //Updating the Dynamics

    KVecFloat2 error=KVecFloat2(CoMMeasured,StatePredict(0));
	error-=KVecFloat2(State(0),(Cd(0)*State(0)+Cd(2)*State(2)));//StatePredict(0));
	error.scalar_mult(0.5);

    State=Ad*State;

    temp=Bd;
    temp.scalar_mult(u);
    State+=temp;

    State+=L*error;


    //Estimated COM position
    Com=State(0)+State(1)*OurRobot.getWalkParameter(Ts)/2;
}

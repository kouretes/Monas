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
    Ad(0,2)=OurRobot.getWalkParameter(Ts)*OurRobot.getWalkParameter(Ts)/2.0000;
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
    Bd(3)=0.0000;
    Cd(0)=1.0000;
    Cd(1)=0.0000;
    Cd(2)=-OurRobot.getWalkParameter(ComZ)/OurRobot.getWalkParameter(g);

  //Defining the Optimal Gains for the Preview Controller
    //Integral Feedback Gain
    Gi=1.5277e+03;
    //State Feedback Gain
    Gx(0,0)=1.0e+04 * 5.2363 ;
    Gx(0,1)=1.0e+04 * 0.9236;
    Gx(0,2)=1.0e+04 * 0.0122;
    //Predicted Reference Gain
   Gd(0)=-1.5277;
   Gd(1)=-2.1459;
   Gd(2)=-2.5852;
   Gd(3)=-2.7168;
   Gd(4)=-2.6433;
   Gd(5)=-2.4842;
   Gd(6)=-2.3114;
   Gd(7)=-2.1535;
   Gd(8)=-2.0149;
   Gd(9)=-1.8916;
   Gd(10)=-1.7789;
   Gd(11)=-1.6738;
   Gd(12)=-1.5748;
   Gd(13)=-1.4813;
   Gd(14)=-1.3932;
   Gd(15)=-1.3102;
   Gd(16)=-1.2321;
   Gd(17)=-1.1586;
   Gd(18)=-1.0896;
   Gd(19)=-1.0246;
   Gd(20)=-0.9636;
   Gd(21)=-0.9062;
   Gd(22)=-0.8522;
   Gd(23)=-0.8014;
   Gd(24)=-0.7537;
   Gd(25)=-0.7088;
   Gd(26)=-0.6665;
   Gd(27)=-0.6268;
   Gd(28)=-0.5895;
   Gd(29)=-0.5543;
   Gd(30)=-0.5213;
   Gd(31)=-0.4902;
   Gd(32)=-0.4610;
   Gd(33)=-0.4336;
   Gd(34)=-0.4077;
   Gd(35)=-0.3834;
   Gd(36)=-0.3606;
   Gd(37)=-0.3391;
   Gd(38)=-0.3189;
   Gd(39)=-0.2999;
   Gd(40)=-0.2820;
   Gd(41)=-0.2652;
   Gd(42)=-0.2494;
   Gd(43)=-0.2346;
   Gd(44)=-0.2206;
   Gd(45)=-0.2074;
   Gd(46)=-0.1951;
   Gd(47)=-0.1835;
   Gd(48)=-0.1725;
   Gd(49)=-0.1622;
    Gd.scalar_mult(1.0e+03);
    //Defining the Optimal Observer Gain L
    //L(0)=0.0360;;
    //L(1)=-0.3445;
    //L(2)=-0.5056;
    /*L(0,0)=0.1128;
    L(0,1)=0.0606;
    L(1,0)=-0.5004;
    L(1,1)=-0.2043;
    L(2,0)=-12.8674;
    L(2,1)=-22.9822;
    L(3,0)=-0.1403;
    L(3,1)=0.0821;
*/

    L(0,0)=0.1307;
    L(0,1)=0.0392;
    L(1,0)=-0.6250;
    L(1,1)=-0.2445;
    L(2,0)=-26.2666;
    L(2,1)=-11.9174;
    L(3,0)= -0.1623;
    L(3,1)=0.0557;


// 0.1061 0.0531 -0.3630 -0.3406 -42.2434 -10.8218




	State.zero();
    //Initializing Variables
    Integrationfb=0.000;
    Statefb=0.000;
    Predictionfb=0.000;
    /*Akalman(0,0)=1.000;
    Akalman(0,1)=0;//-OurRobot.getWalkParameter(Ts);
    Akalman(1,0)=0.000;
    Akalman(1,1)=1.000;*/
    Bkalman.zero();
    Bkalman(0)=1.000;//OurRobot.getWalkParameter(Ts);
    //Bkalman(1)=0.000;
    StateKalman.zero();
    ProcessNoise.zero();
	ProcessNoise(0,0)=1e-7;
	P.zero();
	P(0,0)=1e-20;
	MeasurementNoise.identity();
	MeasurementNoise(0,0)=0.001;
	MeasurementNoise(1,1)=0.5;
	Ckalman.zero();
	Ckalman(0,0)=1.000;
	Ckalman(1,0)=1.000;

	//MeasurementNoise.scalar_mult(0.1);
	uBuffer.push(0.000);




}



void LIPMPreviewController::LIPMComPredictor(CircularBuffer<float> & ZmpBuffer,float CoMMeasured,float ZMPMeasured)
{
    //Update
		/*Kalman filter for the ZMP in the corresponding axis taking into account the bias of the FSRs */
        //Predict

		StateKalman=StateKalman+Bkalman*uBuffer.front();


		float ct1=0.000,ct2=0.000;
		float zmpfromcom=0.000;
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
			//ykalman.prettyPrint();
			ykalman+=(Ckalman*(StateKalman)).scalar_mult(-1.0);//innovation value
			s=Ckalman*P*Ckalman.transp()+MeasurementNoise;
			s.fast_invert();
			Kgain=(P*Ckalman.transp())*s;
			StateKalman+=Kgain*ykalman;
		}

        //StateKalman.prettyPrint();

		Kgain.scalar_mult(-1.0);
		P+=Kgain*Ckalman*P;
		if(combuffer.size()>2)
			combuffer.pop();


		if(uBuffer.size()>3)
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
    Statefb=0.000;
    Statefb=Gx(0,0)*State(0)+Gx(0,1)*State(1)+Gx(0,2)*State(2);

    //Updating the Integral Feedback
    Integrationfb+=Gi*((Cd(0)*State(0)+Cd(2)*State(2))-ZMPReference(0));
    //Predicted Feedback Computation
   //Predicted Feedback Computation
    Predictionfb=0.000;
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
    error(0)*=0.6;
    State=Ad*State;

    temp=Bd;
    temp.scalar_mult(u);
    State+=temp;

    State+=L*error;
    //State.prettyPrint();

    uBuffer.push((Cd(0)*State(0)+Cd(2)*State(2))-zmpstate);//(ZMPReference(1)-ZMPReference(0)));


    //Estimated COM position
    Com=State(0);//+0.5*(State(1)+1/2*State(2)*OurRobot.getWalkParameter(Ts))*OurRobot.getWalkParameter(Ts);//
}

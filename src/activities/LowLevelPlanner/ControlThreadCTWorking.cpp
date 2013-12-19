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

    /** Defining the Optimal Gains for the Preview Controller **/
    //Integral Feedback Gain
    Gi=2.2069e+03;
    //State Feedback Gain
    Gx(0,0)=1.0e+04 * 7.3550 ;
    Gx(0,1)=1.0e+04 * 1.2624;
    Gx(0,2)=1.0e+04 * 0.0147;
    //Predicted Reference Gain
   Gd(0)=-2.2069;
   Gd(1)=-3.4473;
   Gd(2)=-3.9953;
   Gd(3)=-3.9547;
   Gd(4)=-3.7037;
   Gd(5)=-3.4391;
   Gd(6)=-3.2096;
   Gd(7)=-3.0095;
   Gd(8)=-2.8273;
   Gd(9)=-2.6567;
   Gd(10)=-2.4959;
   Gd(11)=-2.3445;
   Gd(12)=-2.2021;
   Gd(13)=-2.0683;
   Gd(14)=-1.9427;
   Gd(15)=-1.8247;
   Gd(16)=-1.7139;
   Gd(17)=-1.6098;
   Gd(18)=-1.5121;
   Gd(19)=-1.4202;
   Gd(20)=-1.3340;
   Gd(21)=-1.2530;
   Gd(22)=-1.1769;
   Gd(23)=-1.1054;
   Gd(24)=-1.0383;
   Gd(25)=-0.9752;
   Gd(26)=-0.9160;
   Gd(27)=-0.8604;
   Gd(28)=-0.8081;
   Gd(29)=-0.7590;
   Gd(30)=-0.7129;
   Gd(31)=-0.6696;
   Gd(32)=-0.6290;
   Gd(33)=-0.5908;
   Gd(34)=-0.5549;
   Gd(35)=-0.5212;
   Gd(36)=-0.4896;
   Gd(37)=-0.4598;
   Gd(38)=-0.4319;
   Gd(39)=-0.4057;
   Gd(40)=-0.3810;
   Gd(41)=-0.3579;
   Gd(42)=-0.3362;
   Gd(43)=-0.3157;
   Gd(44)=-0.2966;
   Gd(45)=-0.2786;
   Gd(46)=-0.2616;
   Gd(47)=-0.2457;
   Gd(48)=-0.2308;
   Gd(49)=-0.2168;
   Gd(50)=-0.2036;
    Gd(51)=-0.1913;
    Gd(52)=-0.1797;
    Gd(53)=-0.1687;
    Gd(54)=-0.1585;
    Gd(55)=-0.1489;
    Gd(56)=-0.1398;
    Gd(57)=-0.1313;
    Gd(58)=-0.1234;
    Gd(59)=-0.1159;
      Gd(60)=-0.1088;
        Gd(61)=-0.1022;
       Gd(62)=-0.0960;
       Gd(63)=-0.0902;
       Gd(64)=-0.0847;
        Gd(65)=-0.0796;
         Gd(66)=-0.0747;
         Gd(67)=-0.0702;
          Gd(68)=-0.0659;
           Gd(69)=-0.0619;
           Gd(70)=-0.0582;
           Gd(71)= -0.0546;
           Gd(72)= -0.0513;
           Gd(73)= -0.0482;
           Gd(74)= -0.0453;
           Gd(75)= -0.0425;
           Gd(76)= -0.0399;
           Gd(77)= -0.0375;
           Gd(78)= -0.0352;
           Gd(79)= -0.0331;
           Gd(80)= -0.0311;
           Gd(81)= -0.0292;
           Gd(82)= -0.0274;
           Gd(83)= -0.0258;
           Gd(84)= -0.0242;
           Gd(85)= -0.0227;
           Gd(86)= -0.0213;
           Gd(87)= -0.0201;
           Gd(88)= -0.0188;
           Gd(89)= -0.0177;
           Gd(90)= -0.0166;
           Gd(91)= -0.0156;
           Gd(92)= -0.0147;
           Gd(93)= -0.0138;
           Gd(94)= -0.0129;
           Gd(95)= -0.0121;
           Gd(96)= -0.0114;
           Gd(97)= -0.0107;
           Gd(98)= -0.0101;
           Gd(99)= -0.0095;
    Gd.scalar_mult(1.0e+03);
    //Defining the Optimal Observer Gain L
   /* L(0,0)=0.0654;
    L(0,1)=0.0264;
    L(1,0)=-0.3713;
    L(1,1)=-0.2079;
    L(2,0)=-10.6090;
    L(2,1)=1.4385;
    L(3,0)=-0.0783;
    L(3,1)=0.0402;
*/

    L(0,0)=0.1307;
    L(0,1)=0.0392;
    L(1,0)=-0.6250;
    L(1,1)=-0.2445;
    L(2,0)=-26.2666;
    L(2,1)=-11.9174;
    L(3,0)= -0.1623;
    L(3,1)=0.0557;






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
    error.scalar_mult(1.0);

    State=Ad*State;

    temp=Bd;
    temp.scalar_mult(u);
    State+=temp;

    State+=L*error;
    //State.prettyPrint();
    std::cout<<"ZMPREF"<<std::endl;
    std::cout<<ZMPReference(1)<<std::endl;
     std::cout<<"predicted"<<std::endl;
    std::cout<<Cd(0)*State(0)+Cd(2)*State(2)<<std::endl;
    predictedError=Cd(0)*State(0)+Cd(2)*State(2)-ZMPReference(1);

     uBuffer.push((Cd(0)*State(0)+Cd(2)*State(2))-zmpstate);//(ZMPReference(1)-ZMPReference(0)));


    //Estimated COM position
    Com=State(0);//+0.5*(State(1)+1/2*State(2)*OurRobot.getWalkParameter(Ts))*OurRobot.getWalkParameter(Ts);//
}

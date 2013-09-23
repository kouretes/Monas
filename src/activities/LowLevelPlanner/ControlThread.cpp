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
    Ad(0,2)=0;
    Ad(1,0)=(OurRobot.getWalkParameter(g)/OurRobot.getWalkParameter(ComZ))*OurRobot.getWalkParameter(Ts);
    Ad(1,1)=1.0000;
    Ad(1,2)=-Ad(1,0);
    Ad(2,0)=0.0000;
    Ad(2,1)=0.0000;
    Ad(2,2)=1.0000;
    Aobs(0,0)=1.0000;
    Aobs(0,1)=OurRobot.getWalkParameter(Ts);
    Aobs(0,2)=-0.0374;
    Aobs(1,0)=0.4088;
    Aobs(1,1)=1.0000;
    Aobs(1,2)=-0.0362;
    Aobs(2,0)=0;
    Aobs(2,1)=0;
    Aobs(2,2)=1.5055;
    Bd(0)=0;
    Bd(1)=0;
    Bd(2)=OurRobot.getWalkParameter(Ts);
    Cd(0)=0;
    Cd(1)=0;
    Cd(2)=1;
    
    
    //Defining the Optimal Gains for the Preview Controller
    //Integral Feedback Gain
    Gi=-70.0323;
    //State Feedback Gain
    Gx(0,0)=1.0e3*(-2.6230);
    Gx(0,1)=1.0e3*(-0.4123);
    Gx(0,2)=1.0e3*(0.2568);
    //Predicted Reference Gain
    Gd(0)=70.0323;
    Gd(1)=119.0775;
    Gd(2)=125.5550;
    Gd(3)=120.6681;
    Gd(4)=114.0059;
    Gd(5)=107.4075;
    Gd(6)=101.1454;
    Gd(7)=95.2385;
    Gd(8)=89.6712;
    Gd(9)=84.4247;
    Gd(10)=79.4809;
    Gd(11)=74.8226;
    Gd(12)=70.4335;
    Gd(13)=66.2983;
    Gd(14)=62.4027;
    Gd(15)=58.7328;
    Gd(16)=55.2758;
    Gd(17)=52.0196;
    Gd(18)=48.9526;
    Gd(19)=46.0641;
    Gd(20)=43.3438;
    Gd(21)=40.7820;
    Gd(22)=38.3696;
    Gd(23)=36.0981;
    Gd(24)=33.9594;
    Gd(25)=31.9457;
    Gd(26)=30.0499;
    Gd(27)=28.2652;
    Gd(28)=26.5851;
    Gd(29)=25.0037;
    Gd(30)=23.5151;
    Gd(31)=22.1141;
    Gd(32)=20.7955;
    Gd(33)=19.5545;
    Gd(34)=18.3867;
    Gd(35)=17.2878;
    Gd(36)=16.2537;
    Gd(37)=15.2808;
    Gd(38)=14.3654;
    Gd(39)=13.5041;
    Gd(40)=12.6939;
    Gd(41)=11.9317;
    Gd(42)=11.2147;
    Gd(43)=10.5403;
    Gd(44)=9.9060;
    Gd(45)=9.3094;
    Gd(46)=8.7483;
    Gd(47)=8.2206;
    Gd(48)=7.7243;
    Gd(49)=7.2577;
    
    //Defining the Optimal Observer Gain L
    L(0)=0.0374;
    L(1)=-0.3725;
    L(2)=-0.5055;
    State(0)=0;
    State(1)=0;
    State(2)=0;
    //Initializing Variables
    Integrationfb=0;
    Statefb=0;
    Predictionfb=0;
    counter=0;
}


void LIPMPreviewController::LIPMComPredictor(CircularBuffer<float> & ZmpBuffer/*, std::vector<float> ZMP,bool print*/ )
{
    //Setting the Reference Signal
    unsigned int l=0;

    for (unsigned int i=0;i<=50;i++,l++)
    {
        if (i<ZmpBuffer.size())
           ZMPReference(l)=ZmpBuffer[i];
        else
           ZMPReference(l)=ZmpBuffer[ZmpBuffer.size()-1];

    }
//    l=0;
//    for (unsigned int i=counter;i<=counter+50;i++)
//    {
//        if (i<ZMP.size())
//            ZMPReference2(l)=ZMP[i];//ZMP[i];
//        else
//            ZMPReference2(l)=ZMP.back();
//
//        l++;
//    }
//
//
//    if(print)
//    for (unsigned int i=0;i<=50;i++)
//    	std::cout << "Counter "<< counter << " i = " << i << " ZMPReference " << ZMPReference(i) <<  " ZMPReference2 " << ZMPReference2(i) << std::endl;
//    counter++;
    ZMPMeasured=State(2);

    //State Feedback Computation
    Statefb=0;
    Statefb=Gx*State;

    //Updating the Integral Feedback
    Integrationfb+=Gi*(ZMPMeasured-ZMPReference(0));
    //Predicted Feedback Computation
    Predictionfb=0;
    unsigned  int z=0;
    for (unsigned int i=0;i<50;i++)
    {
        Predictionfb+=Gd(z)*ZMPReference(i+1);
        z++;
    }
    //Optimal Preview Control
    u=-Statefb-Integrationfb-Predictionfb;

    //Updating the Dynamics
    State=Aobs*State;
    temp=L;
    temp.scalar_mult(ZMPMeasured);
    State+=temp;
    temp=Bd;
    temp.scalar_mult(u);
    State+=temp;
    //Estimated COM position
    Com=State(0);
    //std::cout << " Com " << Com << std::endl;
}


void LIPMPreviewController::LIPMComPredictor(std::vector<float> ZMP)
{
    //Setting the Reference Signal
    unsigned int l=0;
    for (unsigned int i=counter;i<=counter+50;i++)
    {
        if (i<ZMP.size())
        {
            ZMPReference(l)=ZMP[i];//ZMP[i];
        }
        else
        {
            ZMPReference(l)=ZMP.back();
        }
     //   std::cout << " l: " << l << " i " << i << std::endl;
        l++;


        
    }
    counter++;

    ZMPMeasured=State(2);

    //State Feedback Computation
    Statefb=0;
    Statefb=Gx*State;
    
    //Updating the Integral Feedback
    Integrationfb+=Gi*(ZMPMeasured-ZMPReference(0));
    //Predicted Feedback Computation
    Predictionfb=0;
    unsigned  int z=0;
    for (unsigned  int i=0;i<50;i++)
    {
        Predictionfb+=Gd(z)*ZMPReference(i+1);
        z++;
    }
    //Optimal Preview Control
    u=-Statefb-Integrationfb-Predictionfb;

    //Updating the Dynamics
    State=Aobs*State;
    temp=L;
    temp.scalar_mult(ZMPMeasured);
    State+=temp;
    temp=Bd;
    temp.scalar_mult(u);
    State+=temp;

    //Estimated COM position
    Com=State(0);
    COM.push_back(Com); // for cout purposes

    
}

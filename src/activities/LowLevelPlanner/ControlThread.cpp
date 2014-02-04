//
//  ControlThread.cpp
//  Kouretes Walk Engine
//
//  Created by Stelios Piperakis on 8/14/13.
//  Copyright (c) 2013 SP. All rights reserved.
//

#include "ControlThread.h"
#include <iostream>
LIPMPreviewController::LIPMPreviewController(RobotParameters &rp ) : walkprof("ControlThread"), OurRobot(rp), DynamicsX(rp), DynamicsY(rp), KalmanX(rp), KalmanY(rp),flog("log",RAW,20){

    KalmanX.uBuffer.push(0.000);
    KalmanY.uBuffer.push(0.000);
    uX=0.000;
    uY=0.000;
    flog.insert("ZMPx",0);
    flog.insert("ZMPy",0);
    flog.insert("refZMPx",0);
    flog.insert("refZMPy",0);
    flog.insert("COMx",0);
    flog.insert("COMy",0);
    flog.insert("Ux",0);
    flog.insert("Uy",0);
    flog.insert("Bx",0);
    flog.insert("By",0);

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
    //Initializing Variables
    Integrationfbx=0.000;
    Statefbx=0.000;
    Predictionfbx=0.000;
    Integrationfby=0.000;
    Statefby=0.000;
    Predictionfby=0.000;




}



void LIPMPreviewController::LIPMComPredictor(CircularBuffer<KVecFloat3> & ZmpBuffer, float CoMMeasuredX,float CoMMeasuredY,float ZMPMeasuredX,float ZMPMeasuredY )
{
    KalmanX.Filter(ZMPMeasuredX,CoMMeasuredX);

    KalmanY.Filter(ZMPMeasuredY,CoMMeasuredY);
	    //Setting the Reference Signal

	for (unsigned int i = 0; i < PreviewWindow; i++)
	{
		if (i < ZmpBuffer.size()){
			ZMPReferenceX(i) = ZmpBuffer[i](0);
		    ZMPReferenceY(i) = ZmpBuffer[i](1);
		}
		else{
			ZMPReferenceX(i) = ZmpBuffer[ZmpBuffer.size() - 1](0);
			ZMPReferenceY(i) = ZmpBuffer[ZmpBuffer.size() - 1](1);
			}
	}


    //State Feedback Computation
    Statefbx=0.000;
    Statefby=0.000;
    Statefbx=Gx(0,0)*DynamicsX.State(0)+Gx(0,1)*DynamicsX.State(1)+Gx(0,2)*DynamicsX.State(2);
    Statefby=Gx(0,0)*DynamicsY.State(0)+Gx(0,1)*DynamicsY.State(1)+Gx(0,2)*DynamicsY.State(2);
    //Updating the Integral Feedback
    Integrationfbx+=Gi*((DynamicsX.Cd(0)*DynamicsX.State(0)+DynamicsX.Cd(2)*DynamicsX.State(2))-ZMPReferenceX(0));
    Integrationfby+=Gi*((DynamicsY.Cd(0)*DynamicsY.State(0)+DynamicsY.Cd(2)*DynamicsY.State(2))-ZMPReferenceY(0));

    //Predicted Feedback Computation
   //Predicted Feedback Computation
    Predictionfbx=0.000;
    Predictionfby=0.000;
    unsigned  int z=0;
    for (unsigned  int i=0;i<PreviewWindow-1;i++)
    {
        Predictionfbx+=Gd(z)*ZMPReferenceX(i+1);
        Predictionfby+=Gd(z)*ZMPReferenceY(i+1);
        z++;
    }
    walkprof.generate_report(1000);
    //Optimal Preview Control
    uX=-Statefbx-Integrationfbx-Predictionfbx;
    uY=-Statefby-Integrationfby-Predictionfby;
        //Updating the Dynamics
        KVecFloat2 errorX=KVecFloat2(CoMMeasuredX,KalmanX.StatePredict(0));
        KVecFloat2 errorY=KVecFloat2(CoMMeasuredY,KalmanY.StatePredict(0));
        DynamicsX.AugmentState();
        DynamicsY.AugmentState();
        DynamicsX.Update(uX,errorX);
        DynamicsY.Update(uY,errorY);

        KalmanX.uBuffer.push(DynamicsX.zmpstateNew-DynamicsX.zmpstate);
        //KalmanX.uBuffer.push(ZMPReferenceX(1)-ZmpBufferX[0]);
        KalmanY.uBuffer.push(DynamicsY.zmpstateNew-DynamicsY.zmpstate);
        //KalmanY.uBuffer.push(ZMPReferenceY(1)-ZmpBufferY[0]);

        //Estimated COM position
        COM(0)=DynamicsX.State(0);
        COM(1)=DynamicsY.State(0);       //+0.5*(State(1)+1/2*State(2)*OurRobot.getWalkParameter(Ts))*OurRobot.getWalkParameter(Ts);//
        predictedErrorX=DynamicsX.predictedError*0;
        predictedErrorY=DynamicsY.predictedError*0;

        flog.insert("ZMPx",DynamicsX.zmpstate);
        flog.insert("ZMPy",DynamicsY.zmpstate);
        flog.insert("refZMPx",ZmpBuffer[0](0));
        flog.insert("refZMPy",ZmpBuffer[0](1));
        flog.insert("COMx",DynamicsX.State(0));
        flog.insert("COMy",DynamicsY.State(0));
        flog.insert("Ux",uX);
        flog.insert("Uy",uY);
        flog.insert("Bx",DynamicsX.State(3));
        flog.insert("By",DynamicsY.State(3));
        flog.periodic_save();

}

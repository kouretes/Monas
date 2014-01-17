//
//  ControlThread.cpp
//  Kouretes Walk Engine
//
//  Created by Stelios Piperakis on 8/14/13.
//  Copyright (c) 2013 SP. All rights reserved.
//

#include "ControlThread.h"
#include <iostream>
LIPMPreviewController::LIPMPreviewController(RobotParameters &rp ) : OurRobot(rp), DynamicsX(OurRobot), DynamicsY(OurRobot), KalmanX(OurRobot), KalmanY(OurRobot)
{
    KalmanX.uBuffer.push(0.000);
    KalmanY.uBuffer.push(0.000);


    /** Compute Gains Kx, Ky **/
    DMPC();

    //Initializing Variables
    DeltauX=0.000;
    DeltauY=0.000;
    uX=0.000;
    uY=0.000;
}



void LIPMPreviewController::LIPMComPredictor(CircularBuffer<float> & ZmpBufferX,CircularBuffer<float> & ZmpBufferY, float CoMMeasuredX,float CoMMeasuredY,float ZMPMeasuredX,float ZMPMeasuredY )
{
    KalmanX.Filter(ZMPMeasuredX,CoMMeasuredX);

    KalmanY.Filter(ZMPMeasuredY,CoMMeasuredY);

    //Setting the Reference Signal
    int l = 0;
	for (int i = 1; i < PreviewWindow; i++, l++)
	{
		if (i < ZmpBufferX.size()){
			ZMPReferenceX(l) = ZmpBufferX[i];
			ZMPReferenceY(l) = ZmpBufferY[i];
		}
		else{

			ZMPReferenceX(l) = ZmpBufferX[ZmpBufferX.size() - 1];
			ZMPReferenceY(l) = ZmpBufferY[ZmpBufferY.size() - 1];
		}
	}


      DynamicsX.AugmentState();
      DynamicsY.AugmentState();

      /**define Laguerre Coefficients **/
      httaX=Ky*ZMPReferenceX-Kx*DynamicsX.State_e;
      httaY=Ky*ZMPReferenceY-Kx*DynamicsY.State_e;
      /** Optimal Preview Control **/
      DeltauX=L0.transp()*httaX;
      DeltauY=L0.transp()*httaY;

      KVecFloat2 errorX=KVecFloat2(CoMMeasuredX,KalmanX.StatePredict(0));
      KVecFloat2 errorY=KVecFloat2(CoMMeasuredY,KalmanY.StatePredict(0));
      //std::cout<<"X : "<< KalmanX.StatePredict(0)<<std::endl;
      //std::cout<<"Y : "<< KalmanY.StatePredict(0)<<std::endl;

      uX=uX+DeltauX;
      uY=uY+DeltauY;
      DynamicsX.Update(uX,errorX);
      DynamicsY.Update(uY,errorY);


        KalmanX.uBuffer.push(DynamicsX.zmpstateNew-DynamicsX.zmpstate);
        //KalmanX.uBuffer.push(ZMPReferenceX(1)-ZmpBufferX[0]);
        KalmanY.uBuffer.push(DynamicsY.zmpstateNew-DynamicsY.zmpstate);
        //KalmanY.uBuffer.push(ZMPReferenceY(1)-ZmpBufferY[0]);

         //Estimated COM position
        COM(0)=DynamicsX.State(0);
        COM(1)=DynamicsY.State(0);       //+0.5*(State(1)+1/2*State(2)*OurRobot.getWalkParameter(Ts))*OurRobot.getWalkParameter(Ts);//
        predictedErrorX=DynamicsX.predictedError;
        predictedErrorY=DynamicsY.predictedError;

}

void LIPMPreviewController::generateLaguerre()
{
	float alpha=0.8;

	float beta=1-alpha*alpha;


	KMath::KMat::GenMatrix<float, LagN, 1> v;
	v(0)=alpha;
	Al.zero();
	L0.zero();
	L0(0)=1;
	for(unsigned i=1;i<LagN;i++)
	{
		v(i)=pow(-alpha,i-1)*beta;
		L0(i)=pow(-alpha,i);
	}

	L0.scalar_mult(sqrt(beta));
	for(unsigned i=0;i<LagN;i++)
	{
        Al(i,0)=v(i);
	}

	for(unsigned j=1;j<LagN;j++)
	{
        for(unsigned i=j;i<LagN;i++)
		{
			Al(i,j)=v(i-j);
		}
	}


	/*v.prettyPrint();
	L0.prettyPrint();
	Al.prettyPrint();*/

}
void LIPMPreviewController::DMPC()
{
	generateLaguerre();
	float rl=1e-6;




    KMath::KMat::GenMatrix<float,3,3> A;
    KMath::KMat::GenMatrix<float,3,1> b,c;
    KMath::KMat::GenMatrix<float,4,4> Ae,Aepower;
    KMath::KMat::GenMatrix<float,4,1> Be;
    KMath::KMat::GenMatrix<float,1,4> Cetransp;
    KMath::KMat::GenMatrix<float,1,3> temp;
    KMath::KMat::GenMatrix<float,1,4> temp2;
    KMath::KMat::GenMatrix<float,LagN,1> Li;
    KMath::KMat::GenMatrix<float,4,LagN> tempSc;
    KMath::KMat::GenMatrix<float,1,LagN> tempPhi;

	KMath::KMat::GenMatrix<float, LagN, LagN> R_l,Tres;
	KMath::KMat::GenMatrix<float, LagN, PreviewWindow-1> Meg;

	R_l.identity();
	R_l.scalar_mult(rl);

    A(0,0)=1.0000;
    A(0,1)=OurRobot.getWalkParameter(Ts);
    A(0,2)=OurRobot.getWalkParameter(Ts)*OurRobot.getWalkParameter(Ts)/2.0000;
    A(1,0)=0.0000;
    A(1,1)=1.0000;
    A(1,2)=OurRobot.getWalkParameter(Ts);
    A(2,0)=0.0000;
    A(2,1)=0.0000;
    A(2,2)=1.0000;

    b(0)=OurRobot.getWalkParameter(Ts)*OurRobot.getWalkParameter(Ts)*OurRobot.getWalkParameter(Ts)/6.0000;
    b(1)=OurRobot.getWalkParameter(Ts)*OurRobot.getWalkParameter(Ts)/2.0000;
    b(2)=OurRobot.getWalkParameter(Ts);

	c(0)=1.0000;
    c(1)=0.0000;
    c(2)=-OurRobot.getWalkParameter(ComZ)/OurRobot.getWalkParameter(g);

    //-------------------Ae
    Ae.zero();

    for(unsigned i=0;i<3;i++)
		for(unsigned j=0;j<3;j++)
			Ae(i,j)=A(i,j);
	temp=c.transp()*A;
	for(unsigned j=0;j<3;j++)
		Ae(3,j)=temp(0,j);
	Ae(3,3)=1;


	//----------------Be
	for(unsigned j=0;j<3;j++)
		Be(j)=b(j);
    Be(3)=c.transp()*b;

    //---------------CeTransp

    Cetransp.zero();
    Cetransp(0,3)=1;


    //---------------CREATE PHI AND TAU
	Li=L0;
	Tau.zero();
	tempSc.zero();
	Aepower.identity();

    for(unsigned i=0;i<PreviewWindow-1;i++)
	{
		tempSc=Ae*tempSc;
		tempSc+=Be*Li.transp();
		tempPhi=Cetransp*tempSc;

		for(unsigned j=0;j<LagN;j++)
			Phi(i,j)=tempPhi(0,j);

		Aepower*=Ae;
		//Tau row i
		temp2=Cetransp*Aepower;

		for(unsigned j=0;j<4;j++)
			Tau(i,j)=temp2(0,j);
		Li=Al*Li;
	}

	Tau.prettyPrint();
	Phi.prettyPrint();

	Tres=(Phi.transp()*Phi+R_l);
	Tres.fast_invert();
	Meg=Tres*Phi.transp();

	Kx=Meg*Tau;
	Ky=Meg;


}


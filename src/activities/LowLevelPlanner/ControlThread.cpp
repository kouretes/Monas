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



void LIPMPreviewController::LIPMComPredictor(CircularBuffer<KVecFloat3> & ZmpBuffer, float CoMMeasuredX,float CoMMeasuredY,float ZMPMeasuredX,float ZMPMeasuredY )
{
    KalmanX.Filter(ZMPMeasuredX,CoMMeasuredX);

    KalmanY.Filter(ZMPMeasuredY,CoMMeasuredY);

    //Setting the Reference Signal
    int l = 0;
	for (int i = 1; i < PreviewWindow; i++, l++)
	{
		if (i < ZmpBuffer.size()){
			ZMPReferenceX(l) = ZmpBuffer[i](0);
			ZMPReferenceY(l) = ZmpBuffer[i](1);
			ZMPtheta(l) = ZmpBuffer[i](2);
		}
		else{

			ZMPReferenceX(l) = ZmpBuffer[ZmpBuffer.size() - 1](0);
			ZMPReferenceY(l) = ZmpBuffer[ZmpBuffer.size() - 1](1);
			ZMPtheta(l)		 = ZmpBuffer[ZmpBuffer.size() - 1](2);
		}
	}


      DynamicsX.AugmentState();
      DynamicsY.AugmentState();

      /**define Laguerre Coefficients **/


      solveConstrainedMPC();
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

void LIPMPreviewController::solveConstrainedMPC()
{
	KMath::FunctionQuadraticSymmetric<float,2*LagN> costfunct;
	KMath::FunctionAffine<float,2*LagN,PreviewWindow-1> af1,af2,af3,af4;
	KMath::FunctionQuadraticPenalty<float,2*LagN,PreviewWindow-1> c1(af1),c2(af2),c3(af3),c4(af4);


	pX=ZMPReferenceX-Tau*DynamicsX.State_e;
	pY=ZMPReferenceY-Tau*DynamicsY.State_e;
	httaX=Ky*pX;
    httaY=Ky*pY;


    costfunct.setH(H);
    fx=Phi.transp()*pX;
    fx.scalar_mult(2);
    fy=Phi.transp()*pY;
    fy.scalar_mult(2);

    for(unsigned i=0;i<LagN;i++)
	{
		f(i)=-fx(i);
		f(i+LagN)=-fy(i);
	}
	//std::cout<<"WHAT?"<<std::endl;
	costfunct.setf(f);
    //CONSTRAINT 1
    fillConstraints(Aineq1,bineq1,true,false);
    bineq1.scalar_add(OurRobot.getWalkParameter(StepX)/2.0);
    af1.setA(Aineq1);
    af1.setb(bineq1);
    c1.setFunction(af1);
    //std::cout<<"WHAT?2"<<std::endl;
    //CONSTRAINT 2
    /*fillConstraints(Aineq2,bineq2,false,true);
    bineq2.scalar_add(OurRobot.getWalkParameter(StepY)/3);
    af2.setA(Aineq2);
    af2.setb(bineq2);
    c2.setFunction(af2);

	//CONSTRAINT 3
    fillConstraints(Aineq3,bineq3,true,false);
    Aineq3.scalar_mult(-1);
    bineq3.scalar_mult(-1);
    bineq3.scalar_add(OurRobot.getWalkParameter(StepX)/3);

    af3.setA(Aineq3);
    af3.setb(bineq3);
    c3.setFunction(af3);

    fillConstraints(Aineq4,bineq4,false,true);
    Aineq4.scalar_mult(-1);
    bineq4.scalar_mult(-1);
    bineq4.scalar_add(OurRobot.getWalkParameter(StepY)/3);

	//std::cout<<"WHAT?set"<<std::endl;
	af4.setA(Aineq4);
    af4.setb(bineq4);
    c4.setFunction(af4);*/

	//std::cout<<"WHAT?add"<<std::endl;
    solver.addCostFunction(&costfunct);
    solver.addPenaltyFunction(&c1);
    //solver.addPenaltyFunction(&c2);
    //solver.addPenaltyFunction(&c3);
    //solver.addPenaltyFunction(&c4);
    //std::cout<<"WHAT?assign"<<std::endl;

    for(unsigned i=0;i<LagN;i++)
	{
		htta(i)=httaX(i);
		htta(i+LagN)=httaY(i);
	}
	//htta.prettyPrint();
	af1.setX(htta);
    af1.evaluate().prettyPrint();
    htta=solver.solve(htta);

    //htta.prettyPrint();
    //std::cout<<"WHAT?assignback"<<std::endl;
	for(unsigned i=0;i<LagN;i++)
	{
		httaX(i)=htta(i);
		httaY(i)=htta(i+LagN);
	}



}

void LIPMPreviewController::fillConstraints(
						KMath::KMat::GenMatrix<float,PreviewWindow-1,2*LagN> & A,
						KMath::KMat::GenMatrix<float,PreviewWindow-1,1> &b,
						bool ecos,bool fcos
						)
{
	for(unsigned i=0;i<PreviewWindow-1;i++)
	{
		float a,c;
		if(ecos)
			a= cos(ZMPtheta(i));
		else
			a= -sin(ZMPtheta(i));
		if(fcos)
			c= cos(ZMPtheta(i));
		else
			c= sin(ZMPtheta(i));
		std::cout<<c<<std::endl;

		for(unsigned j=0;j<LagN;j++)
		{
			A(i,j)=(a)*Phi(i,j);
			A(i,j+LagN)=(c)*Phi(i,j);
		}

		b(i)=(a)*pX(i)+(c)*pY(i);
	}

}
void LIPMPreviewController::generateLaguerre()
{
	float alpha=0.5;

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
	//FIX H for constrained
	H.zero();
    for(unsigned i=0;i<LagN;i++)
		for(unsigned j=0;j<LagN;j++)
		{
			H(i,j)=Tres(i,j);
			H(i+LagN,j+LagN)=Tres(i,j);

		}
	std::cout<<"EDW?"<<std::endl;



	Tres.fast_invert();
	Meg=Tres*Phi.transp();

	Kx=Meg*Tau;
	Ky=Meg;


}


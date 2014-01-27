//
//  ControlThread.cpp
//  Kouretes Walk Engine
//
//  Created by Stelios Piperakis on 8/14/13.
//  Copyright (c) 2013 SP. All rights reserved.
//

#include "ControlThread.h"
#include <iostream>
//#define SCALECONSTRAINT(i) (0.9-((float)(i))/(50.0*CONST_SIZE))
#define SCALECONSTRAINT(i) 0.9
#define BASISM 3.0
#define BASISD 3.0

LIPMPreviewController::LIPMPreviewController(RobotParameters &rp ) : walkprof("ControlThread"), OurRobot(rp), DynamicsX(rp), DynamicsY(rp), KalmanX(rp), KalmanY(rp)
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
	for (int i = 1; i < PreviewWindow; i++)
	{
		if (i < ZmpBuffer.size()){
			ZMPReferenceX(i-1) = ZmpBuffer[i](0);
			ZMPReferenceY(i-1) = ZmpBuffer[i](1);
			ZMPtheta(i-1) = ZmpBuffer[i](2);
		}
		else{

			ZMPReferenceX(i-1) = ZmpBuffer[ZmpBuffer.size() - 1](0);
			ZMPReferenceY(i-1) = ZmpBuffer[ZmpBuffer.size() - 1](1);
			ZMPtheta(i-1)		 = ZmpBuffer[ZmpBuffer.size() - 1](2);
		}
	}


      DynamicsX.AugmentState();
      DynamicsY.AugmentState();

      /**define Laguerre Coefficients **/


      solveConstrainedMPC();
      walkprof.generate_report(1000);
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

	KPROF_SCOPE(walkprof,"solveConstrainedMPC")
	KMath::FunctionQuadraticSymmetric<float,2*LagN> costfunct;
	KMath::FunctionAffine<float,2*LagN,CONST_SIZE> af1,af2,af3,af4;
	KMath::FunctionQuadraticPenalty<float,2*LagN,CONST_SIZE> c1(af1),c2(af2),c3(af3),c4(af4);


	pX=ZMPReferenceX-Tau*DynamicsX.State_e;
	pY=ZMPReferenceY-Tau*DynamicsY.State_e;



	httaX=Ky*pX;
	httaY=Ky*pY;
	htta.zero();
	return ;
	for(unsigned i=0;i<LagN;i++)
	{
		htta(i)=httaX(i);
		htta(i+LagN)=httaY(i);
	}






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
	float yconstr=OurRobot.getWalkParameter(StepY)/2.0;
	if(isDoubleSupport)
		yconstr+=OurRobot.getWalkParameter(StepY);
	//std::cout<<"WHAT?"<<std::endl;
	costfunct.setf(f);
    //CONSTRAINT 1
    fillConstraints(Aineq1,bineq1,true,false);
    //bineq1.prettyPrint();
    //std::cout<<OurRobot.getWalkParameter(StepX)<<std::endl;
    for(unsigned i=0;i<CONST_SIZE;i++)
		bineq1(i)+=( SCALECONSTRAINT(i)*OurRobot.getWalkParameter(StepX)/2.0);
    af1.setA(Aineq1);
    af1.setb(bineq1);
    c1.setFunction(af1);
    //std::cout<<"WHAT?2"<<std::endl;
    //CONSTRAINT 2
    fillConstraints(Aineq2,bineq2,false,true);
	for(unsigned i=0;i<CONST_SIZE;i++)
		bineq2(i)+=(SCALECONSTRAINT(i)* yconstr);
    af2.setA(Aineq2);
    af2.setb(bineq2);
    c2.setFunction(af2);

	//CONSTRAINT 3
    fillConstraints(Aineq3,bineq3,true,false);
    Aineq3.scalar_mult(-1);
    bineq3.scalar_mult(-1);
    for(unsigned i=0;i<CONST_SIZE;i++)
		bineq3(i)+=(SCALECONSTRAINT(i)*OurRobot.getWalkParameter(StepX)/2.0);

    af3.setA(Aineq3);
    af3.setb(bineq3);
    c3.setFunction(af3);

    fillConstraints(Aineq4,bineq4,false,true);
    Aineq4.scalar_mult(-1);
    bineq4.scalar_mult(-1);
    for(unsigned i=0;i<CONST_SIZE;i++)
		bineq4(i)+=(SCALECONSTRAINT(i)* yconstr);

	//std::cout<<"WHAT?set"<<std::endl;
	af4.setA(Aineq4);
    af4.setb(bineq4);
    c4.setFunction(af4);

	//std::cout<<"WHAT?add"<<std::endl;

    solver.addCostFunction(&costfunct);
    solver.addPenaltyFunction(&c1);
    solver.addPenaltyFunction(&c2);
    solver.addPenaltyFunction(&c3);
	solver.addPenaltyFunction(&c4);
    //std::cout<<"WHAT?assign"<<std::endl;

    //for(unsigned i=0;i<LagN;i++)
	//{
	//	htta(i)=httaX(i);
	//	htta(i+LagN)=httaY(i);
	//}
	//htta.prettyPrint();
	//af1.setX(htta);
	c1.setX(htta);
	c2.setX(htta);
	c3.setX(htta);
	c4.setX(htta);
	if(c1.allSatisfied()==false ||
		//c2.allSatisfied()==false ||
		c3.allSatisfied()==false)// ||
		//c4.allSatisfied()==false)
	std::cout<<"========================== VIOLATE BEFORE"<<std::endl;
    //af1.evaluate().prettyPrint();

	{
		KPROF_SCOPE(walkprof,"solver.solve");
		 htta=solver.solve(htta);

	}
	if(c1.allSatisfied()==false ||
		//c2.allSatisfied()==false ||
		c3.allSatisfied()==false )// ||
		//c4.allSatisfied()==false)
	std::cout<<"========================== VIOLATE AFTER"<<std::endl;

  //  af1.setX(htta);
  //  af1.evaluate().prettyPrint();
  //  htta.prettyPrint();
    //std::cout<<"WHAT?assignback"<<std::endl;
	for(unsigned i=0;i<LagN;i++)
	{
		httaX(i)=htta(i);
		httaY(i)=htta(i+LagN);
	}

	solver.clearCostFunctions();


}

void LIPMPreviewController::fillConstraints(
						KMath::KMat::GenMatrix<float,CONST_SIZE,2*LagN> & A,
						KMath::KMat::GenMatrix<float,CONST_SIZE,1> &b,
						bool ecos,bool fcos
						)
{
	for(unsigned i=0;i<CONST_SIZE;i++)
	{
		float a,c;
		if(ecos)
			a= cos(-ZMPtheta(i*CONST_STEP+CONST_SKIP));
		else
			a= sin(-ZMPtheta(i*CONST_STEP+CONST_SKIP));
		if(fcos)
			c= cos(-ZMPtheta(i*CONST_STEP+CONST_SKIP));
		else
			c= -sin(-ZMPtheta(i*CONST_STEP+CONST_SKIP));
		//std::cout<<c<<std::endl;

		for(unsigned j=0;j<LagN;j++)
		{
			A(i,j)=(a)*Phi(i*CONST_STEP+CONST_SKIP,j);
			A(i,j+LagN)=(c)*Phi(i*CONST_STEP+CONST_SKIP,j);
		}

		b(i)=(a)*pX(i*CONST_STEP+CONST_SKIP)+(c)*pY(i*CONST_STEP+CONST_SKIP);
	}

}
void LIPMPreviewController::generateLaguerre()
{

	float alpha=0.4;

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
/*
void LIPMPreviewController::DMPC()
{
	generateLaguerre();
	float rl=4e-6;




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
    c(2)=-(OurRobot.getWalkParameter(ComZ)*OurRobot.getWalkParameter(CoMZModelError))/OurRobot.getWalkParameter(g);

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

	Aineq1.zero();
	Aineq2.zero();
	Aineq3.zero();
	Aineq4.zero();

	bineq1.zero();
	bineq2.zero();
	bineq3.zero();
	bineq4.zero();

}
*/

void LIPMPreviewController::DMPC()
{
	float rl=4e-6;
	float ttl=(BASISM*(BASISM+1.0)*BASISD)/2.0;
	float s=floor((PreviewWindow-1.0-ttl)/(LagN-BASISM*BASISD));
	std::cout<<"    ---"<<s<<" "<<ttl<<std::endl;




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
    c(2)=-(OurRobot.getWalkParameter(ComZ)*OurRobot.getWalkParameter(CoMZModelError))/OurRobot.getWalkParameter(g);

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
    L0.zero();
    L0(0)=1;
	Tau.zero();
	tempSc.zero();
	Aepower.identity();

    for(unsigned i=0;i<PreviewWindow-1;i++)
	{

		//COMPUTE Li
		Li.zero();
		unsigned base;
		float denom=1;

		if(i+1<=ttl)
		{
			unsigned w=0;
			unsigned sum=0;
			for(unsigned l=0;l<BASISM;l++)
			{
				for(unsigned j=0;j<BASISD;j++)
				{
					sum+=(l+1);
					w=w+1;
					if(sum>=i+1)
					{
						denom=(l+1);
						break;
					}

				}
				if(sum>=i+1)
				{
					denom=(l+1);
					break;

				}
			}
			base=w-1;

		}
		else
		{
			base=ceil((i+1-ttl)/s)+BASISD*BASISM-1;
			denom=s;

		}
		if(base<LagN)
		{
			Li(base)=1.0/sqrt(denom);
		}
		std::cout<<i<<" "<<base<<" "<<1.0/sqrt(denom)<<std::endl;
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

	Aineq1.zero();
	Aineq2.zero();
	Aineq3.zero();
	Aineq4.zero();

	bineq1.zero();
	bineq2.zero();
	bineq3.zero();
	bineq4.zero();

}



#include "Kalman.h"

Kalman::Kalman(RobotParameters &robot):OurRobot(robot)
{
    /*Akalman(0,0)=1.000;
    Akalman(0,1)=0;//-OurRobot.getWalkParameter(Ts);
    Akalman(1,0)=0.000;
    Akalman(1,1)=1.000;*/
    Bkalman.zero();
    Bkalman(0)=1.000;//OurRobot.getWalkParameter(Ts);
    StateKalman.zero();
    ProcessNoise.zero();
	ProcessNoise(0,0)=5e-7;
	P.zero();
	P(0,0)=1e-21;
	MeasurementNoise.identity();
	MeasurementNoise(0,0)=0.001;
	MeasurementNoise(1,1)=0.2;
	Ckalman.zero();
	Ckalman(0,0)=1.000;
	Ckalman(1,0)=1.000;
	StatePredict.zero();

}

 void Kalman::Filter(float ZMPMeasured,float CoMMeasured){


	/** Kalman filter to  deal with Delay, Bias , Noise **/

        /** Predict **/

		StateKalman=StateKalman+Bkalman*uBuffer.front();

		float ct1=0.000,ct2=0.000;
		float zmpfromcom=0.000;

		bool doup=false ;
		if(fabs(ZMPMeasured)>1e-15)
		{
			P=P+ProcessNoise;
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

        /** Update **/
		//StateKalman.prettyPrint();
		if(doup)
		{
			ykalman=KVecFloat2(ZMPMeasured,zmpfromcom);
			ykalman+=(Ckalman*(StateKalman)).scalar_mult(-1.0);/** innovation value **/
			s=Ckalman*P*Ckalman.transp()+MeasurementNoise;
			s.fast_invert();
			Kgain=(P*Ckalman.transp())*s;
			StateKalman+=Kgain*ykalman;
			//StateKalman.prettyPrint();
		}

        //StateKalman.prettyPrint();

		Kgain.scalar_mult(-1.0);
		P+=Kgain*Ckalman*P;
		if(combuffer.size()>2)
			combuffer.pop();


		if(uBuffer.size()>ZMPKALMANDELAY)
			uBuffer.pop();



	StatePredict=StateKalman;
	unsigned bufsize=uBuffer.size();
	for(unsigned i=0;i<bufsize;i++)
	{
		StatePredict=StatePredict+Bkalman*uBuffer.front();
		uBuffer.push(uBuffer.front());
		uBuffer.pop();
	}
   /** ------------------------------------------------------------- **/
   }

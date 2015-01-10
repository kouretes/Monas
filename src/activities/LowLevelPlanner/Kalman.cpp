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
	ProcessNoise(0,0)=1e-5;
	P.zero();
	P(0,0)=1e-20;
	MeasurementNoise.identity();
	MeasurementNoise(0,0)=0.005;
	MeasurementNoise(1,1)=0.9;
	Ckalman.zero();
	Ckalman(0,0)=1.000;
	Ckalman(1,0)=1.000;
	StatePredict.zero();

}
/** Kalman filter to  deal with Delay, and  Noise **/
 void Kalman::Filter(float ZMPMeasured,float CoMMeasured){



        /** Predict **/

		StateKalman=StateKalman+Bkalman*uBuffer.front();
        /** Estimated ZMP from the COM measurement
		 * using the Cart and Table Model
		 **/
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


				float comddot=(CoMMeasured-2*ct1+ct2)/(OurRobot.getWalkParameter(Ts)*OurRobot.getWalkParameter(Ts) );
				zmpfromcom=CoMMeasured-(OurRobot.getWalkParameter(ComZ)/OurRobot.getWalkParameter(g))*comddot;
			}

		}

		combuffer.push(CoMMeasured);

        /** Update **/
		//StateKalman.prettyPrint();
		if(doup)
		{
			/** innovation value **/
			ykalman=KVecFloat2(ZMPMeasured,zmpfromcom);
			ykalman+=(Ckalman*(StateKalman)).scalar_mult(-1.0);

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


     /** Getting Rid of the ZMP delay **/
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

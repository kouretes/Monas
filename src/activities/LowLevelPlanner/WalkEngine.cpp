#include "WalkEngine.hpp"
#include "hal/robot/generic_nao/KinematicsDefines.h"

WalkEngine::WalkEngine(RobotParameters &rp) : NaoLIPM(rp),NaoRobot(rp),Zbuffer(PreviewWindow*50),walkbuffer(0)
{

	Tilerror.identity();
	Tirerror.identity();
	fsrlbias.zero();
	fsrrbias.zero();
	currentstep=0;
	ci.targetSupport=KDeviceLists::SUPPORT_LEG_NONE;
	supportleg = KDeviceLists::SUPPORT_LEG_NONE;

};

void WalkEngine::Reset()
{
	comzmeasured=0;
	ci.targetSupport=KDeviceLists::SUPPORT_LEG_NONE;
	currentstep=0;
	supportleg = KDeviceLists::SUPPORT_LEG_LEFT;
	chainsupport=KDeviceLists::CHAIN_L_LEG;
	Tsp=nkin.getForwardEffector((NAOKinematics::Effectors)KDeviceLists::CHAIN_L_LEG);

	NAOKinematics::FKvars v;
	v.p=Tsp.getTranslation();
	v.a=Tsp.getEulerAngles();
	v.p(0)=0;
    v.p(2)=0;

    v.a(0)=0;
    v.a(1)=0;
    Tis=NAOKinematics::getTransformation(v);
	Tsp.fast_invert();

    Tilold=Tis*Tsp*nkin.getForwardEffector((NAOKinematics::Effectors)KDeviceLists::CHAIN_L_LEG);
    Tirold=Tis*Tsp*nkin.getForwardEffector((NAOKinematics::Effectors)KDeviceLists::CHAIN_R_LEG);
	planL=getPositionInertial((NAOKinematics::Effectors)KDeviceLists::CHAIN_L_LEG);
	startL=planL;
	planR=getPositionInertial((NAOKinematics::Effectors)KDeviceLists::CHAIN_R_LEG);
	startR=planR;

	std::cout<<"Reset:"<<std::endl;

	planned.targetZMP=KDeviceLists::SUPPORT_LEG_NONE;
	planned.targetSupport=KDeviceLists::SUPPORT_LEG_NONE;
	predicterror.zero();

};
void WalkEngine::addInit()
{
	WalkInstruction i;
	i.targetSupport=KDeviceLists::SUPPORT_LEG_LEFT;
	i.targetZMP=KDeviceLists::SUPPORT_LEG_BOTH;
	i.steps=NaoRobot.getWalkParameter(Tinit)/NaoRobot.getWalkParameter(Ts);
	walkbuffer.add(i);
}

std::vector<float> WalkEngine::Calculate_IK()
{



	NAOKinematics::kmatTable Tip,Tpi,Til,Tir;
	/**  pelvis to inertial  **/

	Tip=Tis*Tsp;

	/** inertial to pelvis **/

	Tpi=Tip;
	Tpi.fast_invert();


    /** Get Transformation from left foot to Pelvis in local frame **/
	NAOKinematics::kmatTable l=nkin.getForwardEffector((NAOKinematics::Effectors)KDeviceLists::CHAIN_L_LEG);
    /** Get Transformation from right foot to Pelvis in local frame **/
	NAOKinematics::kmatTable r=nkin.getForwardEffector((NAOKinematics::Effectors)KDeviceLists::CHAIN_R_LEG);


    /** Get Transformation from left foot to inertial **/
	l=Tip*l;
	l.fast_invert();
	/** Get Transformation from right foot to inertial **/
	r=Tip*r;
	r.fast_invert();

    /** Get Transformation from OLD left foot to NEW left foot **/
	Tilerror=l*Tilold;
    /** Get Transformation from OLD right foot to NEW right foot **/
	Tirerror=r*Tirold;

    /** Correction with 10% of Error **/
	NAOKinematics::FKvars t;
	t.p=Tilerror.getTranslation();
	t.a=Tilerror.getEulerAngles();

	t.p.scalar_mult(0.1);
	if(t.p(2)<0)
		t.p(2)=0;
	t.a.scalar_mult(0.1);

	Tilerror=NAOKinematics::getTransformation(t);

	t.p=Tirerror.getTranslation();
	t.a=Tirerror.getEulerAngles();

	t.p.scalar_mult(0.1);
	if(t.p(2)<0)
		t.p(2)=0;

	t.a.scalar_mult(0.1);

    Tirerror=NAOKinematics::getTransformation(t);


	/** Interpolation **/

	KVecFloat3 dl=startL,dr=startR;
	if(ci.targetSupport==KDeviceLists::SUPPORT_LEG_LEFT&&double_support==false)
		dr=ci.target+predicterror;
	else if(ci.targetSupport==KDeviceLists::SUPPORT_LEG_RIGHT&&double_support==false)
		dl=ci.target+predicterror;

	float h=NaoRobot.getWalkParameter(StepZ);

    //float vel=NaoRobot.getWalkParameter(StepZ)*(1-cos((( ((float)currentstep)/ci.steps )*M_PI*2)))/2.0;
	float vel= interp.CubicSplineInterpolation( ((float)currentstep),0.0,h/4.0,h,h/4.0,0.0  ,ci.steps);
	float ldiff=KMath::anglediff2(dl(2),startL(2));
	float rdiff=KMath::anglediff2(dr(2),startR(2));

    dl(0)=interp.trigIntegInterpolation(((float)currentstep)/ci.steps,startL(0),dl(0),1.0);

    dl(1)=interp.trigIntegInterpolation(((float)currentstep)/ci.steps,startL(1),dl(1),1.0);

    dl(2)=startL(2)+interp.trigIntegInterpolation(((float)currentstep)/ci.steps,0,ldiff,1.0);

    dr(0)=interp.trigIntegInterpolation(((float)currentstep)/ci.steps,startR(0),dr(0),1.0);
    dr(1)=interp.trigIntegInterpolation(((float)currentstep)/ci.steps,startR(1),dr(1),1.0);
    dr(2)=startR(2)+interp.trigIntegInterpolation(((float)currentstep)/ci.steps,0,rdiff,1.0);

    float zl=0,zr=0;
    if(ci.targetSupport==KDeviceLists::SUPPORT_LEG_LEFT&&double_support==false)
		zr=vel;
	else if(ci.targetSupport==KDeviceLists::SUPPORT_LEG_RIGHT&&double_support==false)
		 zl=vel;

	Til=getTransformation(dl,zl);
	Tir=getTransformation(dr,zr);


	if(ci.targetSupport==KDeviceLists::SUPPORT_LEG_LEFT&&double_support==false)
		Til=Tis;
	else if(ci.targetSupport==KDeviceLists::SUPPORT_LEG_LEFT&&double_support==false)
		Tir=Tis;



	NAOKinematics::kmatTable Tpprimei,Tipprime; ///Transformation of next pelvis p'

	KVecDouble3 com_error,desired;///All in inertial frame;

	desired=KVecDouble3( NaoLIPM.COM(0),NaoLIPM.COM(1),NaoRobot.getWalkParameter(ComZ)).scalar_mult(1000);
//desired=KVecDouble3( NaoLIPMx.Com,NaoLIPMy.Com,NaoRobot.getWalkParameter(ComZ)).scalar_mult(1000);
	std::vector<float> ret;
	Tipprime=Tip;

	for(unsigned iter=0;iter<1;iter++)
	{

	    ret.clear();

		KVecDouble3 measured = nkin.calculateCenterOfMass();
		if(double_support)
			comzmeasured=comzmeasured*0.991+measured(2)*0.009;
		measured(2)=comzmeasured;
		com_error=desired;
		com_error-=Tipprime.transform(measured);

    /** Fix rotation first, using yawpitchroll coupling **/


		/**First generate Tipprime and then invert **/
       /** Generate Transformation from inertial to Pelvis' **/
		KMath::KMat::transformations::makeRotationXYZ(Tpprimei,
													0.0,
													0.0,
													(double)
													anglemean(dl(2),dr(2))
													);

		Tpprimei.setTranslation(com_error+Tipprime.getTranslation());
		/** Generate Transformation from Pelvis' to Inertial **/
		Tipprime=Tpprimei;
		Tpprimei.fast_invert();

        /** Generate the Inverse Kinematics Targets **/
		NAOKinematics::kmatTable Tpprimel,Tpprimer;

		Tpprimel=Tpprimei*Til*Tilerror;
		Tilold=Til*Tilerror;
		Tpprimer=Tpprimei*Tir*Tirerror;
		Tirold=Tir*Tirerror;


		std::vector<std::vector<float> > resultR, resultL;
		resultL = nkin.inverseLeftLeg(Tpprimel);
		resultR = nkin.inverseRightLeg(Tpprimer);
        /** Set Leg Kinematics Chains **/
		if (!resultL.empty())
		{
			ret = resultL.at(0);
			if (!resultR.empty())
			{
				ret.insert(ret.end(), resultR.at(0).begin(), resultR.at(0).end());
				nkin.setChain(KDeviceLists::CHAIN_L_LEG,resultL[0]);
				nkin.setChain(KDeviceLists::CHAIN_R_LEG,resultR[0]);
			}

            else
				std::cerr << "Right Leg EMPTY VECTOR " << std::endl;
		} else
			std::cerr << "Left Leg EMPTY VECTOR " << std::endl;

		armangles.zero();
		KVecDouble3 armt;
		armt=Tpprimel.getTranslation();
		//armt.prettyPrint();
		armangles(2)=asin((-armt(0)+NaoRobot.getWalkParameter(HX)*1000)/(UpperArmLength*1.5) )+M_PI_2;
		armangles(1)=asin((armt(1)+70-ShoulderOffsetY)/(UpperArmLength*1.5));
		armt=Tpprimer.getTranslation();
		//armt.prettyPrint();
		armangles(0)=asin((-armt(0)+NaoRobot.getWalkParameter(HX)*1000)/(UpperArmLength*1.5) )+M_PI_2;
		armangles(3)=asin((-armt(1)+70-ShoulderOffsetY)/(UpperArmLength*1.5) );
		//armangles.prettyPrint();


	}
	//X axes

	currentstep++;
	return ret;
}



void WalkEngine::Calculate_Desired_COM()
{


    /** Get Center of Pressure in Inertial Frame **/
	KVecFloat2 copi=getCoP();
	/** Get current Com in Inertial Frame **/
	KVecDouble3 CoMm =(Tis*Tsp).transform(nkin.calculateCenterOfMass());
	CoMm.scalar_mult(1.0/1000.0);
	//std::cout<<"PREDICTED ZMP ERROR"<<std::endl;
    /** Get Target Com in Inertial Frame **/

	NaoLIPM.LIPMComPredictor(Zbuffer,CoMm(0),CoMm(1),copi(0),copi(1));
	//NaoLIPMx.LIPMComPredictor(ZbufferX,CoMm(0),copi(0));
	//NaoLIPMy.LIPMComPredictor(ZbufferY,CoMm(1),copi(1));
/*	KVecFloat3 e(NaoLIPM.predictedErrorX,NaoLIPM.predictedErrorY,0);

	if(e(0)>NaoRobot.getWalkParameter(AdaptiveStepTolx) || e(1)>NaoRobot.getWalkParameter(AdaptiveStepToly))
	{
        predicterror.scalar_mult(0.35);
        e.scalar_mult(0.65);
	}
    else
    {
       predicterror.scalar_mult(0.9);
       e.scalar_mult(0.1);
    }

    predicterror+=e;
//    predicterror.prettyPrint();
*/
	/** Pop the used Point **/

	Zbuffer.pop();
}
void WalkEngine::setFSR(KMath::KMat::GenMatrix<double,4,1> l,KMath::KMat::GenMatrix<double,4,1> r)
{
	fsrl=l;
	fsrr=r;
}
KVecFloat2 WalkEngine::getCoP()
{

	KVecFloat2 res;
	KVecDouble3 copl,copr,copi,cops,copsprime;
	float weightl,weightr,weights, weightsprime;
    //fsrl.prettyPrint();
	weightl=fsrl(0)+fsrl(1)+fsrl(2)+fsrl(3);
	weightr=fsrr(0)+fsrr(1)+fsrr(2)+fsrr(3);

	copl=fsrposl*fsrl;
	copr=fsrposr*fsrr;

	copl.scalar_mult(1./(weightl));
	copr.scalar_mult(1./(weightr));

	if(weightl<0.05)
	{
		copl.zero();
		weightl=0;
		if(supportleg==KDeviceLists::SUPPORT_LEG_RIGHT&&double_support==false)
			fsrlbias+=fsrl*0.5;
	}

	if(weightr<0.05)
	{
		copr.zero();
		weightr=0;
		if(supportleg==KDeviceLists::SUPPORT_LEG_LEFT&&double_support==false)
			fsrrbias+=fsrr*0.5;
	}


#define Margin 0.002

	if(weightl==0 || weightr==0)
	{
		if(copl(1)>=fsrposl(0,1)-Margin || copl(1)<= fsrposl(1,1)+Margin || copl(0)>=fsrposl(0,0)-Margin ||copl(0)<=fsrposl(2,0)+Margin)
		{
			weightl=0;
			copl.zero();
		}


		if(copr(1)>=fsrposr(0,1) -Margin|| copr(1)<= fsrposr(1,1)+Margin || copr(0)>=fsrposr(0,0)-Margin ||copr(0)<=fsrposr(2,0)+Margin)
		{
			weightr=0;
			copr.zero();
		}

	}

	copl.scalar_mult(1000);
	copr.scalar_mult(1000);

	if(supportleg==KDeviceLists::SUPPORT_LEG_RIGHT)
	{
		cops=copr;
		copsprime=copl;
		weights=weightr;
		weightsprime=weightl;
	}
	else
	{
		cops=copl;
		copsprime=copr;
		weights=weightl;
		weightsprime=weightr;
	}


    copi=(Tis.transform(cops)).scalar_mult(weights)+ ((Tis*Tssprime).transform(copsprime)).scalar_mult(weightsprime);
    copi.scalar_mult(1.0/(weights+weightsprime));


    copi.scalar_mult(1.0/1000.0);

    if((weights+weightsprime)<1)
		copi.zero();

	res(0)=copi(0);
	res(1)=copi(1);
	//res.prettyPrint();

	return res;
}
void WalkEngine::feed()
{


	//Prepare ZMP buffer
	if(walkbuffer.size()==0)
	{

		addInit();
	}


    if(Zbuffer.size()<PreviewWindow+1&&walkbuffer.size()>0)//ASSUME Y is the same
	{
		WalkInstruction i=walkbuffer.readOne();
				//Add double support
		//std::cout<<"read"<<walkbuffer.size()<<ZbufferX.size()<<std::endl;
		if(planned.targetZMP!=i.targetSupport&&
		   i.targetZMP!=KDeviceLists::SUPPORT_LEG_BOTH&&
		   planned.targetSupport!=KDeviceLists::SUPPORT_LEG_NONE)
		{
			i.targetZMP=i.targetSupport;
			i.steps=NaoRobot.getWalkParameter(Tds)/NaoRobot.getWalkParameter(Ts);

		}
		else
		{
			walkbuffer.removeOne();
		}


		if(i.targetSupport==KDeviceLists::SUPPORT_LEG_LEFT&&i.targetZMP!=KDeviceLists::SUPPORT_LEG_BOTH)
			planR=i.target;
		else if(i.targetSupport==KDeviceLists::SUPPORT_LEG_RIGHT&&i.targetZMP!=KDeviceLists::SUPPORT_LEG_BOTH)
			planL=i.target;
		KVecFloat3 destZMP;
		/*if(i.targetSupport!=planned.targetZMP||i.targetZMP==KDeviceLists::SUPPORT_LEG_BOTH)
		{
			std::cout<<"Fixedlegs"<<std::endl;
			planL=getPositionInertial((NAOKinematics::Effectors)KDeviceLists::CHAIN_L_LEG);
			planR=getPositionInertial((NAOKinematics::Effectors)KDeviceLists::CHAIN_R_LEG);
		}*/

		if(i.targetZMP==KDeviceLists::SUPPORT_LEG_RIGHT)
			destZMP=planR;
		else if(i.targetZMP==KDeviceLists::SUPPORT_LEG_LEFT)
			destZMP=planL;
		else
		{
			destZMP=planL;
			destZMP+=planR;
			destZMP.scalar_mult(0.5);

			destZMP(2)=anglemean(planL(2),planR(2));
		}

		KMath::KMat::GenMatrix<float,2,2> rot;
		KMath::KMat::transformations::makeRotation(rot,(float)destZMP(2));
		KVecFloat2 rr=rot*KVecFloat2(-NaoRobot.getWalkParameter(HX),0.0);
		destZMP(0)+=rr(0);
		destZMP(1)+=rr(1);
		std::cout<<"Plan"<<std::endl;
		//planL.prettyPrint();
		//planR.prettyPrint();
		//destZMP.prettyPrint();
		KVecFloat3 startZMP;
        if(Zbuffer.size()>0)
        {
        	Zbuffer[Zbuffer.size()-1].prettyPrint();
            startZMP(0)=Zbuffer[Zbuffer.size()-1](0);
            startZMP(1)=Zbuffer[Zbuffer.size()-1](1);
            startZMP(2)=Zbuffer[Zbuffer.size()-1](2);
            startZMP.prettyPrint();
        }
        else
            startZMP.zero();

		for(unsigned p=0;p<i.steps;p++)
		{
				KVecFloat3 newpoint;
				newpoint.zero();
				float adiff=KMath::anglediff2(destZMP(2),startZMP(2));

				newpoint(0)=interp.trigIntegInterpolation(((float)p)/i.steps,startZMP(0),destZMP(0),1.0);
				newpoint(1)=interp.trigIntegInterpolation(((float)p)/i.steps,startZMP(1),destZMP(1),1.0);
				newpoint(2)=startZMP(2)+interp.trigIntegInterpolation(((float)p)/i.steps,0,adiff,1.0);
				Zbuffer.cbPush(newpoint);
				//newpoint.prettyPrint();
		}
		planned=i;
		qbuffer.push(i);
	}

 /** EXECUTION PHASE **/

	if((ci.targetSupport==KDeviceLists::SUPPORT_LEG_NONE||currentstep==ci.steps))
	{

		WalkInstruction old=ci;

		ci=qbuffer.front();

		qbuffer.pop();

		if(ci.targetSupport!=supportleg||
			ci.targetZMP==KDeviceLists::SUPPORT_LEG_BOTH||
			old.targetZMP==KDeviceLists::SUPPORT_LEG_BOTH) //double support
		{
			double_support=true;

		}
		else
			double_support=false;
		currentstep=0;

	}




};

std::vector<float> WalkEngine::runStep()
{



	feed();//PrepareBuffers, and target stuff;



		//Get Tps


	KDeviceLists::SupportLeg oldsupportleg=supportleg;


	double_support_progress=0;
	if(double_support)
		double_support_progress=((float)currentstep)/ci.steps;
	//std::cout<<"Doublesupportprogress:"<<double_support_progress<<std::endl;
	supportleg=ci.targetSupport;
	bool rightsupport=supportleg==KDeviceLists::SUPPORT_LEG_RIGHT;
	chainsupport= (rightsupport==true)?KDeviceLists::CHAIN_R_LEG:KDeviceLists::CHAIN_L_LEG;

	Tsp=nkin.getForwardEffector((NAOKinematics::Effectors)chainsupport);
	Tsp.fast_invert();//Tps->Tsp

	KDeviceLists::ChainsNames otherleg= (rightsupport==true)?
										KDeviceLists::CHAIN_L_LEG:KDeviceLists::CHAIN_R_LEG;
	NAOKinematics::kmatTable merger=nkin.getForwardFromTo(
															(NAOKinematics::Effectors)chainsupport,
															(NAOKinematics::Effectors)otherleg
														  );


	NAOKinematics::FKvars t;
	Tssprime=merger;



	if(oldsupportleg!=supportleg )
	{
		NAOKinematics::kmatTable a=Tssprime;
		a.fast_invert();


		std::cout<<"SWITCH LEG----"<<std::endl;
		Tis*=a;




	}

	if(double_support==false)
	{
		t.p=Tis.getTranslation();
		t.a=Tis.getEulerAngles();
		t.p(2)*=0.1;
		t.a(0)*=0.1;
		t.a(1)*=0.1;
		Tis=NAOKinematics::getTransformation(t);
	}

    /** Beginning of command to be executed **/
	if(currentstep==0)
	{
		startL=getPositionInertial((NAOKinematics::Effectors)KDeviceLists::CHAIN_L_LEG);
		startR=getPositionInertial((NAOKinematics::Effectors)KDeviceLists::CHAIN_R_LEG);
	}

	Calculate_Desired_COM();

	return  Calculate_IK();

}



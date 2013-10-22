#include "WalkEngine.hpp"


WalkEngine::WalkEngine(RobotParameters rp) : NaoLIPMx(rp),NaoLIPMy(rp),NaoRobot(rp),ZbufferX(PreviewWindow*50),ZbufferY(PreviewWindow*50),walkbuffer(0)
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
	planL.prettyPrint();
	planR.prettyPrint();
	Tis.prettyPrint();
	planned.targetZMP=KDeviceLists::SUPPORT_LEG_NONE;
	planned.targetSupport=KDeviceLists::SUPPORT_LEG_NONE;

};
void WalkEngine::addInit()
{
	WalkInstruction i;
	i.targetSupport=KDeviceLists::SUPPORT_LEG_LEFT;
	i.targetZMP=KDeviceLists::SUPPORT_LEG_BOTH;
	i.steps=1.0/NaoRobot.getWalkParameter(Ts);
	walkbuffer.add(i);
}

std::vector<float> WalkEngine::Calculate_IK()
{



	NAOKinematics::kmatTable Tip,Tpi,Til,Tir;
	Tip=Tis*Tsp;

	Tpi=Tip;
	Tpi.fast_invert();//Get Inverse transform



	NAOKinematics::kmatTable l=nkin.getForwardEffector((NAOKinematics::Effectors)KDeviceLists::CHAIN_L_LEG);
	NAOKinematics::kmatTable r=nkin.getForwardEffector((NAOKinematics::Effectors)KDeviceLists::CHAIN_R_LEG);

	l=Tip*l;
	l.fast_invert();
	r=Tip*r;
	r.fast_invert();
	//if(supportleg!=KDeviceLists::SUPPORT_LEG_LEFT&&double_support==false)
	Tilerror=l*Tilold;
	//if(supportleg!=KDeviceLists::SUPPORT_LEG_RIGHT&&double_support==false)
	Tirerror=r*Tirold;


	NAOKinematics::FKvars t;
	t.p=Tilerror.getTranslation();
	t.a=Tilerror.getEulerAngles();

	t.p.scalar_mult(0.15);
	if(t.p(2)<0)
		t.p(2)=0;
	t.a.scalar_mult(0.25);
	//if(supportleg!=KDeviceLists::SUPPORT_LEG_LEFT&&double_support==false)
		Tilerror=NAOKinematics::getTransformation(t);

	t.p=Tirerror.getTranslation();
	t.a=Tirerror.getEulerAngles();

	t.p.scalar_mult(0.15);
	if(t.p(2)<0)
		t.p(2)=0;

	t.a.scalar_mult(0.25);
	//if(supportleg!=KDeviceLists::SUPPORT_LEG_RIGHT&&double_support==false)
		Tirerror=NAOKinematics::getTransformation(t);

	/*
	std::cout<<"Tip:"<<std::endl;
	Tip.prettyPrint();
	Tpi.prettyPrint();*/
	//Construct Ti{l,r}
	KVecFloat3 dl=startL,dr=startR;
	if(ci.targetSupport==KDeviceLists::SUPPORT_LEG_LEFT&&double_support==false)
		dr=ci.target;
	else if(ci.targetSupport==KDeviceLists::SUPPORT_LEG_RIGHT&&double_support==false)
		dl=ci.target;
	//dl.prettyPrint();
	float vel=(1-cos(( ( ((float)currentstep)/ci.steps )*M_PI*2)))/2.0;
	float ldiff=KMath::anglediff2(dl(2),startL(2));
	float rdiff=KMath::anglediff2(dr(2),startR(2));
    dl(0)=interp.trigIntegInterpolation(((float)currentstep)/ci.steps,startL(0),dl(0),1.0);
    dl(1)=interp.trigIntegInterpolation(((float)currentstep)/ci.steps,startL(1),dl(1),1.0);
    dl(2)=startL(2)+interp.trigIntegInterpolation(((float)currentstep)/ci.steps,0,ldiff,1.0);
    //dl(2)=KMath::wrapTo0_2Pi(dl(2));

    dr(0)=interp.trigIntegInterpolation(((float)currentstep)/ci.steps,startR(0),dr(0),1.0);
    dr(1)=interp.trigIntegInterpolation(((float)currentstep)/ci.steps,startR(1),dr(1),1.0);
    dr(2)=startR(2)+interp.trigIntegInterpolation(((float)currentstep)/ci.steps,0,rdiff,1.0);
    //getTransformation(dr,0).prettyPrint();
	//dr(2)=KMath::wrapTo0_2Pi(dr(2));
	//getTransformation(dr,0).prettyPrint();

    //dl.prettyPrint();
    //dr.prettyPrint();
    float zl=0,zr=0;
    if(ci.targetSupport==KDeviceLists::SUPPORT_LEG_LEFT&&double_support==false)
		zr=NaoRobot.getWalkParameter(StepZ)*vel;
	else if(ci.targetSupport==KDeviceLists::SUPPORT_LEG_RIGHT&&double_support==false)
		zl=NaoRobot.getWalkParameter(StepZ)*vel;
	Til=getTransformation(dl,zl);
	Tir=getTransformation(dr,zr);


	if(ci.targetSupport==KDeviceLists::SUPPORT_LEG_LEFT&&double_support==false)
		Til=Tis;
	else if(ci.targetSupport==KDeviceLists::SUPPORT_LEG_LEFT&&double_support==false)
		Tir=Tis;
	//Til.prettyPrint();
	//Tir.prettyPrint();
	//Fix current support leg....


	NAOKinematics::kmatTable Tpprimei,Tipprime; //Transformation of next pelvis p' pprime :)

	/*Til.getTranslation().prettyPrint();
	Tir.getTranslation().prettyPrint();*/
	KVecDouble3 com_error,desired;//All in inertial frame;
	//std::cout<<NaoRobot.getWalkParameter(ComZ)<<std::endl;
	desired=KVecDouble3( NaoLIPMx.Com,NaoLIPMy.Com,NaoRobot.getWalkParameter(ComZ)).scalar_mult(1000);
	//desired(0)=0;
	//desired(1)=0;
	//Til.getTranslation().prettyPrint();
	//.prettyPrint();
	//Tis.getTranslation().prettyPrint();
	//desired.prettyPrint();
	std::vector<float> ret;
	Tipprime=Tip;

	for(unsigned iter=0;iter<1;iter++)
	{

		KVecDouble3 measured = nkin.calculateCenterOfMass();
		if(double_support)
			comzmeasured=comzmeasured*0.991+measured(2)*0.009;
		measured(2)=comzmeasured;
		com_error=desired;
		com_error-=Tipprime.transform(measured);




		//comzerror=com_error(2)*0.05+comzerror*0.95;
		//com_error(2)=comzerror;
		//if(double_support)
			//com_error(2)*=(vel+0.5);
		//else
			//com_error(2)*=0.1;

		/*com_error(2)*=corr;
		if((com_error(2))>5)
			com_error(2)=5;
		else if((com_error(2))<-1)
			com_error(2)=-1;
		*/

		//if(double_support==false)
			//com_error(2)*=0.99;
		//com_error(2)*=0.1;

		//com_error.scalar_mult(0.001);
		//com_error(2)/=2;
		//com_error.scalar_mult(-1);
		//com_error.scalar_mult(0.5);
		//com_error(2)*=2;
		//com_error.prettyPrint();
		//com_error.zero();
		//com_error(0)*=0.6;
		//com_error.scalar_mult(0.999);
		//com_error.scalar_mult(1.0/100);
		//Fix rotation first, using yawpitchroll coupling
		//First generate Tipprime and then invert
		KMath::KMat::transformations::makeRotationXYZ(Tpprimei,
													0.0,
													0.0,
													(double)
													anglemean(dl(2),dr(2))
													);

		//Tpprimei.identity();
		//(com_error+Tipprime.getTranslation()).prettyPrint();
		Tpprimei.setTranslation(com_error+Tipprime.getTranslation());
		//Tpprimei(2,3)=270;
		Tipprime=Tpprimei;
		//Tipprime.prettyPrint();

		Tpprimei.fast_invert(); // Tip'->Tp'i
		//Generate inverse kin targets as Tp'{l,r}
		//Tpprimei.prettyPrint();

		NAOKinematics::kmatTable Tpprimel,Tpprimer;

		Tpprimel=Tpprimei*Til*Tilerror;
		Tilold=Til*Tilerror;
		Tpprimer=Tpprimei*Tir*Tirerror;
		Tirold=Tir*Tirerror;

		//Tpprimel.prettyPrint();
		//Tpprimer.prettyPrint();

		std::vector<std::vector<float> > resultR, resultL;
		resultL = nkin.inverseLeftLeg(Tpprimel);
		resultR = nkin.inverseRightLeg(Tpprimer);


		if (!resultL.empty())
		{
			ret = resultL.at(0);
			if (!resultR.empty())
			{
				ret.insert(ret.end(), resultR.at(0).begin(), resultR.at(0).end());
				nkin.setChain(KDeviceLists::CHAIN_L_LEG,resultL[0]);
				nkin.setChain(KDeviceLists::CHAIN_R_LEG,resultR[0]);
			}

			//else
				//std::cerr << "Right Leg EMPTY VECTOR " << std::endl;
		} //else
			//std::cerr << "Left Leg EMPTY VECTOR " << std::endl;


	}
	currentstep++;
	//ret.clear();
	return ret;
}



void WalkEngine::Calculate_Desired_COM()
{



	KVecFloat2 copi=getCoP();
	//copi.prettyPrint();
	KVecDouble3 CoMm =(Tis*Tsp).transform(nkin.calculateCenterOfMass());// (Tis*t).transform();
	CoMm.scalar_mult(1.0/1000.0);

	NaoLIPMx.LIPMComPredictor(ZbufferX,CoMm(0),copi(0));

	NaoLIPMy.LIPMComPredictor(ZbufferY,CoMm(1),copi(1));
	ZbufferX.pop();
	ZbufferY.pop();

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


	fsrl-=fsrlbias;
	fsrr-=fsrrbias;
	/*fsrl.prettyPrint();
	fsrr.prettyPrint();*/
	/*fsrposl.prettyPrint();
	fsrposr.prettyPrint();*/
	//weightl=sqrt(fsrl.norm2());
	//weightr=sqrt(fsrr.norm2());

	weightl=fsrl(0)+fsrl(1)+fsrl(2)+fsrl(3);
	weightr=fsrr(0)+fsrr(1)+fsrr(2)+fsrr(3);
	//fsrposr=fsrposl;
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
	/*fsrlbias.prettyPrint();
	fsrrbias.prettyPrint();*/
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

	/*copl.prettyPrint();
	copr.prettyPrint();*/

	/*copr.prettyPrint();
	copl.prettyPrint();*/
	if(chainsupport==KDeviceLists::CHAIN_R_LEG)
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



	//std::cout<<"-----------"<<std::endl;
	/*cops.prettyPrint();
	copsprime.prettyPrint();
	std::cout<<weights<<" "<<weightsprime<<std::endl; */
    //copi=(Tis.transform(cops)).scalar_mult(weights)+ ((Tis*Tssprime).transform(copsprime)).scalar_mult(weightsprime);
    copi=(Tis.transform(cops)).scalar_mult(weights)+ ((Tis*Tssprime).transform(copsprime)).scalar_mult(weightsprime);
    copi.scalar_mult(1./(weights+weightsprime));


    copi.scalar_mult(1.0/1000.0);
    //std::cout<<"COP"<<std::endl;
    //copi.prettyPrint();
    if((weights+weightsprime)<1)
		copi.zero();

	res(0)=copi(0);
	res(1)=copi(1);
	//res.prettyPrint();

	return res;
}
void WalkEngine::feed()
{



	if(ZbufferX.size()==0)
	{
		ZbufferX.cbPush(0);
		ZbufferY.cbPush(0);
	}
	//Prepare ZMP buffer
	if(walkbuffer.size()==0)
	{

		addInit();
	}


    if(ZbufferX.size()<PreviewWindow+1&&walkbuffer.size()>0)//ASSUME Y is the same
	{
		WalkInstruction i=walkbuffer.readOne();
				//Add double support
		std::cout<<"read"<<walkbuffer.size()<<ZbufferX.size()<<std::endl;
		if(planned.targetZMP!=i.targetSupport&&
		   i.targetZMP!=KDeviceLists::SUPPORT_LEG_BOTH&&
		  //i.targetSupport!=KDeviceLists::SUPPORT_LEG_BOTH&&
		   planned.targetSupport!=KDeviceLists::SUPPORT_LEG_NONE)
		{
			std::cout<<"here"<<std::endl;
			i.targetZMP=i.targetSupport;
			i.target.prettyPrint();
			i.steps=NaoRobot.getWalkParameter(Tds)/NaoRobot.getWalkParameter(Ts);

		}
		else
		{
			std::cout<<"read:"<<walkbuffer.size()<<std::endl;
			walkbuffer.removeOne();
			std::cout<<"read:"<<walkbuffer.size()<<std::endl;
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

		if(i.targetZMP==KDeviceLists::SUPPORT_LEG_RIGHT&&i.targetZMP!=KDeviceLists::SUPPORT_LEG_BOTH)
			destZMP=planR;
		else if(i.targetZMP==KDeviceLists::SUPPORT_LEG_LEFT&&i.targetZMP!=KDeviceLists::SUPPORT_LEG_BOTH)
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
		KVecFloat2 rr=rot*KVecFloat2(-NaoRobot.getWalkParameter(HX),0);
		destZMP(0)+=rr(0);
		destZMP(1)+=rr(1);
		KVecFloat2 startZMP;

		startZMP(0)=ZbufferX[ZbufferX.size()-1];
		startZMP(1)=ZbufferY[ZbufferY.size()-1];
		std::cout<<"Plan:"<<std::endl;
		i.target.prettyPrint();
		planL.prettyPrint();
		planR.prettyPrint();
		startZMP.prettyPrint();
		destZMP.prettyPrint();
		for(unsigned p=0;p<i.steps;p++)
		{
				ZbufferX.cbPush(interp.trigIntegInterpolation(((float)p)/i.steps,startZMP(0),destZMP(0),1.0));//+0.02
				ZbufferY.cbPush(interp.trigIntegInterpolation(((float)p)/i.steps,startZMP(1),destZMP(1),1.0)  );
		}
		planned=i;
		qbuffer.push(i);
	}

	if((ci.targetSupport==KDeviceLists::SUPPORT_LEG_NONE||currentstep==ci.steps))//&&qbuffer.size()>0)
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

		std::cout<<"double:"<<double_support<<std::endl;

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
		t.p(2)*=0.8;
		t.a(0)*=0.8;
		t.a(1)*=0.8;
		Tis=NAOKinematics::getTransformation(t);
	}


	if(currentstep==0)//start
	{
		startL=getPositionInertial((NAOKinematics::Effectors)KDeviceLists::CHAIN_L_LEG);
		startR=getPositionInertial((NAOKinematics::Effectors)KDeviceLists::CHAIN_R_LEG);
	}




	/*std::cout<<"TIS:::"<<std::endl;
	Tis.prettyPrint();
	std::cout<<"S:::"<<std::endl;
	startL.prettyPrint();
	startR.prettyPrint();
	*/

	//Tis.getTranslation().prettyPrint();



	//std::cout << dcm_counter << " " << current_buffer << " " << dcm_length[current_buffer] << std::endl;
	Calculate_Desired_COM();




	//std::cout<<"Sup:"<<whichleg<<","<<nextleg<<std::endl;

	return  Calculate_IK();


}



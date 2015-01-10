#include "WalkEngine.hpp"
#include "hal/robot/nao/generic_nao/KinematicsDefines.h"


WalkEngine::WalkEngine(RobotParameters &rp) : NaoLIPM(rp),NaoRobot(rp),Zbuffer(PreviewWindow*50),walkbuffer(0)
{

	/** Initializing the Transformation matrices **/
	Tilerror.identity();
	Tirerror.identity();
	Tir.identity();
	Til.identity();
	Tirold.identity();
	Tilold.identity();
	Tis.identity();
	Tsp.identity();
	Tssprime.identity();
	/** WalkEngine Step **/
	currentstep=0;
	/** Instruction **/
	ci.targetSupport=KDeviceLists::SUPPORT_LEG_NONE;
	supportleg = KDeviceLists::SUPPORT_LEG_NONE;

}

void WalkEngine::Reset()
{

	measuredcom.zero();

	ci.targetSupport=KDeviceLists::SUPPORT_LEG_NONE;
	currentstep=0;

	/** Randomly picking the support leg **/
	supportleg = KDeviceLists::SUPPORT_LEG_LEFT;
	chainsupport=KDeviceLists::CHAIN_L_LEG;

	/** Transformation from Support Leg to Pelvis  **/
	Tsp=nkin.getForwardEffector((NAOKinematics::Effectors)KDeviceLists::CHAIN_L_LEG);

	/** Kinematics Instances **/
	NAOKinematics::FKvars v;

	/** Transformation from Support Leg  to Inertial Frame of Reference **/
	v.p=Tsp.getTranslation();
	v.a=Tsp.getEulerAngles();
	v.p(0)=0;
    v.p(2)=0;

    v.a(0)=0;
    v.a(1)=0;
    Tis=NAOKinematics::getTransformation(v);

	/** Transformation from Pelvis to Support Leg **/
	Tsp.fast_invert();

	/** Transformation from Left Leg to Inertial Frame of Reference **/
    Tilold=Tis*Tsp*nkin.getForwardEffector((NAOKinematics::Effectors)KDeviceLists::CHAIN_L_LEG);

	/** Transformation from Right Leg to Inertial Frame of Reference **/
    Tirold=Tis*Tsp*nkin.getForwardEffector((NAOKinematics::Effectors)KDeviceLists::CHAIN_R_LEG);

	/** Compute Reference Points in the x-y plane with orientation about the vertical axis z **/
	planL=getPositionInertial((NAOKinematics::Effectors)KDeviceLists::CHAIN_L_LEG);
	startL=planL;
	planR=getPositionInertial((NAOKinematics::Effectors)KDeviceLists::CHAIN_R_LEG);
	startR=planR;
	dl=startL;
	dr=startR;

	std::cout<<"Walkengine Resetted"<<std::endl;

	/** Reference ZMP Point **/
	planned.targetZMP=KDeviceLists::SUPPORT_LEG_NONE;
	planned.targetSupport=KDeviceLists::SUPPORT_LEG_NONE;

	predicterror.zero();

};


/**
* This function adds the Initial Walking Instruction to the
* walking buffer for execution
**/

void WalkEngine::addInit()
{
	/** Initial Walking Instruction **/
	WalkInstruction i;

	/** Randomly chosen Left Leg as Initial Support Leg **/
	i.targetSupport=KDeviceLists::SUPPORT_LEG_LEFT;

	/** ZMP in the Middle of Convex Hull **/
	i.targetZMP=KDeviceLists::SUPPORT_LEG_BOTH;

	/** Number of Discrete Time steps of the Initial Walking Instruction **/
	i.steps=NaoRobot.getWalkParameter(Tinit)/NaoRobot.getWalkParameter(Ts);

	/** Adding the Walking Instruction to the Walking Buffer for Execution **/
	walkbuffer.add(i);
	std::cout<<"Initial Walking Instruction added to Walking Command buffer"<<std::endl;

}
/**
* This function computes the joint angles that the actuators must be set to
**/
std::vector<float> WalkEngine::Calculate_IK()
{


	/** Kinematics Instances **/
	NAOKinematics::kmatTable Tip,Tpi;

	/**  Transformation from Pelvis to Inertial Frame of Reference  **/
	Tip=Tis*Tsp;

	/** Transformation from Inertial Frame of Reference to Pelvis **/
	Tpi=Tip;
	Tpi.fast_invert();

    /**  Transformation from Left Leg to Pelvis in local frame **/
	NAOKinematics::kmatTable l=nkin.getForwardEffector((NAOKinematics::Effectors)KDeviceLists::CHAIN_L_LEG);

	/**  Transformation from Right Leg to Pelvis in local frame **/
	NAOKinematics::kmatTable r=nkin.getForwardEffector((NAOKinematics::Effectors)KDeviceLists::CHAIN_R_LEG);


    /**  Transformation from Inertial Frame of Reference to Left Leg  **/
	l=Tip*l;
	l.fast_invert();

	/**  Transformation from Inertial Frame of Reference to Right Leg  **/
	r = Tip*r;
	r.fast_invert();

    /** Error Transformation matrix for Left Leg **/
	Tilerror=l*Tilold;

    /** Error Transformation matrix for Right Leg **/
	Tirerror=r*Tirold;

    /** Correction with 5% of Error **/
	NAOKinematics::FKvars t;
	t.p=Tilerror.getTranslation();
	t.a=Tilerror.getEulerAngles();
	t.p.scalar_mult(0.05);
	//if(t.p(2)<0)
	//	t.p(2)=0;
	t.a.scalar_mult(0.05);
	Tilerror=NAOKinematics::getTransformation(t);

	t.p=Tirerror.getTranslation();
	t.a=Tirerror.getEulerAngles();
	t.p.scalar_mult(0.05);
	//if(t.p(2)<0)
	//	t.p(2)=0;
	t.a.scalar_mult(0.05);
    Tirerror=NAOKinematics::getTransformation(t);
    //Tilerror.identity();
    //Tirerror.identity();

	/** Feet Trajectory Interpolation and online Correction with Predicted ZMP Error **/
	dl=startL;
	dr=startR;
	if(ci.targetSupport==KDeviceLists::SUPPORT_LEG_LEFT&&double_support==false)
		dr=ci.target+predicterror;
	else if(ci.targetSupport==KDeviceLists::SUPPORT_LEG_RIGHT&&double_support==false)
		dl=ci.target+predicterror;

	float h=(NaoRobot.getWalkParameter(StepZ)+ci.ttlspeed*0.4);
	float vel= h* interp.BezierZ( ((float)currentstep) /ci.steps);
	float ldiff=KMath::anglediff2(dl(2),startL(2));
	float rdiff=KMath::anglediff2(dr(2),startR(2));

    dl(0)=interp.ManosBezierLinearInterpolation(((float)currentstep)/ci.steps,startL(0),dl(0),1.0);
    dl(1)=interp.ManosBezierLinearInterpolation(((float)currentstep)/ci.steps,startL(1),dl(1),1.0);
    dl(2)=startL(2)+interp.ManosBezierLinearInterpolation(((float)currentstep)/ci.steps,0,ldiff,1.0);

    dr(0)=interp.ManosBezierLinearInterpolation(((float)currentstep)/ci.steps,startR(0),dr(0),1.0);
    dr(1)=interp.ManosBezierLinearInterpolation(((float)currentstep)/ci.steps,startR(1),dr(1),1.0);
    dr(2)=startR(2)+interp.ManosBezierLinearInterpolation(((float)currentstep)/ci.steps,0,rdiff,1.0);


    float zl=0, zr=0;
    if(ci.targetSupport==KDeviceLists::SUPPORT_LEG_LEFT&&double_support==false)
		zr=vel;
	else if(ci.targetSupport==KDeviceLists::SUPPORT_LEG_RIGHT&&double_support==false)
		zl=vel;

	Til=getTransformation(dl,zl);
	Tir=getTransformation(dr,zr);


	/*if(ci.targetSupport==KDeviceLists::SUPPORT_LEG_LEFT&&double_support==false)
		Til=Tis;
	else if(ci.targetSupport==KDeviceLists::SUPPORT_LEG_LEFT&&double_support==false)
		Tir=Tis;*/

    /*if(ci.targetSupport==KDeviceLists::SUPPORT_LEG_LEFT&&double_support==false)
		Tis=Til;
	else if(ci.targetSupport==KDeviceLists::SUPPORT_LEG_LEFT&&double_support==false)
		Tis=Tir;*/




	NAOKinematics::kmatTable Tpprimei,Tipprime; /** Transformation of next pelvis p' **/

	KVecDouble3 com_error,desired;	/** All in inertial frame **/

	desired=KVecDouble3( NaoLIPM.COM(0),NaoLIPM.COM(1),NaoRobot.getWalkParameter(ComZ)).scalar_mult(1000);

	std::vector<float> ret;
	Tipprime=Tip;

	for(unsigned iter=0;iter<1;iter++)
	{

	    ret.clear();
	    /** COM smoothing **/
	    measuredcom=nkin.calculateCenterOfMass();
        if(double_support)
		    measuredcom =measuredcom*0.99+ nkin.calculateCenterOfMass()*0.01;
        else
            measuredcom =measuredcom*0.999+ nkin.calculateCenterOfMass()*0.001;

        /** Computing the COM error **/
		com_error=desired;
		com_error-=Tipprime.transform(measuredcom);

		if(com_error(2)>20)
			com_error(2)=20;


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



		/** Actuation Error Correction **/
		Tpprimel=Tpprimei*Til;//*Tilerror;
		Tilold=Til*Tilerror;
		Tpprimer=Tpprimei*Tir;//*Tirerror;
		Tirold=Tir*Tirerror;

		/** Joint Vectors obtained from the Inverse Kinematics **/
		std::vector<std::vector<float> > resultR, resultL;


		/** Inverse Kinematics Procedure   **/
		resultL = nkin.inverseLeftLeg(Tpprimel);
		resultR = nkin.inverseRightLeg(Tpprimer);

		/** Set Leg Kinematics Chains **/
		if (!resultL.empty())
		{

			if (!resultR.empty())
			{

				nkin.setChain(KDeviceLists::CHAIN_L_LEG,resultL[0]);
				nkin.setChain(KDeviceLists::CHAIN_R_LEG,resultR[0]);
				ret = resultL.at(0);
				ret.insert(ret.end(), resultR.at(0).begin(), resultR.at(0).end());
			}

            else
				std::cerr << "Right Leg EMPTY VECTOR " << std::endl;
		} else
			std::cerr << "Left Leg EMPTY VECTOR " << std::endl;

		/*Tpprimel.fast_invert();
        Tpprimer.fast_invert();
        (Tpprimel*nkin.getForwardEffector((NAOKinematics::Effectors)KDeviceLists::CHAIN_L_LEG)).prettyPrint();
        (Tpprimer*nkin.getForwardEffector((NAOKinematics::Effectors)KDeviceLists::CHAIN_R_LEG)).prettyPrint(); */

		/** Arm Joint Angles Computation **/
		armangles.zero();
		KVecDouble3 armt;
		armt=Tpprimel.getTranslation();
		armt.scalar_mult(0.4);
		//armt.prettyPrint();
		armangles(2)=asin((-armt(0)+NaoRobot.getWalkParameter(HX)*1000)/(UpperArmLength*1.5) )+M_PI_2;
		armangles(1)=asin((armt(1)+85-ShoulderOffsetY)/(UpperArmLength*1.5));
		armt=Tpprimer.getTranslation();
		armt.scalar_mult(0.4);
		//armt.prettyPrint();
		armangles(0)=asin((-armt(0)+NaoRobot.getWalkParameter(HX)*1000)/(UpperArmLength*1.5) )+M_PI_2;
		armangles(3)=asin((-armt(1)+85-ShoulderOffsetY)/(UpperArmLength*1.5) );
		//armangles.prettyPrint();


	}

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
    /** Get Target Com in Inertial Frame **/
	NaoLIPM.isDoubleSupport=double_support;
	NaoLIPM.LIPMComPredictor(Zbuffer,CoMm(0),CoMm(1),copi(0),copi(1));

	KVecFloat3 e(NaoLIPM.predictedErrorX,NaoLIPM.predictedErrorY,0);
	if(e(0)>NaoRobot.getWalkParameter(AdaptiveStepTolx) || e(1)>NaoRobot.getWalkParameter(AdaptiveStepToly))
	{
        predicterror.scalar_mult(0.1);
        e.scalar_mult(0.9);
	}
    else
    {
       predicterror.scalar_mult(0.6);
       e.scalar_mult(0.4);
    }

    predicterror+=e*0.2;
    predicterror.zero();

    /** Pop the used ZMP **/
	Zbuffer.pop();
}



/**
 *  This function sets the FSR values
 **/
void WalkEngine::setFSR(KMath::KMat::GenMatrix<double,4,1> l,KMath::KMat::GenMatrix<double,4,1> r)
{
	fsrl=l;
	fsrr=r;
}

/**
* This Function returns the position of the Center of Pressure (CoP) as measured by
* the Foot Sensitive Resistors (FSRs) in the Inertial Frame of Reference
**/
KVecFloat2 WalkEngine::getCoP()
{

	KVecFloat2 res;
	KVecDouble3 copl, copr, copi, cops, copsprime;
	float weightl, weightr, weights, weightsprime;

	/** Computation of the CoP in the Local Coordinate Frame of the Foot **/
	weightl=fsrl(0) + fsrl(1) + fsrl(2) + fsrl(3);
	weightr=fsrr(0) + fsrr(1) + fsrr(2) + fsrr(3);
	//fsrposl.prettyPrint();
	//fsrposr.prettyPrint();

	copl=fsrposl * fsrl;
	copr=fsrposr * fsrr;

	copl.scalar_mult(1.0/(weightl));
	copr.scalar_mult(1.0/(weightr));

	if (weightl < 0.01) //No Contact
	{
		copl.zero();
		weightl = 0;
	}

	if(weightr < 0.01) //No Contact
	{
		copr.zero();
		weightr = 0;

	}

	/** Convert CoP to mm **/
	copl.scalar_mult(1000);
	copr.scalar_mult(1000);


	if(supportleg == KDeviceLists::SUPPORT_LEG_RIGHT)
	{
		cops = copr;
		copsprime = copl;
		weights = weightr;
		weightsprime = weightl;
	}
	else
	{
		cops = copl;
		copsprime = copr;
		weights = weightl;
		weightsprime = weightr;
	}

	/** Compute the CoP wrt the Inertial Frame of Reference **/
    copi = (Tis.transform(cops)).scalar_mult(weights) + ((Tis*Tssprime).transform(copsprime)).scalar_mult(weightsprime);
    copi.scalar_mult(1.0/(weights + weightsprime));

	/** Convert CoP to m **/
    copi.scalar_mult(1.0/1000.0);

	/** If No Contact **/
    if((weights + weightsprime) < 1.0)
		copi.zero();

	res(0)=copi(0);
	res(1)=copi(1);
	//res.prettyPrint();

	return res;
}


void WalkEngine::planInstruction(KVecFloat3 destZMP, unsigned steps)
{
	KVecFloat3 startZMP;

	/** If there exists a planned ZMP point in the ZMP buffer **/
	if(Zbuffer.size()>0)
	{
		startZMP(0)=Zbuffer[Zbuffer.size()-1](0);  // ZMP position x
		startZMP(1)=Zbuffer[Zbuffer.size()-1](1);  // ZMP position y
		startZMP(2)=Zbuffer[Zbuffer.size()-1](2);  // orientation theta
	}
	else
	{
		startZMP.zero();
	}
	for(unsigned p = 1 ; p <= steps ; p++)
	{
			KVecFloat3 newpoint;
			newpoint.zero();

			/** Angle between the ending and starting foot orientation **/
			float adiff = KMath::anglediff2(destZMP(2),startZMP(2));

			/** ZMP Trajectory Generation **/
			newpoint(0)=interp.ManosBezierLinearInterpolation(((float)p)/steps,startZMP(0),destZMP(0),1.0);
			newpoint(1)=interp.ManosBezierLinearInterpolation(((float)p)/steps,startZMP(1),destZMP(1),1.0);
			newpoint(2)=startZMP(2)+interp.ManosBezierLinearInterpolation(((float)p)/steps,0,adiff,1.0);

			/** ZMP Point pushed to ZMP buffer **/
			Zbuffer.cbPush(newpoint);
	}
}
void WalkEngine::feed()
{

	/** Prepare the Instruction buffer **/
	if(walkbuffer.size() == 0)
	{
		addInit();
	}


	/** Robot is Ready to begin the walking Cycle **/
    if(Zbuffer.size() < PreviewWindow + 1 && walkbuffer.size() > 0)
	{
		WalkInstruction i = walkbuffer.readOne();

		/** Add a double support phase in the middle of i and planned  **/
		if(planned.targetZMP != i.targetSupport &&
		   i.targetZMP != KDeviceLists::SUPPORT_LEG_BOTH &&
		   planned.targetSupport != KDeviceLists::SUPPORT_LEG_NONE)
		{
			i.targetZMP = i.targetSupport;
			i.steps=i.steps * NaoRobot.getWalkParameter(Tds) / NaoRobot.getWalkParameter(Tss); //CAUTION
			//std::cout<<i.steps<<std::endl;
			//if(i.steps>20)
            //    i.steps=20;

		}
		else /**  Leep walking command i **/
		{
			if(Zbuffer.size()>0)
				walkbuffer.removeOne();
		}


		if(i.targetSupport == KDeviceLists::SUPPORT_LEG_LEFT && i.targetZMP != KDeviceLists::SUPPORT_LEG_BOTH)
			planR = i.target;
		else if(i.targetSupport == KDeviceLists::SUPPORT_LEG_RIGHT && i.targetZMP!=KDeviceLists::SUPPORT_LEG_BOTH)
			planL=i.target;


		/*if(i.targetSupport!=planned.targetZMP||i.targetZMP==KDeviceLists::SUPPORT_LEG_BOTH)
		{
			std::cout<<"Fixedlegs"<<std::endl;
			planL=getPositionInertial((NAOKinematics::Effectors)KDeviceLists::CHAIN_L_LEG);
			planR=getPositionInertial((NAOKinematics::Effectors)KDeviceLists::CHAIN_R_LEG);
		}*/


		/** Computing the  Reference ZMP point **/
		if(i.targetZMP==KDeviceLists::SUPPORT_LEG_RIGHT)
			/** Right Support Phase **/
			destZMP=planR;
		else if(i.targetZMP==KDeviceLists::SUPPORT_LEG_LEFT)
			/** Left Support Phase **/
			destZMP=planL;
		else
		{
			/** Double Support Phase **/
			destZMP=planL;
			destZMP+=planR;
			destZMP.scalar_mult(0.5);
			destZMP(2)=anglemean(planL(2),planR(2));
		}



		/** planL,planR are the ankle positions, transforming them to Reference ZMP **/
		KMath::KMat::GenMatrix<float,2,2> rot;
		KMath::KMat::transformations::makeRotation(rot,(float)destZMP(2));
		KVecFloat2 rr = rot * KVecFloat2(-NaoRobot.getWalkParameter(HX),0.0);
		destZMP(0) += rr(0);
		destZMP(1) += rr(1);

		/**  When ZMP buffer is empty
		 * 	 Normally shouldn't happen
		 **/
		if(Zbuffer.size() == 0)
		{
			destZMP.zero();
			i.steps = (PreviewWindow-1) / 4.0;
		}
		planInstruction(destZMP,i.steps);
		planned = i;
		qbuffer.push(i);
	}

 /** If the previous command i is completed
  *  use the next one in the command buffer
  **/
	if((ci.targetSupport == KDeviceLists::SUPPORT_LEG_NONE || currentstep > ci.steps))
	{

		WalkInstruction old = ci;

		ci=qbuffer.front();

		qbuffer.pop();

       /** Check if double support phase **/
		if(ci.targetSupport != supportleg ||
			ci.targetZMP == KDeviceLists::SUPPORT_LEG_BOTH ||
			old.targetZMP == KDeviceLists::SUPPORT_LEG_BOTH)
		{
		    double_support=true;

		}
		else
			double_support=false;

		currentstep=1;

	}




};

/**
* This function is the main function computing the Walk Trajectories
**/
std::vector<float> WalkEngine::runStep()
{


	/** PrepareBuffers, and targets **/
	feed();

	KDeviceLists::SupportLeg oldsupportleg=supportleg;
	double_support_progress=0;

	if(double_support)
		double_support_progress = ((float) currentstep) / ci.steps;

	supportleg = ci.targetSupport;
	bool rightsupport = supportleg == KDeviceLists::SUPPORT_LEG_RIGHT;
	chainsupport = (rightsupport == true)?KDeviceLists::CHAIN_R_LEG:KDeviceLists::CHAIN_L_LEG;

	Tsp = nkin.getForwardEffector((NAOKinematics::Effectors) chainsupport);

	/** Get the Transformation from pelvis to support **/
	Tsp.fast_invert();

	KDeviceLists::ChainsNames otherleg = (rightsupport == true)?
										KDeviceLists::CHAIN_L_LEG:KDeviceLists::CHAIN_R_LEG;
	NAOKinematics::kmatTable merger = nkin.getForwardFromTo(
															(NAOKinematics::Effectors)chainsupport,
															(NAOKinematics::Effectors)otherleg
														  );


	NAOKinematics::FKvars t;
	Tssprime = merger;

	if(oldsupportleg != supportleg)
	{
		NAOKinematics::kmatTable a = Tssprime, b = Tssprime;

		t.p = a.getTranslation();
		t.a = a.getEulerAngles();
		t.p(2) *= 0.5;
		t.a(0) *= 0.5;
		t.a(1) *= 0.5;
		a = NAOKinematics::getTransformation(t);
		a.fast_invert();
		std::cout<<"Leg Switching Happening"<<std::endl;
		Tis *= a;
		if(rightsupport == false)
		    Tis = Til;
	    else
		    Tis = Tir;
	}
	if(double_support == false)
	{
		t.p = Tis.getTranslation();
		t.a = Tis.getEulerAngles();
		t.p(2) *= 0.9;
		t.a(0) *= 0.9;
		t.a(1) *= 0.9;
		Tis = NAOKinematics::getTransformation(t);
	}


    /** Beginning of command to be executed **/
	if(currentstep==1)
	{
	    std::cout<<"Walking Command Starting"<<std::endl;
		startL=dl;//getPositionInertial((NAOKinematics::Effectors)KDeviceLists::CHAIN_L_LEG);
		startR=dr;//getPositionInertial((NAOKinematics::Effectors)KDeviceLists::CHAIN_R_LEG);
	}

	Calculate_Desired_COM();

	return  Calculate_IK();

}


#include "Stepplanner.h"

/*Stepplanner::Stepplanner()
{
    ;

}*/

void Stepplanner::initialize(RobotParameters OurRobot)
{
	lastvelocity.zero();
	Robot = OurRobot;
	WalkInstruction i;
	i.targetSupport=KDeviceLists::SUPPORT_LEG_LEFT;
	i.targetZMP=KDeviceLists::SUPPORT_LEG_LEFT;
	i.target(0)=Robot.getWalkParameter(HX);
	i.target(1)=-Robot.getWalkParameter(H0);
	i.target(2)=0;
	i.steps=Robot.getWalkParameter(Tstep)*Robot.getWalkParameter(Tss)/Robot.getWalkParameter(Ts);
	ankler=i.target;
	inst.push(i);
	i.targetSupport=KDeviceLists::SUPPORT_LEG_RIGHT;
	i.targetZMP=KDeviceLists::SUPPORT_LEG_RIGHT;
	i.target(0)=Robot.getWalkParameter(HX);
	i.target(1)=Robot.getWalkParameter(H0);
	anklel=i.target;
	inst.push(i);


	i.targetSupport=KDeviceLists::SUPPORT_LEG_RIGHT;
	i.targetZMP=KDeviceLists::SUPPORT_LEG_BOTH;
	inst.push(i);
	Pelvis.zero();


}


void Stepplanner::oneStep(std::vector<float> v)
{
	WalkInstruction i;
    KMath::KMat::transformations::makeRotation(RotPelvisZ,(float)Pelvis(2));


    /** Switch Support for next step **/
    if(support==KDeviceLists::SUPPORT_LEG_LEFT)
		support=KDeviceLists::SUPPORT_LEG_RIGHT;
	else if(support==KDeviceLists::SUPPORT_LEG_RIGHT)
		support=KDeviceLists::SUPPORT_LEG_LEFT;
	else /** BOTH --- choose where to put the support foot **/
	{
		 /**     If Walk Engine just Started this procedure determines which leg will move first Refactor
		     **/
		if( v[1]>0||v[2]>0)
		{
			support=KDeviceLists::SUPPORT_LEG_RIGHT;
		}
		else
		{
			support=KDeviceLists::SUPPORT_LEG_LEFT;
		}

	}

	KVecFloat2 h=KVecFloat2(Robot.getWalkParameter(HX),Robot.getWalkParameter(H0));
	if(support!=KDeviceLists::SUPPORT_LEG_RIGHT)
		h(1)=-h(1);

	KVecFloat2 a(v[0],v[1]);
	a=RotPelvisZ*a;

    velocity(0)= a(0) * Robot.getWalkParameter(Tstep)* Robot.getWalkParameter(MaxStepX);
	velocity(1)= a(1) * Robot.getWalkParameter(Tstep)* Robot.getWalkParameter(MaxStepY);
    velocity(2) = v[2] *  Robot.getWalkParameter(Tstep)* Robot.getWalkParameter(MaxStepTheta);

    if(velocity.norm2()==0)
    	    lastvelocity.zero();

    velocity=velocity*0.1+lastvelocity*0.9;


    tv=velocity;
    tv(0)/= Robot.getWalkParameter(MaxStepX);
    tv(1)/= Robot.getWalkParameter(MaxStepY);
    tv(2)/= Robot.getWalkParameter(MaxStepTheta);
    float mt=max(tv(0),tv(1));
    mt=max(mt,(float)tv(2));

    lastvelocity=velocity;
    /** Either reduce the step i or increase it
     * depending to the current velocity
     */
    float tstep=Robot.getWalkParameter(Tstep);
    if(mt>1)
    {
        velocity.scalar_mult(1/mt);
        tstep/=mt;
    }
    if(tstep<0.28)
    {
        mt=0.28/tstep;
        velocity.scalar_mult(mt);
        tstep*=mt;
    }

    /** Compute the ankle location and orientation **/
    Pelvis+=velocity;
    KMath::KMat::transformations::makeRotation(RotPelvisZ,(float)Pelvis(2));
    h=RotPelvisZ*h;
	i.target=Pelvis;
	i.target(0)+=h(0);

	i.target(1)+=h(1);
	i.targetSupport=support;
	i.targetZMP=support;
	/**  Foot is equal to the imaginary Pelvis **/
	i.target=i.target+velocity*0.5;
	/** Double Support Phase time interval **/
	i.ttlspeed=sqrt(velocity.norm2());
	if(support==KDeviceLists::SUPPORT_LEG_LEFT)
		ankler=i.target;
	else
		anklel=i.target;
	i.steps=ceil(Robot.getWalkParameter(Tss)*(tstep)/Robot.getWalkParameter(Ts));
	inst.push(i);


}


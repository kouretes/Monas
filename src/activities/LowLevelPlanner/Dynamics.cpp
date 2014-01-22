#include "Dynamics.h"
Dynamics::Dynamics(RobotParameters &robot): OurRobot(robot)
{
  /** Defining the System Dynamics Augmented with the Observer Structure **/

    Ad(0,0)=1.0000;
    Ad(0,1)=OurRobot.getWalkParameter(Ts);
    Ad(0,2)=OurRobot.getWalkParameter(Ts)*OurRobot.getWalkParameter(Ts)/2.0000;
    Ad(1,0)=0.0000;
    Ad(1,1)=1.0000;
    Ad(1,2)=Ad(0,1);
    Ad(2,0)=0.0000;
    Ad(2,1)=0.0000;
    Ad(2,2)=1.0000;
/*----------------------*/
    Ad(0,3)=0.0000;
    Ad(1,3)=0.0000;
    Ad(2,3)=0.0000;
    Ad(3,0)=0.0000;
    Ad(3,1)=0.0000;
    Ad(3,2)=0.0000;
    Ad(3,3)=1.0000;


    Bd(0)=OurRobot.getWalkParameter(Ts)*OurRobot.getWalkParameter(Ts)*OurRobot.getWalkParameter(Ts)/6.0000;
    Bd(1)=OurRobot.getWalkParameter(Ts)*OurRobot.getWalkParameter(Ts)/2.0000;
    Bd(2)=OurRobot.getWalkParameter(Ts);
    Bd(3)=0.0000;
    Cd(0)=1.0000;
    Cd(1)=0.0000;
    Cd(2)=-OurRobot.getWalkParameter(ComZ)/OurRobot.getWalkParameter(g);

    /** Defining the Optimal Gain for the Observer **/
    L(0,0)=0.1307;
    L(0,1)=0.0392;
    L(1,0)=-0.6250;
    L(1,1)=-0.2445;
    L(2,0)=-26.2666;
    L(2,1)=-11.9174;
    L(3,0)= -0.1623;
    L(3,1)=0.0557;

    State.zero();
	Stateold.zero();
	State_e.zero();
	zmpstate=0.000;
	zmpstateNew=0.000;

}
void Dynamics::Update(float u,KVecFloat2 error)
{
    /** Updating the Dynamics **/
	error-=KVecFloat2(State(0),(Cd(0)*State(0)+Cd(2)*State(2)+State(3)));//StatePredict(0));
    //error.scalar_mult(0.0);
    Stateold=State;
    State=Ad*State;
    temp=Bd;
    temp.scalar_mult(u);
    State+=temp;
    State+=L*error;
    //State.prettyPrint();
    predictedError=0.000;//Cd(0)*State(0)+Cd(2)*State(2)-ZMPReference(1);
    zmpstateNew=(Cd(0)*State(0)+Cd(2)*State(2));
}
void Dynamics::AugmentState()
{
    /**Augment State **/
      zmpstate=(Cd(0)*State(0)+Cd(2)*State(2));
      State_e(0)=State(0)-Stateold(0);
      State_e(1)=State(1)-Stateold(1);
      State_e(2)=State(2)-Stateold(2);
      State_e(3)=zmpstate;
      //State_e.scalar_mult(-1.000);
}

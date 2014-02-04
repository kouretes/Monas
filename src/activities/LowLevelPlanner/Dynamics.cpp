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
    Cd(2)=-(OurRobot.getWalkParameter(ComZ)*OurRobot.getWalkParameter(CoMZModelError))/OurRobot.getWalkParameter(g);

    /** Defining the Optimal Gain for the Observer **/
   // MPC OBSERVER
   /*L(0,0) =0.044452;
   L(0,1) =0.037499;
   L(1,0) =-0.22693;
   L(1,1) =-0.14403;
   L(2,0) =-3.0241;
   L(2,1) =-13.5139;
   L(3,0) =-0.048684;
   L(3,1) =0.060181;*/
    //L.scalar_mult(0);

    /** PREVIEW CONTROLLER OBSERVER **/
    L(0,0)=0.1307;
    L(0,1)=0.0392;
    L(1,0)=-0.6250;
    L(1,1)=-0.2445;
    L(2,0)=-26.2666;
    L(2,1)=-11.9174;
    L(3,0)= -0.1623;
    L(3,1)=0.0557;
    //L.scalar_mult(0);


/*
    L(0,0)=0.0009;
    L(0,1)=0.0015;
    L(1,0)=-0.0051;
    L(1,1)=-0.0074;
    L(2,0)=-0.0303;
    L(2,1)=-0.3325;
    L(3,0)= -0.0003;
	L(3,1)=0.0099;
	L.scalar_mult(50);*/
	//L.scalar_mult(0);
	//0.7900 -0.0000 4.4120 -0.0003 3.6503 -0.0003 0.0886 0.0100

	/*L(0,0)=0.7900;
    L(0,1)=-0.0000;
    L(1,0)=4.4120;
    L(1,1)=-0.0003;
    L(2,0)=3.6503;
    L(2,1)=-0.0003;
    L(3,0)= 0.0886;
	L(3,1)=0.0100;
	L.scalar_mult(0.09);*/

/*
    L(0,0)=0.1307;
    L(0,1)=0.0392;
    L(1,0)=-0.6250;
    L(1,1)=-0.2445;
    L(2,0)=-26.2666;
    L(2,1)=-11.9174;
    L(3,0)= -0.1623;
    L(3,1)=0.0557;
    L.scalar_mult(0.0005);*/

    State.zero();
	Stateold.zero();
	State_e.zero();
	zmpstate=0.000;
	zmpstateNew=0.000;
	startup=0;

}
void Dynamics::Update(float u,KVecFloat2 error)
{
    /** Updating the Dynamics **/
    error-=KVecFloat2(State(0),(Cd(0)*State(0)+Cd(2)*State(2)+State(3)));//StatePredict(0));
	if(startup<100)
	{
		error.scalar_mult(0.001*startup);
		startup++;
	}

	//error(0)*=0.5;//.scalar_mult(0.99);
    //error.scalar_mult(0.2);
    //std::cout<<"ERROR:"<<std::endl;
    //error.prettyPrint();
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

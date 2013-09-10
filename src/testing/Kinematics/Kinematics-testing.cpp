#include "core/elements/math/KMat.hpp"
#include "hal/robot/generic_nao/NAOKinematics.h"
#include "hal/robot/generic_nao/robot_consts.h"
#include <iostream>

using namespace std;
using namespace KMath::KMat;
typedef NAOKinematics::kmatTable Table;
using namespace KDeviceLists;

int main ()
{

	NAOKinematics kin;
	std::vector<float> joints(NUMOFJOINTS);
	joints[R_ARM+ELBOW_YAW]=M_PI/2;//-M_PI/2-0.001;
	joints[L_ARM+ELBOW_YAW]=M_PI/2;//M_PI/2-0.001;
	kin.setJoints(joints);
	Table l,r,t;
	l=kin.getForwardEffector((NAOKinematics::Effectors)CHAIN_L_ARM);
	l.prettyPrint();
	r=kin.getForwardEffector((NAOKinematics::Effectors)CHAIN_R_ARM);
	r.prettyPrint();
	t=kin.getForwardFromTo((NAOKinematics::Effectors)CHAIN_L_ARM,(NAOKinematics::Effectors)CHAIN_R_ARM);
    t.prettyPrint();

    {

		std::cout<<"Left Arm:"<<std::endl;
		std::vector<std::vector<float> > res= kin.inverseLeftHand(l);
		for(unsigned i=0;i<res.size();i++)
		{
			std::cout<<std::endl;
			for(unsigned j=0;j<res[i].size();j++)
				std::cout<<(res[i])[j]*180.0/M_PI<<" ";
			std::cout<<std::endl;
		}



    }


   {

		std::cout<<"Right Arm:"<<std::endl;
		std::vector<std::vector<float> > res= kin.inverseRightHand(r);
		for(unsigned i=0;i<res.size();i++)
		{
			std::cout<<std::endl;
			for(unsigned j=0;j<res[i].size();j++)
				std::cout<<(res[i])[j]*180.0/M_PI<<" ";
			std::cout<<std::endl;
		}



    }
	for (int k=0;k<10;k++)
	{
		    cout<<k<<"============================================================="<<std::endl;
		NAOKinematics::FKvars sl,sr;
		sl.p(0)=0;
		sl.p(1)=k*1.0;
		sl.p(2)=-290.0+k*1.0;
		sl.a(0)=0;//-((float) k)/100.0;
		sl.a(2)=-((float) k)/100.0;

		sr.p(0)=0;
		sr.p(1)=-k*1.0;
		sr.p(2)=-290.0+k*1.0;
		sr.a(0)=0;//+((float) k)/100.0;
		sr.a(2)=((float) k)/100.0;

		sr.p.prettyPrint();
		sr.a.prettyPrint();
		//std::cout<<k*10.0<<std::endl;
		//r=kin.getForwardEffector((NAOKinematics::Effectors)CHAIN_R_LEG);
		//r.prettyPrint();

		//t=kin.getForwardFromTo((NAOKinematics::Effectors)CHAIN_L_LEG,(NAOKinematics::Effectors)CHAIN_R_LEG);
		//t.prettyPrint();

		{
			//std::cout<<"Left Leg:"<<std::endl;
			std::vector<std::vector<float> > res= kin.inverseLeftLeg(sl);
			std::cout<<"Left Leg:"<<res.size()<<std::endl;
			for(unsigned i=0;i<res.size();i++)
			{
				std::cout<<std::endl;
				for(unsigned j=0;j<(res[i]).size();j++)
					std::cout<<(res[i])[j]<<" ";
				std::cout<<std::endl;
			}
		}

		 {

			std::vector<std::vector<float> > res= kin.inverseRightLeg(sr);
			std::cout<<"Right Leg:"<<res.size()<<std::endl;
			for(unsigned i=0;i<res.size();i++)
			{
				std::cout<<std::endl;
				for(unsigned j=0;j<(res[i]).size();j++)
					std::cout<<(res[i])[j]<<" ";
				std::cout<<std::endl;
			}

		}

	}




	return 0;
}

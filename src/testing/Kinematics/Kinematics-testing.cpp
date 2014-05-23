#include "core/elements/math/KMat.hpp"
#include "hal/robot/nao/generic_nao/NAOKinematics.h"
#include "hal/robot/nao/generic_nao/robot_consts.h"
#include <iostream>
#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;

using namespace std;
using namespace KMath::KMat;
typedef NAOKinematics::kmatTable Table;
using namespace KDeviceLists;

int main ()
{
	ptime end, start;
	NAOKinematics kin;
	std::vector<float> joints(NUMOFJOINTS);
	joints[L_ARM+SHOULDER_ROLL]=M_PI_2;
	joints[R_ARM+SHOULDER_ROLL]=-M_PI_2;
	joints[R_ARM+ELBOW_YAW]=0;
	joints[L_ARM+ELBOW_YAW]=0;
	joints[L_LEG+HIP_ROLL]=M_PI_2;
	joints[R_LEG+HIP_ROLL]=-M_PI_2;
	kin.setJoints(joints);
	Table l,r,t;
	kin.calculateCenterOfMass().prettyPrint();
	//joints[HEAD+YAW]=-M_PI/2;
	//joints[HEAD+PITCH]=M_PI/4;
	kin.setJoints(joints);
	kin.calculateCenterOfMass().prettyPrint();

	start = microsec_clock::universal_time();
	for(unsigned i=0;i<1e5;i++){
		kin.setJoints(joints);
		l=kin.getForwardEffector((NAOKinematics::Effectors)CHAIN_L_ARM);
		r=kin.getForwardEffector((NAOKinematics::Effectors)CHAIN_R_ARM);

	}
	end = microsec_clock::universal_time();
	cout << "1e5x2 forward:" << (end - start).total_microseconds() << " microseconds " << endl;

	l.prettyPrint();
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
		sl.a(0)=-((float) k)/100.0;
		sl.a(1)=k/200;
		sl.a(2)=-((float) k)/100.0;
		sl.p.prettyPrint();
		sl.a.prettyPrint();
		sr.p(0)=0;
		sr.p(1)=-k*1.0;
		sr.p(2)=-290.0+k*1.0;
		sr.a(0)=+((float) k)/100.0;
		sr.a(1)=k/200;
		sr.a(2)=((float) k)/100.0;

		sr.p.prettyPrint();
		sr.a.prettyPrint();
		//std::cout<<k*10.0<<std::endl;
		//r=kin.getForwardEffector((NAOKinematics::Effectors)CHAIN_R_LEG);
		//r.prettyPrint();

		//t=kin.getForwardFromTo((NAOKinematics::Effectors)CHAIN_L_LEG,(NAOKinematics::Effectors)CHAIN_R_LEG);
		//t.prettyPrint();
		std::vector<float> resl,resr;

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
			if(res.size()>0)
				resl=res[0];
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
			if(res.size()>0)
			{
				resr=res[0];
				//std::cout<<"set vector"<<std::endl;
			}



		}
		kin.setChain(CHAIN_L_LEG,resl);
		kin.setChain(CHAIN_R_LEG,resr);
		kin.calculateCenterOfMass().prettyPrint();

	}


	return 0;
}

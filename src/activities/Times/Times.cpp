#include "Times.h"


#include <cstdlib>
#include <iostream>
#include <ctime>
#include <sstream>


#define FULLSTIFFNESS 0.9
#define LEANTOOMUCH 0.7
#define ANGLEHOR 1.6
#define INTTIME 0.35 //angle integration time. Look ahead for so many seconds Too large valies mean large sensitivity, too small means too late reaction

//#define MAXHEADYAWSPEED 1.8
//#define MAXHEADPITCHSPEED 1.8

#define KME_ACTIONPID -1

ACTIVITY_REGISTER(Times);

using namespace std;
using namespace KDeviceLists;

Times::Times(Blackboard &b) : IActivity(b)
{

}

void Times::UserInit()
{

	
	//Self Reset for initialization
	Reset();
}

void Times::Reset(){

}

int Times::Execute()
{
	//testcommands();
	//if(counter%100==0)
	//	readWalkParameters();
	srand(time(0));
	NAOKinematics nkin;
	NAOKinematics::kmatTable output1, output2, output3, output4, output5,T;

	std::vector<float> joints(NUMOFJOINTS);
	std::vector<float> jointsPrev(NUMOFJOINTS);
	double pi = KMath::KMat::transformations::PI;
	double times[16];
	for(int i=0; i<16; i++){
		times[i] = 0;
	}
	for(int i=0; i<joints.size(); i++){
		joints[i]=0;
	}
	int head,legr,legl,arml,armr;
	head = legr = legl = arml = armr = 0;
	cout << "Gia des: " <<  (double)RAND_MAX+1 << endl;
	for(int i=0; i<10000; i++){
		for(int i=0; i<joints.size(); i++){
			jointsPrev[i] = joints[i];
		}
		//Left Hand
		joints[L_ARM+SHOULDER_PITCH]=((double) rand() / (((double)RAND_MAX) + 1.0 )) * (LShoulderPitchHigh-LShoulderPitchLow+1) + LShoulderPitchLow;
		joints[L_ARM+SHOULDER_ROLL]=((double) rand() / (((double)RAND_MAX) + 1.0 )) * (LShoulderRollHigh-LShoulderRollLow+1) + LShoulderRollLow;
		joints[L_ARM+ELBOW_YAW]=((double) rand() / (((double)RAND_MAX) + 1.0 )) * (LElbowYawHigh-LElbowYawLow+1) + LElbowYawLow;
		joints[L_ARM+ELBOW_ROLL]=((double) rand() / (((double)RAND_MAX) + 1.0 )) * (LElbowRollHigh-LElbowRollLow+1) + LElbowRollLow;
		joints[L_ARM+WRIST_YAW]=((double) rand() / (((double)RAND_MAX) + 1.0 )) * (LWristYawHigh-LWristYawLow+1) + LWristYawLow;
		//Right Hand
		joints[R_ARM+SHOULDER_PITCH]=((double) rand() / (((double)RAND_MAX) + 1.0 )) * (RShoulderPitchHigh-RShoulderPitchLow+1) + RShoulderPitchLow;
		joints[R_ARM+SHOULDER_ROLL]=((double) rand() / (((double)RAND_MAX) + 1.0 )) * (RShoulderRollHigh-RShoulderRollLow+1) + RShoulderRollLow;
		joints[R_ARM+ELBOW_YAW]=((double) rand() / (((double)RAND_MAX) + 1.0 )) * (RElbowYawHigh-RElbowYawLow+1) + RElbowYawLow;
		joints[R_ARM+ELBOW_ROLL]=((double) rand() / (((double)RAND_MAX) + 1.0 )) * (RElbowRollHigh-RElbowRollLow+1) + RElbowRollLow;
		joints[R_ARM+WRIST_YAW]=((double) rand() / (((double)RAND_MAX) + 1.0 )) * (RWristYawHigh-RWristYawLow+1) + RWristYawLow;
		//Left Leg
		joints[L_LEG+HIP_YAW_PITCH]=((double) rand() / (((double)RAND_MAX) + 1.0 )) * (LHipYawPitchHigh-LHipYawPitchLow+1) + LHipYawPitchLow;
		joints[L_LEG+HIP_ROLL]=((double) rand() / (((double)RAND_MAX) + 1.0 )) * (LHipRollHigh-LHipRollLow+1) + LHipRollLow;
		joints[L_LEG+HIP_PITCH]=((double) rand() / (((double)RAND_MAX) + 1.0 )) * (LHipPitchHigh-LHipPitchLow+1) + LHipPitchLow;
		joints[L_LEG+KNEE_PITCH]=((double) rand() / (((double)RAND_MAX) + 1.0 )) * (LKneePitchHigh-LKneePitchLow+1) + LKneePitchLow;
		joints[L_LEG+ANKLE_PITCH]=((double) rand() / (((double)RAND_MAX) + 1.0 )) * (LAnklePitchHigh-LAnklePitchLow+1) + LAnklePitchLow;
		joints[L_LEG+ANKLE_ROLL]=((double) rand() / (((double)RAND_MAX) + 1.0 )) * (LAnkleRollHigh-LAnkleRollLow+1) + LAnkleRollLow;
		//Right Leg
		joints[R_LEG+HIP_YAW_PITCH]=((double) rand() / (((double)RAND_MAX) + 1.0 )) * (RHipYawPitchHigh-RHipYawPitchLow+1) + RHipYawPitchLow;
		joints[R_LEG+HIP_ROLL]=((double) rand() / (((double)RAND_MAX) + 1.0 )) * (RHipRollHigh-RHipRollLow+1) + RHipRollLow;
		joints[R_LEG+HIP_PITCH]=((double) rand() / (((double)RAND_MAX) + 1.0 )) * (RHipPitchHigh-RHipPitchLow+1) + RHipPitchLow;
		joints[R_LEG+KNEE_PITCH]=((double) rand() / (((double)RAND_MAX) + 1.0 )) * (RKneePitchHigh-RKneePitchLow+1) + RKneePitchLow;
		joints[R_LEG+ANKLE_PITCH]=((double) rand() / (((double)RAND_MAX) + 1.0 )) * (RAnklePitchHigh-RAnklePitchLow+1) + RAnklePitchLow;
		joints[R_LEG+ANKLE_ROLL]=((double) rand() / (((double)RAND_MAX) + 1.0 )) * (RAnkleRollHigh-RAnkleRollLow+1) + RAnkleRollLow;
		//Head
		joints[HEAD+YAW]=((double) rand() / (((double)RAND_MAX) + 1.0 )) * (HeadYawHigh-HeadYawLow+1) + HeadYawLow;
		joints[HEAD+PITCH]=((double) rand() / (((double)RAND_MAX) + 1.0 )) * (HeadPitchHigh-HeadPitchLow+1) + HeadPitchLow;
		
		//Now we can set the joints to the class
		nkin.setJoints(joints);
		timer.restart();
		output1= nkin.getForwardEffector((NAOKinematics::Effectors)CHAIN_L_ARM);
		times[0] += timer.elapsed();
		//Right Hand
		timer.restart();
		output2 = nkin.getForwardEffector((NAOKinematics::Effectors)CHAIN_R_ARM);
		times[1] += timer.elapsed();
		//Left Leg
		timer.restart();
		output3 = nkin.getForwardEffector((NAOKinematics::Effectors)CHAIN_L_LEG);
		times[2] += timer.elapsed();
		//Right Leg
		timer.restart();
		output4 = nkin.getForwardEffector((NAOKinematics::Effectors)CHAIN_R_LEG);
		times[3] += timer.elapsed();
		//Camera
		timer.restart();
		output5 = nkin.getForwardEffector(NAOKinematics::EFF_CAMERA_BOT);
		times[4] += timer.elapsed();
		vector<vector<float> > result;
		timer.restart();
		result = nkin.inverseLeftHand(output1);
		times[5] += timer.elapsed();
		if(!result.empty()){
			arml++;
			nkin.setJoints(jointsPrev);
			//timer.restart();
			//result = nkin.jacobianInverseLeftHand(output1);
			//times[6] += timer.elapsed();
		}
		timer.restart();
		result = nkin.inverseRightHand(output2);
		times[7] += timer.elapsed();
		if(!result.empty()){
			armr++;
			nkin.setJoints(jointsPrev);
			//timer.restart();
			//result = nkin.jacobianInverseRightHand(output2);
			//times[8] += timer.elapsed();
		}
		timer.restart();
		result = nkin.inverseLeftLeg(output3);
		times[9] += timer.elapsed();
		if(!result.empty()){
			legl++;
			nkin.setJoints(jointsPrev);
			timer.restart();
			result = nkin.jacobianInverseLeftLeg(output3);
			times[10] += timer.elapsed();
		}
		timer.restart();
		result = nkin.inverseRightLeg(output4);
		times[11] += timer.elapsed();
		if(!result.empty()){
			legr++;
			nkin.setJoints(jointsPrev);
			timer.restart();
			result = nkin.jacobianInverseRightLeg(output4);
			times[12] += timer.elapsed();
		}
		timer.restart();
		result = nkin.inverseHead(output5, false, false);
		times[13] += timer.elapsed();
		if(!result.empty()){
			head++;
			nkin.setJoints(jointsPrev);
			timer.restart();
			result = nkin.jacobianInverseHead(output5,false);
			times[14] += timer.elapsed();
		}
		timer.restart();
		nkin.calculateCenterOfMass();
		times[15] += timer.elapsed();
		
	}
	times[0] = times[0]/10000;
	times[1] = times[1]/10000;
	times[2] = times[2]/10000;
	times[3] = times[3]/10000;
	times[4] = times[4]/10000;
	times[5] = times[5]/arml;
	times[7] = times[7]/armr;
	times[9] = times[9]/legl;
	times[10] = times[10]/legl;
	times[11] = times[11]/legr;
	times[12] = times[12]/legr;
	times[13] = times[13]/head;
	times[14] = times[14]/head;
	times[15] = times[15]/10000;
	for(int i=0; i<16; i++){
		LogEntry(LogLevel::Info, GetName())<<"i = " << i << " " << times[i];
	}
	return 0;
}

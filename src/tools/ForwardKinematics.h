#ifndef __FWKIN_H__
#define __FWKIN_H__
#include <iostream>
#include <stdexcept>
#include <string.h>
#include <vector>
#include <iomanip>
#include <limits>
#include <math.h>
#include "KMat.h"

#define ShoulderOffsetY 98.0f
#define ElbowOffsetY	15.0f
#define UpperArmLength	105.0f
#define ShoulderOffsetZ	100.0f
#define LowerArmLength	57.75f
#define HandOffsetX		55.95f
#define HipOffsetZ		85.0f
#define HipOffsetY		50.0f
#define ThighLength		100.0f
#define TibiaLength		102.9f	
#define FootHeight		45.11f
#define NeckOffsetZ		126.5f 
#define CameraBotomX	48.8f
#define CameraBotomZ	23.81f 
#define CameraTopX		53.9f
#define CameraTopZ		67.9f

#define kmatTable KMat::ATMatrix<float,4>
#define KMatTransf KMat::transformations

/**
 * This is the code for the Forward Kinematics for nao v3.3 robot.
 
 * @author Kofinas Nikos aka eldr4d, 2011 kouretes team
 *
 	 * 1 = vector's head position\n
	 * Arms:	
	 *			-# ShoulderPitch
	 * 			-# ShoulderRoll
	 *			-# ElbowYaw
	 *			-# ElbowRoll
	 * .
	 *
	 * Legs:
	 *			-# HipYawPitch
	 *			-# HipRoll
	 *			-# HipPitch
	 *			-# KneePitch
	 *			-# AnkleRoll
	 *			-# AnklePitch
	 *
	 * .
	 * Camera:	
	 *			-# HeadYaw
	 *			-# HeadPitch
	 * .
	 * Format of inputs for filter forward
	 * "CameraTop" = forward for Top camera
	 * "CameraBot" = forward for Bottom camera
	 * "LeftLeg" = forward for left leg
	 * "RightLeg" = forward for right leg
	 * "LeftArm" = forward for left arm
	 * "RightArm" = forward for right arm
 * \file ForwardKinematics.h
*/
using namespace std;
namespace FKin
{
	/**
	*@struct FKvars
	*@brief This struct contains all the cartesian points and angles that we extract from the forward kinematics
	*/
	struct FKvars {
		float pointX,pointY,pointZ;
		float angleX,angleY,angleZ;
	};
	
	
	/**
	 * @fn void forwardLeftHand(kmatTable & EndTransf, float ShoulderPitch, float ShoulderRoll, float ElbowYaw, float ElbowRoll)
	 * @brief Forward kinematic for the left hand.
	 * @param EndTransf. The matrix table that we will return the result.
	 * @param ShoulderPitch. The value of the Left's arm shoulder pitch joint.
	 * @param ShoulderRoll. The value of the Left's arm shoulder roll joint.
	 * @param ElbowYaw. The value of the Left's arm elbow yaw joint.
	 * @param ElbowRoll. The value of the Left's arm elbow roll joint.
	 * */
	void forwardLeftHand(kmatTable & EndTransf, float ShoulderPitch, float ShoulderRoll, float ElbowYaw, float ElbowRoll){
		kmatTable base,T1,T2,T3,T4,R,endTr;
		float PI = KMatTransf::PI;
		KMatTransf::makeTranslationMatrix(base, 0.0f,ShoulderOffsetY + ElbowOffsetY,ShoulderOffsetZ);
		KMatTransf::makeTransformationMatrix(T1,0.0f,-PI/2,0.0f,ShoulderPitch);
		KMatTransf::makeTransformationMatrix(T2,0.0f,PI/2,0.0f,ShoulderRoll-PI/2);
		KMatTransf::makeTransformationMatrix(T3,0.0f,-PI/2,UpperArmLength,ElbowYaw);
		KMatTransf::makeTransformationMatrix(T4,0.0f,PI/2,0.0f,ElbowRoll);
		
		KMatTransf::rotationMatrix(R,0.0f,0.0f,PI/2);
		KMatTransf::makeTranslationMatrix(endTr,HandOffsetX + LowerArmLength,0.0f,0.0f);
		kmatTable Tend;
		Tend = base;
		Tend *= T1;
		Tend *= T2;
		Tend *= T3;
		Tend *= T4;
		Tend *= R;
		Tend *= endTr;
		EndTransf = Tend;
	};
	
	/**
	 * @fn void FKin::forwardRightHand(kmatTable & EndTransf, float ShoulderPitch, float ShoulderRoll, float ElbowYaw, float ElbowRoll)
	 * @brief Forward kinematic for the right hand.
	 * @param EndTransf. The matrix table that we will return the result.
	 * @param ShoulderPitch. The value of the Right's arm shoulder pitch joint.
	 * @param ShoulderRoll. The value of the Right's arm shoulder roll joint.
	 * @param ElbowYaw. The value of the Right's arm elbow yaw joint.
	 * @param ElbowRoll. The value of the Right's arm elbow roll joint.
	 * */
	void forwardRightHand(kmatTable & EndTransf, float ShoulderPitch, float ShoulderRoll, float ElbowYaw, float ElbowRoll){
		kmatTable base,T1,T2,T3,T4,R,endTr;
		float PI = KMatTransf::PI;
		
		KMatTransf::makeTranslationMatrix(base, 0.0f,-(ShoulderOffsetY+ElbowOffsetY),ShoulderOffsetZ);
		
		KMatTransf::makeTransformationMatrix(T1,0.0f,-PI/2,0.0f,ShoulderPitch);
		KMatTransf::makeTransformationMatrix(T2,0.0f,PI/2,0.0f,ShoulderRoll+PI/2);//Allagh apo matlab
		KMatTransf::makeTransformationMatrix(T3,0.0f,-PI/2,-UpperArmLength,ElbowYaw);
		KMatTransf::makeTransformationMatrix(T4,0.0f,PI/2,0.0f,ElbowRoll);//Allagh apo matlab
		
		KMatTransf::rotationMatrix(R,0.0f,0.0f,PI/2);
		KMatTransf::makeTranslationMatrix(endTr,-(HandOffsetX+LowerArmLength),0.0f,0.0f);
		
		kmatTable Tend;
		Tend = base;
		Tend *= T1;
		Tend *= T2;
		Tend *= T3;
		Tend *= T4;
		Tend *= R;
		Tend *= endTr;
		EndTransf = Tend;
		
	};
	
	/**
	 * @fn void forwardLeftLeg(kmatTable & EndTransf, float HipYawPitch, float HipRoll, float HipPitch, float KneePitch, float AnkleRoll, float AnklePitch)
	 * @brief Forward kinematic for the left leg.
	 * @param EndTransf. The matrix table that we will return the result.
	 * @param HipYawPitch. The value of the Left's leg hip yaw pitch joint.
	 * @param HipRoll. The value of the Left's leg hip roll joint.
	 * @param HipPitch. The value of the Left's hip pitch yaw joint.
	 * @param KneePitch. The value of the Left's knee pitch roll joint.
	 * @param AnkleRoll. The value of the Left's ankle elbow roll joint.
	 * @param AnklePitch. The value of the Left's ankle pitch roll joint.
	 * */
	void forwardLeftLeg(kmatTable & EndTransf, float HipYawPitch, float HipRoll, float HipPitch, float KneePitch, float AnkleRoll, float AnklePitch){
		kmatTable base,T1,T2,T3,T4,T5,T6,R1,R2,endTr;
		float PI = KMatTransf::PI;
		
		KMatTransf::makeTranslationMatrix(base, 0.0f,HipOffsetY,-HipOffsetZ);
		
		KMatTransf::makeTransformationMatrix(T1,0.0f,-3*PI/4,0.0f,HipYawPitch-PI/2);
		KMatTransf::makeTransformationMatrix(T2,0.0f,-PI/2,0.0f,HipRoll+PI/4);
		KMatTransf::makeTransformationMatrix(T3,0.0f,PI/2,0.0f,HipPitch);
		KMatTransf::makeTransformationMatrix(T4,-ThighLength,0.0f,0.0f,KneePitch);
		KMatTransf::makeTransformationMatrix(T5,-TibiaLength,-PI/2,0.0f,AnkleRoll);
		KMatTransf::makeTransformationMatrix(T6,0.0f,PI/2,0.0f,AnklePitch);
		
		KMatTransf::rotationMatrix(R1,0.0f,0.0f,PI);
		KMatTransf::rotationMatrix(R2,PI/2,-PI/2,0.0f);
		KMatTransf::makeTranslationMatrix(endTr,0.0f,0.0f,-FootHeight);
		kmatTable Tend;
		Tend = base;
		Tend *= T1;
		Tend *= T2;
		Tend *= T3;
		Tend *= T4;
		Tend *= T5;
		Tend *= T6;
		Tend *= R1;
		Tend *= R2;
		Tend *= endTr;
		EndTransf = Tend;
		
	};
	
	/**
	 * @fn void forwardRightLeg(kmatTable & EndTransf, float HipYawPitch, float HipRoll, float HipPitch, float KneePitch, float AnkleRoll, float AnklePitch)
	 * @brief Forward kinematic for the right leg.
	 * @param EndTransf. The matrix table that we will return the result.
	 * @param HipYawPitch. The value of the Right's leg hip yaw pitch joint.
	 * @param HipRoll. The value of the Right's leg hip roll joint.
	 * @param HipPitch. The value of the Right's hip pitch yaw joint.
	 * @param KneePitch. The value of the Right's knee pitch roll joint.
	 * @param AnkleRoll. The value of the Right's ankle elbow roll joint.
	 * @param AnklePitch. The value of the Right's ankle pitch roll joint.
	 * */
	void forwardRightLeg(kmatTable & EndTransf, float HipYawPitch, float HipRoll, float HipPitch, float KneePitch, float AnkleRoll, float AnklePitch){
		kmatTable base,T1,T2,T3,T4,T5,T6,R1,R2,endTr;
		float PI = KMatTransf::PI;
		
		KMatTransf::makeTranslationMatrix(base, 0.0f,-HipOffsetY,-HipOffsetZ);
		
		KMatTransf::makeTransformationMatrix(T1,0.0f,-PI/4,0.0f,HipYawPitch-PI/2);
		KMatTransf::makeTransformationMatrix(T2,0.0f,PI/2,0.0f,-HipRoll+PI/4);//allagh
		KMatTransf::makeTransformationMatrix(T3,0.0f,-PI/2,0.0f,HipPitch);
		KMatTransf::makeTransformationMatrix(T4,-ThighLength,0.0f,0.0f,KneePitch);
		KMatTransf::makeTransformationMatrix(T5,-TibiaLength,PI/2,0.0f,-AnkleRoll);//allagh
		KMatTransf::makeTransformationMatrix(T6,0.0f,-PI/2,0.0f,AnklePitch);
		
		KMatTransf::rotationMatrix(R1,0.0f,0.0f,PI);
		KMatTransf::rotationMatrix(R2,PI/2,-PI/2,0.0f);
		KMatTransf::makeTranslationMatrix(endTr,0.0f,0.0f,-FootHeight);
		kmatTable Tend;
		Tend = base;
		Tend *= T1;
		Tend *= T2;
		Tend *= T3;
		Tend *= T4;
		Tend *= T5;
		Tend *= T6;
		Tend *= R1;
		Tend *= R2;
		Tend *= endTr;
		EndTransf = Tend;
		
	};
	
	/**
	 * @fn void forwardCamera(kmatTable & EndTransf, float HeadYaw, float HeadPitch, bool topCamera)
	 * @brief Forward kinematic for the camera's on the head.
	 * @param EndTransf. The matrix table that we will return the result.
	 * @param HeadYaw. The value of the Head's yaw joint.
	 * @param HeadPitch. The value of the Head's pitch joint.
	 * @param topCamera. This value is true if we want forward kinematics for the top camera, or false if we want for the bottom camera.
	 * */
	void forwardCamera(kmatTable & EndTransf, float HeadYaw, float HeadPitch, bool topCamera){
		kmatTable base,T1,T2,R,endTr;
		float PI = KMatTransf::PI;
		
		KMatTransf::makeTranslationMatrix(base, 0.0f,0.0f,NeckOffsetZ);
		
		KMatTransf::makeTransformationMatrix(T1,0.0f,0.0f,0.0f,HeadYaw);
		KMatTransf::makeTransformationMatrix(T2,0.0f,-PI/2,0.0f,HeadPitch-PI/2);
		
		KMatTransf::rotationMatrix(R,PI/2,PI/2,0.0f);
		if(!topCamera)
			KMatTransf::makeTranslationMatrix(endTr,CameraBotomX,0.0f,CameraBotomZ);
		else
			KMatTransf::makeTranslationMatrix(endTr,CameraTopX,0.0f,CameraTopZ);
		kmatTable Tend;
		Tend = base;
		Tend *= T1;
		Tend *= T2;
		Tend *= R;
		Tend *= endTr;
		EndTransf = Tend;
		
	};
	
	/**
	 * @fn void filterForward(kmatTable & Tmatrix, string WhatForward, std::vector<float> joints)
	 * @brief This function take the name of the end effector and one vector with joint and then it call's the apropriate function.
	 * @param Tmatrix. The matrix table that we will return the result.
	 * @param WhatForward. With this string, this function understands whate forward chain we want.
	 * @param joints. One vector with all the joints for the chain.
	 * */
	void filterForward(kmatTable & Tmatrix, string WhatForward, std::vector<float> joints){
		Tmatrix.zero();
		std::vector<float>::iterator iter;
		iter = joints.begin();
		if(!WhatForward.compare("CameraTop")){
			float HP = *iter;
			iter++;
			float HY = *iter;
			forwardCamera(Tmatrix,HP,HY,true);
		}else if(!WhatForward.compare("CameraBot")){
			float HP = *iter;
			iter++;
			float HY = *iter;
			forwardCamera(Tmatrix,HP,HY,false);
		}else if(!WhatForward.compare("LeftArm")){
			float SP = *iter;
			iter++;
			float SR = *iter;
			iter++;
			float EY = *iter;
			iter++;
			float ER = *iter;
			forwardLeftHand(Tmatrix,SP,SR,EY,ER);
		}else if(!WhatForward.compare("RightArm")){
			float SP = *iter;
			iter++;
			float SR = *iter;
			iter++;
			float EY = *iter;
			iter++;
			float ER = *iter;
			forwardRightHand(Tmatrix,SP,SR,EY,ER);
		}else if(!WhatForward.compare("LeftLeg")){
			float HYP = *iter;
			iter++;
			float HR = *iter;
			iter++;
			float HP = *iter;
			iter++;
			float KP = *iter;
			iter++;
			float AR = *iter;
			iter++;
			float AP = *iter;
			forwardLeftLeg(Tmatrix,HYP,HR,HP,KP,AR,AP);
		}else if(!WhatForward.compare("RightLeg")){
			float HYP = *iter;
			iter++;
			float HR = *iter;
			iter++;
			float HP = *iter;
			iter++;
			float KP = *iter;
			iter++;
			float AR = *iter;
			iter++;
			float AP = *iter;
			forwardRightLeg(Tmatrix,HYP,HR,HP,KP,AR,AP);
		}
		Tmatrix.check();
	}
	
	/**
	 * @fn FKvars filterForwardFromTo(std::string start, std::string stop, std::vector<float> jointsStart, std::vector<float> jointsEnd)
	 * @brief This function take's the name of the start point for the chain, the name for the end point and returns the cartesian values of the end effector.
	 * @param start. The name of the start point of the chain.
	 * @param stop. The name of the end point of the chain.
	 * @param jointsStart. One vector with all the joints for the chain of the start point.
	 * @param jointsEnd. One vector with all the joints for the chain of the end point.
	 * @returns FKVariables. The struct with the 3 cartesian points and with the 3 cartesian angles.
	 * 
 	 * @details Format of vector for filtering.
	 * */
	FKvars filterForwardFromTo(std::string start, std::string stop, std::vector<float> jointsStart, std::vector<float> jointsEnd){
		kmatTable Tmatrix1, Tmatrix2;
		if(!start.compare("Torso")){
			Tmatrix1.identity();
		}else
			filterForward(Tmatrix1,start, jointsStart);
		if(!stop.compare("Torso")){
			Tmatrix2.identity();
		}else
			filterForward(Tmatrix2,stop, jointsEnd);
		Tmatrix1.fast_invert();
		Tmatrix1 *= Tmatrix2;
		FKvars FKVariables;
		FKVariables.pointX = Tmatrix1(0,3);
		FKVariables.pointY = Tmatrix1(1,3);
		FKVariables.pointZ = Tmatrix1(2,3);
		FKVariables.angleZ = atan2(Tmatrix1(1,0),Tmatrix1(0,0));
		FKVariables.angleY = atan2(-Tmatrix1(2,0),sqrt(pow(Tmatrix1(2,1),2)+pow(Tmatrix1(2,2),2)));
		FKVariables.angleX = atan2(Tmatrix1(2,1),Tmatrix1(2,2));
		return FKVariables;
		
	}
	
	/**
	 * @fn FKvars forwardFromTo(std::string start, std::string stop, std::vector<float> jointsStart, std::vector<float> jointsEnd)
	 * @brief This function take's the name of the start point for the chain, the name for the end point and returns the transformation table.
	 * @param start. The name of the start point of the chain.
	 * @param stop. The name of the end point of the chain.
	 * @param jointsStart. One vector with all the joints for the chain of the start point.
	 * @param jointsEnd. One vector with all the joints for the chain of the end point.
	 * @returns Tamatrix1. The transformation matrix.
	 * 
 	 * @details Return the whole transformation table
	 * */
	kmatTable forwardFromTo(std::string start, std::string stop, std::vector<float> jointsStart, std::vector<float> jointsEnd){
		kmatTable Tmatrix1, Tmatrix2;
		if(!start.compare("Torso")){
			Tmatrix1.identity();
		}else
			filterForward(Tmatrix1,start, jointsStart);
		if(!stop.compare("Torso")){
			Tmatrix2.identity();
		}else
			filterForward(Tmatrix2,stop, jointsEnd);
		Tmatrix1.fast_invert();
		Tmatrix1 *= Tmatrix2;
		return Tmatrix1;
		
	}
	//std::vector<int>
	//std::vector<float> lala;
	//vector<float>::iterator iter;
	//lala.insert(iter,asl);
	/**
	 
	 */
	
	
}

#endif

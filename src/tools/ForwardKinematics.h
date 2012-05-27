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
#include "hal/robot/generic_nao/KinematicsDefines.h"

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
	 *			-# AnklePitch
	 *			-# AnkleRoll
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
	typedef KMat::ATMatrix<float,4> kmatTable;
	typedef KMat::transformations KMatTransf;
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
	 * @fn void forwardLeftLeg(kmatTable & EndTransf, float HipYawPitch, float HipRoll, float HipPitch, float KneePitch, float AnklePitch, float AnkleRoll)
	 * @brief Forward kinematic for the left leg.
	 * @param EndTransf. The matrix table that we will return the result.
	 * @param HipYawPitch. The value of the Left's leg hip yaw pitch joint.
	 * @param HipRoll. The value of the Left's leg hip roll joint.
	 * @param HipPitch. The value of the Left's hip pitch yaw joint.
	 * @param KneePitch. The value of the Left's knee pitch roll joint.
	 * @param AnklePitch. The value of the Left's ankle pitch roll joint.
	 * @param AnkleRoll. The value of the Left's ankle elbow roll joint.
	 * */
	void forwardLeftLeg(kmatTable & EndTransf, float HipYawPitch, float HipRoll, float HipPitch, float KneePitch, float AnklePitch, float AnkleRoll){
		kmatTable base,T1,T2,T3,T4,T5,T6,R,endTr;
		float PI = KMatTransf::PI;
		
		KMatTransf::makeTranslationMatrix(base, 0.0f,HipOffsetY,-HipOffsetZ);
		KMatTransf::makeTransformationMatrix(T1,0.0f,-3*PI/4,0.0f,HipYawPitch-PI/2);
		KMatTransf::makeTransformationMatrix(T2,0.0f,-PI/2,0.0f,HipRoll+PI/4);
		KMatTransf::makeTransformationMatrix(T3,0.0f,PI/2,0.0f,HipPitch);
		KMatTransf::makeTransformationMatrix(T4,-ThighLength,0.0f,0.0f,KneePitch);
		KMatTransf::makeTransformationMatrix(T5,-TibiaLength,0.0f,0.0f,AnklePitch);
		KMatTransf::makeTransformationMatrix(T6,0.0f,-PI/2,0.0f,AnkleRoll);
		
		KMatTransf::rotationMatrixZYX(R,PI,-PI/2,0.0f);
		KMatTransf::makeTranslationMatrix(endTr,0.0f,0.0f,-FootHeight);
		kmatTable Tend;
		Tend = base;
		Tend *= T1;
		Tend *= T2;
		Tend *= T3;
		Tend *= T4;
		Tend *= T5;
		Tend *= T6;
		Tend *= R;
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
	 * @param AnklePitch. The value of the Right's ankle pitch roll joint.
	 * @param AnkleRoll. The value of the Right's ankle elbow roll joint.
	 * */
	void forwardRightLeg(kmatTable & EndTransf, float HipYawPitch, float HipRoll, float HipPitch, float KneePitch, float AnklePitch, float AnkleRoll){
		kmatTable base,T1,T2,T3,T4,T5,T6,R,endTr;
		float PI = KMatTransf::PI;
		
		KMatTransf::makeTranslationMatrix(base, 0.0f,-HipOffsetY,-HipOffsetZ);
		KMatTransf::makeTransformationMatrix(T1,0.0f,-PI/4,0.0f,HipYawPitch-PI/2);
		KMatTransf::makeTransformationMatrix(T2,0.0f,-PI/2,0.0f,HipRoll-PI/4);//allagh
		KMatTransf::makeTransformationMatrix(T3,0.0f,PI/2,0.0f,HipPitch);
		KMatTransf::makeTransformationMatrix(T4,-ThighLength,0.0f,0.0f,KneePitch);
		KMatTransf::makeTransformationMatrix(T5,-TibiaLength,0.0f,0.0f,AnklePitch);//allagh
		KMatTransf::makeTransformationMatrix(T6,0.0f,-PI/2,0.0f,AnkleRoll);
		
		KMatTransf::rotationMatrixZYX(R,PI,-PI/2,0.0f);
		KMatTransf::makeTranslationMatrix(endTr,0.0f,0.0f,-FootHeight);
		kmatTable Tend;
		Tend = base;
		Tend *= T1;
		Tend *= T2;
		Tend *= T3;
		Tend *= T4;
		Tend *= T5;
		Tend *= T6;
		Tend *= R;
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
			float AP = *iter;
			iter++;
			float AR = *iter;
			forwardLeftLeg(Tmatrix,HYP,HR,HP,KP,AP,AR);
		}else if(!WhatForward.compare("RightLeg")){
			float HYP = *iter;
			iter++;
			float HR = *iter;
			iter++;
			float HP = *iter;
			iter++;
			float KP = *iter;
			iter++;
			float AP = *iter;
			iter++;
			float AR = *iter;
			forwardRightLeg(Tmatrix,HYP,HR,HP,KP,AP,AR);
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
	
	/**
	 * @fn FKvars calculateCenterOfMass(vector<float> allJoints)
	 * @brief Calculate the center of mass of the robot
	 * @param allJoints. all the joint of the robot. They must be Larm,Rarm,Lleg,Rleg,Head with that order.
	 * */
	//Makaronada code
	FKvars calculateCenterOfMass(vector<float> allJoints){
		kmatTable base,T1,T2,T3,T4,T5,T6,endTr1,endTr2,endTr3,endTr4,endTr5,endTr6,temp;
		KMat::GenMatrix<float,3,1> lh1,lh2,lh3,lh4,rh1,rh2,rh3,rh4,ll1,ll2,ll3,ll4,ll5,ll6,rl1,rl2,rl3,rl4,rl5,rl6,h1,h2,t;
		float PI = KMatTransf::PI;
		//Left Hand
		KMatTransf::makeTranslationMatrix(endTr1, LShoulderPitchX,LShoulderPitchY,LShoulderPitchZ);
		KMatTransf::makeTranslationMatrix(endTr2, LShoulderRollX,LShoulderRollY,LShoulderRollZ);
		KMatTransf::makeTranslationMatrix(endTr3, LElbowYawX,LElbowYawY,LElbowYawZ);
		KMatTransf::makeTranslationMatrix(endTr4, LElbowRollX,LElbowRollY,LElbowRollZ);
		KMatTransf::makeTranslationMatrix(base, 0.0f,ShoulderOffsetY + ElbowOffsetY,ShoulderOffsetZ);
		KMatTransf::makeTransformationMatrix(T1,0.0f,-PI/2,0.0f,allJoints.front());
		allJoints.erase(allJoints.begin());
		KMatTransf::makeTransformationMatrix(T2,0.0f,PI/2,0.0f,allJoints.front()-PI/2);
		allJoints.erase(allJoints.begin());
		KMatTransf::makeTransformationMatrix(T3,0.0f,-PI/2,UpperArmLength,allJoints.front());
		allJoints.erase(allJoints.begin());
		KMatTransf::makeTransformationMatrix(T4,0.0f,PI/2,0.0f,allJoints.front());
		allJoints.erase(allJoints.begin());
		base*=T1;
		temp=base;
		temp*=endTr1;
		lh1=temp.get_translation();
		lh1.scalar_mult(LShoulderPitchMass);
		base*=T2;
		temp=base;
		temp*=endTr2;
		lh2=temp.get_translation();
		lh2.scalar_mult(LShoulderRollMass);
		base*=T3;
		temp=base;
		temp*=endTr3;
		lh3=temp.get_translation();
		lh3.scalar_mult(LElbowYawMass);
		base*=T4;
		temp=base;
		temp*=endTr4;
		lh4=temp.get_translation();
		lh4.scalar_mult(LElbowRollMass);
		lh1+=lh2;
		lh1+=lh3;
		lh1+=lh4;
		//Right Hand
		KMatTransf::makeTranslationMatrix(endTr1, RShoulderPitchX,RShoulderPitchY,RShoulderPitchZ);
		KMatTransf::makeTranslationMatrix(endTr2, RShoulderRollX,RShoulderRollY,RShoulderRollZ);
		KMatTransf::makeTranslationMatrix(endTr3, RElbowYawX,RElbowYawY,RElbowYawZ);
		KMatTransf::makeTranslationMatrix(endTr4, RElbowRollX,RElbowRollY,RElbowRollZ);
		
		KMatTransf::makeTranslationMatrix(base, 0.0f,-(ShoulderOffsetY+ElbowOffsetY),allJoints.front());
		KMatTransf::makeTransformationMatrix(T1,0.0f,-PI/2,0.0f,allJoints.front());
		allJoints.erase(allJoints.begin());
		KMatTransf::makeTransformationMatrix(T2,0.0f,PI/2,0.0f,allJoints.front()+PI/2);
		allJoints.erase(allJoints.begin());
		KMatTransf::makeTransformationMatrix(T3,0.0f,-PI/2,-UpperArmLength,allJoints.front());
		allJoints.erase(allJoints.begin());
		KMatTransf::makeTransformationMatrix(T4,0.0f,PI/2,0.0f,allJoints.front());
		allJoints.erase(allJoints.begin());
		base*=T1;
		temp=base;
		temp*=endTr1;
		rh1=temp.get_translation();
		rh1.scalar_mult(RShoulderPitchMass);
		base*=T2;
		temp=base;
		temp*=endTr2;
		rh2=temp.get_translation();
		rh2.scalar_mult(RShoulderRollMass);
		base*=T3;
		temp=base;
		temp*=endTr3;
		rh3=temp.get_translation();
		rh3.scalar_mult(RElbowYawMass);
		base*=T4;
		temp=base;
		temp*=endTr4;
		rh4=temp.get_translation();
		rh4.scalar_mult(RElbowRollMass);
		rh1+=rh2;
		rh1+=rh3;
		rh1+=rh4;
		//Left Leg
		KMatTransf::makeTranslationMatrix(endTr1, LHipYawPitchX,LHipYawPitchY,LHipYawPitchZ);
		KMatTransf::makeTranslationMatrix(endTr2, LHipRollX,LHipRollY,LHipRollZ);
		KMatTransf::makeTranslationMatrix(endTr3, LHipPitchX,LHipPitchY,LHipPitchZ);
		KMatTransf::makeTranslationMatrix(endTr4, LKneePitchX,LKneePitchY,LKneePitchZ);
		KMatTransf::makeTranslationMatrix(endTr5, LAnklePitchX,LAnklePitchY,LAnklePitchZ);
		KMatTransf::makeTranslationMatrix(endTr6, LAnkleRollX,LAnkleRollY,LAnkleRollZ);
		
		KMatTransf::makeTranslationMatrix(base, 0.0f,HipOffsetY,-HipOffsetZ);
		KMatTransf::makeTransformationMatrix(T1,0.0f,-3*PI/4,0.0f,allJoints.front()-PI/2);
		allJoints.erase(allJoints.begin());
		KMatTransf::makeTransformationMatrix(T2,0.0f,-PI/2,0.0f,allJoints.front()+PI/4);
		allJoints.erase(allJoints.begin());
		KMatTransf::makeTransformationMatrix(T3,0.0f,PI/2,0.0f,allJoints.front());
		allJoints.erase(allJoints.begin());
		KMatTransf::makeTransformationMatrix(T4,-ThighLength,0.0f,0.0f,allJoints.front());
		allJoints.erase(allJoints.begin());
		KMatTransf::makeTransformationMatrix(T5,-TibiaLength,0.0f,0.0f,allJoints.front());
		allJoints.erase(allJoints.begin());
		KMatTransf::makeTransformationMatrix(T6,0.0f,-PI/2,0.0f,allJoints.front());
		allJoints.erase(allJoints.begin());
		base*=T1;
		temp=base;
		temp*=endTr1;
		ll1=temp.get_translation();
		ll1.scalar_mult(LHipYawPitchMass);
		base*=T2;
		temp=base;
		temp*=endTr2;
		ll2=temp.get_translation();
		ll2.scalar_mult(LHipRollMass);
		base*=T3;
		temp=base;
		temp*=endTr3;
		ll3=temp.get_translation();
		ll3.scalar_mult(LHipPitchMass);
		base*=T4;
		temp=base;
		temp*=endTr4;
		ll4=temp.get_translation();
		ll4.scalar_mult(LKneePitchMass);
		base*=T5;
		temp=base;
		temp*=endTr5;
		ll5=temp.get_translation();
		ll5.scalar_mult(LAnklePitchMass);
		base*=T6;
		temp=base;
		temp*=endTr6;
		ll6=temp.get_translation();
		ll6.scalar_mult(LAnkleRollMass);
		ll1+=ll2;
		ll1+=ll3;
		ll1+=ll4;
		ll1+=ll5;
		ll1+=ll6;
		//Right Leg
		KMatTransf::makeTranslationMatrix(endTr1, RHipYawPitchX,RHipYawPitchY,RHipYawPitchZ);
		KMatTransf::makeTranslationMatrix(endTr2, RHipRollX,RHipRollY,RHipRollZ);
		KMatTransf::makeTranslationMatrix(endTr3, RHipPitchX,RHipPitchY,RHipPitchZ);
		KMatTransf::makeTranslationMatrix(endTr4, RKneePitchX,RKneePitchY,RKneePitchZ);
		KMatTransf::makeTranslationMatrix(endTr5, RAnklePitchX,RAnklePitchY,RAnklePitchZ);
		KMatTransf::makeTranslationMatrix(endTr6, RAnkleRollX,RAnkleRollY,RAnkleRollZ);
		
		KMatTransf::makeTranslationMatrix(base, 0.0f,-HipOffsetY,-HipOffsetZ);
		KMatTransf::makeTransformationMatrix(T1,0.0f,-PI/4,0.0f,allJoints.front()-PI/2);
		allJoints.erase(allJoints.begin());
		KMatTransf::makeTransformationMatrix(T2,0.0f,-PI/2,0.0f,allJoints.front()-PI/4);
		allJoints.erase(allJoints.begin());
		KMatTransf::makeTransformationMatrix(T3,0.0f,PI/2,0.0f,allJoints.front());
		allJoints.erase(allJoints.begin());
		KMatTransf::makeTransformationMatrix(T4,-ThighLength,0.0f,0.0f,allJoints.front());
		allJoints.erase(allJoints.begin());
		KMatTransf::makeTransformationMatrix(T5,-TibiaLength,0.0f,0.0f,allJoints.front());
		allJoints.erase(allJoints.begin());
		KMatTransf::makeTransformationMatrix(T6,0.0f,-PI/2,0.0f,allJoints.front());
		allJoints.erase(allJoints.begin());
		base*=T1;
		temp=base;
		temp*=endTr1;
		rl1=temp.get_translation();
		rl1.scalar_mult(RHipYawPitchMass);
		base*=T2;
		temp=base;
		temp*=endTr2;
		rl2=temp.get_translation();
		rl2.scalar_mult(RHipRollMass);
		base*=T3;
		temp=base;
		temp*=endTr3;
		rl3=temp.get_translation();
		rl3.scalar_mult(RHipPitchMass);
		base*=T4;
		temp=base;
		temp*=endTr4;
		rl4=temp.get_translation();
		rl4.scalar_mult(RKneePitchMass);
		base*=T5;
		temp=base;
		temp*=endTr5;
		rl5=temp.get_translation();
		rl5.scalar_mult(RAnklePitchMass);
		base*=T6;
		temp=base;
		temp*=endTr6;
		rl6=temp.get_translation();
		rl6.scalar_mult(RAnkleRollMass);
		rl1+=rl2;
		rl1+=rl3;
		rl1+=rl4;
		rl1+=rl5;
		rl1+=rl6;
		//Head
		KMatTransf::makeTranslationMatrix(endTr1, HeadYawX,HeadYawY,HeadYawZ);
		KMatTransf::makeTranslationMatrix(endTr2, HeadPitchX,HeadPitchY,HeadPitchZ);
		
		KMatTransf::makeTranslationMatrix(base, 0.0f,0.0f,NeckOffsetZ);
		KMatTransf::makeTransformationMatrix(T1,0.0f,0.0f,0.0f,allJoints.front());
		allJoints.erase(allJoints.begin());
		KMatTransf::makeTransformationMatrix(T2,0.0f,-PI/2,0.0f,allJoints.front()-PI/2);
		allJoints.erase(allJoints.begin());
		base*=T1;
		temp=base;
		temp*=endTr1;
		h1=temp.get_translation();
		h1.scalar_mult(HeadYawMass);
		base*=T2;
		temp=base;
		temp*=endTr2;
		h2=temp.get_translation();
		h2.scalar_mult(HeadPitchMass);
		h1+=h2;
		//Torso		
		t(0,0)=TorsoX;
		t(1,0)=TorsoY;
		t(2,0)=TorsoZ;
		t.scalar_mult(TorsoMass);
		t+=lh1;
		t+=rh1;
		t+=ll1;
		t+=rl1;
		t+=h1;
		float tmass = 1/TotalMassH21;
		t.scalar_mult(tmass);
		FKvars FKVariables;
		FKVariables.pointX = t(0,0);
		FKVariables.pointY = t(1,0);
		FKVariables.pointZ = t(2,0);
		FKVariables.angleZ = 0;
		FKVariables.angleY = 0;
		FKVariables.angleX = 0;
		return FKVariables;
	}
	
}

#endif

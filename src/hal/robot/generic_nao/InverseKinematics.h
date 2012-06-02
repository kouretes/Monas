#ifndef __INVKIN_H__
#define __INVKIN_H__
#include <iostream>
#include <stdexcept>
#include <string.h>
#include <vector>
#include <iomanip>
#include <limits>
#include <math.h>
#include "hal/robot/generic_nao/KinematicsDefines.h"
//#include "KinematicsDefines.h"
#include "ForwardKinematics.h"
/**
 * This is the code for the Invers Kinematics for nao v3.3 and v4 robot.
 
 * @author Kofinas Nikos aka eldr4d, 2012 kouretes team
 * special thanks to Orf or vosk for the KMat library
 *
	
 * DON'T TRY to understand the code, it's just chaotic maths. If u want the maths
 * e-mail me (nikofinas@gmail.com) for the matlab files.
 * \file InverseKinematics.h
*/

namespace IKin
{
	typedef KMat::transformations KMatTransf;
	/**
	 * vector<vector<float> > inverseHead(float px,float py,float pz, float rx, float ry, float rz, bool withAngles, bool topCamera)
	 * @brief Inverse Kinematics for the head (DON'T try to understand the code, it's just maths)
	 * @param px. The x cartesian coordinate.
	 * @param py. The y cartesian coordinate.
	 * @param pz. The z cartesian coordinate.
	 * @param ax. The x rotation. For the head it's every time 0.0f so don't bother.
	 * @param ay. The y rotation.
	 * @param az. The z rotation.
	 * @param wihtAngles. If true, the problem will be solved only with angles, else only with the cartesian points.
	 * @param topCamera. If true, the top camera is chosen as end point, else the bottom camera is chosen.
	 * @returns vector<vector<float> >. It returns n vectors of float where n is the number of solutions (almost every time it's 0 or 1).
		Each solutions vector contains the angles with this order: HeadYaw,HeadPitch.
	 * */
	vector<vector<float> > inverseHead(float px,float py,float pz, float rx, float ry, float rz, bool withAngles, bool topCamera){
		std::vector<float> fc,empty;
		std::vector<vector<float> > returnResult;
		float PI = KMatTransf::PI;
		FKin::kmatTable T;
		FKin::FKvars output;
		KMatTransf::makeTransformation(T,px,py,pz,rx,ry,rz);	
		float theta1,theta2;
		if(rx!=0.0f && withAngles)
			return returnResult;
		if(withAngles){
			theta1 = rz;
			theta2 = ry;
			//---------------------------Forward validation step--------------------------------------------------------------------------------------
			fc.clear();
			fc.push_back(theta1);fc.push_back(theta2);
			if(topCamera)
				output = FKin::filterForwardFromTo("Torso","CameraTop",empty,fc);
			else
				output = FKin::filterForwardFromTo("Torso","CameraBot",empty,fc);
			float x = output.pointX,y = output.pointY,z = output.pointZ,ax = output.angleX,ay=output.angleY,az=output.angleZ;
			if(px-1<=x && x<=px+1 && py-1<=y && y<=py+1 && pz-1<=z && z<=pz+1 && rx-0.001<=ax && rx+0.001>=ax && ry-0.001<=ay && ry+0.001>=ay && rz-0.001<=az && rz+0.001>=az){
				returnResult.push_back(fc);
			}
			//------------------------------------------------------------------------------------------------------------------------------------------
		}else{
			float up = - pz + NeckOffsetZ;
			float downt2,downt1,psi;
			if(topCamera){
				downt2 = sqrt(pow(CameraTopX,2)+pow(CameraTopZ,2));
				psi = atan2(CameraTopX,CameraTopZ);
			}else{
				downt2 = sqrt(pow(CameraBotomX,2)+pow(CameraBotomZ,2));
				psi = atan2(CameraBotomX,CameraBotomZ);
			}
			float tempTheta2 = asin(up/downt2);//-psi;
			for(int j=0;j<2;j++){
				if(j == 0 && (tempTheta2 - psi > HeadYawHigh || tempTheta2 - psi < HeadYawLow))
					continue;
				else if(j == 1 && (PI - tempTheta2 - psi > HeadYawHigh || PI - tempTheta2 - psi < HeadYawLow))
					continue;
				else if(j == 1)
					theta2 = PI - tempTheta2 - psi;
				else
					theta2 = tempTheta2 - psi;
				if(topCamera){
					downt1 = CameraTopZ*cos(theta2) - CameraTopX*sin(theta2);
				}else{
					downt1 = CameraBotomZ*cos(theta2) - CameraBotomX*sin(theta2);
				}
				theta2 = theta2 + PI/2;
				theta1 = acos(px/downt1);
				for(int i=0;i<2;i++){
					if(i == 0 && (theta1 > HeadPitchHigh || theta1 < HeadPitchLow))
						continue;
					else if(i == 1 && (-theta1 > HeadPitchHigh || -theta1 < HeadPitchLow))
						continue;
					else if(i == 0)
						theta1 = theta1;
					else
						theta1 = -theta1;
				}
				//---------------------------Forward validation step--------------------------------------------------------------------------------------
				fc.clear();
				fc.push_back(theta1);fc.push_back(theta2);
				if(topCamera)
					output = FKin::filterForwardFromTo("Torso","CameraTop",empty,fc);
				else
					output = FKin::filterForwardFromTo("Torso","CameraBot",empty,fc);
				float x = output.pointX,y = output.pointY,z = output.pointZ;
				//Validate only the points
				if(px-1<=x && x<=px+1 && py-1<=y && y<=py+1 && pz-1<=z && z<=pz+1){
					returnResult.push_back(fc);
				}
				//-----------------------------------------------------------------------------------------------------------------------------------------
			}
		}
		
		return returnResult;
	}
	
	/**
	 * vector<vector<float> > inverseLeftHand(float px,float py,float pz, float rx, float ry, float rz)
	 * @brief Inverse Kinematics for the left hand (DON'T try to understand the code, it's just maths)
	 * @param px. The x cartesian coordinate.
	 * @param py. The y cartesian coordinate.
	 * @param pz. The z cartesian coordinate.
	 * @param ax. The x rotation.
	 * @param ay. The y rotation.
	 * @param az. The z rotation.
	 * @returns vector<vector<float> >. It returns n vectors of float where n is the number of solutions (almost every time it's 0 or 1).
		Each solutions vector contains the angles with this order: LShoulderPitch,LShoulderRoll,LElbowYaw,LElbowRoll
	 * */
	vector<vector<float> > inverseLeftHand(float px,float py,float pz, float rx, float ry, float rz){
		std::vector<float> flh,empty;
		std::vector<vector<float> > returnResult;
		FKin::kmatTable Tinit;
		KMatTransf::makeTransformation(Tinit,px,py,pz,rx,ry,rz);
		float PI = KMatTransf::PI;
		float startX = 0;
		float startY = ShoulderOffsetY + ElbowOffsetY;
		float startZ = ShoulderOffsetZ;
		float side1 = UpperArmLength;
		float side2 = LowerArmLength + HandOffsetX; 
		float value1 = ShoulderOffsetY + ElbowOffsetY; //113
		float value2 = LowerArmLength + HandOffsetX; //113.7
		float value3 = UpperArmLength; //to allo
		//Calculate Theta 4
		float distance = sqrt(pow(startX - px,2) + pow(startY - py,2) + pow(startZ -pz,2));
		float theta4 = PI - acos((pow(side1,2)+pow(side2,2) - pow(distance,2))/(2*side1*side2));
		theta4 = - theta4;
		if(theta4!=theta4 || theta4 > LElbowRollHigh || theta4 < LElbowRollLow)
			return returnResult;
		float cth4 = cos(theta4);
		float sth4 = sin(theta4);
	
		//Calculate Theta 2
		float equationForTheta2 = sin(rz)*sin(rx)*sin(ry) + cos(rz)*cos(rx);
		float upForTheta2 = py - value1 - (equationForTheta2*value2*sth4)/cth4;
		float downForTheta2 = value3 + value2*cth4 + value2*pow(sth4,2)/cth4;
		float theta2temp = acos(upForTheta2/downForTheta2);
		for(int i=0;i<2;i++){
			float theta2 = theta2temp;
			if(i == 0 && (theta2 + PI/2 > LShoulderRollHigh || theta2 + PI/2 < LShoulderRollLow))
				continue;
			else if(i == 1 && (-theta2 + PI/2 > LShoulderRollHigh || -theta2 + PI/2 < LShoulderRollLow))
				continue;
			else if(i == 0)
				theta2 = theta2 + PI/2;
			else
				theta2 = -theta2 + PI/2;
			//Calculate Theta 3
			float equationForTheta3 = sin(rz)*sin(ry)*cos(rx) - cos(rz)*sin(rx);
			float upForTheta3 = equationForTheta3;
			float downForTheta3 = sin(theta2-PI/2);
			float theta3temp = asin(upForTheta3/downForTheta3);
			for(int j=0;j<2;j++){
				float theta3 = theta3temp;
				if(j == 0 && (theta3 > LElbowYawHigh || theta3 < LElbowYawLow))
					continue;
				else if(j == 1 && (PI - theta3 > LElbowYawHigh || PI - theta3 < LElbowYawLow))
					continue;
				else if(j == 1)
					theta3 = PI - theta3;
				//Calculate Theta 1
				float equation1ForTheta1 = cos(rz)*sin(ry)*cos(rx) + sin(rz)*sin(rx);
				float equation2ForTheta1 = cos(ry)*cos(rx);
				float theta1temp;
				if(cos(theta3) == 0){
					theta1temp = acos(equation1ForTheta1/cos(theta2 - PI/2));
				}else{
					float upForTheta1 = equation2ForTheta1+equation1ForTheta1*sin(theta3)*cos(theta2 - PI/2)/cos(theta3);
					float downForTheta1 = cos(theta3) + pow(cos(theta2 - PI/2),2)*pow(sin(theta3),2)/cos(theta3);
					theta1temp = acos(upForTheta1/downForTheta1);
				}
				for(int l=0;l<2;l++){
					float theta1 = theta1temp;
					if(l == 0 && (theta1 > LShoulderPitchHigh || theta1 < LShoulderPitchLow))
						continue;
					else if(l == 1 && (-theta1 > LShoulderPitchHigh || -theta1 < LShoulderPitchLow))
						continue;
					else if(l == 1)
						theta1 = -theta1;
					//---------------------------Forward validation step--------------------------------------------------------------------------------------
					flh.clear();
					flh.push_back(theta1);flh.push_back(theta2);flh.push_back(theta3);flh.push_back(theta4);
					FKin::kmatTable test;						
					FKin::filterForward(test,"LeftHand", flh);
					if(test.almostEqualTo(Tinit)){
						std::cout << "Niki!!\nTheta 1 = " << theta1 << "\nTheta 2 = " << theta2 << "\nTheta 3 = " << theta3 << "\nTheta 4 = " << theta4 << std::endl;							
						returnResult.push_back(flh);
					}
					//------------------------------------------------------------------------------------------------------------------------------------------
				}
			}
		
		
		}
		
		return returnResult;
	}

	/**
	 * vector<vector<float> > inverseRightHand(float px,float py,float pz, float rx, float ry, float rz)
	 * @brief Inverse Kinematics for the right hand (DON'T try to understand the code, it's just maths)
	 * @param px. The x cartesian coordinate.
	 * @param py. The y cartesian coordinate.
	 * @param pz. The z cartesian coordinate.
	 * @param ax. The x rotation.
	 * @param ay. The y rotation.
	 * @param az. The z rotation.
	 * @returns vector<vector<float> >. It returns n vectors of float where n is the number of solutions (almost every time it's 0 or 1).
		Each solutions vector contains the angles with this order: RShoulderPitch,RShoulderRoll,RElbowYaw,RElbowRoll
	 * */
	vector<vector<float> > inverseRightHand(float px,float py,float pz, float rx, float ry, float rz){
		std::vector<float> frh,empty;
		std::vector<vector<float> > returnResult;
		float PI = KMatTransf::PI;

		//Rotate input to remvoe Rfix
		FKin::kmatTable T,Rfix,Tinit;
		KMatTransf::makeTransformation(T,px,py,pz,rx,ry,rz);
		Tinit = T;	
		KMatTransf::makeRotationXYZ(Rfix,0.0f,0.0f,-PI);
		Rfix.fast_invert();
		T*=Rfix;
		px = T(0,3);
		py = T(1,3);
		pz = T(2,3);
		rx = atan2(T(2,1),T(2,2));
		ry = atan2(-T(2,0),sqrt(pow(T(2,1),2)+pow(T(2,2),2)));
		rz = atan2(T(1,0),T(0,0));
		//continue with the rotated input
		float startX = 0;
		float startY = -ShoulderOffsetY - ElbowOffsetY;
		float startZ = ShoulderOffsetZ;

		float side1 = UpperArmLength;
		float side2 = LowerArmLength + HandOffsetX; 

		float value1 = ShoulderOffsetY + ElbowOffsetY; //113
		float value2 = LowerArmLength + HandOffsetX; //113.7
		float value3 = UpperArmLength; //to allo
		//Calculate Theta 4
		float distance = sqrt(pow(startX - px,2) + pow(startY - py,2) + pow(startZ -pz,2));
		float theta4 = PI - acos((pow(side1,2)+pow(side2,2) - pow(distance,2))/(2*side1*side2));
		theta4 = theta4;
		if(theta4!=theta4 || theta4 > RElbowRollHigh || theta4 < RElbowRollLow)
			return returnResult;
		float cth4 = cos(theta4);
		float sth4 = sin(theta4);
	
		//Calculate Theta 2
		float equationForTheta2 = sin(rz)*sin(rx)*sin(ry) + cos(rz)*cos(rx);
		float upForTheta2 = - py - value1 - (equationForTheta2*value2*sth4)/cth4;
		float downForTheta2 = value3 + value2*cth4 + value2*pow(sth4,2)/cth4;
		float theta2temp = acos(upForTheta2/downForTheta2);

		for(int i=0;i<2;i++){
			float theta2 = theta2temp;
			if(i == 0 && (theta2 - PI/2 > RShoulderRollHigh || theta2 - PI/2 < RShoulderRollLow))
				continue;
			else if(i == 1 && (-theta2 - PI/2 > RShoulderRollHigh || -theta2 - PI/2 < RShoulderRollLow))
				continue;
			else if(i == 0)
				theta2 = theta2 - PI/2;
			else
				theta2 = -theta2 - PI/2;
			//Calculate Theta 3
			float equationForTheta3 = sin(rz)*sin(ry)*cos(rx) - cos(rz)*sin(rx);
			float upForTheta3 = equationForTheta3;
			float downForTheta3 = sin(theta2+PI/2);
			float theta3temp = asin(upForTheta3/downForTheta3);
			for(int j=0;j<2;j++){
				float theta3 = theta3temp;
				if(j == 0 && (theta3 > RElbowYawHigh || theta3 < RElbowYawLow))
					continue;
				else if(j == 1 && (PI - theta3 > RElbowYawHigh || PI - theta3 < RElbowYawLow))
					continue;
				else if(j == 1)
					theta3 = PI - theta3;
				//Calculate Theta 1
				float equation1ForTheta1 = cos(rz)*sin(ry)*cos(rx) + sin(rz)*sin(rx);
				float equation2ForTheta1 = cos(ry)*cos(rx);
				float theta1temp;
				if(cos(theta3) == 0){
					theta1temp = acos(equation1ForTheta1/cos(theta2 - PI/2));
				}else{
					float upForTheta1 = equation2ForTheta1+equation1ForTheta1*sin(theta3)*cos(theta2 + PI/2)/cos(theta3);
					float downForTheta1 = cos(theta3) + pow(cos(theta2 + PI/2),2)*pow(sin(theta3),2)/cos(theta3);
					theta1temp = acos(upForTheta1/downForTheta1);
				}
				for(int l=0;l<2;l++){
					float theta1 = theta1temp;
					if(l == 0 && (theta1 > RShoulderPitchHigh || theta1 < RShoulderPitchLow))
						continue;
					else if(l == 1 && (-theta1 > RShoulderPitchHigh || -theta1 < RShoulderPitchLow))
						continue;
					else if(l == 1)
						theta1 = -theta1;
					//---------------------------Forward validation step--------------------------------------------------------------------------------------
					frh.clear();
					frh.push_back(theta1);frh.push_back(theta2);frh.push_back(theta3);frh.push_back(theta4);
					FKin::kmatTable test;						
					FKin::filterForward(test,"RightHand", frh);
					if(test.almostEqualTo(Tinit)){
						std::cout << "Niki!!\nTheta 1 = " << theta1 << "\nTheta 2 = " << theta2 << "\nTheta 3 = " << theta3 << "\nTheta 4 = " << theta4 << std::endl;							
						returnResult.push_back(frh);
					}
					//-----------------------------------------------------------------------------------------------------------------------------------------
					
				}
			}
		
		
		}
		frh.clear();
		return returnResult;
	}
	/**
	 * vector<vector<float> > inverseLeftLeg(float px,float py,float pz, float rx, float ry, float rz)
	 * @brief Inverse Kinematics for the left leg (DON'T try to understand the code, it's just maths)
	 * @param px. The x cartesian coordinate.
	 * @param py. The y cartesian coordinate.
	 * @param pz. The z cartesian coordinate.
	 * @param ax. The x rotation.
	 * @param ay. The y rotation.
	 * @param az. The z rotation.
	 * @returns vector<vector<float> >. It returns n vectors of float where n is the number of solutions (almost every time it's 0 or 1).
		Each solutions vector contains the angles with this order: LHipYawPitch,LHipRoll,LHipPitch,LKneePitch,LAnklePitch,LAnkleRoll
	 * */
	vector<vector<float> > inverseLeftLeg(float px,float py,float pz, float rx, float ry, float rz){
		std::vector<float> fll,empty;
		std::vector<vector<float> > returnResult;
		float PI = KMatTransf::PI;
		FKin::kmatTable T,base,T4,T5,T6,R,endTr,Rot,Tstart,Ttemp,Ttemp2,TtempTheta5,Tinit;
		KMatTransf::makeTransformation(T,px,py,pz,rx,ry,rz);	
		Tinit=T;
		KMatTransf::makeTranslation(base, 0.0f,HipOffsetY,-HipOffsetZ);	
		KMatTransf::makeTranslation(endTr,0.0f,0.0f,-FootHeight);
		KMatTransf::makeRotationXYZ(Rot,PI/4,0.0f,0.0f);
		//Move the start point to the hipyawpitch point
		base.fast_invert();
		base *= T;
		//Move the end point to the anklePitch joint
		endTr.fast_invert();
		base *= endTr;
		//Rotate hipyawpitch joint
		Rot *= base;
		//Invert the table, because we need the chain from the ankle to the hip
		Tstart = Rot;
		Rot.fast_invert();
		T = Rot;
		//Build the rotation table
		KMatTransf::makeRotationZYX(R,PI,-PI/2,0.0f);
		float startX = 0;
		float startY = 0;
		float startZ = 0;
		float side1 = ThighLength;
		float side2 = TibiaLength; 
		
		float pxInvert = T(0,3);
		float pyInvert = T(1,3);
		float pzInvert = T(2,3);
		//Calculate Theta 4
		float distance = sqrt(pow(startX - pxInvert,2) + pow(startY - pyInvert,2) + pow(startZ -pzInvert,2));
		float theta4 = PI - acos((pow(side1,2)+pow(side2,2) - pow(distance,2))/(2*side1*side2));
		
		if(theta4!=theta4 || theta4 < LKneePitchLow || theta4 > LKneePitchHigh)
			return returnResult;
		
		KMatTransf::makeDHTransformation(T4,-ThighLength,0.0f,0.0f,theta4);
		float theta6 = atan(pyInvert/pzInvert);
		
		if(theta6 < LAnkleRollLow || theta6 > LAnkleRollHigh)
			return returnResult;
		//cout << "theta6 = " << theta6 << endl;
		KMatTransf::makeDHTransformation(T6,0.0f,-PI/2,0.0f,theta6);
		T6 *= R;
		try{
			T6.fast_invert();
			Tstart *= T6;
			TtempTheta5 = Tstart;
			TtempTheta5.fast_invert();
		}catch(KMat::SingularMatrixInvertionException d){
			return returnResult;
		}

		float up = TtempTheta5(1,3)*(TibiaLength + ThighLength*cos(theta4)) + ThighLength*TtempTheta5(0,3)*sin(theta4);
		float down = pow(ThighLength,2)*pow(sin(theta4),2)+pow(TibiaLength + ThighLength*cos(theta4),2);
		float theta5= asin(-up/down);
		if(theta5 != theta5 && up/down<0)
			theta5 = -PI/2;
		else if(theta5 != theta5)
			theta5 = PI/2;
			
		for(int i=0;i<2;i++){
			if(i == 0 && (theta5 > LAnklePitchHigh || theta5 < LAnklePitchLow))
				continue;
			else if(i == 1 && (PI - theta5 > LAnklePitchHigh || PI - theta5 < LAnklePitchLow))
				continue;
			else if(i == 1)
				theta5 = PI - theta5;
			KMatTransf::makeDHTransformation(T5,-TibiaLength,0.0f,0.0f,theta5);
			Ttemp = T4;
			Ttemp *= T5; 
			try{
				Ttemp.fast_invert();
			}catch(KMat::SingularMatrixInvertionException d){
				continue;			
			}
			Ttemp2 = Tstart;
			Ttemp2 *= Ttemp;
			float temptheta2 = acos(Ttemp2(1,2));
			float theta2;
      		for(int l=0;l<2;l++){
      			if(l == 0 && (temptheta2 - PI/4> LHipRollHigh || temptheta2 -PI/4 < LHipRollLow))
					continue;
				else if(l == 1 && (-temptheta2 - PI/4> LHipRollHigh || -temptheta2 -PI/4 < LHipRollLow))
					continue;
				else if(l==0)
					theta2 = temptheta2 - PI/4;
				else if(l == 1)
					theta2 = -temptheta2 - PI/4;
				float theta3 = asin(Ttemp2(1,1)/sin(theta2+PI/4));
				if(theta3 != theta3 && Ttemp2(1,1)/sin(theta2+PI/4)<0)
					theta3 = -PI/2;
				else if(theta3 != theta3)
					theta3 = PI/2;
				for(int k=0;k<2;k++){
					if(k == 0 && (theta3 > LHipPitchHigh || theta3 < LHipPitchLow))
						continue;
					else if(k == 1 && (PI - theta3 > LHipPitchHigh || PI - theta3 < LHipPitchLow))
						continue;
					else if(k == 1)
						theta3 = PI - theta3;
					float temptheta1 = acos(Ttemp2(0,2)/sin(theta2+PI/4));
					if(temptheta1 != temptheta1)
						temptheta1 = 0;
					for(int p=0;p<2;p++){
						float theta1;
						if(p == 0 && (temptheta1 + PI/2> LHipYawPitchHigh || -temptheta1 + PI/2 < LHipYawPitchLow))
							continue;
						else if(p == 1 && (-temptheta1 + PI/2> LHipYawPitchHigh || - temptheta1 + PI/2< LHipYawPitchLow))
							continue;
						else if(p == 0)
							theta1 = temptheta1 + PI/2;
						else if(p == 1)
							theta1 = -temptheta1 + PI/2;
						
						//--------------------------------------Forward validation step------------------------------------------------------------------------------
						fll.clear();
						fll.push_back(theta1);fll.push_back(theta2);fll.push_back(theta3);fll.push_back(theta4);fll.push_back(theta5);fll.push_back(theta6);
						FKin::kmatTable test;						
						FKin::filterForward(test,"LeftLeg", fll);
						if(test.almostEqualTo(Tinit)){
							//std::cout << "Niki!!\nTheta 1 = " << theta1 << "\nTheta 2 = " << theta2 << "\nTheta 3 = " << theta3 << "\nTheta 4 = " << theta4  << "\nTheta 5 = " << theta5 << "\nTheta 6 = " << theta6 << std::endl;							
							returnResult.push_back(fll);
						}
						//---------------------------------------------------------------------------------------------------------------------------------------------
					}
				}
			}
		}
		return returnResult;
	}

	/**
	 * vector<vector<float> > inverseRightLeg(float px,float py,float pz, float rx, float ry, float rz)
	 * @brief Inverse Kinematics for the right leg (DON'T try to understand the code, it's just maths)
	 * @param px. The x cartesian coordinate.
	 * @param py. The y cartesian coordinate.
	 * @param pz. The z cartesian coordinate.
	 * @param ax. The x rotation.
	 * @param ay. The y rotation.
	 * @param az. The z rotation.
	 * @returns vector<vector<float> >. It returns n vectors of float where n is the number of solutions (almost every time it's 0 or 1).
		Each solutions vector contains the angles with this order: RHipYawPitch,RHipRoll,RHipPitch,RKneePitch,RAnklePitch,RAnkleRoll
	 * */
	vector<vector<float> > inverseRightLeg(float px,float py,float pz, float rx, float ry, float rz){
		std::vector<float> frl,empty;
		std::vector<vector<float> > returnResult;
		float PI = KMatTransf::PI;
		FKin::kmatTable T,base,T4,T5,T6,R,endTr,Rot,Tstart,Ttemp,Ttemp2,TtempTheta5,Tinit;
		KMatTransf::makeTransformation(T,px,py,pz,rx,ry,rz);
		Tinit=T;
		KMatTransf::makeTranslation(base, 0.0f,-HipOffsetY,-HipOffsetZ);	
		KMatTransf::makeTranslation(endTr,0.0f,0.0f,-FootHeight);
		KMatTransf::makeRotationXYZ(Rot,-PI/4,0.0f,0.0f);
		//Move the start point to the hipyawpitch point
		base.fast_invert();
		base *= T;
		//Move the end point to the anklePitch joint
		endTr.fast_invert();
		base *= endTr;
		//Rotate hipyawpitch joint
		Rot *= base;
		//Invert the table, because we need the chain from the ankle to the hip
		Tstart = Rot;
		Rot.fast_invert();

		T = Rot;
		//Build the rotation table
		KMatTransf::makeRotationZYX(R,PI,-PI/2,0.0f);
		float startX = 0;
		float startY = 0;
		float startZ = 0;
		float side1 = ThighLength;
		float side2 = TibiaLength; 
		
		float pxInvert = T(0,3);
		float pyInvert = T(1,3);
		float pzInvert = T(2,3);
		//Calculate Theta 4
		float distance = sqrt(pow(startX - pxInvert,2) + pow(startY - pyInvert,2) + pow(startZ -pzInvert,2));
		float theta4 = PI - acos((pow(side1,2)+pow(side2,2) - pow(distance,2))/(2*side1*side2));
		if(theta4!=theta4 || theta4 < RKneePitchLow || theta4 > RKneePitchHigh)
			return returnResult;
		KMatTransf::makeDHTransformation(T4,-ThighLength,0.0f,0.0f,theta4);
		float theta6 = atan(pyInvert/pzInvert);
		
		if(theta6 < RAnkleRollLow || theta6 > RAnkleRollHigh)
			return returnResult;

		KMatTransf::makeDHTransformation(T6,0.0f,-PI/2,0.0f,theta6);
		T6 *= R;
		try{
			T6.fast_invert();
			Tstart *= T6;
			TtempTheta5 = Tstart;
			TtempTheta5.fast_invert();
		}catch(KMat::SingularMatrixInvertionException d){
			return returnResult;
		}

		float up = TtempTheta5(1,3)*(TibiaLength + ThighLength*cos(theta4)) + ThighLength*TtempTheta5(0,3)*sin(theta4);
		float down = pow(ThighLength,2)*pow(sin(theta4),2)+pow(TibiaLength + ThighLength*cos(theta4),2);
		float theta5= asin(-up/down);
		if(theta5 != theta5 && up/down<0)
			theta5 = -PI/2;
		else if(theta5 != theta5)
			theta5 = PI/2;
			
		for(int i=0;i<2;i++){
			if(i == 0 && (theta5 > RAnklePitchHigh || theta5 < RAnklePitchLow))
				continue;
			else if(i == 1 && (PI - theta5 > RAnklePitchHigh || PI - theta5 < RAnklePitchLow))
				continue;
			else if(i == 1)
				theta5 = PI - theta5;
			KMatTransf::makeDHTransformation(T5,-TibiaLength,0.0f,0.0f,theta5);
			Ttemp = T4;
			Ttemp *= T5; 
			try{
				Ttemp.fast_invert();
			}catch(KMat::SingularMatrixInvertionException d){
				continue;			
			}
			Ttemp2 = Tstart;
			Ttemp2 *= Ttemp;
			float temptheta2 = acos(Ttemp2(1,2));
			float theta2;
      		for(int l=0;l<2;l++){
      			if(l == 0 && (temptheta2 + PI/4> RHipRollHigh || temptheta2 + PI/4 < RHipRollLow))
					continue;
				else if(l == 1 && (-temptheta2 + PI/4>RHipRollHigh || -temptheta2 + PI/4 < RHipRollLow))
					continue;
				else if(l==0)
					theta2 = temptheta2 + PI/4;
				else if(l == 1)
					theta2 = -temptheta2 + PI/4;
				float theta3 = asin(Ttemp2(1,1)/sin(theta2-PI/4));
				if(theta3 != theta3 && Ttemp2(1,1)/sin(theta2-PI/4)<0)
					theta3 = -PI/2;
				else if(theta3 != theta3)
					theta3 = PI/2;
				for(int k=0;k<2;k++){
					if(k == 0 && (theta3 > RHipPitchHigh || theta3 < RHipPitchLow))
						continue;
					else if(k == 1 && (PI - theta3 > RHipPitchHigh || PI - theta3 < RHipPitchLow))
						continue;
					else if(k == 1)
						theta3 = PI - theta3;
					float temptheta1 = acos(Ttemp2(0,2)/sin(theta2-PI/4));
					if(temptheta1 != temptheta1)
						temptheta1 = 0;
					for(int p=0;p<2;p++){
						float theta1;
						if(p == 0 && (temptheta1 + PI/2> RHipYawPitchHigh || -temptheta1 + PI/2 < RHipYawPitchLow))
							continue;
						else if(p == 1 && (-temptheta1 + PI/2> RHipYawPitchHigh || - temptheta1 + PI/2< RHipYawPitchLow))
							continue;
						else if(p == 0)
							theta1 = temptheta1 + PI/2;
						else if(p == 1)
							theta1 = -temptheta1 + PI/2;
						//--------------------------------------Forward validation step------------------------------------------------------------------------------
						frl.clear();
						frl.push_back(theta1);frl.push_back(theta2);frl.push_back(theta3);frl.push_back(theta4);frl.push_back(theta5);frl.push_back(theta6);
						FKin::kmatTable test;						
						FKin::filterForward(test,"RightLeg", frl);
						if(test.almostEqualTo(Tinit)){
						//	std::cout << "Px = " << px << " Py = " << py << " Pz = " << pz << "Ax = " << rx << " Ay = " << ry << " Az = " << rz << std::endl;
						//	std::cout << "Niki!!\nTheta 1 = " << theta1 << "\nTheta 2 = " << theta2 << "\nTheta 3 = " << theta3 << "\nTheta 4 = " << theta4  << "\nTheta 5 = " << theta5 << "\nTheta 6 = " << theta6 << std::endl;							
							returnResult.push_back(frl);
						}
						//---------------------------------------------------------------------------------------------------------------------------------------------
					}
				}
			}
		}
		return returnResult;
	}
	
}
#endif

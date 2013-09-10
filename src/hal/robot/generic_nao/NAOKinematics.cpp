#include "NAOKinematics.h"
#include "hal/robot/generic_nao/KinematicsDefines.h"


using namespace KMath;
using namespace KMath::KMat;
using namespace KDeviceLists;

float PI=M_PI;
float PI_2=M_PI_2;
NAOKinematics::	NAOKinematics() :T(FR_SIZE),joints(FR_SIZE),coms(FR_SIZE)
	{
		kmatTable t1,t2;
		for (unsigned i=0;i<FR_SIZE;i++)
			T[i].identity();

		//Base and End  Transforms
		//--------HEAD
		KMatTransf::makeTranslation(T[FR_BASE_T+CHAIN_HEAD], 0.0f, 0.0f, NeckOffsetZ);
		KMatTransf::makeRotationXYZ(T[FR_END_T+CHAIN_HEAD],  PI_2,  PI_2, 0.0f);


		//--------LARM
		KMatTransf::makeTranslation(T[FR_BASE_T+CHAIN_L_ARM], 0.0f,  ShoulderOffsetY, ShoulderOffsetZ);
		KMatTransf::makeRotationXYZ(T[FR_END_T+CHAIN_L_ARM], 0.0f, 0.0f, -PI_2);
		KMatTransf::makeTranslation(t1, (HandOffsetX + LowerArmLength),0.0f,HandOffsetZ );
		T[FR_END_T+CHAIN_L_ARM]*=t1;


		//--------RARM


		KMatTransf::makeTranslation(T[FR_BASE_T+CHAIN_R_ARM], 0.0f, -(ShoulderOffsetY), ShoulderOffsetZ);
		KMatTransf::makeRotationXYZ(T[FR_END_T+CHAIN_R_ARM], 0.0f, 0.0f, -PI_2);
		KMatTransf::makeTranslation(t1, (HandOffsetX + LowerArmLength),0.0f,HandOffsetZ );
		T[FR_END_T+CHAIN_R_ARM]*=t1;

		RotRArmFixInv=t1;
		RotRArmFixInv.fast_invert();

		//------LLEG
		KMatTransf::makeTranslation(T[FR_BASE_T+CHAIN_L_LEG], 0.0f, HipOffsetY, -HipOffsetZ);
		TBaseLLegInv=T[FR_BASE_T+CHAIN_L_LEG];
		TBaseLLegInv.fast_invert();

		KMatTransf::makeRotationZYX(T[FR_END_T+CHAIN_L_LEG], PI ,  -PI_2, 0.0f);


		KMatTransf::makeTranslation(t1, 0.0f, 0.0f, -FootHeight);
		T[FR_END_T+CHAIN_L_LEG]*=t1;

		TEndLLegInv=t1;
		TEndLLegInv.fast_invert();

		KMatTransf::makeRotationXYZ(RotFixLLeg,  PI_2 / 2, 0.0f, 0.0f);


		//-----RLEG
		KMatTransf::makeTranslation(T[FR_BASE_T+CHAIN_R_LEG], 0.0f, -HipOffsetY, -HipOffsetZ);
		TBaseRLegInv=T[FR_BASE_T+CHAIN_R_LEG];
		TBaseRLegInv.fast_invert();
		KMatTransf::makeRotationZYX(T[FR_END_T+CHAIN_R_LEG],  PI ,  -PI_2, 0.0f);


		RotRLeg=T[FR_END_T+CHAIN_R_LEG];
		KMatTransf::makeTranslation(t1, 0.0f, 0.0f, -FootHeight);
		T[FR_END_T+CHAIN_R_LEG]*=t1;
		TEndRLegInv=t1;
		TEndRLegInv.fast_invert();

		KMatTransf::makeRotationXYZ(RotFixRLeg,  -PI_2 / 2, 0.0f, 0.0f);

		//Effectors
		KMatTransf::makeTranslation(T[FR_END_T+EFF_CAMERA_BOT], CameraBotomX, 0.0f, CameraBotomZ);
		KMatTransf::makeTranslation(T[FR_END_T+EFF_CAMERA_TOP], CameraTopX, 0.0f, CameraTopZ);


		//---Calculate Coms------
		coms[HEAD+YAW]=KVecFloat3(HeadYawX,HeadYawY,HeadYawZ).scalar_mult(HeadYawMass);
		coms[HEAD+PITCH]=KVecFloat3(HeadPitchX,HeadPitchY,HeadPitchZ).scalar_mult(HeadPitchMass);


		coms[L_ARM+SHOULDER_PITCH]=KVecFloat3(LShoulderPitchX,LShoulderPitchY,LShoulderPitchZ)
									.scalar_mult(LShoulderPitchMass);
		coms[L_ARM+SHOULDER_ROLL]=KVecFloat3(LShoulderRollX,LShoulderRollY,LShoulderRollZ)
									.scalar_mult(LShoulderRollMass);
		coms[L_ARM+ELBOW_YAW]=KVecFloat3(LElbowYawX,LElbowYawY,LElbowYawZ)
									.scalar_mult(LElbowYawMass);
		coms[L_ARM+ELBOW_ROLL]=KVecFloat3(LElbowRollX,LElbowRollY,LElbowRollZ)
									.scalar_mult(LElbowRollMass);


		coms[R_ARM+SHOULDER_PITCH]=KVecFloat3(RShoulderPitchX,RShoulderPitchY,RShoulderPitchZ)
									.scalar_mult(RShoulderPitchMass);
		coms[R_ARM+SHOULDER_ROLL]=KVecFloat3(RShoulderRollX,RShoulderRollY,RShoulderRollZ)
									.scalar_mult(RShoulderRollMass);
		coms[R_ARM+ELBOW_YAW]=KVecFloat3(RElbowYawX,RElbowYawY,RElbowYawZ)
									.scalar_mult(RElbowYawMass);
		coms[R_ARM+ELBOW_ROLL]=KVecFloat3(RElbowRollX,RElbowRollY,RElbowRollZ)
									.scalar_mult(RElbowRollMass);

		prepareForward();

	}

bool NAOKinematics::setJoints(std::vector<float> jointsset)
{

	if(jointsset.size() != KDeviceLists::NUMOFJOINTS)
		return false;
	joints=jointsset;
	return prepareForward();
}
bool NAOKinematics::prepareForward(KDeviceLists::ChainsNames ch)
{

	if(ch==CHAIN_L_ARM||ch==CHAINS_SIZE)
	{
		KMatTransf::makeDHTransformation(T[L_ARM+SHOULDER_PITCH],0.0f, -PI_2, 0.0f, joints[L_ARM+SHOULDER_PITCH]);
		KMatTransf::makeDHTransformation(T[L_ARM+SHOULDER_ROLL], 0.0f, PI_2, 0.0f, joints[L_ARM+SHOULDER_ROLL] + PI_2);
		KMatTransf::makeDHTransformation(T[L_ARM+ELBOW_YAW], +ElbowOffsetY, PI_2, UpperArmLength, joints[L_ARM+ELBOW_YAW] );
		KMatTransf::makeDHTransformation(T[L_ARM+ELBOW_ROLL], 0.0f, -PI_2, 0.0f, joints[L_ARM+ELBOW_ROLL]);
	}

	if(ch==CHAIN_R_ARM||ch==CHAINS_SIZE)
	{
		KMatTransf::makeDHTransformation(T[R_ARM+SHOULDER_PITCH], 0.0f, -PI_2, 0.0f, joints[R_ARM+SHOULDER_PITCH]);
		KMatTransf::makeDHTransformation(T[R_ARM+SHOULDER_ROLL], 0.0f, PI_2, 0.0f, joints[R_ARM+SHOULDER_ROLL] + PI_2); //Allagh apo matlab
		KMatTransf::makeDHTransformation(T[R_ARM+ELBOW_YAW], -ElbowOffsetY, PI_2, UpperArmLength, joints[R_ARM+ELBOW_YAW]);
		KMatTransf::makeDHTransformation(T[R_ARM+ELBOW_ROLL], 0.0f, -PI_2, 0.0f, joints[R_ARM+ELBOW_ROLL]); //Allagh apo matlab
	}

	if(ch==CHAIN_L_LEG||ch==CHAINS_SIZE)
	{
		KMatTransf::makeDHTransformation(T[L_LEG+HIP_YAW_PITCH], 0.0f, -3 * PI_2 / 2, 0.0f, joints[L_LEG+HIP_YAW_PITCH] - PI_2);
		KMatTransf::makeDHTransformation(T[L_LEG+HIP_ROLL], 0.0f, -PI_2, 0.0f, joints[L_LEG+HIP_ROLL] + PI_2 / 2);
		KMatTransf::makeDHTransformation(T[L_LEG+HIP_PITCH], 0.0f, PI_2, 0.0f, joints[L_LEG+HIP_PITCH]);
		KMatTransf::makeDHTransformation(T[L_LEG+KNEE_PITCH], -ThighLength, 0.0f, 0.0f, joints[L_LEG+KNEE_PITCH]);
		KMatTransf::makeDHTransformation(T[L_LEG+ANKLE_PITCH], -TibiaLength, 0.0f, 0.0f, joints[L_LEG+ANKLE_PITCH]);
		KMatTransf::makeDHTransformation(T[L_LEG+ANKLE_ROLL], 0.0f, -PI_2, 0.0f, joints[L_LEG+ANKLE_ROLL]);
	}

	if(ch==CHAIN_R_LEG||ch==CHAINS_SIZE)
	{
		KMatTransf::makeDHTransformation(T[R_LEG+HIP_YAW_PITCH], 0.0f, -PI_2 / 2, 0.0f, joints[R_LEG+HIP_YAW_PITCH] - PI_2);
		KMatTransf::makeDHTransformation(T[R_LEG+HIP_ROLL], 0.0f, -PI_2, 0.0f, joints[R_LEG+HIP_ROLL] - PI_2 / 2); //allagh
		KMatTransf::makeDHTransformation(T[R_LEG+HIP_PITCH], 0.0f, PI_2, 0.0f, joints[R_LEG+HIP_PITCH]);
		KMatTransf::makeDHTransformation(T[R_LEG+KNEE_PITCH], -ThighLength, 0.0f, 0.0f, joints[R_LEG+KNEE_PITCH]);
		KMatTransf::makeDHTransformation(T[R_LEG+ANKLE_PITCH], -TibiaLength, 0.0f, 0.0f, joints[R_LEG+ANKLE_PITCH]); //allagh
		KMatTransf::makeDHTransformation(T[R_LEG+ANKLE_ROLL], 0.0f, -PI_2, 0.0f,  joints[R_LEG+ANKLE_ROLL]);
	}

	if(ch==CHAIN_HEAD||ch==CHAINS_SIZE)
	{
		KMatTransf::makeDHTransformation(T[HEAD+YAW], 0.0f, 0.0f, 0.0f, joints[HEAD+YAW]);
		KMatTransf::makeDHTransformation(T[HEAD+PITCH], 0.0f, -PI_2, 0.0f, joints[HEAD+PITCH] - PI_2);
	}
	for( unsigned i=0;i<T.size();i++)
		T[i].check();

	return true;
}

NAOKinematics::kmatTable  NAOKinematics::getForwardEffector(Effectors ef)
{
	unsigned chsize;
	Frames frstart;
	Frames baseframe=(Frames) ef;

	switch(ef)
	{

	case EFF_CAMERA_BOT:
	case EFF_CAMERA_TOP:
			baseframe=(Frames)CHAIN_HEAD;
	case CHAIN_HEAD:
			frstart=(Frames)HEAD;
			chsize=HEAD_SIZE;
			break;
	case CHAIN_L_LEG:
			frstart=(Frames)L_LEG;
			chsize=LEG_SIZE;
			break;
	case CHAIN_R_LEG:
			frstart=(Frames)R_LEG;
			chsize=LEG_SIZE;
			break;
	case CHAIN_L_ARM:
			frstart=(Frames)L_ARM;
			chsize=ARM_SIZE;
			break;
	case CHAIN_R_ARM:
			frstart=(Frames)R_ARM;
			chsize=ARM_SIZE;
			break;
	default :
			return kmatTable().identity();
	}
	kmatTable t;
	t=T[FR_BASE_T+baseframe];
	for (unsigned i=0;i<chsize;i++)
		t*=T[FR_BASE+frstart+i];
	t*=T[FR_END_T+ef];



	return t;
}


NAOKinematics::kmatTable NAOKinematics::getForwardFromTo(Effectors start, Effectors stop)
{

	kmatTable t1= getForwardEffector(start);
	t1.fast_invert();
	t1*=getForwardEffector(stop);
	return t1;
}

KVecFloat3 NAOKinematics::calculateCenterOfMass()
{

}

std::vector<std::vector<float> > NAOKinematics::inverseHead(const FKvars s, bool withAngles, bool topCamera)
{
	return inverseHead(getTransformation(s), withAngles, topCamera);
}

std::vector<std::vector<float> > NAOKinematics::inverseHead(kmatTable targetPoint, bool withAngles, bool topCamera)
{
	kmatTable T;
	kmatTable outputT;
	std::vector<std::vector<float> > returnResult;
	T = targetPoint;
	float theta1, theta2;

	if(T(0,3) != 0.0f && withAngles)
		return returnResult;

	if(withAngles)
	{
		theta1 = atan2(T(1,0), T(0,0));
		theta2 = atan2(-T(2,0), sqrt(pow(T(2,1), 2) + pow(T(2,2), 2)));
		std::vector<float> r(HEAD_SIZE);
		r[YAW]=theta1;
		r[PITCH]=theta2;
		returnResult.push_back(r);
	}
	else
	{
		float up = - T(2,3) + NeckOffsetZ;
		float downt2, downt1, psi;

		if(topCamera)
		{
			downt2 = sqrt(pow(CameraTopX, 2) + pow(CameraTopZ, 2));
			psi = atan2(CameraTopX, CameraTopZ);
		}
		else
		{
			downt2 = sqrt(pow(CameraBotomX, 2) + pow(CameraBotomZ, 2));
			psi = atan2(CameraBotomX, CameraBotomZ);
		}

		float tempTheta2 = asin(up / downt2); //-psi;
		float posOrNegPI = (tempTheta2 >= 0) ? PI : -PI;

		for(int j = 0; j < 2; j++)
		{
			if(j == 0 && (tempTheta2 - psi > HeadYawHigh || tempTheta2 - psi < HeadYawLow))
				continue;
			else if(j == 1 && (posOrNegPI - tempTheta2 - psi > HeadYawHigh || posOrNegPI - tempTheta2 - psi < HeadYawLow))
				continue;
			else if(j == 1)
				theta2 = posOrNegPI - tempTheta2 - psi;
			else
				theta2 = tempTheta2 - psi;

			if(topCamera)
			{
				downt1 = CameraTopZ * cos(theta2) - CameraTopX * sin(theta2);
			}
			else
			{
				downt1 = CameraBotomZ * cos(theta2) - CameraBotomX * sin(theta2);
			}

			theta2 = theta2 + PI_2;
			theta1 = acos(T(0,3) / downt1);

			for(int i = 0; i < 2; i++)
			{
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


			joints[HEAD+YAW]=theta1;
			joints[HEAD+PITCH]=theta2;
			prepareForward(CHAIN_HEAD);

			outputT = getForwardEffector(topCamera?EFF_CAMERA_TOP:EFF_CAMERA_BOT);
			KVecFloat3 r=targetPoint.getTranslation();
			r-=outputT.getTranslation();
			//Validate only the translation
			if(r.norm2()<3)
			{
				std::vector<float> r(HEAD_SIZE);
				r[YAW]=theta1;
				r[PITCH]=theta2;
				returnResult.push_back(r);
			}

			//-----------------------------------------------------------------------------------------------------------------------------------------
		}
	}

	return returnResult;
}

std::vector<std::vector<float> > NAOKinematics::inverseLeftHand(const FKvars s)
{
	return inverseLeftHand(getTransformation(s));
}

std::vector<std::vector<float> > NAOKinematics::inverseLeftHand(kmatTable targetPoint)
{
	std::vector<std::vector<float> > returnResult;
	kmatTable Tinit = targetPoint;
	float startX = 0;
	float startY = ShoulderOffsetY + ElbowOffsetY;
	float startZ = ShoulderOffsetZ;
	float side1 = UpperArmLength;
	float side2 = LowerArmLength + HandOffsetX;
	float value1 = ShoulderOffsetY + ElbowOffsetY; //113
	float value2 = LowerArmLength + HandOffsetX; //113.7
	float value3 = UpperArmLength; //to allo
	//Calculate Theta 4
	float distance = sqrt(pow(startX - Tinit(0,3), 2) + pow(startY - Tinit(1,3), 2) + pow(startZ - Tinit(2,3), 2));
	float theta4 = PI - acos((pow(side1, 2) + pow(side2, 2) - pow(distance, 2)) / (2 * side1 * side2));
	theta4 = - theta4;

	if(theta4 != theta4 || theta4 > LElbowRollHigh || theta4 < LElbowRollLow)
		return returnResult;

	float cth4 = cos(theta4);
	float sth4 = sin(theta4);
	//Calculate Theta 2
	float equationForTheta2 = Tinit(1,1);
	float upForTheta2 = Tinit(1,3) - value1 - (equationForTheta2 * value2 * sth4) / cth4;
	float downForTheta2 = value3 + value2 * cth4 + value2 * pow(sth4, 2) / cth4;
	float theta2temp = acos(upForTheta2 / downForTheta2);

	for(int i = 0; i < 2; i++)
	{
		float theta2 = theta2temp;

		if(i == 0 && (theta2 + PI_2 > LShoulderRollHigh || theta2 + PI_2 < LShoulderRollLow))
			continue;
		else if(i == 1 && (-theta2 + PI_2 > LShoulderRollHigh || -theta2 + PI_2 < LShoulderRollLow))
			continue;
		else if(i == 0)
			theta2 = theta2 + PI_2;
		else
			theta2 = -theta2 + PI_2;

		//Calculate Theta 3
		float equationForTheta3 = Tinit(1,2);
		float upForTheta3 = equationForTheta3;
		float downForTheta3 = sin(theta2 - PI_2);
		float theta3temp = asin(upForTheta3 / downForTheta3);
		float posOrNegPI = (theta3temp >= 0) ? PI : -PI;

		for(int j = 0; j < 2; j++)
		{
			float theta3 = theta3temp;

			if(j == 0 && (theta3 > LElbowYawHigh || theta3 < LElbowYawLow))
				continue;
			else if(j == 1 && (posOrNegPI - theta3 > LElbowYawHigh || posOrNegPI - theta3 < LElbowYawLow))
				continue;
			else if(j == 1)
				theta3 = posOrNegPI - theta3;

			//Calculate Theta 1
			float equation1ForTheta1 = Tinit(0,2);
			float equation2ForTheta1 = Tinit(2,2);
			float theta1temp;

			if(cos(theta3) == 0)
			{
				theta1temp = acos(equation1ForTheta1 / cos(theta2 - PI_2));
			}
			else
			{
				float upForTheta1 = equation2ForTheta1 + equation1ForTheta1 * sin(theta3) * cos(theta2 - PI_2) / cos(theta3);
				float downForTheta1 = cos(theta3) + pow(cos(theta2 - PI_2), 2) * pow(sin(theta3), 2) / cos(theta3);
				theta1temp = acos(upForTheta1 / downForTheta1);
			}

			for(int l = 0; l < 2; l++)
			{
				float theta1 = theta1temp;

				if(l == 0 && (theta1 > LShoulderPitchHigh || theta1 < LShoulderPitchLow))
					continue;
				else if(l == 1 && (-theta1 > LShoulderPitchHigh || -theta1 < LShoulderPitchLow))
					continue;
				else if(l == 1)
					theta1 = -theta1;

				//---------------------------Forward validation step--------------------------------------------------------------------------------------
				joints[L_ARM+SHOULDER_PITCH]=theta1;
				joints[L_ARM+SHOULDER_ROLL]=theta2;
				joints[L_ARM+ELBOW_YAW]=theta3;
				joints[L_ARM+ELBOW_ROLL]=theta4;
				prepareForward(CHAIN_L_ARM);
				kmatTable test=getForwardEffector((Effectors)CHAIN_L_ARM);
				if(test.almostEqualTo(Tinit))
				{
					std::vector<float> r(ARM_SIZE);
					r[SHOULDER_PITCH]=theta1;
					r[SHOULDER_ROLL]=theta2;
					r[ELBOW_YAW]=theta3;
					r[ELBOW_ROLL]=theta4;
					//std::cout << "Niki!!\nTheta 1 = " << theta1 << "\nTheta 2 = " << theta2 << "\nTheta 3 = " << theta3 << "\nTheta 4 = " << theta4 << std::endl;
					returnResult.push_back(r);
				}
				else
				{
					;//std::cout << "Hta!!\nTheta 1 = " << theta1 << "Theta 2 = " << theta2 << "Theta 3 = " << theta3 << "Theta 4 = " << theta4 << std::endl;
				}

				//------------------------------------------------------------------------------------------------------------------------------------------
			}
		}
	}

	return returnResult;
}

std::vector<std::vector<float> > NAOKinematics::inverseRightHand(const FKvars s)
{
	return inverseRightHand(getTransformation(s));
}

std::vector<std::vector<float> > NAOKinematics::inverseRightHand(kmatTable targetPoint)
{

}

std::vector<std::vector<float> > NAOKinematics::inverseLeftLeg(const FKvars s)
{
	return inverseLeftLeg(getTransformation(s));
}

std::vector<std::vector<float> > NAOKinematics::inverseLeftLeg(kmatTable targetPoint)
{
	std::vector<std::vector<float> > returnResult;
	kmatTable TtempTheta5,T4i,T5i,T6i,Ttemp,Ttemp2;
	kmatTable T = targetPoint;
	kmatTable Tinit = T;
	//Move the start point to the hipyawpitch point
	kmatTable base = TBaseLLegInv;
	base *= T;
	//Move the end point to the anklePitch joint
	base *= TEndLLegInv;
	//Rotate hipyawpitch joint
	kmatTable Rot = RotFixLLeg;
	Rot *= base;
	//Invert the table, because we need the chain from the ankle to the hip
	kmatTable Tstart = Rot;
	try
	{
		Rot.fast_invert();
	}
	catch(KMath::KMat::SingularMatrixInvertionException d)
	{
		return returnResult;
	}

	T = Rot;
	//Build the rotation table
	float side1 = ThighLength;
	float side2 = TibiaLength;
	//Calculate Theta 4
	float distancesqrd = T.getTranslation().norm2();
	float theta4 = PI - acos((pow(side1, 2) + pow(side2, 2) -distancesqrd) / (2 * side1 * side2));

	if(theta4 != theta4)
	{
		return returnResult;
	}


	float theta6 = atan(T(1,3) / T(2,3));

	if(theta6 < LAnkleRollLow || theta6 > LAnkleRollHigh)
		return returnResult;

	KMatTransf::makeDHTransformation(T6i, 0.0f, -PI_2, 0.0f, theta6);
	T6i *= RotRLeg;

	try
	{
		T6i.fast_invert();
		Tstart *= T6i;
		TtempTheta5 = Tstart;
		TtempTheta5.fast_invert();
	}
	catch(KMath::KMat::SingularMatrixInvertionException d)
	{
		return returnResult;
	}

	for(int itter = 0; itter < 2; itter++)
	{
		theta4 = (itter == 0) ? theta4 : -theta4;

		if(theta4 < RKneePitchLow || theta4 > RKneePitchHigh)
			continue;

		KMatTransf::makeDHTransformation(T4i, -ThighLength, 0.0f, 0.0f, theta4);
		float up = TtempTheta5(1, 3) * (TibiaLength + ThighLength * cos(theta4)) + ThighLength * TtempTheta5(0, 3) * sin(theta4);
		float down = pow(ThighLength, 2) * pow(sin(theta4), 2) + pow(TibiaLength + ThighLength * cos(theta4), 2);
		float theta5 = asin(-up / down);
		float posOrNegPIt5 = (theta5 >= 0) ? PI : -PI;

		if(theta5 != theta5 && up / down < 0)
			theta5 = -PI_2;
		else if(theta5 != theta5)
			theta5 = PI_2;

		for(int i = 0; i < 2; i++)
		{
			if(i == 0 && (theta5 > LAnklePitchHigh || theta5 < LAnklePitchLow))
				continue;
			else if(i == 1 && (posOrNegPIt5 - theta5 > LAnklePitchHigh || posOrNegPIt5 - theta5 < LAnklePitchLow))
				continue;
			else if(i == 1)
				theta5 = posOrNegPIt5 - theta5;

			KMatTransf::makeDHTransformation(T5i, -TibiaLength, 0.0f, 0.0f, theta5);
			Ttemp = T4i;
			Ttemp *= T5i;

			try
			{
				Ttemp.fast_invert();
			}
			catch(KMath::KMat::SingularMatrixInvertionException d)
			{
				continue;
			}

			Ttemp2 = Tstart;
			Ttemp2 *= Ttemp;
			float temptheta2 = acos(Ttemp2(1, 2));
			float theta2;

			for(int l = 0; l < 2; l++)
			{
				if(l == 0 && (temptheta2 - PI_2 / 2 > LHipRollHigh || temptheta2 - PI_2 / 2 < LHipRollLow))
					continue;
				else if(l == 1 && (-temptheta2 - PI_2 / 2 > LHipRollHigh || -temptheta2 - PI_2 / 2 < LHipRollLow))
					continue;
				else if(l == 0)
					theta2 = temptheta2 - PI_2 / 2;
				else if(l == 1)
					theta2 = -temptheta2 - PI_2 / 2;

				float theta3 = asin(Ttemp2(1, 1) / sin(theta2 + PI_2 / 2));
				float posOrNegPIt3 = (theta3 >= 0) ? PI : -PI;

				if(theta3 != theta3 && Ttemp2(1, 1) / sin(theta2 + PI_2 / 2) < 0)
					theta3 = -PI_2;
				else if(theta3 != theta3)
					theta3 = PI_2;

				for(int k = 0; k < 2; k++)
				{
					if(k == 0 && (theta3 > LHipPitchHigh || theta3 < LHipPitchLow))
						continue;
					else if(k == 1 && (posOrNegPIt3 - theta3 > LHipPitchHigh || posOrNegPIt3 - theta3 < LHipPitchLow))
						continue;
					else if(k == 1)
						theta3 = posOrNegPIt3 - theta3;

					float temptheta1 = acos(Ttemp2(0, 2) / sin(theta2 + PI_2 / 2));

					if(temptheta1 != temptheta1)
						temptheta1 = 0;

					for(int p = 0; p < 2; p++)
					{
						float theta1;

						if(p == 0 && (temptheta1 + PI_2 > LHipYawPitchHigh || -temptheta1 + PI_2 < LHipYawPitchLow))
							continue;
						else if(p == 1 && (-temptheta1 + PI_2 > LHipYawPitchHigh || - temptheta1 + PI_2 < LHipYawPitchLow))
							continue;
						else if(p == 0)
							theta1 = temptheta1 + PI_2;
						else if(p == 1)
							theta1 = -temptheta1 + PI_2;

						//--------------------------------------Forward validation step------------------------------------------------------------------------------

						joints[L_LEG+HIP_YAW_PITCH]=theta1;
						joints[L_LEG+HIP_ROLL]=theta2;
						joints[L_LEG+HIP_PITCH]=theta3;
						joints[L_LEG+KNEE_PITCH]=theta4;
						joints[L_LEG+ANKLE_PITCH]=theta5;
						joints[L_LEG+ANKLE_ROLL]=theta6;

						prepareForward(CHAIN_L_LEG);
						kmatTable test=getForwardEffector((Effectors)CHAIN_L_LEG);

						if(test.almostEqualTo(Tinit))
						{
							std::vector<float> r(LEG_SIZE);
							r[HIP_YAW_PITCH]=theta1;
							r[HIP_ROLL]=theta2;
							r[HIP_PITCH]=theta3;
							r[KNEE_PITCH]=theta4;
							r[ANKLE_PITCH]=theta5;
							r[ANKLE_ROLL]=theta6;
							//std::cout << "Niki!!\nTheta 1 = " << theta1 << "\nTheta 2 = " << theta2 << "\nTheta 3 = " << theta3 << "\nTheta 4 = " << theta4  << "\nTheta 5 = " << theta5 << "\nTheta 6 = " << theta6 << std::endl;
							returnResult.push_back(r);
						}

						//-
						//---------------------------------------------------------------------------------------------------------------------------------------------
					}
				}
			}
		}
	}
	return returnResult;
}

std::vector<std::vector<float> > NAOKinematics::inverseRightLeg(const FKvars s)
{
	return inverseRightLeg(getTransformation(s));
}

std::vector<std::vector<float> > NAOKinematics::inverseRightLeg(kmatTable targetPoint)
{

	//Mirror problem to other foot
	targetPoint(0,1)=-targetPoint(0,1);
	targetPoint(0,2)=-targetPoint(0,2);
	targetPoint(1,0)=-targetPoint(1,0);
	targetPoint(1,2)=-targetPoint(1,2);
	targetPoint(2,0)=-targetPoint(2,0);
	targetPoint(2,1)=-targetPoint(2,1);
	targetPoint(1,3)=-targetPoint(1,3);


	std::vector<std::vector<float> > res=inverseLeftLeg(targetPoint);
	for(unsigned i=0;i<res.size();i++)
	{

		//(res[i])[HIP_YAW_PITCH]=-(res[i])[HIP_YAW_PITCH];
		(res[i])[HIP_ROLL]=-(res[i])[HIP_ROLL];
		(res[i])[ANKLE_ROLL]=-(res[i])[ANKLE_ROLL];
	}


	return res;
}


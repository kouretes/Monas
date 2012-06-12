#ifndef __DKIN_H__
#define __DKIN_H__

//Define for the lengths of the arms foots etc
#define ShoulderOffsetY 	98.0f
#define ElbowOffsetY		15.0f
#define UpperArmLength		105.0f
#define ShoulderOffsetZ		100.0f
#define LowerArmLength		57.75f
#define HandOffsetX			55.95f
#define HipOffsetZ			85.0f
#define HipOffsetY			50.0f
#define ThighLength			100.0f
#define TibiaLength			102.9f	
#define FootHeight			45.11f
#define NeckOffsetZ			126.5f 
#define CameraBotomX		48.8f
#define CameraBotomZ		23.81f 
#define CameraTopX			53.9f
#define CameraTopZ			67.9f

//Head Limits
#define HeadYawHigh			2.0857f
#define HeadYawLow			-2.0857f
#define HeadPitchHigh		0.5149f
#define HeadPitchLow		-0.6720f
//Left Hand limits
#define LShoulderPitchHigh	2.0857f
#define LShoulderPitchLow 	-2.0857f
#define LShoulderRollHigh	1.3265f
#define LShoulderRollLow 	-0.3142f
#define LElbowYawHigh		2.0875f
#define LElbowYawLow 		-2.0875f
#define LElbowRollHigh  	0.00001f//Aldebaran gives this value (-0.0349f) but the hand can go further
#define LElbowRollLow 		-1.5446f
//Right Hand limits
#define RShoulderPitchHigh 	2.0857f
#define RShoulderPitchLow 	-2.0857f
#define RShoulderRollHigh	0.3142f
#define RShoulderRollLow	-1.3265f
#define RElbowYawHigh 		2.0875f
#define RElbowYawLow 		-2.0875f
#define RElbowRollHigh		1.5446f
#define RElbowRollLow		0.00001f//Aldebaran gives this value (0.0349f) but the hand can go further
//Left Leg limits
#define LHipYawPitchHigh	0.7408f
#define LHipYawPitchLow		-1.1453f
#define LHipRollHigh		0.7904f
#define LHipRollLow			-0.3794f
#define LHipPitchHigh		0.4840f
#define LHipPitchLow		-1.7739f
#define LKneePitchHigh		2.1125f
#define LKneePitchLow		-0.0923f
#define LAnklePitchHigh		0.9227f
#define LAnklePitchLow		-1.1895f
#define LAnkleRollHigh		0.7690f
#define LAnkleRollLow		-0.3978f
//Left Right limits
#define RHipYawPitchHigh	0.7408f
#define RHipYawPitchLow		-1.1453f
#define RHipRollHigh		0.4147f
#define RHipRollLow			-0.7383f
#define RHipPitchHigh		0.4856f
#define RHipPitchLow		-1.7723f
#define RKneePitchHigh		2.1201f
#define RKneePitchLow		-0.1030f
#define RAnklePitchHigh		0.9320f
#define RAnklePitchLow		-1.1864f
#define RAnkleRollHigh		0.3886f
#define RAnkleRollLow		-1.1864f

//Masses defines
//Total mass
#define TotalMassH21		4.879f
//Torso
#define TorsoMass			1.03948f
#define TorsoX				-0.00415f
#define TorsoY				0.00007f
#define TorsoZ				0.04258f
//Head
#define HeadYawMass			0.05930f
#define HeadYawX			-0.00002f
#define HeadYawY			0.00017f
#define HeadYawZ			-0.02556f

#define HeadPitchMass		0.52065f
#define HeadPitchX			0.00120f
#define HeadPitchY			-0.00084f
#define HeadPitchZ			0.05353f
//Left Hand
#define LShoulderPitchMass	0.06996f
#define LShoulderPitchX		-0.00178f
#define LShoulderPitchY		0.02496f
#define LShoulderPitchZ		0.00018f

#define LShoulderRollMass	0.12309f
#define LShoulderRollX		0.01885f
#define LShoulderRollY		-0.00577f
#define LShoulderRollZ		0.00065f

#define LElbowYawMass		0.05971f
#define LElbowYawX			-0.02560f
#define LElbowYawY			0.00001f
#define LElbowYawZ			-0.00019f

#define LElbowRollMass		0.18500f
#define LElbowRollX			0.06536f
#define LElbowRollY			-0.00034f
#define LElbowRollZ			-0.00002f
//Right Hand
#define RShoulderPitchMass	0.06996f
#define RShoulderPitchX		-0.00178f
#define RShoulderPitchY		0.02496f
#define RShoulderPitchZ		0.00018f

#define RShoulderRollMass	0.12309f
#define RShoulderRollX		0.01885f
#define RShoulderRollY		-0.00577f
#define RShoulderRollZ		0.00065f

#define RElbowYawMass		0.05971f
#define RElbowYawX			-0.02560f
#define RElbowYawY			0.00001f
#define RElbowYawZ			-0.00019f

#define RElbowRollMass		0.18500f
#define RElbowRollX			0.06536f
#define RElbowRollY			-0.00034f
#define RElbowRollZ			-0.00002f
//LeftLeg Leg
#define LHipYawPitchMass	0.07117f
#define LHipYawPitchX		-0.00766f
#define LHipYawPitchY		0.01200f
#define LHipYawPitchZ		0.02717f

#define LHipRollMass		0.13530f
#define LHipRollX			-0.01649f
#define LHipRollY			-0.00029f
#define LHipRollZ			-0.00475f

#define LHipPitchMass		0.39421
#define LHipPitchX			0.00132f
#define LHipPitchY			-0.00235f
#define LHipPitchZ			-0.05352f

#define LKneePitchMass		0.29159
#define LKneePitchX			0.00422f
#define LKneePitchY			-0.00252f
#define LKneePitchZ			-0.04868f

#define LAnklePitchMass		0.13892
#define LAnklePitchX		0.00142f
#define LAnklePitchY		-0.00028f
#define LAnklePitchZ		0.00638f

#define LAnkleRollMass		0.16175
#define LAnkleRollX			0.02540f
#define LAnkleRollY			-0.00332f
#define LAnkleRollZ			-0.03241f
//Right Leg
#define RHipYawPitchMass	0.07117f
#define RHipYawPitchX		-0.00766f
#define RHipYawPitchY		0.01200f
#define RHipYawPitchZ		0.02717f

#define RHipRollMass		0.13530f
#define RHipRollX			-0.01649f
#define RHipRollY			-0.00029f
#define RHipRollZ			-0.00475f

#define RHipPitchMass		0.39421
#define RHipPitchX			0.00132f
#define RHipPitchY			-0.00235f
#define RHipPitchZ			-0.05352f

#define RKneePitchMass		0.29159
#define RKneePitchX			0.00422f
#define RKneePitchY			-0.00252f
#define RKneePitchZ			-0.04868f

#define RAnklePitchMass		0.13892
#define RAnklePitchX		0.00142f
#define RAnklePitchY		-0.00028f
#define RAnklePitchZ		0.00638f

#define RAnkleRollMass		0.16175
#define RAnkleRollX			0.02540f
#define RAnkleRollY			-0.00332f
#define RAnkleRollZ			-0.03241f
#endif

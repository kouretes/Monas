//
//  Stepplanner.h
//  Kouretes Walk Engine
//
//  Created by Stelios Piperakis on 8/14/13.
//  Copyright (c) 2013 SP. All rights reserved.
//
#ifndef STEPPLANNER_H_
#define STEPPLANNER_H_
#include "RobotParameters.h"
#include <core/elements/math/KMat.hpp>
#include <queue>

enum
{
	X = 0, Y, Z, Theta, SIZE_OF_TRAJECTORY_ELEMENTS
};
enum
{
	LEFT = 0, RIGHT
};

struct GroundPoint
{
		float x, y;
};

struct PlanePoint
{
		float x, y, theta;
};

class Stepplanner
{
	private:
		PlanePoint Pelvis;
		PlanePoint Step;
		RobotParameters Robot;
		GroundPoint ZMP;
		int stepcounter;
		float Time;
	public:
		std::queue<GroundPoint> ZmpQ;
		std::queue<PlanePoint> FootQ;
		Stepplanner();
		int getStepcounter();
		void initialize(RobotParameters);
		PlanePoint showPelvis();
		void oneStep(std::vector<float>);
		void afterStep();
		void finalStep();
		void clear();
		const std::queue<PlanePoint>& getFootQ() const
		{
			return FootQ;
		}

		void setFootQ(const std::queue<PlanePoint>& footQ)
		{
			FootQ = footQ;
		}

		const std::queue<GroundPoint>& getZmpQ() const
		{
			return ZmpQ;
		}

		void setZmpQ(const std::queue<GroundPoint>& zmpQ)
		{
			ZmpQ = zmpQ;
		}
};
#endif

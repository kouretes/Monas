#ifndef FORMATIONGENERATOR_H
#define FORMATIONGENERATOR_H

#include <iostream>
#include <vector>
#include "tools/mathcommon.h"

/**
 * @brief Formation Generator Interface used by the behavior to determine
 * the formation of the team. Given the global ball position (Shared World Model)
 * it generates positions on the formationField dynamically.
 *
 * @author Vagelis Mihelioudakis, KOURETES Team 2013
 *
 * \file FormationGenerator.h
*/

using std::vector;
using std::string;

class FormationGenerator {

private:
	
	/* factors to calculate positions regardless formationField dimensions, that way the
	 * formation is generated dynamically.
	 */

	// on ball role offset from the ball
	static const float ONBALL_OFFSET = -0.2;

	// support role factors
	static const float SUPPORT_FACTOR_X = 0.22;
	static const float SUPPORT_FACTOR_Y = 0.5;
	static const float SUPPORT_FORWARDING_FACTOR = 0.33;
	static const float SUPPORT_DEFENCE_FACTOR = 0.44;
	static const float SUPPORT_BOUND = 0.5;
	static const float MIDDLE_LANE_FACTOR = 0.268;

	// defender role factros
	static const float DEFENDER_FACTOR = 0.78;
	static const float DEFENDER_MOVEMENT = 0.2;

	enum Role {
		GOALIE = 0, DEFENDER = 1, ONBALL = 2, SUPPORTER_L = 3, SUPPORTER_R = 4
	};

	enum Type {
		OFFENSIVE = 0, DEFENSIVE = 1
	};

	struct posInfo {
		float X;
		float Y;
		Role role;
	};

	// may be used later for formation on larger robot teams
	int positions;
	vector<posInfo> *formation;
	Type formationType;
	
public:
	
	struct {
		float MaxX;
		float MinX;
		float MaxY;
		float MinY;
		float LeftPenaltyAreaMaxX;
		float LeftPenaltyAreaMinX;
		float LeftPenaltyAreaMaxY;
		float LeftPenaltyAreaMinY;
		float RightPenaltyAreaMaxX;
		float RightPenaltyAreaMinX;
		float RightPenaltyAreaMaxY;
		float RightPenaltyAreaMinY;
		float LeftGoalPost;
		float RightGoalPost;
	} Field;
	
	FormationGenerator();
	
	virtual ~FormationGenerator();
	
	void Init(int teamPlayers);

	void Generate(float ballX, float ballY);

	vector<posInfo>* getFormation();
	
};

#endif

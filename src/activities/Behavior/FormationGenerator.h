#ifndef FORMATIONGENERATOR_H
#define FORMATIONGENERATOR_H

#include <iostream>
#include <vector>
#include "core/architecture/configurator/Configurator.hpp"
#include "core/elements/math/Common.hpp"
#include "FormationParameters.h"
#include "tools/toString.h"

/**
 * @brief Formation Generator Interface used by the behavior to determine
 * the formation of the team. Given the global ball position (Shared World Model)
 * it generates positions on the field dynamically.
 *
 * @author Vagelis Mihelioudakis, Kouretes Team 2013
 *
 * \file FormationGenerator.h
*/

namespace FormationConsts{

	/*
	 * Factors to calculate positions regardless formation field dimensions, that way the
	 * formation is generated dynamically.
	 */

	// on ball role offset from the ball. The on ball role has always an offset of 0.2 on x axis.
	const float ONBALL_OFFSET = -0.2;

	// support roles factors
	const float SUPPORT_FACTOR_X = 0.22;
	const float SUPPORT_FACTOR_Y = 0.5;
	const float SUPPORT_FORWARDING_FACTOR = 0.33;
	const float SUPPORT_DEFENCE_FACTOR = 0.44;
	const float SUPPORT_BOUND = 0.5;
	const float MIDDLE_LANE_FACTOR = 0.268;

	// defender roles factros
	const float DEFENDER_FACTOR = 0.75;
	const float DEFENDER_MOVEMENT = 0.2;
	const float LEFT_DEFENDER_OFFSET = 0.55;
	const float RIGHT_DEFENDER_OFFSET = -0.55;

};

class FormationGenerator {

private:

	unsigned int positions; // number of positions to be generated.

	std::vector<FormationParameters::posInfo> *formation; // vector pointer that holds the formation positions.

	FormationParameters::Type formationType; // enumeration variable used to hold the formation type.

public:
	
	/**
	 * @struct field configiration
	 * @brief Used to store all values that we read from the field xml file. Values
	 * are updated by behavior Reset().
	 */
	struct FieldParameters {
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
		float DiameterCCircle;
	} Field;

	/**
	 * @fn FormationGenerator()
	 * @brief Empty Constructor.
	 */
	FormationGenerator();

	/**
	 * @fn virtual ~FormationGenerator()
	 * @brief Empty Destructor.
	 */
	virtual ~FormationGenerator();

	/**
	 * @fn void Init(unsigned int teamPlayers, bool kickOff)
	 * @brief Use to initialize the generator object and create the initial team
	 * fotmation. MUST be called after each empty constructor!
	 * Parameters:
	 * @teamPlayers number of team players, used to determine the number of positions
	 * needed to be produced on each formation generate.
	 */
	void Init(unsigned int teamPlayers, bool kickOff);
	
	/**
	 * @fn void InitXml(unsigned int teamPlayers, bool kickOff)
	 * @brief Use to initialize the generator object and create the initial team
	 * fotmation using xml file. MUST be called after each empty constructor!
	 * Parameters:
	 * @teamPlayers number of team players, used to determine the number of positions
	 * needed to be produced on each formation generate.
	 */
	void InitXml(unsigned int teamPlayers, bool kickOff);

	/**
	 * @fn void Generate(float ballX, float ballY)
	 * @brief Generate positions based on the ball coordinates (x,y). The ball
	 * position is a global estimation produced by the shared world model.
	 * Parameters:
	 * @ballX ball x coordinate
	 * @ballY ball y coordinate
	 */
	void Generate(float ballX, float ballY, bool ballFound);
	
	/**
	 * FormationParameters::posInfo find(FormationParameters::Role role)
	 * @brief Use to a specific role information on the formation vector.
	 */
	FormationParameters::posInfo findRoleInfo(FormationParameters::Role role);
	
	/**
	 * @fn vector<posInfo>* getFormation()
	 * @brief Use to get a vector pointer of the last generated positions.
	 */
	std::vector<FormationParameters::posInfo>* getFormation() {
		return formation;
	}
	
	/**
	 * @fn FormationParameters::Type getFormationType()
	 * @brief Use to get the type of the last generated formation.
	 */
	FormationParameters::Type getFormationType() {
		return formationType;
	}
	
};

#endif

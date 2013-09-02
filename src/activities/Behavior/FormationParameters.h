#ifndef FORMATIONPARAMETERS_H
#define FORMATIONPARAMETERS_H

#include <algorithm>
#include "tools/toString.h"

using std::vector;
using std::sort;

namespace FormationParameters {
	
	/**
	 * @enum Role
	 * @brief enum to attach roles on positions.
	 */
	enum Role {
		GOALIE,
		DEFENDER,
		DEFENDER_R,
		DEFENDER_L,
		ONBALL,
		SUPPORTER,
		SUPPORTER_L,
		SUPPORTER_R,
		ROLE_COUNT // NEVER add enum roles under that one, for obvious reasons.
	};

	/**
	 * Strings for all possible roles, used for debug, so please if you modify the Role enum
	 * also change that one!
	 */
	static const char* roleString[] = { "Goalie",
										"Defender",
										"Right Defender",
										"Left Defender",
									    "Attacker",
										"Supporter",
	 									"Left Supporter",
										"Right Supporter" };
	
	/**
	 * @fn inline unsigned int roles()
	 * @brief Returns the number of all possible roles.
	 */
	inline unsigned int roles() {
		return ROLE_COUNT;
	}
	
	/**
	 * @fn inline const char* getRoleString(unsigned int roleEnum)
	 * @brief Use to get the string value of the corresponing Role enum.
	 */
	inline const char* getRoleString(unsigned int roleEnum) {
 		return roleString[roleEnum];
	}
		
	/**
	 * @enum Type
	 * @brief enum to determine the type of formation.
	 */
	enum Type {
		OFFENSIVE,
		DEFENSIVE,
	};
	
	/**
	 * @struct posInfo (position Information)
	 * @brief Used to store information for each position that is generated.
	 */
	struct posInfo {
		float X;
		float Y;
		Role role;
		unsigned int id; // used for multiple roles of the same functionality, for example Supporter 0, Supporter 1...
	};
	
	/**
	 * @struct Robot (Robot Information)
	 * @brief Used to store information for each robot.
	 */
	struct Robot {
		unsigned int robotId;
		float robotX;
		float robotY;
		float robotPhi;
		float robotConfidence;
		unsigned int robotStability;
	};
	
	/**
	 * @fn inline bool compareRobots(const Robot &robot1, const Robot &robot2)
	 * @brief Comperator used to sort robots according to their Id value.
	 */
	inline bool compareRobots(const Robot &robot1, const Robot &robot2) {
    	return (robot1.robotId < robot2.robotId);
	}
	
	/**
	 * @fn inline void sortRobotsbyId(vector<Robot> &robots)
	 * @brief Custom sort function used to sort a vector of robot structs by
	 * Id value.
	 */
	inline void sortRobotsbyId(vector<Robot> &robots) {
		sort(robots.begin(), robots.end(), compareRobots);
	}
	
	/**
	 * @fn inline int getRobotIndex(vector<Robot> &robots, unsigned int robotId)
	 * @brief Use to get the robot index on the vector using the robot Id value (robot number).
	 * @return -1 if the robot does not exist.
	 */
	inline int getRobotIndex(vector<Robot> &robots, unsigned int robotId) {
		for(unsigned int r = 0 ; r < robots.size() ; r++) {
			if(robots[r].robotId == robotId)
				return r;
		}
		return -1;
	}
	
};

#endif

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

	static const char* roleString[] = { "Goalie",
										"Defender",
										"Right Defender",
										"Left Defender",
									    "Attacker",
										"Supporter",
	 									"Left Supporter",
										"Right Supporter" };

	inline unsigned int roles() {
		return ROLE_COUNT;
	}

	inline const char* getRoleString(int roleEnum) {
 		return roleString[roleEnum];
	}
		
	/**
	 * @enum Type
	 * @brief enum to determine the type of formation.
	 */
	enum Type {
		OFFENSIVE,
		DEFENSIVE,
		STATIC
	};
	
	/**
	 * @struct posInfo (position Information)
	 * @brief Used to store information for each position that is generated.
	 */
	struct posInfo {
		float X;
		float Y;
		Role role;
	};
	
	struct Robot {
		unsigned int robotId;
		float robotX;
		float robotY;
		float robotPhi;
		float robotConfidence;
		unsigned int robotStability;
	};
	
	inline bool compareRobots(const Robot &robot1, const Robot &robot2) {
    	return (robot1.robotId < robot2.robotId);
	}
	
	inline void sortRobotsbyId(vector<Robot> &robots) {
		sort(robots.begin(), robots.end(), compareRobots);
	}
	
	inline unsigned int getRobotIndex(vector<Robot> &robots, unsigned int robotId) {
		for(unsigned int r = 0 ; r < robots.size() ; r++) {
			if(robots[r].robotId == robotId)
				return r;
		}
		// THERE MUST BE AN ERROR RETURN VALUE OR EXCEPTION
	}
	
};

#endif

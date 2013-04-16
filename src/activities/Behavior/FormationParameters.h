#ifndef FORMATIONPARAMETERS_H
#define FORMATIONPARAMETERS_H

#include "core/elements/Singleton.hpp"

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
		ROLE_COUNT
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

};

#endif

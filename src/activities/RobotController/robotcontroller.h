/*! \file robotcontroller.h
 *	\brief A Monas Activity that reads game_state and button events and publishes
 *	\brief messages to Behavior and Led Handler. RobotController Sets colors to Chest Button and Feet Leds.
 *
 * \bug No known bugs, Yeah realy!
 */

#ifndef ROBOTCONTROLLER_H
#define ROBOTCONTROLLER_H
#include "gamecontroller.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include "messages/RoboCupGameControlData.h"
#include "messages/Gamecontroller.pb.h"
#include <iostream>
#include <string>
#include <architecture/executables/IActivity.h>

//Game Controller return data interval
#define ALIVEMS 750


/**
* \class RobotController
*
* \file RobotController.h
* @brief A Monas Activity that receives the gamestate either from network interface either from buttor interfaces,
*  publishes the gamestate ledchange messages regarding the current player state.
*/

ACTIVITY_START
class RobotController: public IActivity {
	public:
		/**
		* @brief Does nothing
		*/

		ACTIVITY_VISIBLE RobotController(Blackboard &b,XmlNode &x);
		/**
		* @brief Activity's Execute function. Either reads new gamestate if available and checks if it is changed regarding the old one.
		* Either read the button events ALmemory's values and changes the state.
		* Generally if the state has been changed sends a gamestatemessage and ledchangedmessage
		* @return 0
		*/
		int ACTIVITY_VISIBLE IEX_DIRECTIVE_HOT Execute();
		/**
		* @brief Here the Activity initializes. The configuration file is being readed and a new GameController thread is created and started.
		* Also AlMemmory proxy and event values are initialized
		*/
		void ACTIVITY_VISIBLE UserInit();
		std::string ACTIVITY_VISIBLE GetName() {
			return "RobotController";
		}
	private:
		/**
		* @brief An update message is send to the LedHandler according to current game state
		*/
		void sendLedUpdate();
		/**
		* @brief Read the team configuration xml to extract the team number, the player number, and the default values of the color, the port number
		*
		* @param file_name
		*
		* @returns true at the moment
		*/
		bool readConfiguration(const std::string& file_name);
		GameController gm;
		bool showover;
		RoboCupGameControlData game_data;
		LedChangeMessage leds;
		/**
		 * @brief This message of type GameStateMessage holds the current values set by the GameController or the button interface.
		 * When initialized it assumes values from message conf of type ConfigMessage for team_number, player_number, team_color
		 * and the default values for all other fields from the GameStateMessage definition in GameStateMessage.proto.
		 */
		GameStateMessage gm_state;
		GameStateMessage new_gm_state;
		boost::posix_time::ptime lastalive;
		/**
		 * @brief This message of type ConfigMessage holds the default values found in team_config.xml. If one or more values are
		 * not specified in team_config.xml then it assumes the default value of the ConfigMessage definition in GameStateMessage.proto.
		 */
		ConfigMessage conf;
};
ACTIVITY_END
#endif

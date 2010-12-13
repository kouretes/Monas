/*! \file robotcontroller.h
 *	\brief A Monad Activity that reads game_state and button events and publishes
 *	\brief messages to Behavior and Led Handler. RobotController Sets colors to Chest Button and Feet Leds.
 *
 * \bug No known bugs, Yeah realy!
 */

#ifndef ROBOTCONTROLLER_H
#define ROBOTCONTROLLER_H
#include "architecture/narukom/system/Mutex.h"
#include "gamecontroller.h"
#include "messages/RoboCupGameControlData.h"
#include "messages/Gamecontroller.pb.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <iostream>
#include <string>

#include "hal/robot/aldebaran-robotcontroller.h"
#include <architecture/IActivity.h>

/**
* \class RobotController
*
* \file RobotController.h
* @brief A Monad Activity that receives the gamestate either from network interface either from buttor interfaces,
*  publishes the gamestate ledchange messages regarding the current player state.
*/
class RobotController: public IActivity {
	public:
		/**
		* @brief Does nothing
		*/
		RobotController();
		/**
		* @brief Activity's Execute function. Either reads new gamestate if available and checks if it is changed regarding the old one.
		* Either read the button events ALmemory's values and changes the state.
		* Generally if the state has been changed sends a gamestatemessage and ledchangedmessage
		* @return 0
		*/
		int Execute();
		/**
		* @brief Here the Activity initializes. The configuration file is being readed and a new GameController thread is created and started.
		* Also AlMemmory proxy and event values are initialized
		*/
		void UserInit();
		std::string GetName() {
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
		GameController* gm;

		RoboCupGameControlData game_data;
		AL::ALPtr<AL::ALMemoryProxy> memory;
		bool received_data;
		int chest_button_pressed;
		int left_bumper_pressed;
		int right_bumper_pressed;
		LedChangeMessage leds;
		GameStateMessage gm_state;
		GameStateMessage new_gm_state;

		ConfigMessage conf;
		Mutex mx;

		boost::posix_time::ptime start_timer, endwait;
};

#endif // ROBOTCONTROLLER_H

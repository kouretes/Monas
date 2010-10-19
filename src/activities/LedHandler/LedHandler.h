#ifndef LED_HANDLER_H
#define LED_HANDLER_H
#include "hal/thread.h"
#include "alptr.h"
#include "albroker.h"
#include "alproxy.h"
#include <vector>
#include <string>
#include "messages/Gamecontroller.pb.h"
#include "architecture/IActivity.h"
using std::string;
using std::vector;
/**
 * \class LedHandler

 * \file LedHandler.h
 * \brief A Monad Activity Intended to receive ledChangeMessages and
 * controls the robots light (eye leds, ear leds, chest button leds, feet leds)
 */

class LedHandler: public IActivity {
public:
	LedHandler();

	/**
	 * \brief setLed. Sets the device, a specified color
	 *
	 * \param device. The device name string
	 * Available devices names "chest","l_foot","r_foot", "r_ear", "l_ear", "ears",
	 * "r_eye", "l_eye", "eyes"
	 * \param color. The color name.
	 * Available colors "off", "on" (white), "red", "blue", "green", "yellow", "orange", "purple"
	 */
	void setLed(const std::string& device, const std::string& color);
	/**
	 * \brief setChestColor. Sets the color of the chest button
	 * \param color. The desired color as described in SetLed
	 * */
	void setChestColor(const string& color);
	/**
	 * \brief setChestColor. Sets the color of the specified foot led
	 * \param device. Available devices "r_foot", "l_foot"
	 * \param color. The desired color as described in SetLed
	 */
	void setFootColor(const string& device, const string& color);
	/**
	 * \brief setEyesColor. Set the same color both eyes
	 * \param color. The desired color as described in SetLed
	 */
	void setEyesColor(const string& color);
	/**
	 * \brief setEarsColor. Sets the color of the ear led
	 * \param color. Available colors "off", "blue"
	 */
	void setEarsColor(const string& color);
	/**
	 * \brief setIndividualEarColor. Sets the color of the specified ear
	 * \param device. Available devices "l_ear", "r_ear"
	 * \param color. Available colors "off", "blue"
	 */
	void setIndividualEarColor(const string& device, const string& color);
	/**
	 * \brief setIndividualEyeColor. Sets the color of the specified eye
	 * \param device. Available devices "l_eye", "r_eye"
	 * \param color. The desired color as described in SetLed
	 */
	void setIndividualEyeColor(const string& device, const string& color);
	/**
	 * \brief Agents execute function. Here is where leds color is changed according to the incomming messages.
	 * */
	int Execute();
	/**
	 * \brief Here is where incoming messages are received. In the first execution all the leds are setted off.
	 */
	void process_messages();
	/**
	 * \brief Activity initialization. Blackboard subscription, Leds proxy creation.
	 * In addition chest button, l_foot, r_foot leds become on (white)
	 */
	void UserInit();

	std::string GetName() {
		return "LedHandler";
	}

private:

	LedChangeMessage* led_change;

	/**
	 * \brief Inside this function rgb hex colors are scored in a <hex color, color name map>
	 */
	void initializeColorMap();
	/**
	 * \brief A color map wrapper
	 * \return returns the HEX value of the RGB color, stored in the map
	 */
	int getColor(string);
	AL::ALPtr<AL::ALProxy> leds; ///proxy to handle led colors
	std::map<string, int> colors; /// The map where the pairs of color and hex rgb value are storeds
};

#endif // LED_HANDLER_H

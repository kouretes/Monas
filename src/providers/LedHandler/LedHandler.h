#ifndef LEDHANDLER_H
#define LEDHANDLER_H

#include "hal/robot/nao/generic_nao/aldebaran-proxy.h"
#include <string>
#include "messages/Gamecontroller.pb.h"
#include "core/include/IProvider.hpp"




using std::string;
PROVIDER_START
/**
 * \class LedHandler

 * \file LedHandler.h
 * \brief A Monas activity intented to receive ledChangeMessages and
 * control the robot's LEDs (eye leds, ear leds, chest button leds, feet leds)
 */

class LedHandler: public IProvider
{

public:

	PROVIDER_CONSTRUCTOR(LedHandler);

	/**
	 * \brief setLed. Sets the device to a specified color.
	 *
	 * \param device. The device name as a string.
	 * Available devices names: "chest", "l_foot", "r_foot", "r_ear", "l_ear", "ears",
	 * "r_eye", "l_eye", "eyes"
	 * \param color. The color name as a string.
	 * Available colors: "off", "on" (white), "red", "blue", "green", "yellow", "orange", "purple"
	 */
	void setLed(const string& device, const string& color);

	/**
	 * \brief setChestColor. Sets the color of the chest button.
	 * \param color. The desired color as described in SetLed.
	 * */
	void setChestColor(const string& color);

	/**
	 * \brief setChestColor. Sets the color of the specified foot led.
	 * \param device. Available devices: "r_foot", "l_foot"
	 * \param color. The desired color as described in SetLed.
	 */
	void setFootColor(const string& device, const string& color);

	/**
	 * \brief setEyesColor. Sets the same color to both eyes.
	 * \param color. The desired color as described in SetLed.
	 */
	void setEyesColor(const string& color);

	/**
	 * \brief setIndividualEyeColor. Sets the color of the specified eye
	 * \param device. Available devices "l_eye", "r_eye"
	 * \param color. The desired color as described in SetLed
	 */
	void setIndividualEyeColor(const string& device, const string& color);

	/**
	 * \brief setEarsColor. Sets the same color to both ears.
	 * \param color. Available colors: "off", "blue"
	 */
	void setEarsColor(const string& color);

	/**
	 * \brief setIndividualEarColor. Sets the color of the specified ear.
	 * \param device. Available devices: "l_ear", "r_ear"
	 * \param color. Available colors: "off", "blue"
	 */
	void setIndividualEarColor(const string& device, const string& color);

	inline void processBuffer( Messaging::MessageBuffer  * m)
	{
		IdlingThread::wakeUpThread();
	};


	/**
	 * \brief Agents execute function. This is where leds color is changed according to the incomming messages.
	 * */
	int PROVIDER_VISIBLE IEX_DIRECTIVE_HOT Execute();

	/**
	 * \brief Reads the raw battery level and displays the level in left ear led. Full Battery: all leds off. Empty Battery: all leds on.
	 * Also at the last Level the leds flash. As battery empties flash frequency increases.
	 */
	void SetBatteryLevel();

	/**
	 * \brief Activity initialization. Blackboard subscription, Leds proxy creation.
	 * In addition chest button, l_foot, r_foot leds become on (white)
	 */
	void PROVIDER_VISIBLE UserInit();

	/**
	 * \brief Activity reset capability. Reload xmls etc
	 */
	void PROVIDER_VISIBLE Reset();

	const::string PROVIDER_VISIBLE GetName() const
	{
		return "LedHandler";
	}

private:

	boost::shared_ptr<const LedChangeMessage> led_change;

	std::vector<std::string> left_ear_names;
	std::vector<std::string> right_ear_names;

	/**
	 * \brief Inside this function, HEX RGB colors are stored in a <hex color, color name> map
	 */
	void initializeColorMap();

	/**
	 * \brief A color map wrapper
	 * \return returns the HEX value of the RGB color, stored in the map
	 */
	int getColor(string);

	float battery_level; ///the level of the battery
	boost::shared_ptr<AL::ALProxy> leds; ///proxy to handle led colors
	boost::shared_ptr<AL::ALMemoryProxy> memory;  ///proxy to read ALmemory values

	std::map<string, int> colors; /// The map where the pairs of color and HEX RGB values are stored
};

PROVIDER_END

#endif // LED_HANDLER_H

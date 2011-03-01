#include "LedHandler.h"
#include "hal/robot/generic_nao/kAlBroker.h"

using std::string;
namespace {
	ActivityRegistrar<LedHandler>::Type temp("LedHandler");
}

int LedHandler::Execute() {
	static bool firstRun = true;

	if (firstRun) {
		leds->callVoid<string, int, float> ("fadeRGB", "AllLeds", colors["off"], 0.0);
		firstRun = false;
	}
	process_messages();

	if (led_change != 0) {
		for (int i = 0; i < led_change->leds_size(); i++) {
			setLed(led_change->leds(i).chain(), led_change->leds(i).color());
		}
	}
	return 0;
}

void LedHandler::process_messages() {
	//_blk->process_messages();

	//if (led_change != 0)
	//	delete led_change;

	led_change = _blk->readSignal<LedChangeMessage> ("leds");
}

void LedHandler::UserInit() {
	//led_change = 0;
	_blk->subscribeTo("leds", ON_TOPIC);

	try {

		leds = KAlBroker::Instance().GetBroker()->getProxy("ALLeds");

	} catch (AL::ALError& e) {
		Logger::Instance().WriteMsg("LedHandler", "Could not create ALLEds Proxy", Logger::FatalError);
	}
	initializeColorMap();

	setChestColor("on");
	setFootColor("r_foot", "on");
	setFootColor("l_foot", "on");

	Logger::Instance().WriteMsg("LedHandler", "Initialized", Logger::Info);
}

LedHandler::LedHandler() {

}

void LedHandler::setLed(const string& device, const string& color) {
	if (device.compare("chest") == 0) {
		setChestColor(color);
		return;
	}
	if (device.compare("l_foot") == 0) {
		setFootColor(device, color);
		return;
	}
	if (device.compare("r_foot") == 0) {
		setFootColor(device, color);
		return;
	}
	if ((device.compare("r_ear") == 0) || (device.compare("l_ear") == 0)) {
		setIndividualEarColor(device, color);
		return;
	}
	if ((device.compare("r_eye") == 0) || (device.compare("l_eye") == 0)) {
		setIndividualEyeColor(device, color);
		return;
	}
	if (device.compare("eyes") == 0) {
		setEyesColor(color);
		return;
	}
	if (device.compare("ears") == 0) {
		setEarsColor(color);
		return;
	}
}

void LedHandler::setChestColor(const string& color) {
	leds->callVoid<string, int, float> ("fadeRGB", "ChestLeds", colors[color], 0.0);
}

void LedHandler::setFootColor(const string& device, const string& color) {
	if (device.find("l") != device.npos) {
		leds->callVoid<string, int, float> ("fadeRGB", "LeftFootLeds", colors[color], 0.0);
	} else {
		leds->callVoid<string, int, float> ("fadeRGB", "RightFootLeds", colors[color], 0.0);
	}
}

void LedHandler::setEyesColor(const string& color) {
	leds->callVoid<string, int, float> ("fadeRGB", "FaceLeds", colors[color], 0.0);
}

void LedHandler::setIndividualEyeColor(const string& device, const string& color) {
	if (device.find("l") != device.npos) {
		leds->callVoid<string, int, float> ("fadeRGB", "FaceLedsLeftExternal", colors[color], 0.0);
		leds->callVoid<string, int, float> ("fadeRGB", "FaceLedsLeftInternal", colors[color], 0.0);
		leds->callVoid<string, int, float> ("fadeRGB", "FaceLedsLeftBottom", colors[color], 0.0);
		leds->callVoid<string, int, float> ("fadeRGB", "FaceLedsLeftTop", colors[color], 0.0);

	} else {
		leds->callVoid<string, int, float> ("fadeRGB", "FaceLedsRightExternal", colors[color], 0.0);
		leds->callVoid<string, int, float> ("fadeRGB", "FaceLedsRightInternal", colors[color], 0.0);
		leds->callVoid<string, int, float> ("fadeRGB", "FaceLedsRightBottom", colors[color], 0.0);
		leds->callVoid<string, int, float> ("fadeRGB", "FaceLedsRightTop", colors[color], 0.0);
	}

}
void LedHandler::setEarsColor(const string& color) {

	if (color.compare("off") != 0 && color.compare("blue") != 0) {
		return;
	}
	leds->callVoid<string, int, float> ("fadeRGB", "EarLeds", colors[color.c_str()], 0.0);
}

void LedHandler::setIndividualEarColor(const string& device, const string& color) {
	if (color.compare("off") != 0 && color.compare("blue") != 0) {
		//cout << "Invalid color for Ears \n Please use only blue and off" << endl;
		return;
	}
	if (device.find("l") != device.npos)
		leds->callVoid<string, int, float> ("fadeRGB", "LeftEarLeds", colors[color.c_str()], 0.0);
	else
		leds->callVoid<string, int, float> ("fadeRGB", "RightEarLeds", colors[color.c_str()], 0.0);
}

void LedHandler::initializeColorMap() {
	//add off to color map
	colors["off"] = 0x00000000;
	//add on = white to color map
	colors["on"] = 0x00FFFFFF;
	//add red to color map
	colors["red"] = 0x00FF0000;
	//add green to color map
	colors["green"] = 0x0000FF00;
	//add blue to color map
	colors["blue"] = 0x000000FF;
	//add yellow to color map
	colors["yellow"] = 0x00FFE000;//FFF52C;

	//add orange to color map
	colors["orange"] = 0x00FFFF00;
	colors["purple"] = 0x00FF00FF;

}
int LedHandler::getColor(string color) {
	return colors[color];
}

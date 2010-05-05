#ifndef ROBOTCONTROLLER_H
#define ROBOTCONTROLLER_H
#include "hal/thread.h"
#include "architecture/narukom/system/Mutex.h"
#include "architecture/narukom/pub_sub/publisher.h"
#include "gamecontroller.h"
#include "messages/RoboCupGameControlData.h"
#include "messages/Gamecontroller.pb.h"

#include "alptr.h"
#include "albroker.h"
#include "alproxy.h"
#include "almemoryproxy.h"
#include <architecture/IActivity.h>


class ButtonClient;
class RobotController : public IActivity, public Publisher
{
public:
    RobotController(/*AL::ALPtr<AL::ALBroker> pBroker,MessageQueue* mq, const std::string& configuration_file*/);
    int Execute();
    void UserInit();
private:
    void sendLedUpdate();
    bool readConfiguration(const std::string& file_name);
    int  extract_player_state(RoboCupGameControlData* data);
    GameController* gm;

    RoboCupGameControlData game_data;
    AL::ALPtr<AL::ALMemoryProxy> memory;
    bool received_data;
    int chest_button_pressed;
    int left_bumper_pressed;
    int right_bumper_pressed;
    LedChangeMessage leds;
    GameStateMessage gm_state;
    ConfigMessage conf;
    Mutex mx;

};

#endif // ROBOTCONTROLLER_H

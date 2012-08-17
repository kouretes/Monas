#include "hal/robot/generic_nao/earLed.h"

void EarLed::AllOn () {

    KAlBroker::Instance().GetBroker()->getProxy("ALLeds")->callVoid("on",std::string("EarLeds"));

}

void EarLed::AllOff () {
    
    KAlBroker::Instance().GetBroker()->getProxy("ALLeds")->callVoid("off",std::string("EarLeds"));

}


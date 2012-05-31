#include "IProvider.h"


IProvider::IProvider ( KSystem::ThreadConfig &c, Narukom & com) : PeriodicThread(c),EndPoint(""), _com(com){
    this->attachTo(*_com.get_message_queue());
    this->UserInit();
}


#include "IProvider.h"


IProvider::IProvider ( Narukom & com) : _com(com){
    this->attachTo(*_com,get_message_queue())
    this->UserInit();
}

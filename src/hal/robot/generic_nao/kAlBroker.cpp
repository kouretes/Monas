#include "kAlBroker.h"

void KAlBrokerClass::SetBroker( AL::ALPtr<AL::ALBroker> broker) {
    _broker = broker;
}

AL::ALPtr<AL::ALBroker> KAlBrokerClass::GetBroker() const {
    return _broker;
}


KAlBrokerClass::KAlBrokerClass() {
    //_broker = 0;
}


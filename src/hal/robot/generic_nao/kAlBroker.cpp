#include "kAlBroker.h"

void KAlBrokerClass::SetBroker( AL::ALBroker* broker) {
    _broker = broker;
}

AL::ALBroker* KAlBrokerClass::GetBroker() const {
    return _broker;
}


KAlBrokerClass::KAlBrokerClass() {
    _broker = 0;
}


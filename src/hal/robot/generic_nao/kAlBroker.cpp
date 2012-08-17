#include "kAlBroker.h"

void KAlBrokerClass::SetBroker( boost::shared_ptr<AL::ALBroker> broker) {
    _broker = broker;
}

boost::shared_ptr<AL::ALBroker> KAlBrokerClass::GetBroker() const {
    return _broker;
}


KAlBrokerClass::KAlBrokerClass() {
    //_broker = 0;
}


#include "kAlBroker.h"

void KAlBroker::SetBroker( AL::ALBroker* broker) {
    _broker = broker;
}

AL::ALBroker* KAlBroker::GetBroker() const {
    return _broker;
}

KAlBroker* KAlBroker::Instance() {
    static KAlBroker br;
    return &br;
}

KAlBroker::KAlBroker() {
    _broker = 0;
}


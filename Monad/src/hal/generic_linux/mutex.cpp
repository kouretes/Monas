#include "mutex.h"



Mutex::Mutex() {
    pthread_mutex_init(&mtex, NULL);
}


Mutex::~Mutex() {
    pthread_mutex_destroy(&mtex);
}

void Mutex::lock() const {
    pthread_mutex_lock(&mtex);
}

void Mutex::release() const {
    pthread_mutex_unlock(&mtex);
}



#include "IProvider.h"


IProvider::IProvider ( const std::string &name, KSystem::ThreadConfig &c, Narukom & com) :
	PeriodicThread(c), EndPoint(name), _com(com)
{
	this->attachTo(*_com.get_message_queue());
}


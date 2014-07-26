#include "IProvider.hpp"


IProvider::IProvider ( const std::string &name, KSystem::ThreadConfig &c, Messaging::MessageHub & com) :
	IdlingThread(c), EndPoint(name), _com(com)
{
	this->attachTo(_com);
}


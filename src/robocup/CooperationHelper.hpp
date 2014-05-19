#ifndef COOPERATIONHELPER_HPP
#define COOPERATIONHELPER_HPP

#include "core/include/Blackboard.hpp"



class CooperationHelper
{

public:
	CooperationHelper(Messaging::Blackboard &b, std::size_t t): _blk(b), team(t)
	{
		_blk.updateSubscription("worldstate", Messaging::MessageEntry::SUBSCRIBE_ON_TOPIC, Messaging::MessageEntry::HOST_ID_ANY_HOST);
		_blk.updateSubscription("communication", Messaging::MessageEntry::SUBSCRIBE_ON_TOPIC);
	};

private:
	Messaging::Blackboard & _blk;
	std::size_t team;

	std::size_t getHostByPlayerNum(std::size_t p);
};




#endif // _COOPERATIONACTIVITY_HPP_

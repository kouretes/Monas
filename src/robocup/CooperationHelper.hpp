#ifndef _COOPERATIONACTIVITY_HPP_
#define _COOPERATIONACTIVITY_HPP_

#include "core/include/Blackboard.hpp"



class CooperationHelper
{

public:
	CooperationHelper(Blackboard &b, std::size_t t): _blk(b), team(t)
	{
		_blk.updateSubscription("worldstate", msgentry::SUBSCRIBE_ON_TOPIC, msgentry::HOST_ID_ANY_HOST);
		_blk.updateSubscription("communication", msgentry::SUBSCRIBE_ON_TOPIC);
	};

private:
	Blackboard & _blk;
	std::size_t team;

	std::size_t getHostByPlayerNum(std::size_t p);
};




#endif // _COOPERATIONACTIVITY_HPP_

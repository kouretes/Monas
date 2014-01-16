#ifndef CONDVAR_HPP
#define CONDVAR_HPP

#include <boost/thread/condition_variable.hpp>

namespace KSystem
{

	class CondVar  : public boost::condition_variable
	{

	};

};
#endif // _CONDVAR_H_

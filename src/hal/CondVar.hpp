
#ifndef _CONDVAR_H_
#define _CONDVAR_H_ 1

#include <boost/thread/condition_variable.hpp>

namespace KSystem
{

	class CondVar  : public boost::condition_variable
	{

	};

};
#endif // _CONDVAR_H_

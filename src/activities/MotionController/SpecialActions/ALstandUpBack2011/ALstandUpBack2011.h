#ifndef _ALstandUpBack2011_h_
#define _ALstandUpBack2011_h_ 1

#include "../../ISpecialAction.h"

#include "hal/robot/generic_nao/aldebaran-kme.h"


class ALstandUpBack2011: public ISpecialAction
{

public:
	std::string GetName ()
	{
		return "ALstandUpBack2011";
	}

	ALstandUpBack2011();

	int ExecutePost();

private:

	void Initialize();

	AL::ALMotionProxy *motion;

	std::vector<std::string> names;
	AL::ALValue times, keys;

};

#endif // _ALstandUpBack2011_h_ 

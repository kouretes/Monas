#ifndef _ALstandUpBack2011_2_h_
#define _ALstandUpBack2011_2_h_ 1

#include "../../ISpecialAction.h"

#include "hal/robot/generic_nao/aldebaran-kme.h"


class ALstandUpBack2011_2: public ISpecialAction
{

public:
	std::string GetName ()
	{
		return "ALstandUpBack2011_2";
	}

	ALstandUpBack2011_2();

	int ExecutePost();

private:

	void Initialize();

	boost::shared_ptr<AL::ALMotionProxy> motion;

	std::vector<std::string> names;
	AL::ALValue times, keys;

};

#endif // _ALstandUpBack2011_2_h_

#ifndef _ALstandUpCross2011_h_
#define _ALstandUpCross2011_h_ 1

#include "../../ISpecialAction.h"

#include "hal/robot/generic_nao/aldebaran-kme.h"


class ALstandUpCross2011: public ISpecialAction
{

public:
	std::string GetName ()
	{
		return "ALstandUpCross2011";
	}

	ALstandUpCross2011();

	int ExecutePost();

private:

	void Initialize();

	boost::shared_ptr<AL::ALMotionProxy> motion;

	std::vector<std::string> names;
	AL::ALValue times, keys;

};

#endif // _ALstandUpCross2011_h_ 

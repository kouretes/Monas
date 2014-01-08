#ifndef ALSTANDUPALCROSS2011_H
#define ALSTANDUPALCROSS2011_H

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

	AL::ALMotionProxy *motion;

	std::vector<std::string> names;
	AL::ALValue times, keys;

};

#endif // _ALstandUpCross2011_h_

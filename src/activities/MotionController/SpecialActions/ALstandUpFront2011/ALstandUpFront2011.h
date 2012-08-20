#ifndef _ALstandUpFront2011_h_
#define _ALstandUpFront2011_h_ 1

#include "../../ISpecialAction.h"

#include "hal/robot/generic_nao/aldebaran-kme.h"

class ALstandUpFront2011: public ISpecialAction
{

public:
	std::string GetName ()
	{
		return "ALstandUpFront2011";
	}

	ALstandUpFront2011();

	int ExecutePost();

private:

	void Initialize();

	AL::ALPtr<AL::ALMotionProxy> motion;

	std::vector<std::string> names;
	AL::ALValue times, keys;

};

#endif // _ALstandUpFront2011_h_ 

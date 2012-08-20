#ifndef _THEALSP_h_
#define _THEALSP_h_ 1

#include "../../ISpecialAction.h"

#include "hal/robot/generic_nao/aldebaran-kme.h"


class THEALSP: public ISpecialAction
{

public:
	std::string GetName ()
	{
		return "THEALSP";
	}

	THEALSP();

	int ExecutePost();

private:

	void Initialize();

	AL::ALPtr<AL::ALMotionProxy> motion;

	std::vector<std::string> names;
	AL::ALValue times, keys;

};

#endif // _THEALSP_h_ 

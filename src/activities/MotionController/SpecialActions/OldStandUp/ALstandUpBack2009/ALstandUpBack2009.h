#ifndef _ALstandUpBack2009_h_
#define _ALstandUpBack2009_h_ 1

#include "../../ISpecialAction.h"

#include "hal/robot/generic_nao/aldebaran-kme.h"

class ALstandUpBack2009: public ISpecialAction
{

public:
	std::string GetName ()
	{
		return "ALstandUpBack2009";
	}

	ALstandUpBack2009();

	int ExecutePost();

private:

	void Initialize();

	boost::shared_ptr<AL::ALMotionProxy> motion;

	AL::ALValue jointCodes, angles, times;

};

#endif // _ALstandUpBack2009_h_

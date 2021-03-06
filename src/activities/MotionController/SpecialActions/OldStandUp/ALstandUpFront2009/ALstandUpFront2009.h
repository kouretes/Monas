#ifndef _ALstandUpFront2009_h_
#define _ALstandUpFront2009_h_ 1

#include "../../ISpecialAction.h"

#include "hal/robot/generic_nao/aldebaran-kme.h"

class ALstandUpFront2009: public ISpecialAction
{

public:
	std::string GetName ()
	{
		return "ALstandUpFront2009";
	}

	ALstandUpFront2009();

	int ExecutePost();

private:

	void Initialize();

	boost::shared_ptr<AL::ALMotionProxy> motion;

	AL::ALValue jointCodes, angles, times;

};

#endif // _ALstandUpFront2009_h_ 

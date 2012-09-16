#ifndef _RightKick_h_
#define _RightKick_h_ 1

#include "../../ISpecialAction.h"

#include "hal/robot/generic_nao/aldebaran-kme.h"


class RightKick: public ISpecialAction
{

public:
	std::string GetName ()
	{
		return "RightKick";
	}

	RightKick();

	int ExecutePost();

private:

	void Initialize();

	boost::shared_ptr<AL::ALMotionProxy> motion;

	AL::ALValue RightKick_names, RightKick_times, RightKick_keys;

};

#endif // _RightKick_h_ 

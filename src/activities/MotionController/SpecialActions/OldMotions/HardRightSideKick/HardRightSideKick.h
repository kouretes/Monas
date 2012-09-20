#ifndef _HardRightSideKick_h_
#define _HardRightSideKick_h_ 1

#include "../../ISpecialAction.h"

#include "hal/robot/generic_nao/aldebaran-kme.h"

class HardRightSideKick: public ISpecialAction
{

public:
	std::string GetName ()
	{
		return "HardRightSideKick";
	}

	HardRightSideKick();

	int ExecutePost();

private:

	void Initialize();

	boost::shared_ptr<AL::ALMotionProxy> motion;

	std::vector<std::string> HardRightSideKick_names;
	AL::ALValue HardRightSideKick_times, HardRightSideKick_keys;

};

#endif // _HardRightSideKick_h_ 

#ifndef _LieDown_h_
#define _LieDown_h_ 1

#include "../../ISpecialAction.h"

#include "hal/robot/generic_nao/aldebaran-kme.h"

class LieDown: public ISpecialAction
{

public:
	std::string GetName ()
	{
		return "LieDown";
	}

	LieDown();

	int ExecutePost();

private:

	void Initialize();

	boost::shared_ptr<AL::ALMotionProxy> motion;

	AL::ALValue LieDown_names, LieDown_times, LieDown_keys;

};

#endif // _LieDown_h_ 

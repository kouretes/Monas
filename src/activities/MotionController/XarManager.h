#ifndef XARMANAGER_H
#define XARMANAGER_H

#include <vector>
#include "ISpecialAction.h"




#include "hal/robot/nao/generic_nao/aldebaran-motion-extra.h"



class XarManager
{

public:

	static std::vector<ISpecialAction*> LoadActionsXAR(AL::ALFrameManagerProxy *frame);

	struct XarBehavior
	{
		std::string filename;
		std::string name;
		std::string identifier;
	};

};

#endif //_XARMANAGER_H_H

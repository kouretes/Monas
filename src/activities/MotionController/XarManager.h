#ifndef _XARMANAGER_H_H
#define _XARMANAGER_H_H 1

#include <vector>
#include "ISpecialAction.h"


#include <fstream>
#include <dirent.h>
#include "tools/logger.h"

#include "alptr.h"
#include <alproxy.h>
#include "alvalue.h"
#include "alframemanagerproxy.h"

#include "hal/robot/generic_nao/kAlBroker.h"
#include "ISpecialAction.h"
#include "XarAction.h"

class XarManager {

	public:

		static std::vector<ISpecialAction*> LoadActionsXAR(AL::ALPtr<AL::ALFrameManagerProxy> frame);

		struct XarBehavior {
				std::string filename;
				std::string name;
				std::string identifier;
		};

};

#endif //_XARMANAGER_H_H

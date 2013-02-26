#ifndef _config_h_
#define _config_h_ 1

#include <string>

#include "core/elements/Singleton.hpp"

class GlobalConfClass
{

public:

	// Implicit set of the configuration path!
	// Configuration files expected at the same level as archconf
	std::string ConfigurationFile ()
	{
		return "config/archconf.xml";
	}

	std::string ConfigurationFileAlter ()
	{
		return "/home/nao/naoqi/config/archconf.xml";
	}

	std::string ConfigurationFileGUIAlter ()
	{
		return "../../../config/archconf.xml";
	}

};

typedef Singleton<GlobalConfClass> GlobalConf;

#endif // _config_h_

#ifndef _archConfig_h_
#define _archConfig_h_ 1

#include <string>
#include <iostream>
#include <fstream>

#include "tools/XMLConfig.h"
#include "config.h"
#include "core/hal/syscall.h"

#include "tools/singleton.h"

class ArchConfigClass
{

public:

	const std::string GetConfigPrefix () const
	{
		return ConfigPrefix;
	}

	const std::string GetThreadType () const
	{
		return ThreadType;
	}

	const std::string GetAgentCfgFile () const
	{
		return AgentCfgFile;
	}


	ArchConfigClass ()
	{
		std::string ConfFilePrefix;
		std::ifstream _testConf ( GlobalConf::Instance().ConfigurationFile().c_str() );
		std::ifstream _testConfAlt (GlobalConf::Instance().ConfigurationFileAlter().c_str() );
		std::ifstream _testConfGUIAlt (GlobalConf::Instance().ConfigurationFileGUIAlter().c_str() );

		if ( _testConf.is_open() )
		{
			std::cout << "Configuration file found @ " << GlobalConf::Instance().ConfigurationFile() << std::endl;
			ConfFilePrefix = GlobalConf::Instance().ConfigurationFile();
		}
		else if ( _testConfAlt.is_open() )
		{
			std::cout << "Configuration file found @ " << GlobalConf::Instance().ConfigurationFileAlter() << std::endl;
			ConfFilePrefix = GlobalConf::Instance().ConfigurationFileAlter();
		}
		else if ( _testConfGUIAlt.is_open() )
		{
			std::cout << "Configuration file found @ " << GlobalConf::Instance().ConfigurationFileGUIAlter() << std::endl;
			ConfFilePrefix = GlobalConf::Instance().ConfigurationFileGUIAlter();
		}
		else
		{
			std::cerr << "Configuration file not found either @ " << GlobalConf::Instance().ConfigurationFile() << std::endl;
			std::cerr << "Configuration file not found or @ " << GlobalConf::Instance().ConfigurationFileAlter() << std::endl;
			std::cerr << "Configuration file not found or @ " << GlobalConf::Instance().ConfigurationFileGUIAlter() << std::endl;
			SysCall::_exit( 1 );
		}

		_testConf.close();
		_testConfAlt.close();
		_testConfGUIAlt.close();
		ConfigPrefix.assign( ConfFilePrefix, 0, ConfFilePrefix.find_last_of("/") + 1 ); //FIXME no '/' ?, end in '/'
		std::cout << "Setting config prefix @ " << ConfigPrefix << std::endl;
		XMLConfig ConfFile( ConfFilePrefix );

		if ( ! ConfFile.IsLoadedSuccessfully() )
		{
			std::cerr << "Can't parse initial configuration file @ " << ConfFilePrefix << std::endl;
			SysCall::_exit( 1 );
		}

		if ( ! ConfFile.QueryElement( "AgentConfigurationFile", AgentCfgFile ) )
		{
			std::cerr << "The initial configuration file is not accepted" << std::endl;
			std::cerr << "AgentConfigurationFile is not set" << std::endl;
			SysCall::_exit(1);
		}

		if ( ! ConfFile.QueryElement( "ThreadType", ThreadType ) )
			ThreadType = "PThread";
	}

private:

	std::string ConfigPrefix;

	std::string ThreadType;
	std::string AgentCfgFile;

};


typedef Singleton<ArchConfigClass> ArchConfig;

#endif // _archConfig_h_

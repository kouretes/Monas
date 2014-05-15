#include "Logger.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <set>
#include <cstdio>



#include "sys/stat.h"
#include "hal/syscall.h"

#include "core/elements/Singleton.hpp"
#include "core/include/Configurator.hpp"

#include "tools/toString.h"

//TODO mutex needed
//it's not thread safe but it is instantiated long before any thread creation


LoggerClass::~LoggerClass ()
{
    ErrorLog.close();
}

LoggerClass::LoggerClass ()
	{
		ReadConfiguration();
		lastConfRead = KSystem::Time::SystemTime::now();
		struct stat stFileInfo;
		std::string fullfilename ;
		fullfilename = (Configurator::Instance().getDirectoryPath() + MsgLogFile);
		int intStat = stat((fullfilename + ".0").c_str(), &stFileInfo);
		if (intStat == 0)
			if(stFileInfo.st_size > 512)
			{
				rename( (fullfilename + ".0").c_str() , (fullfilename + ".1").c_str() );
			}

		ErrorLog.open( ( Configurator::Instance().getDirectoryPath() + MsgLogFile + ".0" ).c_str() );

		if ( ! ErrorLog.is_open() )
		{
			std::cerr << "Can't open MessageLog file: " << MsgLogFile << std::endl;
			SysCall::_exit(1);
		}

		ColorMap[C_RED]     = "\033[1;31m";
		ColorMap[C_BLUE]    = "\033[1;34m";
		ColorMap[C_LBLUE]   = "\033[21;34m";
		ColorMap[C_GREEN]   = "\033[1;32m";
		ColorMap[C_YELLOW]   = "\033[1;33m";
		ColorMap[C_DEFAULT] = "\033[0m";
	}

void LoggerClass::ReadConfiguration ()
	{
		std::string ConfFileStr( Configurator::Instance().getDirectoryPath() + "logger.xml" );

		MsgLogFile = Configurator::Instance().findValueForKey("logger.MessageLogFile");

		VerbosityLevel = atoi(Configurator::Instance().findValueForKey("logger.LogFileVerbosityLevel").c_str());
		VerbosityLevel = VerbosityLevel < 0 ? 0 : VerbosityLevel;

		CerrEnabled = atoi(Configurator::Instance().findValueForKey("logger.MessageLogCerr").c_str());

		reparsingPeriod = atoi(Configurator::Instance().findValueForKey("logger.PollingPeriod").c_str());

		int numOfDebug = Configurator::Instance().numberOfNodesForKey("logger.DebugFilter");
		DebugFilter.clear();
		if (numOfDebug == 0 )
			DebuggingMode = false;
		else
		{
			DebuggingMode = true;
			DebugAll = false;

			for (int i=0;i<numOfDebug;i++)
			{
				std::string value = Configurator::Instance().findValueForKey("logger.DebugFilter~" + _toString(i));
				if (value.compare("all")==0)
				{
					DebugAll = true;
					break;
				}

				DebugFilter.insert(value);
			}
		}

		int numOfActive = Configurator::Instance().numberOfNodesForKey("logger.MessageLogFilter");
		ActivityFilter.clear();
		if (numOfActive == 0)
			ActivityFilterEnabled = false;
		else
		{
			ActivityFilterEnabled = true;

			for (int i=0;i<numOfActive;i++)
			{
				std::string value = Configurator::Instance().findValueForKey("logger.MessageLogFilter~" + _toString(i));
				if (value.compare("all")==0)
				{
					ActivityFilterEnabled = false;
					break;
				}

				ActivityFilter.insert(value);
			}
		}

		ColorEnabled = atoi(Configurator::Instance().findValueForKey("logger.MessageLogCerrColor").c_str());

		lastConfRead = KSystem::Time::SystemTime::now();
	}


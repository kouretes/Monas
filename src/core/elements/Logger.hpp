#ifndef _LOGGER_HPP_
#define _LOGGER_HPP_

#include <iostream>
#include <sstream>
#include <fstream>
#include <set>
#include <cstdio>

#include <boost/date_time/posix_time/posix_time.hpp>

#include "sys/stat.h"
#include "hal/syscall.h"

#include "core/elements/Singleton.hpp"
#include "core/architecture/configurator/Configurator.hpp"

#include "tools/toString.h"

//TODO mutex needed
//it's not thread safe but it is instantiated long before any thread creation


class LogLevel
{
	public:
	enum LogType { FatalError = 0, Error, Warning, Info, ExtraInfo, ExtraExtraInfo, Debug };
};

class LoggerClass
{

public:



	~LoggerClass ()
	{
		ErrorLog.close();
	}

	template<class T>
	void WriteMsg ( std::string name, const T & msg, LogLevel::LogType type )
	{
		if ( lastConfRead + boost::posix_time::seconds(reparsingPeriod)
		        < boost::posix_time::microsec_clock::universal_time() )
			ReadConfiguration();

		if ( type > VerbosityLevel )
			return;


		switch (type)
		{
		case LogLevel::FatalError:
		case LogLevel::Error:
			WriteMsgToBuffers ( name, msg, "red" );
			break;

		case LogLevel::Warning:
			WriteMsgToBuffers ( name, msg, "yellow" );
			break;

		case LogLevel::Info:
		case LogLevel::ExtraInfo:
			if ( ! ActivityFilterEnabled || ActivityFilter.find(name) != ActivityFilter.end() )
				WriteMsgToBuffers ( name, msg, "default" );
			break;

		case LogLevel::ExtraExtraInfo:
			if ( ! ActivityFilterEnabled || ActivityFilter.find(name) != ActivityFilter.end() )
				WriteMsgToBuffers ( name, msg, "blue" );
		case LogLevel::Debug:
			if ( DebuggingMode )
			{
				if ( DebugAll )
					WriteMsgToBuffers ( name, msg, "red" );
				else if ( ActivityFilter.find(name) != ActivityFilter.end() )
					WriteMsgToBuffers ( name, msg, "red" );
			}
		}
	}


	LoggerClass ()
	{
		ReadConfiguration();
		lastConfRead = boost::posix_time::microsec_clock::universal_time();
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

		ColorMap["red"]     = "\033[1;31m";
		ColorMap["blue"]    = "\033[1;34m";
		ColorMap["lBlue"]   = "\033[21;34m";
		ColorMap["green"]   = "\033[1;32m";
		ColorMap["yellow"]   = "\033[1;33m";
		ColorMap["default"] = "\033[0m";
	}

private:

	void ReadConfiguration ()
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

		lastConfRead = boost::posix_time::microsec_clock::universal_time();
	}

	template< class T>
	void WriteMsgToBuffers ( std::string name, const T& msg, std::string color )
	{
		ErrorLog << name << " : " << msg << std::endl;

		if ( CerrEnabled )
		{
			if ( ColorEnabled )
				std::cerr << ColorMap[color] << name << " : " << msg << ColorMap["default"] << std::endl;
			else
				std::cerr << name << " : " << msg << std::endl;
		}
	}



	int VerbosityLevel;

	std::string MsgLogFile;
	std::ofstream ErrorLog;

	std::set<std::string> ActivityFilter;
	bool ActivityFilterEnabled;

	bool CerrEnabled;

	bool ColorEnabled;

	std::set<std::string> DebugFilter;
	bool DebuggingMode;
	bool DebugAll;

	std::map<std::string, std::string> ColorMap;

	boost::posix_time::ptime lastConfRead;
	float reparsingPeriod;

};

typedef Singleton<LoggerClass> LoggerSingleton;

class LogEntry
{
	public:
	LogEntry(LogLevel::LogType t,std::string n) : ss(),type(t), name(n)
	{

	};

	~LogEntry()
	{
		LoggerSingleton::Instance().WriteMsg(name,ss.str(),type);
	}
	template<typename T>
	std::ostream & operator<< (T t)
	{
		return ss<<t;
	}

	private:


	LogEntry & operator=(const LogEntry &r)
	{
		type=r.type;
		name=r.name;
		ss<<r.ss.rdbuf();
	};
	LogEntry(const LogEntry &l) :ss(),type(l.type),name(l.name)
	{

	};

	LogLevel::LogType type;
	std::string name;
	std::ostringstream ss;

};


#endif // _LOGGER_HPP_

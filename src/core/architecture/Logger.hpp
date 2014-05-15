#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>
//#include <sstream>
#include <fstream>
#include <set>
//#include <cstdio>
#include <core/architecture/time/SystemTime.hpp>

#include "core/elements/Singleton.hpp"



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



	~LoggerClass ();

	template<class T>
	void WriteMsg ( std::string name, const T & msg, LogLevel::LogType type )
	{
		if ( lastConfRead + KSystem::Time::seconds(reparsingPeriod)
		        < KSystem::Time::SystemTime::now() )
			ReadConfiguration();

		if ( type > VerbosityLevel )
			return;


		switch (type)
		{
		case LogLevel::FatalError:
		case LogLevel::Error:
			WriteMsgToBuffers ( name, msg, C_RED );
			break;

		case LogLevel::Warning:
			WriteMsgToBuffers ( name, msg, C_YELLOW );
			break;

		case LogLevel::Info:
		case LogLevel::ExtraInfo:
			if ( ! ActivityFilterEnabled || ActivityFilter.find(name) != ActivityFilter.end() )
				WriteMsgToBuffers ( name, msg, C_DEFAULT );
			break;

		case LogLevel::ExtraExtraInfo:
			if ( ! ActivityFilterEnabled || ActivityFilter.find(name) != ActivityFilter.end() )
				WriteMsgToBuffers ( name, msg, C_BLUE );
			break;
		case LogLevel::Debug:
			if ( DebuggingMode )
			{
				if ( DebugAll ||  ActivityFilter.find(name) != ActivityFilter.end() )
					WriteMsgToBuffers ( name, msg, C_RED );
			}
		}
	}


	LoggerClass ();

private:
	enum LogColorsType { C_RED, C_BLUE,C_LBLUE,C_GREEN,C_YELLOW,C_DEFAULT};

	void ReadConfiguration ();

	template< class T>
	void WriteMsgToBuffers ( std::string name, const T& msg, LogColorsType color )
	{
		ErrorLog << name << " : " << msg << std::endl;

		if ( CerrEnabled )
		{
			if ( ColorEnabled )
				std::cerr << ColorMap[color] << name << " : " << msg << ColorMap[C_DEFAULT] << std::endl;
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

	std::map<LogColorsType, std::string> ColorMap;

	KSystem::Time::TimeAbsolute lastConfRead;
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

#ifndef _Policies_h_
#define _Policies_h_ 1

#include <string>
#include <iostream>
#include <stdexcept>

#include "hal/syscall.h"
#include "logger.h"

class AbErrPolicy
{

public:

	virtual void Error ( const std::string& ) = 0;

};



class RuntimeErrPolicy : public AbErrPolicy
{

public:

	void Error( const std::string& er )
	{
		throw std::runtime_error( "Error: " + er );
	}

};

class PrintErrAndExitPolicy: public AbErrPolicy
{

public:

	void Error( const std::string& er )
	{
		std::cerr << er << std::endl;
		SysCall::_exit(1);
	}

};

class LogErrAndExitPolicy: public AbErrPolicy
{

public:

	void Error( const std::string& er )
	{
		Logger::Instance().WriteMsg("Factory", er, Logger::FatalError);
		std::cerr << er << std::endl;
		SysCall::_exit(1);
	}

};

#endif // _Policies_h_

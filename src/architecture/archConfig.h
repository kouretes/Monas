#ifndef _archConfig_h_
#define _archConfig_h_ 1

#include <string>
#include <iostream>

#include "tools/XMLConfig.h"
#include "../config/config.h"
#include "hal/syscall.h"

class ArchConfig {

    public:

        const std::string GetMsgLogFile () { return MsgLogFile; }

        const std::string GetThreadType () { return ThreadType; }

        const std::string GetAgentCfgFile () { return AgentCfgFile; }

        const int GetVerbosityLevel () { return LoggerVerbosityLevel; }

        static ArchConfig * Instance () {
            static ArchConfig Ar;
            return &Ar;
        }

    private:

        ArchConfig () {
            XMLConfig ConfFile( GlobalConf::Instance()->ConfigurationFile() );
            if ( ! ConfFile.IsLoadedSuccessfully() ) {
                std::cerr<<"Can't find or parse initial configuration file @ "<<GlobalConf::Instance()->ConfigurationFile()<<std::endl<<std::flush;
                SysCall::_exit( 1 );
            }

            bool found = ConfFile.QueryElement( "MessageLogFile", MsgLogFile );

            found &= ConfFile.QueryElement( "LogFileVerbosityLevel", LoggerVerbosityLevel );
            found &= ConfFile.QueryElement( "ThreadType", ThreadType );
            found &= ConfFile.QueryElement( "AgentConfigurationFile", AgentCfgFile );

            if ( ! found ) {
                std::cerr<<"The initial configuration file is not accepted"<<std::endl<<std::flush;
                SysCall::_exit(1);
            }

        }         

        std::string MsgLogFile;
        std::string ThreadType;
        std::string AgentCfgFile;

        int LoggerVerbosityLevel; 

};

#endif // _archConfig_h_

#ifndef _archConfig_h_
#define _archConfig_h_ 1

#include <string>
#include <iostream>

#include "tools/XMLConfig.h"
#include "../config/config.h"
#include "hal/syscall.h"

class ArchConfig {

    public:

        const std::string GetMsgLogFile () const { return MsgLogFile; }

        const bool GetMsgLogCerrEnabled () const { return MsgLogCerrEnabled; }
        
        const int GetMsgLogVerbosityLevel () const { return MsgLogVerbosityLevel; }
        
        const std::vector<std::string> GetMsgLogFilter () const { return MsgLogFilter; }

        const std::string GetThreadType () const { return ThreadType; }

        const std::string GetAgentCfgFile () const { return AgentCfgFile; }


        static ArchConfig* Instance () {
            static ArchConfig Ar;
            return &Ar;
        }

    private:

        ArchConfig () {
            XMLConfig ConfFile( GlobalConf::Instance()->ConfigurationFile() );
            if ( ! ConfFile.IsLoadedSuccessfully() ) {
                std::cerr<<"Can't find or parse initial configuration file @ "<<GlobalConf::Instance()->ConfigurationFile()<<std::endl;
                SysCall::_exit( 1 );
            }

            if ( ! ConfFile.QueryElement( "AgentConfigurationFile", AgentCfgFile ) ) {
                std::cerr<<"The initial configuration file is not accepted"<<std::endl;
                std::cerr<<"AgentConfigurationFile is not set"<<std::endl;
                SysCall::_exit(1);
            }

            if ( ! ConfFile.QueryElement( "MessageLogFile", MsgLogFile ) ) {
                std::cerr<<"MessageLogFile is not set"<<std::endl;
                std::cerr<<"Defaulting to MonadLog.txt"<<std::endl;
                MsgLogFile = "MonadLog.txt";
            }
            
            if (! ConfFile.QueryElement( "LogFileVerbosityLevel", MsgLogVerbosityLevel ) )
                MsgLogVerbosityLevel = 0;

            if ( ! ConfFile.QueryElement( "ThreadType", ThreadType ) ) 
                ThreadType = "PThread";

            if ( ! ConfFile.QueryElement( "MessageLogCerr", MsgLogCerrEnabled ) )
                MsgLogCerrEnabled = false;

            ConfFile.QueryElement( "MessageLogFilter", MsgLogFilter );

        }         

        std::string MsgLogFile;
        bool MsgLogCerrEnabled;
        int MsgLogVerbosityLevel; 
        std::vector<std::string> MsgLogFilter;


        std::string ThreadType;
        std::string AgentCfgFile;




};

#endif // _archConfig_h_

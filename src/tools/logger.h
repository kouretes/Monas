#ifndef _logger_h_
#define _logger_h_ 1

#include <iostream>
#include <fstream>
#include <set>

#include "architecture/archConfig.h"
#include "tools/XMLConfig.h"

//TODO mutex needed 
//it's not thread safe but it is instantiated long before any thread creation


class Logger { 

    public:

        enum MsgType { FatalError=0 , Error, Info, ExtraInfo, ExtraExtraInfo };
        
        ~Logger () { ErrorLog.close(); }

        template<class T>
        void WriteMsg ( std::string name, const T & msg, MsgType type ) {

            if ( type > VerbosityLevel )
                return;

            switch (type) {
                case FatalError: 
                case Error:      
                    WriteMsgToBuffers ( name, msg );
                    break;
                case Info:
                case ExtraInfo:
                case ExtraExtraInfo: 
                    if ( ! ActivityFilterEnabled )
                        WriteMsgToBuffers ( name, msg );
                    else if ( ActivityFilter.find(name) != ActivityFilter.end() )
                        WriteMsgToBuffers ( name, msg );
            }
        }


        static Logger * Instance () {
            static Logger L;
            return &L;
        }


    private:

        Logger () {

            std::string ConfFileStr( ArchConfig::Instance()->GetConfigPrefix()+"logger.xml" );
            XMLConfig ConfFile( ConfFileStr );
            if ( ! ConfFile.IsLoadedSuccessfully() ) {
                std::cerr<<"Can't parse logger configuration file @ "<<ConfFileStr<<std::endl;
                SysCall::_exit( 1 );
            }

            if ( ! ConfFile.QueryElement( "MessageLogFile", MsgLogFile ) ) {
                std::cerr<<"MessageLogFile is not set"<<std::endl;
                std::cerr<<"Defaulting to MonadLog.txt"<<std::endl;
                MsgLogFile = "MonadLog.txt";
            }

            if (! ConfFile.QueryElement( "LogFileVerbosityLevel", VerbosityLevel ) )
                VerbosityLevel = 0;
            VerbosityLevel = VerbosityLevel < 0 ? 0 : VerbosityLevel;

            if ( ! ConfFile.QueryElement( "MessageLogCerr", CerrEnabled ) )
                CerrEnabled = false;


            std::vector<std::string> ActFilterStr;
            ConfFile.QueryElement( "MessageLogFilter", ActFilterStr );
            if ( ActFilterStr.size() == 0 )
                ActivityFilterEnabled = false;
            else {
                ActivityFilterEnabled = true;
                for ( std::vector<std::string>::const_iterator it = ActFilterStr.begin(); it != ActFilterStr.end(); it++ ) {
                    if ( (*it) == "all" ) {
                        ActivityFilterEnabled = false;
                        break;
                    }
                    ActivityFilter.insert( (*it) ); 
                }
            }

            ErrorLog.open( MsgLogFile.c_str() );
            if ( ! ErrorLog.is_open() ) {
                std::cerr<<"Can't open MessageLog file: "<<MsgLogFile<<std::endl;
                SysCall::_exit(1);
            }

        }

        template< class T>
        void WriteMsgToBuffers ( std::string name, const T& msg ) {
                ErrorLog<<name<<" : "<<msg<<std::endl;
                if ( CerrEnabled )
                    std::cerr<<name<<" : "<<msg<<std::endl;
        }


        int VerbosityLevel;

        std::string MsgLogFile;
        std::ofstream ErrorLog;

        std::set<std::string> ActivityFilter;
        bool ActivityFilterEnabled;

        bool CerrEnabled;

};

#endif // _logger_h_

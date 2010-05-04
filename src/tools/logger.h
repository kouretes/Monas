#ifndef _logger_h_
#define _logger_h_ 1

#include <iostream>
#include <fstream>
#include <set>

#include "architecture/archConfig.h"

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
            ErrorLog.open( ArchConfig::Instance()->GetMsgLogFile().c_str() );
            if ( ! ErrorLog.is_open() ) {
                std::cerr<<"Can't open MessageLog file: "<<ArchConfig::Instance()->GetMsgLogFile()<<std::endl;
                SysCall::_exit(1);
            }
            VerbosityLevel = ArchConfig::Instance()->GetMsgLogVerbosityLevel();
            VerbosityLevel = VerbosityLevel < 0 ? 0 : VerbosityLevel;

            CerrEnabled = ArchConfig::Instance()->GetMsgLogCerrEnabled();

            std::vector<std::string> ActFilterStr = ArchConfig::Instance()->GetMsgLogFilter();
            if ( ActFilterStr.size() == 0 )
                ActivityFilterEnabled = false;
            else if ( ActFilterStr.size() == 1 && ActFilterStr[0] == "all" )
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

        }

        template< class T>
            void WriteMsgToBuffers ( std::string name, const T& msg ) {
                ErrorLog<<name<<" : "<<msg<<std::endl;
                if ( CerrEnabled )
                    std::cerr<<name<<" : "<<msg<<std::endl;
                ;
            }



        int VerbosityLevel;

        std::ofstream ErrorLog;

        std::set<std::string> ActivityFilter;
        bool ActivityFilterEnabled;

        bool CerrEnabled;

};

#endif // _logger_h_

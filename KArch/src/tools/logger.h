#ifndef _logger_h_
#define _logger_h_ 1

#include <iostream>
#include <fstream>

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
                    ErrorLog<<name<<" : "<<msg<<std::endl<<std::flush; break;
                case Info:
                case ExtraInfo:
                case ExtraExtraInfo:                    
                    ErrorLog<<name<<" : "<<msg<<std::endl;
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
                std::cerr<<"Can't open MessageLog file: "<<ArchConfig::Instance()->GetMsgLogFile()<<std::endl<<std::flush;
                SysCall::_exit(1);
            }
            VerbosityLevel = ArchConfig::Instance()->GetVerbosityLevel();
            VerbosityLevel = VerbosityLevel < 0 ? 0 : VerbosityLevel;
        }

        int VerbosityLevel;

        std::ofstream ErrorLog;

};

#endif // _logger_h_

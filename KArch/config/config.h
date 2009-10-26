#ifndef _config_h_
#define _config_h_ 1

#include <string>

class GlobalConf {

    public:

        static GlobalConf * Instance() {
            static GlobalConf Cfg;
            return &Cfg;
        }

        std::string ConfigurationFile () {
            return "config/archconf.xml";
        }

    private:

        GlobalConf() {;}

};

#endif // _config_h_

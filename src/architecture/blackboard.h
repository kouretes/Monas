#ifndef _blackboard_h_
#define _blackboard_h_ 1

#include <map>
#include <string>


class Provider;
class Representation;

class Blackboard {

    private: 

        std::map<std::string,Provider *> Providers;
        std::map<std::string,Representation *> Representations;

    public:

        Provider * GetProvInstanceByName ( const std::string name ) const { 
            const std::map<std::string,Provider*>::const_iterator it = Providers.find(name);
            return it == Providers.end() ? NULL : it->second ;
        }


        Representation * GetRepInstanceByName ( const std::string name ) const { 
            const std::map<std::string,Representation*>::const_iterator it = Representations.find(name);
            return it == Representations.end() ? NULL : it->second ;
        }

        friend class Provider;
        friend class Representation;

};

#endif // _blackboard_h_

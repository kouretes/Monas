#ifndef _kalbroker_h_
#define _kalbroker_h_ 1

#include "tools/singleton.h"

namespace AL {
    class ALBroker;
}

class KAlBrokerClass {

    public:

        void SetBroker( AL::ALBroker* broker);
        
        AL::ALBroker* GetBroker () const; 

        KAlBrokerClass();
    
    private:

        AL::ALBroker* _broker;

};

typedef Singleton<KAlBrokerClass> KAlBroker;

#endif // _kalbroker_h_ 

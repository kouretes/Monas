#ifndef _kalbroker_h_
#define _kalbroker_h_ 1
#include "hal/robot/generic_nao/aldebaran-proxy.h"
#include "tools/singleton.h"

namespace AL {
    class ALBroker;
//    template <class T> class  ALPtr;
}

class KAlBrokerClass {

    public:

        void SetBroker(AL::ALPtr<AL::ALBroker>  broker);
        
        AL::ALPtr<AL::ALBroker> GetBroker () const;

        KAlBrokerClass();
    
    private:

        AL::ALPtr<AL::ALBroker>  _broker;

};

typedef Singleton<KAlBrokerClass> KAlBroker;

#endif // _kalbroker_h_ 

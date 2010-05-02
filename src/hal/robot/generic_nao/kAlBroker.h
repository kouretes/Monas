#ifndef _kalbroker_h_
#define _kalbroker_h_ 1

//#include <albroker.h>

namespace AL {
    class ALBroker;
}

class KAlBroker {

    public:

        void SetBroker( AL::ALBroker* broker);
        
        AL::ALBroker* GetBroker () const; 

        static KAlBroker* Instance();

    private:

        KAlBroker();

        AL::ALBroker* _broker;

};

#endif // _kalbroker_h_ 

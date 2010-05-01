#ifndef _kalbroker_h_
#define _kalbroker_h_ 1

//#include <albroker.h>

namespace AL {
    class ALBroker;
}

class KAlBroker {

    public:

        void SetBroker( AL::ALBroker* broker) { _broker = broker; }

        AL::ALBroker* GetBroker () const { return _broker; }

        static KAlBroker* Instance() {
            static KAlBroker br;
            return &br;
        }

    private:

        KAlBroker() {;}

        AL::ALBroker* _broker;





};

#endif // _kalbroker_h_ 

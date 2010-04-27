#ifndef _talws_h_
#define _talws_h_ 1

#include "architecture/agent.h"
#include "hal/thread.h"

#include <vector>

#include "provAndRep/providerRegistry.h"
//FIXME add thread registry

class Talws {

    public:

        Talws ();
        
        ~Talws(); 

        void Start();

        void Stop();

    private:

        std::vector<Agent*> Agents;

        std::vector<Thread*> Threads;

};

#endif // _talws_h_

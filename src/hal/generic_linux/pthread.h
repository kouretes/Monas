#ifndef _linux_pthread_
#define _linux_pthread_ 1

#include "hal/thread.h"
#include "hal/syscall.h"

#include "tools/logger.h"


//TODO check for failure on thread creation
class PThread : public Thread {

    public:

        PThread ( Threadable * obj, ThreadConfig & cfg );

        ~PThread () { KillThread(); }

        void StartThread ();

        void StopThread () { running = false; }

        void KillThread ();

    private:

        static void *startHelper ( PThread * obj );

        pthread_t ThreadID;
        pthread_attr_t ThreadAttr;

};



#endif // _linux_pthread_

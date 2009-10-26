#ifndef _thread_h_
#define _thread_h_ 1

#include "hal/threadable.h"
#include "hal/threadconfig.h"
#include "architecture/genFactory.h"

class Thread {

    public:

        Thread ( Threadable * obj, ThreadConfig  cfg ) : 
            running(false),
            object(obj),
            config(cfg)
        { ; }

        virtual ~Thread() { ; }

        bool IsRunning() const { return running; }

        virtual void StartThread() =0;

        virtual void StopThread() =0;

        virtual void KillThread() =0;

    protected:

        volatile bool running;

        Threadable * object;

        ThreadConfig  config;

};

typedef GenericFactory<Thread,std::string,Thread * (*) (Threadable *, ThreadConfig ),Threadable*,ThreadConfig> ThreadFactory;

#endif // _thread_h_ 

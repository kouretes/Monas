#ifndef _thread_h_
#define _thread_h_ 1

#include <boost/thread.hpp>
#include <iostream>
using std::endl;
using std::cout;
class Thread{

    public:

        Thread ( bool start = false ) : running(false) { 
            if ( start )
                StartThread(); 
        }

virtual ~Thread() { cout << "Deleting thread " << endl; }

        bool IsRunning() const { return running; }

        virtual void StartThread() {
            running = true;
            bThread = boost::thread( &Thread::startHelper , this);
        }

        virtual void StopThread() {
            running = false;
        }

        virtual void KillThread() {
            StopThread(); //TODO
        }

        void JoinThread() {
            bThread.join();
        }
        
	virtual int Execute() = 0;
    protected:

        volatile bool running;

        boost::thread bThread;

        void startHelper () {
            while (running) 
                this->Execute();
        }

};

#endif // _thread_h_ 

#include <iostream>
#include <unistd.h>
#define TARGET_LINUX

#include "thread.h"
#include "threadconfig.h"
#include "threadable.h"

using namespace std;


class Asdf : public Threadable {

    public:

        int Execute () {
            cout<<"Hello from Asdf: "<<num<<endl;
            usleep(1000000);
        }

        Asdf ( int num ) : num(num) { 
            cout<<"Instanciating Asdf"<<num<<endl; 
        }

        void Stop() {
            cout<<"Stoping Asdf"<<num<<endl;
            this->Threadable::Stop();            
        }

    private:

        int num;

};


int main () {

    cout<<"-----Main----"<<endl;
    Asdf ex1(1);
    Asdf ex2(2);
    Asdf ex3(3);

    ThreadConfig tcfg;
    tcfg.IsRealTime = true;
    tcfg.Priority = 0;

    Thread * t1 = new PThread( &ex1, tcfg );
    Thread * t2 = new PThread( &ex2, tcfg );
    Thread * t3 = new PThread( &ex3, tcfg );

    t1->StartThread();
    t2->StartThread();
    t3->StartThread();

    usleep(4000000);

    t1->StopThread();
    t2->KillThread();

    delete t3;


    cout<<"Main: going to sleep"<<endl;
    usleep(10000000);
    cout<<"Main: exiting"<<endl;

}

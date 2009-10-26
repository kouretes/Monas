#include "pthread.h"

#include <pthread.h>

PThread::PThread ( Threadable * obj, ThreadConfig & cfg ) :
    Thread ( obj, cfg ), ThreadID(0) 
{
    pthread_attr_init( &ThreadAttr );
    pthread_attr_setdetachstate ( &ThreadAttr, PTHREAD_CREATE_JOINABLE );
    pthread_attr_setschedpolicy( &ThreadAttr, config.IsRealTime? SCHED_FIFO : SCHED_OTHER );
    //TODO find out a convenient way to set the priority
    //struct sched_param parammm;
    //pthread_attr_getschedparam( &ThreadAttr, &parammm );
    //parammm.sched_priority = config.Priority;
    //pthread_attr_setschedparam( &ThreadAttr, &parammm );

    //thread_attr_setinheritsched( &ThreadAttr, PTHREAD_EXPLICIT_SCHED );

}


void PThread::StartThread () {
    if (running)
        KillThread();
    running = true;
    pthread_create( &ThreadID, &ThreadAttr, (void* (*) (void*)) startHelper, this );
}

void PThread::KillThread () {
    StopThread();
    SysCall::_usleep ( 500000 );
    pthread_cancel ( ThreadID );
    pthread_join ( ThreadID, 0 );
    ThreadID = 0;
}


void *PThread::startHelper ( PThread * obj ) {
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, 0);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, 0);
    unsigned long start,end;
    double ExecInterval, Freq2Time = (1/(double)obj->config.ThreadFrequency)*1000000;
    while ( obj->running ) { 
        start = SysCall::_GetCurrentTimeInUSec();
        obj->object->ThreadMain();
        end = SysCall::_GetCurrentTimeInUSec();
        ExecInterval = end - start;
        //SysCall::_usleep(Freq2Time - ExecInterval);                
        if ( ExecInterval > Freq2Time ) //&& ((++execution_num)%50)==0 ) //TODO choose number
            Logger::Instance()->WriteMsg("PThread","Decrease Freq!!!", Logger::ExtraInfo );//TODO Find Name
        else 
            SysCall::_usleep(Freq2Time - ExecInterval);                
    }//TODO multiple
    return 0;
}




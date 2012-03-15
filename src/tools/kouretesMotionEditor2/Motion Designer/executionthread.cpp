#include "executionthread.h"


executionThread::executionThread(){
}
executionThread::executionThread(QStringList *myMotion){

    killFlag = false;
    motion = new QStringList;

    foreach(QString str, *myMotion){
    	motion->append(str);
    }

}

void executionThread::run(){
//
    for(int i=0;i<motion->count();i++){
        if(killFlag)
             break;//or return if you want to see the window while; // End thread
        //if(sendCommand(str))  //The send was OK
        //usleep(1000000);
        emit stepExecuted(i+1);

    }

    emit finished();

}

void executionThread::killThread(){
    killFlag = true;
}

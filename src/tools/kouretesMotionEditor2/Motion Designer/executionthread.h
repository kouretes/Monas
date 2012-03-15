#ifndef EXECUTIONTHREAD_H
#define EXECUTIONTHREAD_H

#include <QThread>
#include <QMutex>
#include <QStringList>
#include <iostream>

class executionThread : public QThread
{
    Q_OBJECT
    public:
        executionThread();
        executionThread(QStringList*);
        void run();
        bool killFlag;

        QStringList *motion;

    signals:
        void stepExecuted(int);
        void finished();
    public slots:
        void killThread();


};

#endif // EXECUTIONTHREAD_H

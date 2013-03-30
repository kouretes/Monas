#include "motionexecution.h"
#include "ui_motionexecution.h"
#include <iostream>

MotionExecution::MotionExecution(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::MotionExecution)
{
    m_ui->setupUi(this);
    //show();
}

MotionExecution::MotionExecution(QWidget *parent, QStringList *myMotion) :
    QDialog(parent),
    m_ui(new Ui::MotionExecution)
{
    m_ui->setupUi(this);

    motion = myMotion;
    playThread = new executionThread(myMotion);

    connect(playThread, SIGNAL(stepExecuted(int)),this, SLOT(updateDialog(int)));
    connect(m_ui->stopExecutionButton, SIGNAL(clicked()), playThread, SLOT(killThread()));
    connect(playThread, SIGNAL(finished()), this, SLOT(hide()));


    playThread->start();

    show();


}


MotionExecution::~MotionExecution()
{
    delete m_ui;
}

void MotionExecution::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MotionExecution::updateDialog(int step){

    m_ui->progressLabel->setText(QString(QString::number(step)+QString(" / ")+QString::number(motion->count())+" Executed"));
    m_ui->progressBar->setValue((int)(100*step/motion->count()));

}



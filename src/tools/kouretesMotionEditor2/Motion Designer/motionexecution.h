#ifndef MOTIONEXECUTION_H
#define MOTIONEXECUTION_H

#include <QtGui/QDialog>
#include <QMutex>
#include "executionthread.h"

namespace Ui {
    class MotionExecution;
}

class MotionExecution : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY(MotionExecution)
public:
    explicit MotionExecution(QWidget *parent = 0);
    MotionExecution( QWidget *parent, QStringList *);
    virtual ~MotionExecution();

    QStringList *motion;
    executionThread *playThread;

protected:
    virtual void changeEvent(QEvent *e);

public slots:
    void updateDialog(int);

private:
    Ui::MotionExecution *m_ui;
};

#endif // MOTIONEXECUTION_H

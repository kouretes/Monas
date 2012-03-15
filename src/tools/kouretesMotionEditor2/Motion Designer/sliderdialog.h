#ifndef SLIDERDIALOG_H
#define SLIDERDIALOG_H

#include <QtGui/QDialog>
#include <QSlider>
#include "sliderinput.h"
#include "jointsliders.h"


class sliderDialog : public QMainWindow {
    Q_OBJECT

public:


    sliderDialog(Robot *, QString, QWidget *parent = 0);
    virtual ~sliderDialog();
    JointSliders *jointsUI;
	void connect_sliders(){
		jointsUI->connect_sliders();
	}
    protected:
        void closeEvent(QCloseEvent *event);

signals:
        void slidersClosed(bool);
        void poseChanged();

};

#endif // SLIDERDIALOG_H

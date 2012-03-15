#include "sliderdialog.h"
#include <qstring.h>
#include <iostream>



sliderDialog::sliderDialog(Robot * robot, QString xmlFile, QWidget *parent) : QMainWindow(parent)
{
    if(robot->loadXml(QString(xmlFile))){
        jointsUI = new JointSliders(robot,this);
        std::cout << "Slidders Completed " << std::endl;
        setCentralWidget(jointsUI);
    }




}

sliderDialog::~sliderDialog()
{
}

void sliderDialog::closeEvent(QCloseEvent *event){

    hide();
    emit slidersClosed(false);
    event->ignore();

}


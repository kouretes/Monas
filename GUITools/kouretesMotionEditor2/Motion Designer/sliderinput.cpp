#include "sliderinput.h"
#include <iostream>

sliderInput::sliderInput(QWidget *parent) :QGroupBox(parent)
{

    dblSpinBox = new QDoubleSpinBox;
    dblSpinBox->setDecimals(3);
    hSlider = new QSlider(Qt::Horizontal);
    chkBox = new QCheckBox;
    chkBox->setChecked(true);

    QHBoxLayout * layout1 = new QHBoxLayout;
    layout1->addWidget(chkBox);
    layout1->addWidget(dblSpinBox);
    layout1->addWidget(hSlider);

     setLayout(layout1);

     connect(hSlider,SIGNAL(valueChanged(int)),this,SLOT(setMySpinBox(int)));
     connect(dblSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setMySlider(double)));


}

sliderInput::~sliderInput()
{
    delete hSlider;
    delete dblSpinBox;
    delete chkBox;
}


void sliderInput::setMySpinBox(int val){

    dblSpinBox->setValue(((double)val)/10000.0);

}

void sliderInput::setMySlider(double val){

    hSlider->setValue((int)(val*10000));
}

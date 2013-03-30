#ifndef SLIDERINPUT_H
#define SLIDERINPUT_H

#include <QtGui>

class sliderInput : public QGroupBox {
    Q_OBJECT

public:
    explicit sliderInput(QWidget *parent = 0);
    virtual ~sliderInput();

    QSlider *hSlider;
    QDoubleSpinBox *dblSpinBox;
    QCheckBox * chkBox;

public slots:
    void setMySpinBox(int);
    void setMySlider(double);

signals:
    void poseChanged();

};

#endif // SLIDERINPUT_H

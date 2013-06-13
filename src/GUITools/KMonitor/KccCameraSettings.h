#ifndef KCCCAMERASETTINGS_H
#define KCCCAMERASETTINGS_H

#include <QDialog>

namespace Ui {
    class KccCameraSettings;
}

class KccCameraSettings : public QDialog
{
    Q_OBJECT

public:
    explicit KccCameraSettings(QWidget *parent = 0);
    ~KccCameraSettings();

private:
    Ui::KccCameraSettings *ui;
};

#endif // KCCCAMERASETTINGS_H

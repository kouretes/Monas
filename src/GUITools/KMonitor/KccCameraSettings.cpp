#include "KccCameraSettings.h"
#include "ui_KccCameraSettings.h"
#include "core/architecture/configurator/Configurator.hpp"

KccCameraSettings::KccCameraSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::KccCameraSettings)
{
    ui->setupUi(this);
}

KccCameraSettings::~KccCameraSettings()
{
    delete ui;
}

#include "aboutdialog.h"
#include "ui_aboutdialog.h"

aboutDialog::aboutDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::aboutDialog)
{
    m_ui->setupUi(this);
}

aboutDialog::~aboutDialog()
{
    delete m_ui;
}

void aboutDialog::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

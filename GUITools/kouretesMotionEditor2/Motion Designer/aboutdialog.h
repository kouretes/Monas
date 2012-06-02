#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QtGui/QDialog>

namespace Ui {
    class aboutDialog;
}

class aboutDialog : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY(aboutDialog)
public:
    explicit aboutDialog(QWidget *parent = 0);
    virtual ~aboutDialog();

protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::aboutDialog *m_ui;
};

#endif // ABOUTDIALOG_H

#ifndef KMONITOR_H
#define KMONITOR_H

#ifdef QT_VERSION_5
#include <QtWidgets/QMainWindow>
#else
#include <QtGui/QMainWindow>
#endif

#include "KGUIMessenger.h"
#include "HostsComboBox.h"
#include "ui_KMonitor.h"

namespace Ui {
	class KMonitor;
}

class KMonitor : public QMainWindow {
	Q_OBJECT

public:
	KMonitor (QWidget *parent = 0);
	~KMonitor();

private slots:
	void quitKMonitor();

	void printCurrentTab (int index);
	void closeEvent(QCloseEvent *event);

private:
	Ui::KMonitor *ui;
	KGUIMessenger *Messenger;
};

#endif // KMONITOR_H

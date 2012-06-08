#ifndef KMONITOR_H
#define KMONITOR_H

#include <QtGui/QMainWindow>

#include "KGUIMessenger.h"
#include "GWRemoteHosts.h"
#include "LWRemoteHosts.h"
#include "LWElementList.h"
#include "LMElementList.h"
#include "LVElementList.h"
#include "KccHandler.h"
#include "ui_KMonitor.h"


class KMonitor : public QMainWindow, private Ui::KMonitorClass
{
    Q_OBJECT

public:
    KMonitor(QWidget *parent = 0);
    ~KMonitor();

private slots:
	void quitKMonitor();
	void printCurrentTab(int index);

private:
    KGUIMessenger* Messenger;
    GWRemoteHosts* availableGWHosts;
    LWRemoteHosts* availableLWHosts;
    LWElementList* LWSElementList;
    LWRemoteHosts* availableLMHosts;
    LMElementList* LPMElementList;

    LWRemoteHosts* availableLVHosts;
    LVElementList* LRVElementList;
<<<<<<< Updated upstream
    LWRemoteHosts* availableKccHosts;
    Ui::KccHandler* KccUi;
=======

    KccHandler* KCC;
>>>>>>> Stashed changes
};

#endif // KMONITOR_H

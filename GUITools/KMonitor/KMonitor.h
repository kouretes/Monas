#ifndef KMONITOR_H
#define KMONITOR_H

#include <QtGui/QMainWindow>

#include "KGUIMessenger.h"
#include "GWRemoteHosts.h"
#include "LWRemoteHosts.h"
#include "LWElementTree.h"
#include "LMElementTree.h"
#include "LVElementList.h"
#include "LSDController.h"
#include "KccHandler.h"
#include "XMLHandler.h"
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
	void LSDInitialization();

    KGUIMessenger* Messenger;
    GWRemoteHosts* availableGWHosts;
    LWRemoteHosts* availableLWHosts;
    LWElementTree* LWSElementTree;
    LWRemoteHosts* availableLMHosts;
    LMElementTree* LPMElementTree;
    LWRemoteHosts* availableLVHosts;
    LVElementList* LRVElementList;
    LWRemoteHosts* availableLSHosts;
    LSDController* LSController;
};

#endif // KMONITOR_H

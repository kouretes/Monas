#include "KMonitor.h"

KMonitor::KMonitor(QWidget *parent)
    : QMainWindow(parent), Messenger(NULL), availableGWHosts(NULL)
{
	setupUi(this);

	Messenger = new KGUIMessenger();
	availableGWHosts = new GWRemoteHosts(this->GWSTreeWidget);
	availableLWHosts = new LWRemoteHosts(this->LWSComboBox);
	LWSElementList = new LWElementList(this->LWSListWidget);

	connect(action_Quit, SIGNAL(triggered()), this, SLOT(quitKMonitor()));

	//Signals-Slots for Global World State
	connect(Messenger, SIGNAL(knownHostsUpdate(KnownHosts)), availableGWHosts, SLOT(emergeAvailableHosts(KnownHosts)));
	connect(Messenger, SIGNAL(gameStateMessageUpdate(GameStateMessage, QString)), availableGWHosts, SLOT(setGWRHGameStateInfo(GameStateMessage, QString)));
	connect(Messenger, SIGNAL(gameStateMessageUpdate(GameStateMessage, QString)), this->GWSGraphicsView, SLOT(setKGFCGameStateInfo(GameStateMessage, QString)));

	connect(availableGWHosts, SIGNAL(GWRHSubscriptionRequest(QString)), Messenger , SLOT(GWRHSubscriptionHandler(QString)));
	connect(availableGWHosts, SIGNAL(GWRHUnsubscriptionRequest(QString)), Messenger, SLOT(GWRHUnsubscriptionHandler(QString)));

	connect(Messenger, SIGNAL(worldInfoUpdate(WorldInfo, QString)), this->GWSGraphicsView, SLOT(worldInfoUpdateHandler(WorldInfo, QString)));

	connect(availableGWHosts, SIGNAL(GWRHSetRobotVisible(QString, bool)), this->GWSGraphicsView, SLOT(GWSGVRobotVisible(QString, bool)));
	connect(availableGWHosts, SIGNAL(GWRHSetBallVisible(QString, bool)), this->GWSGraphicsView, SLOT(GWSGVBallVisible(QString, bool)));
	connect(availableGWHosts, SIGNAL(GWRHOldHostRemoved(QString)), this->GWSGraphicsView, SLOT(removeGraphicalElement(QString)));

	//Signals-Slots for tab changes manipulation
	//connect(this->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(printCurrentTab(int)));


	//Signals-Slots for Local World State
	//Signals-Slots for local remote hosts (comboBox)

	connect(availableGWHosts, SIGNAL(GWRHNewHostAdded(QString, QString)), this->availableLWHosts, SLOT(addComboBoxItem(QString, QString)));
	connect(availableGWHosts, SIGNAL(GWRHOldHostRemoved(QString)), this->availableLWHosts, SLOT(removeComboBoxItem(QString)));
	connect(availableGWHosts, SIGNAL(LWRHGameStateMsgUpdate(QIcon, QString, QString)), this->availableLWHosts, SLOT(setLWRHGameStateInfo(QIcon, QString, QString)));

	connect(availableLWHosts, SIGNAL(LWRHSubscriptionRequest(QString)), Messenger, SLOT(LWRHSubscriptionHandler(QString)));
	connect(availableLWHosts, SIGNAL(LWRHUnsubscriptionRequest(QString)), Messenger, SLOT(LWRHUnsubscriptionHandler(QString)));


	//Signals-Slots btw local remote hosts (comboBox) and available elements (LWSlistwidget)

	connect(availableLWHosts, SIGNAL(LWRHSubscriptionRequest(QString)), LWSElementList, SLOT(LWELSubscriptionHandler(QString)));
	connect(availableLWHosts, SIGNAL(LWRHUnsubscriptionRequest(QString)), LWSElementList, SLOT(LWELUnsubscriptionHandler(QString)));
	connect(availableLWHosts, SIGNAL(LWRHUnsubscriptionRequest(QString)), LWSGraphicsView, SLOT(removeGraphicalElement(QString)));

	connect(Messenger, SIGNAL(worldInfoUpdate(WorldInfo, QString)), LWSGraphicsView, SLOT(worldInfoUpdateHandler(WorldInfo, QString)));
	connect(Messenger, SIGNAL(gameStateMessageUpdate(GameStateMessage, QString)), LWSGraphicsView, SLOT(setKGFCGameStateInfo(GameStateMessage, QString)));

	connect(LWSElementList, SIGNAL(LWRHSetRobotVisible(QString, bool)), LWSGraphicsView, SLOT(LWSGVRobotVisible(QString, bool)));
	connect(LWSElementList, SIGNAL(LWRHSetBallVisible(QString, bool)), LWSGraphicsView, SLOT(LWSGVBallVisible(QString, bool)));


	//Signal slot connections for Vision Observations
	connect(LWSElementList, SIGNAL(LWRHSetVisionBallVisible(QString, bool)), LWSGraphicsView, SLOT(LWSGVVisionBallVisible(QString, bool)));
	connect(LWSElementList, SIGNAL(LWRHSetVisionGoalPostsVisible(QString, bool)), LWSGraphicsView, SLOT(LWSGVVisionGoalPostsVisible(QString, bool)));

	connect(Messenger, SIGNAL(obsmsgUpdate(ObservationMessage, QString)), LWSGraphicsView, SLOT(observationMessageUpdateHandler(ObservationMessage, QString)));

	//Signal slot connections for Particles of Localization
	connect(LWSElementList, SIGNAL(LWRHSetParticlesVisible(QString, bool)), LWSGraphicsView, SLOT(LWSGVParticlesVisible(QString, bool)));
	connect(Messenger, SIGNAL(localizationDataUpdate(LocalizationDataForGUI, QString)), LWSGraphicsView, SLOT(localizationDataUpdateHandler(LocalizationDataForGUI, QString)));

	//Signal slot connections for HFOV
	connect(LWSElementList, SIGNAL(LWRHSetHFOVVisible(QString, bool)), LWSGraphicsView, SLOT(LWSGVHFOVVisible(QString, bool)));
	connect(Messenger, SIGNAL(headYawJointUpdate(float, QString)), LWSGraphicsView, SLOT(headYawJointUpdateHandler(float, QString)));

	//Signal slot connections for Trace Of Estimated Robot Positions
	connect(LWSElementList, SIGNAL(LWRHSetTraceVisible(QString, bool)), LWSGraphicsView, SLOT(LWSGVTraceVisible(QString, bool)));

	setWindowState(Qt::WindowMaximized);

}

KMonitor::~KMonitor()
{

}

void KMonitor::quitKMonitor()
{
	std::cout << "KMonitor::quitKMonitor()" << std::endl;
	this->close();
	this->destroy();
}

void KMonitor::printCurrentTab(int index)
{
	std::cout << "Allaksa kai eimai to :: " << index << std::endl;

}

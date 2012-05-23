#include "KMonitor.h"

KMonitor::KMonitor(QWidget *parent)
    : QMainWindow(parent), Messenger(NULL), availableGWHosts(NULL)
{
	setupUi(this);

	Messenger = new KGUIMessenger();
	availableGWHosts = new GWRemoteHosts(this->GWSTreeWidget);

	availableLWHosts = new LWRemoteHosts(this->LWSComboBox);
	LWSElementList = new LWElementList(this->LWSListWidget);

	availableLMHosts = new LWRemoteHosts(this->LPMComboBox);
	//LPMElementList = new LMElementList(this->LPMListWidget);

	//SIGNAL SLOT CONNECTIONS FOR GLOBAL WORLD STATE
	//Signal slot connections for Robot Position & Orientation, Ball Estimation

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


	//SIGNAL SLOT CONNECTIONS FOR LOCAL WORLD STATE
	//Signal slot connections for Local Remote Hosts ComboBox

	connect(availableGWHosts, SIGNAL(GWRHNewHostAdded(QString, QString)), this->availableLWHosts, SLOT(addComboBoxItem(QString, QString)));
	connect(availableGWHosts, SIGNAL(GWRHOldHostRemoved(QString)), this->availableLWHosts, SLOT(removeComboBoxItem(QString)));
	connect(availableGWHosts, SIGNAL(LWRHGameStateMsgUpdate(QIcon, QString, QString)), this->availableLWHosts, SLOT(setLWRHGameStateInfo(QIcon, QString, QString)));

	connect(availableLWHosts, SIGNAL(LWRHSubscriptionRequest(QString)), Messenger, SLOT(LWRHSubscriptionHandler(QString)));
	connect(availableLWHosts, SIGNAL(LWRHUnsubscriptionRequest(QString)), Messenger, SLOT(LWRHUnsubscriptionHandler(QString)));

	//Signal slot connections for Robot Position & Orientation, Ball Estimation
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

	//Signal slot connections for Motion Walk Command
	connect(LWSElementList, SIGNAL(LWRHSetMWCmdVisible(QString, bool)), LWSGraphicsView, SLOT(LWSGVMWCmdVisible(QString, bool)));
	connect(Messenger, SIGNAL(motionCommandUpdate(MotionWalkMessage, QString)), LWSGraphicsView, SLOT(motionCommandUpdateHandler(MotionWalkMessage, QString)));

	//SIGNAL SLOT CONNECTIONS FOR LOCAL POLAR MAP
	//Signal slot connections for Local Map Hosts ComboBox

	connect(availableGWHosts, SIGNAL(GWRHNewHostAdded(QString, QString)), availableLMHosts, SLOT(addComboBoxItem(QString, QString)));
	connect(availableGWHosts, SIGNAL(GWRHOldHostRemoved(QString)), availableLMHosts, SLOT(removeComboBoxItem(QString)));
	connect(availableGWHosts, SIGNAL(LWRHGameStateMsgUpdate(QIcon, QString, QString)), availableLMHosts, SLOT(setLWRHGameStateInfo(QIcon, QString, QString)));

	connect(availableLMHosts, SIGNAL(LWRHSubscriptionRequest(QString)), Messenger, SLOT(LMRHSubscriptionHandler(QString)));
	connect(availableLMHosts, SIGNAL(LWRHUnsubscriptionRequest(QString)), Messenger, SLOT(LMRHUnsubscriptionHandler(QString)));

	//Signal slot connections for
	/*connect(availableLMHosts, SIGNAL(LWRHSubscriptionRequest(QString)), LPMElementList, SLOT(LMELSubscriptionHandler(QString)));
	connect(availableLMHosts, SIGNAL(LWRHUnsubscriptionRequest(QString)), LPMElementList, SLOT(LMELUnsubscriptionHandler(QString)));*/
	connect(availableLMHosts, SIGNAL(LWRHUnsubscriptionRequest(QString)), LPMLabel, SLOT(removeRobotMap(QString)));

	//Signal slot connections for Obstacles
	connect(Messenger, SIGNAL(gridInfoUpdate(GridInfo, QString)), LPMLabel, SLOT(gridInfoUpdateHandler(GridInfo, QString)));
	connect(Messenger, SIGNAL(gridInfoUpdate(GridInfo, QString)), this->graphicsView, SLOT(gridInfoUpdateHandler(GridInfo, QString)));
	//connect(LPMElementList, SIGNAL(LMRHSetObstaclesVisible(QString, bool)), LPMLabel, SLOT(LMObstaclesVisible(QString, bool)));

	//SIGNAL SLOT CONNECTIONS FOR MAIN WINDOW
	connect(action_Quit, SIGNAL(triggered()), this, SLOT(quitKMonitor()));
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

#include "KMonitor.h"
#include "ui_KccHandler.h"

#include <QComboBox>
#include <QList>

KMonitor::KMonitor(QWidget *parent)
    : QMainWindow(parent), Messenger(NULL), availableGWHosts(NULL)
{
	setupUi(this);

	Messenger = new KGUIMessenger();
	availableGWHosts = new GWRemoteHosts(this->GWSTreeWidget);

	availableLWHosts = new LWRemoteHosts(this->LWSComboBox);
	LWSElementTree = new LWElementTree(this->LWSTreeWidget);

	availableLMHosts = new LWRemoteHosts(this->LPMComboBox);
	LPMElementTree = new LMElementTree(this->LPMTreeWidget);

	availableLVHosts = new LWRemoteHosts(this->LRVComboBox);
	LRVElementList = new LVElementList(this->LRVListWidget);

	availableLSHosts = new LWRemoteHosts(this->LSDComboBox);

	LSDInitialization();

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

	//SIGNAL SLOT CONNECTIONS FOR LOCAL WORLD STATE
	//Signal slot connections for Local Remote Hosts ComboBox

	connect(availableGWHosts, SIGNAL(GWRHNewHostAdded(QString, QString)), this->availableLWHosts, SLOT(addComboBoxItem(QString, QString)));
	connect(availableGWHosts, SIGNAL(GWRHOldHostRemoved(QString)), this->availableLWHosts, SLOT(removeComboBoxItem(QString)));
	connect(availableGWHosts, SIGNAL(LWRHGameStateMsgUpdate(QIcon, QString, QString)), this->availableLWHosts, SLOT(setLWRHGameStateInfo(QIcon, QString, QString)));

	connect(availableLWHosts, SIGNAL(LWRHSubscriptionRequest(QString)), Messenger, SLOT(LWRHSubscriptionHandler(QString)));
	connect(availableLWHosts, SIGNAL(LWRHUnsubscriptionRequest(QString)), Messenger, SLOT(LWRHUnsubscriptionHandler(QString)));

	//Signal slot connections for Robot Position & Orientation, Ball Estimation
	connect(availableLWHosts, SIGNAL(LWRHSubscriptionRequest(QString)), LWSElementTree, SLOT(LWELSubscriptionHandler(QString)));
	connect(availableLWHosts, SIGNAL(LWRHUnsubscriptionRequest(QString)), LWSElementTree, SLOT(LWELUnsubscriptionHandler(QString)));
	connect(availableLWHosts, SIGNAL(LWRHUnsubscriptionRequest(QString)), LWSGraphicsView, SLOT(removeGraphicalElement(QString)));

	connect(Messenger, SIGNAL(worldInfoUpdate(WorldInfo, QString)), LWSGraphicsView, SLOT(worldInfoUpdateHandler(WorldInfo, QString)));
	connect(Messenger, SIGNAL(gameStateMessageUpdate(GameStateMessage, QString)), LWSGraphicsView, SLOT(setKGFCGameStateInfo(GameStateMessage, QString)));

	connect(LWSElementTree, SIGNAL(LWRHSetRobotVisible(QString, bool)), LWSGraphicsView, SLOT(LWSGVRobotVisible(QString, bool)));
	connect(LWSElementTree, SIGNAL(LWRHSetBallVisible(QString, bool)), LWSGraphicsView, SLOT(LWSGVBallVisible(QString, bool)));


	//Signal slot connections for Vision Observations
	connect(LWSElementTree, SIGNAL(LWRHSetVisionBallVisible(QString, bool)), LWSGraphicsView, SLOT(LWSGVVisionBallVisible(QString, bool)));
	connect(LWSElementTree, SIGNAL(LWRHSetVisionGoalPostsVisible(QString, bool)), LWSGraphicsView, SLOT(LWSGVVisionGoalPostsVisible(QString, bool)));

	connect(Messenger, SIGNAL(obsmsgUpdate(ObservationMessage, QString)), LWSGraphicsView, SLOT(observationMessageUpdateHandler(ObservationMessage, QString)));

	//Signal slot connections for Particles of Localization
	connect(LWSElementTree, SIGNAL(LWRHSetParticlesVisible(QString, bool)), LWSGraphicsView, SLOT(LWSGVParticlesVisible(QString, bool)));
	connect(Messenger, SIGNAL(localizationDataUpdate(LocalizationDataForGUI, QString)), LWSGraphicsView, SLOT(localizationDataUpdateHandler(LocalizationDataForGUI, QString)));

	//Signal slot connections for HFOV
	connect(LWSElementTree, SIGNAL(LWRHSetHFOVVisible(QString, bool)), LWSGraphicsView, SLOT(LWSGVHFOVVisible(QString, bool)));

	//Signal slot connections for Trace Of Estimated Robot Positions
	connect(LWSElementTree, SIGNAL(LWRHSetTraceVisible(QString, bool)), LWSGraphicsView, SLOT(LWSGVTraceVisible(QString, bool)));

	//Signal slot connections for Motion Walk Command
	connect(LWSElementTree, SIGNAL(LWRHSetMWCmdVisible(QString, bool)), LWSGraphicsView, SLOT(LWSGVMWCmdVisible(QString, bool)));
	connect(Messenger, SIGNAL(motionCommandUpdate(MotionWalkMessage, QString)), LWSGraphicsView, SLOT(motionCommandUpdateHandler(MotionWalkMessage, QString)));

	//SIGNAL SLOT CONNECTIONS FOR LOCAL POLAR MAP
	//Signal slot connections for Local Map Hosts ComboBox

	connect(availableGWHosts, SIGNAL(GWRHNewHostAdded(QString, QString)), availableLMHosts, SLOT(addComboBoxItem(QString, QString)));
	connect(availableGWHosts, SIGNAL(GWRHOldHostRemoved(QString)), availableLMHosts, SLOT(removeComboBoxItem(QString)));
	connect(availableGWHosts, SIGNAL(LWRHGameStateMsgUpdate(QIcon, QString, QString)), availableLMHosts, SLOT(setLWRHGameStateInfo(QIcon, QString, QString)));

	connect(availableLMHosts, SIGNAL(LWRHSubscriptionRequest(QString)), Messenger, SLOT(LMRHSubscriptionHandler(QString)));
	connect(availableLMHosts, SIGNAL(LWRHUnsubscriptionRequest(QString)), Messenger, SLOT(LMRHUnsubscriptionHandler(QString)));

	//Signal slot connections for user's preferences
	connect(availableLMHosts, SIGNAL(LWRHSubscriptionRequest(QString)), LPMElementTree, SLOT(LMELSubscriptionHandler(QString)));
	connect(availableLMHosts, SIGNAL(LWRHUnsubscriptionRequest(QString)), LPMElementTree, SLOT(LMELUnsubscriptionHandler(QString)));
	connect(availableLMHosts, SIGNAL(LWRHUnsubscriptionRequest(QString)), LPMGraphicsView, SLOT(resetRobotMap(QString)));

	//Signal slot connections for Obstacles, Path, TargetCoordinates
	connect(Messenger, SIGNAL(gridInfoUpdate(GridInfo, QString)), LPMGraphicsView, SLOT(gridInfoUpdateHandler(GridInfo, QString)));
	connect(LPMElementTree, SIGNAL(LMRHSetObstaclesVisible(QString, bool)), LPMGraphicsView, SLOT(LMObstaclesVisible(QString, bool)));
	connect(LPMElementTree, SIGNAL(LMRHSetPathVisible(QString, bool)), LPMGraphicsView, SLOT(LMPathVisible(QString, bool)));
	connect(LPMElementTree, SIGNAL(LMRHSetTargCoordVisible(QString, bool)), LPMGraphicsView, SLOT(LMTargetCoordVisible(QString, bool)));

	//SIGNAL SLOT CONNECTIONS FOR LOCAL ROBOT VIEW
	//Signal slot connections for Robot View ComboBox

	connect(availableGWHosts, SIGNAL(GWRHNewHostAdded(QString, QString)), availableLVHosts, SLOT(addComboBoxItem(QString, QString)));
	connect(availableGWHosts, SIGNAL(GWRHOldHostRemoved(QString)), availableLVHosts, SLOT(removeComboBoxItem(QString)));
	connect(availableGWHosts, SIGNAL(LWRHGameStateMsgUpdate(QIcon, QString, QString)), availableLVHosts, SLOT(setLWRHGameStateInfo(QIcon, QString, QString)));

	connect(availableLVHosts, SIGNAL(LWRHSubscriptionRequest(QString)), Messenger, SLOT(LVRHSubscriptionHandler(QString)));
	connect(availableLVHosts, SIGNAL(LWRHUnsubscriptionRequest(QString)), Messenger, SLOT(LVRHUnsubscriptionHandler(QString)));

	//Signal slot connections for user's preferences
	connect(availableLVHosts, SIGNAL(LWRHSubscriptionRequest(QString)), LRVElementList, SLOT(LVELSubscriptionHandler(QString)));
	connect(availableLVHosts, SIGNAL(LWRHUnsubscriptionRequest(QString)), LRVElementList, SLOT(LVELUnsubscriptionHandler(QString)));
	connect(availableLVHosts, SIGNAL(LWRHUnsubscriptionRequest(QString)), this->LRVLabel, SLOT(resetRobotView(QString)));

	//Signal slot connections for dispaying Robot's Raw Image
	connect(Messenger, SIGNAL(rawImageUpdate(KRawImage, QString)), LRVLabel, SLOT(kRawImageUpdateHandler(KRawImage, QString)));
	connect(LRVElementList, SIGNAL(LVRHSetRawImageVisible(QString, bool)), LRVLabel, SLOT(LVRawImageVisible(QString, bool)));
	connect(LRVElementList, SIGNAL(LVRHSetSegImageVisible(QString, bool)), LRVLabel, SLOT(LVSegImageVisible(QString, bool)));

	//SIGNAL SLOT CONNECTIONS FOR LOCAL SENSOR DATA
	//Signal slot connections for Local Remote Hosts ComboBox
	connect(availableGWHosts, SIGNAL(GWRHNewHostAdded(QString, QString)), this->availableLSHosts, SLOT(addComboBoxItem(QString, QString)));
	connect(availableGWHosts, SIGNAL(GWRHOldHostRemoved(QString)), this->availableLSHosts, SLOT(removeComboBoxItem(QString)));
	connect(availableGWHosts, SIGNAL(LWRHGameStateMsgUpdate(QIcon, QString, QString)), this->availableLSHosts, SLOT(setLWRHGameStateInfo(QIcon, QString, QString)));

	connect(availableLSHosts, SIGNAL(LWRHSubscriptionRequest(QString)), Messenger, SLOT(LSRHSubscriptionHandler(QString)));
	connect(availableLSHosts, SIGNAL(LWRHUnsubscriptionRequest(QString)), Messenger, SLOT(LSRHUnsubscriptionHandler(QString)));

	connect(availableLSHosts, SIGNAL(LWRHSubscriptionRequest(QString)), LSController, SLOT(LSCSubscriptionHandler(QString)));
	connect(availableLSHosts, SIGNAL(LWRHUnsubscriptionRequest(QString)), LSController, SLOT(LSCUnsubscriptionHandler(QString)));

	connect(Messenger, SIGNAL(sensorsDataUpdate(AllSensorValuesMessage, QString)), LSController, SLOT(sensorsDataUpdateHandler(AllSensorValuesMessage, QString)));


	//SIGNAL SLOT CONNECTIONS FOR KCC
	//Signal slot connections for KCC ComboBox
	connect(availableGWHosts, SIGNAL(GWRHNewHostAdded(QString, QString)), this->KCCTab, SLOT(addComboBoxItem(QString, QString)));
	connect(availableGWHosts, SIGNAL(GWRHOldHostRemoved(QString)), this->KCCTab, SLOT(removeComboBoxItem(QString)));
	connect(availableGWHosts, SIGNAL(LWRHGameStateMsgUpdate(QIcon, QString, QString)), this->KCCTab, SLOT(setLWRHGameStateInfo(QIcon, QString, QString)));

	connect(this->KCCTab, SIGNAL(LWRHSubscriptionRequest(QString)), Messenger, SLOT(KCCRHSubscriptionHandler(QString)));
	connect(this->KCCTab, SIGNAL(LWRHUnsubscriptionRequest(QString)), Messenger, SLOT(KCCRHUnsubscriptionHandler(QString)));

	connect(Messenger, SIGNAL(KCCRawImageUpdate(KRawImage, QString)), this->KCCTab, SLOT(changeImage(KRawImage, QString)));


	//Signal slot connections for XML
	//Signal slot connections for KCC ComboBox
	connect(availableGWHosts, SIGNAL(GWRHNewHostAdded(QString, QString)), this->XMLTab, SLOT(addComboBoxItem(QString, QString)));
	connect(availableGWHosts, SIGNAL(GWRHOldHostRemoved(QString)), this->XMLTab, SLOT(removeComboBoxItem(QString)));
	connect(availableGWHosts, SIGNAL(LWRHGameStateMsgUpdate(QIcon, QString, QString)), this->XMLTab, SLOT(setLWRHGameStateInfo(QIcon, QString, QString)));

	connect(this->XMLTab, SIGNAL(LWRHSubscriptionRequest(QString)), Messenger, SLOT(XMLRHSubscriptionHandler(QString)));
	connect(this->XMLTab, SIGNAL(LWRHUnsubscriptionRequest(QString)), Messenger, SLOT(XMLRHUnsubscriptionHandler(QString)));

	connect(Messenger, SIGNAL(xmlGenericAckReceived(GenericACK, QString)), this->XMLTab, SLOT(genericAckReceived(GenericACK, QString)));
	connect(this->XMLTab, SIGNAL(sendConfigMessage(ExternalConfig)), Messenger, SLOT(XMLPublishMessage(ExternalConfig)));

	//Signals-Slots for tab changes manipulation
	connect(this->KMTabWidget, SIGNAL(currentChanged(int)), this, SLOT(printCurrentTab(int)));
	connect(this->KMTabWidget, SIGNAL(currentChanged(int)), Messenger, SLOT(tabChangeHandler(int)));
	
	
	//SIGNAL SLOT CONNECTIONS FOR MAIN WINDOW
	connect(action_Quit, SIGNAL(triggered()), this, SLOT(quitKMonitor()));
	setWindowState(Qt::WindowMaximized);
}

KMonitor::~KMonitor()
{

}

void KMonitor::LSDInitialization()
{
	QList<QComboBox*> list;

	list.append(HYComboBox);	//0-1
	list.append(HPComboBox);

	list.append(AccXComboBox);	//2-6
	list.append(AccYComboBox);
	list.append(AccZComboBox);
	list.append(AngXComboBox);
	list.append(AngYComboBox);

	list.append(RSPComboBox);	//7-10
	list.append(RSRComboBox);
	list.append(REYComboBox);
	list.append(RERComboBox);

	list.append(LSPComboBox);	//11-14
	list.append(LSRComboBox);
	list.append(LEYComboBox);
	list.append(LERComboBox);

	list.append(RHYPComboBox);	//15-20
	list.append(RHRComboBox);
	list.append(RHPComboBox);
	list.append(RKPComboBox);
	list.append(RAPComboBox);
	list.append(RARComboBox);

	list.append(LHYPComboBox);  //21-26
	list.append(LHRComboBox);
	list.append(LHPComboBox);
	list.append(LKPComboBox);
	list.append(LAPComboBox);
	list.append(LARComboBox);

	list.append(RFsrFLComboBox);	//27-30
	list.append(RFsrRLComboBox);
	list.append(RFsrFRComboBox);
	list.append(RFsrRRComboBox);

	list.append(LFsrFLComboBox);	//31-34
	list.append(LFsrRLComboBox);
	list.append(LFsrFRComboBox);
	list.append(LFsrRRComboBox);

	LSController = new LSDController(list);

}

void KMonitor::quitKMonitor()
{
	std::cout << "KMonitor::quitKMonitor()" << std::endl;
	this->close();
	this->destroy();
}

void KMonitor::printCurrentTab(int index)
{
	//std::cout << "Allaksa kai eimai to :: " << index << std::endl;

}

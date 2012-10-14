#include "KMonitor.h"
#include "ui_KccHandler.h"

#include <QComboBox>
#include <QList>

KMonitor::KMonitor(QWidget *parent)
    : QMainWindow(parent), Messenger(NULL),
    ui(new Ui::KMonitor)
{
	ui->setupUi(this);

	Messenger = new KGUIMessenger();

	/****************** GLOBAL WORLD STATE TAB ************************/
	//SIGNAL SLOT CONNECTIONS FOR GLOBAL WORLD STATE
	//Signal slot connections for Robot Position & Orientation, Ball Estimation

	connect(Messenger, SIGNAL(addHost(QString, QString)), ui->GWSTab, SLOT(addNewItem(QString, QString)));
	connect(Messenger, SIGNAL(removeHost(QString)), ui->GWSTab, SLOT(removeOldItem(QString)));
	connect(Messenger, SIGNAL(updateGameState(QString, QString, QString)), ui->GWSTab, SLOT(updateGameState(QString, QString, QString)));
	
	connect(ui->GWSTab, SIGNAL(GWRHSubscriptionRequest(QString)), Messenger , SLOT(GWRHSubscriptionHandler(QString)));
	connect(ui->GWSTab, SIGNAL(GWRHUnsubscriptionRequest(QString)), Messenger, SLOT(GWRHUnsubscriptionHandler(QString)));

	//messages signal/slots
	connect(Messenger, SIGNAL(worldInfoUpdate(WorldInfo, QString)), ui->GWSTab, SLOT(worldInfoUpdateHandler(WorldInfo, QString)));
	connect(Messenger, SIGNAL(gameStateMessageUpdate(GameStateMessage, QString)), ui->GWSTab, SLOT(setKGFCGameStateInfo(GameStateMessage, QString)));

	/****************** LOCAL WORLD STATE TAB ************************/
	//SIGNAL SLOT CONNECTIONS FOR LOCAL WORLD STATE
	//Signal slot connections for Local Remote Hosts ComboBox

	connect(Messenger, SIGNAL(addHost(QString, QString)), ui->LWSTab, SLOT(addComboBoxItem(QString, QString)));
	connect(Messenger, SIGNAL(removeHost(QString)), ui->LWSTab, SLOT(removeComboBoxItem(QString)));
	connect(Messenger, SIGNAL(updateGameState(QString, QString, QString)), ui->LWSTab, SLOT(setLWRHGameStateInfo(QString, QString, QString)));

	connect(ui->LWSTab, SIGNAL(LWRHSubscriptionRequest(QString)), Messenger, SLOT(LWRHSubscriptionHandler(QString)));
	connect(ui->LWSTab, SIGNAL(LWRHUnsubscriptionRequest(QString)), Messenger, SLOT(LWRHUnsubscriptionHandler(QString)));
	
	connect(Messenger, SIGNAL(worldInfoUpdate(WorldInfo, QString)), ui->LWSTab, SLOT(worldInfoUpdateHandler(WorldInfo, QString)));
	connect(Messenger, SIGNAL(gameStateMessageUpdate(GameStateMessage, QString)), ui->LWSTab, SLOT(setKGFCGameStateInfo(GameStateMessage, QString)));
	connect(Messenger, SIGNAL(obsmsgUpdate(ObservationMessage, QString)), ui->LWSTab, SLOT(observationMessageUpdateHandler(ObservationMessage, QString)));
	connect(Messenger, SIGNAL(localizationDataUpdate(LocalizationDataForGUI, QString)), ui->LWSTab, SLOT(localizationDataUpdateHandler(LocalizationDataForGUI, QString)));
	connect(Messenger, SIGNAL(motionCommandUpdate(MotionWalkMessage, QString)), ui->LWSTab, SLOT(motionCommandUpdateHandler(MotionWalkMessage, QString)));
	
	/****************** SONARS TAB ************************/
	//SIGNAL SLOT CONNECTIONS FOR LOCAL POLAR MAP
	//Signal slot connections for Local Map Hosts ComboBox

	
	connect(Messenger, SIGNAL(addHost(QString, QString)), ui->LSonarTab, SLOT(addComboBoxItem(QString, QString)));
	connect(Messenger, SIGNAL(removeHost(QString)), ui->LSonarTab, SLOT(removeComboBoxItem(QString)));
	connect(Messenger, SIGNAL(updateGameState(QString, QString, QString)), ui->LSonarTab, SLOT(setLWRHGameStateInfo(QString, QString, QString)));

	connect(ui->LSonarTab, SIGNAL(LWRHSubscriptionRequest(QString)), Messenger, SLOT(LMRHSubscriptionHandler(QString)));
	connect(ui->LSonarTab, SIGNAL(LWRHUnsubscriptionRequest(QString)), Messenger, SLOT(LMRHUnsubscriptionHandler(QString)));

	//messages signal/slots
	connect(Messenger, SIGNAL(gridInfoUpdate(GridInfo, QString)), ui->LSonarTab, SLOT(gridInfoUpdateHandler(GridInfo, QString)));

	/****************** LOCAL ROBOT VIEW TAB ************************/
	//SIGNAL SLOT CONNECTIONS FOR LOCAL ROBOT VIEW
	//Signal slot connections for Robot View ComboBox

	connect(Messenger, SIGNAL(addHost(QString, QString)), ui->LCamTab, SLOT(addComboBoxItem(QString, QString)));
	connect(Messenger, SIGNAL(removeHost(QString)), ui->LCamTab, SLOT(removeComboBoxItem(QString)));
	connect(Messenger, SIGNAL(updateGameState(QString, QString, QString)), ui->LCamTab, SLOT(setLWRHGameStateInfo(QString, QString, QString)));

	connect(ui->LCamTab, SIGNAL(LWRHSubscriptionRequest(QString)), Messenger, SLOT(LVRHSubscriptionHandler(QString)));
	connect(ui->LCamTab, SIGNAL(LWRHUnsubscriptionRequest(QString)), Messenger, SLOT(LVRHUnsubscriptionHandler(QString)));

	//messages signal/slots
	connect(Messenger, SIGNAL(rawImageUpdate(KRawImage, QString)), ui->LCamTab, SLOT(kRawImageUpdateHandler(KRawImage, QString)));
	/****************** LOCAL SENSOR VIEW TAB ************************/
	//SIGNAL SLOT CONNECTIONS FOR LOCAL SENSOR DATA
	//Signal slot connections for Local Remote Hosts ComboBox
	
	connect(Messenger, SIGNAL(addHost(QString, QString)), ui->LSDTab, SLOT(addComboBoxItem(QString, QString)));
	connect(Messenger, SIGNAL(removeHost(QString)), ui->LSDTab, SLOT(removeComboBoxItem(QString)));
	connect(Messenger, SIGNAL(updateGameState(QString, QString, QString)), ui->LSDTab, SLOT(setLWRHGameStateInfo(QString, QString, QString)));

	connect(ui->LSDTab, SIGNAL(LWRHSubscriptionRequest(QString)), Messenger, SLOT(LSRHSubscriptionHandler(QString)));
	connect(ui->LSDTab, SIGNAL(LWRHUnsubscriptionRequest(QString)), Messenger, SLOT(LSRHUnsubscriptionHandler(QString)));

	//messages signal/slots
	connect(Messenger, SIGNAL(sensorsDataUpdate(AllSensorValuesMessage, QString)), ui->LSDTab, SLOT(sensorsDataUpdateHandler(AllSensorValuesMessage, QString)));

	/****************** KCC TAB ************************/
	//SIGNAL SLOT CONNECTIONS FOR KCC
	//Signal slot connections for KCC ComboBox
	connect(Messenger, SIGNAL(addHost(QString, QString)), ui->KCCTab, SLOT(addComboBoxItem(QString, QString)));
	connect(Messenger, SIGNAL(removeHost(QString)), ui->KCCTab, SLOT(removeComboBoxItem(QString)));
	connect(Messenger, SIGNAL(updateGameState(QString, QString, QString)), ui->KCCTab, SLOT(setLWRHGameStateInfo(QString, QString, QString)));

	connect(ui->KCCTab, SIGNAL(LWRHSubscriptionRequest(QString)), Messenger, SLOT(KCCRHSubscriptionHandler(QString)));
	connect(ui->KCCTab, SIGNAL(LWRHUnsubscriptionRequest(QString)), Messenger, SLOT(KCCRHUnsubscriptionHandler(QString)));

	//messages signal/slots
	connect(Messenger, SIGNAL(KCCRawImageUpdate(KRawImage, QString)), ui->KCCTab, SLOT(changeImage(KRawImage, QString)));

	/****************** XML TAB ************************/
	//Signal slot connections for XML
	//Signal slot connections for XML ComboBox
	connect(Messenger, SIGNAL(addHost(QString, QString)), ui->XMLTab, SLOT(addComboBoxItem(QString, QString)));
	connect(Messenger, SIGNAL(removeHost(QString)), ui->XMLTab, SLOT(removeComboBoxItem(QString)));
	connect(Messenger, SIGNAL(updateGameState(QString, QString, QString)), ui->XMLTab, SLOT(setLWRHGameStateInfo(QString, QString, QString)));

	connect(ui->XMLTab, SIGNAL(LWRHSubscriptionRequest(QString)), Messenger, SLOT(XMLRHSubscriptionHandler(QString)));
	connect(ui->XMLTab, SIGNAL(LWRHUnsubscriptionRequest(QString)), Messenger, SLOT(XMLRHUnsubscriptionHandler(QString)));

	//messages signal/slots
	connect(Messenger, SIGNAL(xmlGenericAckReceived(GenericACK, QString)), ui->XMLTab, SLOT(genericAckReceived(GenericACK, QString)));
	connect(ui->XMLTab, SIGNAL(sendConfigMessage(ExternalConfig)), Messenger, SLOT(XMLPublishMessage(ExternalConfig)));

	/****************** GENERIC ************************/
	//Signals-Slots for tab changes manipulation
	connect(ui->KMTabWidget, SIGNAL(currentChanged(int)), this, SLOT(printCurrentTab(int)));
	connect(ui->KMTabWidget, SIGNAL(currentChanged(int)), Messenger, SLOT(tabChangeHandler(int)));
	
	
	//SIGNAL SLOT CONNECTIONS FOR MAIN WINDOW
	connect(ui->action_Quit, SIGNAL(triggered()), this, SLOT(quitKMonitor()));
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
	//std::cout << "Allaksa kai eimai to :: " << index << std::endl;

}

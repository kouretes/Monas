#include "KMonitor.h"
#include "ui_KccHandler.h"

#include <QComboBox>
#include <QList>

KMonitor::KMonitor(QWidget *parent) : 
	QMainWindow(parent),
	Messenger(NULL),
	ui(new Ui::KMonitor) {

	ui->setupUi(this);
	Messenger = new KGUIMessenger();

	for(int i = 1 ; i < ui->KMTabWidget->count() ; i++) {
		connect(Messenger, SIGNAL(addHost(QString, QString)), ui->KMTabWidget->widget(i), SLOT(addComboBoxItem(QString, QString)) );
		connect(Messenger, SIGNAL(removeHost(QString)), ui->KMTabWidget->widget(i), SLOT(removeComboBoxItem(QString)) );
		connect(Messenger, SIGNAL(updateGameState(QString, QString, QString)), ui->KMTabWidget->widget(i), SLOT(setGameStateInfo(QString, QString, QString)) );
		connect(Messenger, SIGNAL(changeAllTabsToHost(QString)), ui->KMTabWidget->widget(i), SLOT(changeToHost(QString)) );
		connect(ui->KMTabWidget->widget(i), SIGNAL(SubscriptionRequest(QString)), Messenger, SLOT(SubscriptionHandler(QString)) );
	}

	/**
	 * GLOBAL WORLD STATE TAB
	 * SIGNAL SLOT CONNECTIONS FOR GLOBAL WORLD STATE
	 * Signal slot connections for Robot Position & Orientation, Ball Estimation
	 */
	connect(Messenger, SIGNAL(addHost(QString, QString)), ui->GWSTab, SLOT(addNewItem(QString, QString)) );
	
	connect(Messenger, SIGNAL(removeHost(QString)), ui->GWSTab, SLOT(removeOldItem(QString)) );
	
	connect(Messenger, SIGNAL(updateGameState(QString, QString, QString)), ui->GWSTab, SLOT(updateGameState(QString, QString, QString)) );
	
	connect(ui->GWSTab, SIGNAL(GWRHSubscriptionRequest(QString)), Messenger , SLOT(GWSubscriptionHandler(QString)) );
	
	connect(ui->GWSTab, SIGNAL(GWRHUnsubscriptionRequest(QString)), Messenger, SLOT(GWUnsubscriptionHandler(QString)) );
	
	// messages signal/slots
	connect(Messenger, SIGNAL(worldInfoUpdate(WorldInfo, QString)), ui->GWSTab, SLOT(worldInfoUpdateHandler(WorldInfo, QString)) );
	
	connect(Messenger, SIGNAL(sharedWorldInfoUpdate(SharedWorldInfo, QString)), ui->GWSTab, SLOT(sharedWorldInfoUpdateHandler(SharedWorldInfo, QString)) );
	
	connect(Messenger, SIGNAL(gameStateMessageUpdate(GameStateMessage, QString)), ui->GWSTab, SLOT(setKGFCGameStateInfo(GameStateMessage, QString)) );
	
	/**
	 * LOCAL WORLD STATE TAB
	 * SIGNAL SLOT CONNECTIONS FOR LOCAL WORLD STATE
	 * Signal slot connections for Local Remote Hosts ComboBox
	 */
	connect(Messenger, SIGNAL(worldInfoUpdate(WorldInfo, QString)), ui->LWSTab, SLOT(worldInfoUpdateHandler(WorldInfo, QString)) );
	
	connect(Messenger, SIGNAL(sharedWorldInfoUpdate(SharedWorldInfo, QString)), ui->LWSTab, SLOT(sharedWorldInfoUpdateHandler(SharedWorldInfo, QString)) );
	
	connect(Messenger, SIGNAL(gameStateMessageUpdate(GameStateMessage, QString)), ui->LWSTab, SLOT(setKGFCGameStateInfo(GameStateMessage, QString)) );
	
	connect(Messenger, SIGNAL(obsmsgUpdate(ObservationMessage, QString)), ui->LWSTab, SLOT(observationMessageUpdateHandler(ObservationMessage, QString)) );
	
	connect(Messenger, SIGNAL(localizationDataUpdate(LocalizationDataForGUI, QString)), ui->LWSTab, SLOT(localizationDataUpdateHandler(LocalizationDataForGUI, QString)) );
	
	connect(Messenger, SIGNAL(motionCommandUpdate(MotionWalkMessage, QString)), ui->LWSTab, SLOT(motionCommandUpdateHandler(MotionWalkMessage, QString)) );
	
    connect(Messenger, SIGNAL(EKFMHypothesisUpdate(EKFMHypothesis, QString)), ui->LWSTab, SLOT(EKFMHypothesisUpdateHandler(EKFMHypothesis, QString)) );
    
    connect(Messenger, SIGNAL(OdometryUpdate(OdometryInfoMessage, QString)), ui->LWSTab, SLOT(OdometryUpdateHandler(OdometryInfoMessage, QString)) );	
    
    connect(Messenger, SIGNAL(formationDataUpdate(FormationDataForGUI, QString)), ui->LWSTab, SLOT(formationDataUpdateHandler(FormationDataForGUI, QString)) );
    
    connect(Messenger, SIGNAL(PSODataUpdate(PSODataForGUI, QString)), ui->LWSTab, SLOT(PSODataUpdateHandler(PSODataForGUI, QString)) );
    
    connect(Messenger, SIGNAL(MappingDataUpdate(MappingDataForGUI, QString)), ui->LWSTab, SLOT(MappingDataUpdateHandler(MappingDataForGUI, QString)) );
	
	/**
	 * LOCAL POLAR MAP TAB
	 * SIGNAL SLOT CONNECTIONS FOR LOCAL POLAR MAP
	 * Signal slot connections for Local Map Hosts ComboBox
	 */
	connect(Messenger, SIGNAL(gridInfoUpdate(GridInfo, QString)), ui->LSonarTab, SLOT(gridInfoUpdateHandler(GridInfo, QString)) );
	
	/**
	 * LOCAL SENSOR VIEW TAB
	 * SIGNAL SLOT CONNECTIONS FOR LOCAL SENSOR VIEW
	 * Signal slot connections for Local Remote Hosts ComboBox
	 */
	connect(Messenger, SIGNAL(sensorsDataUpdate(AllSensorValuesMessage, QString)), ui->LSDTab, SLOT(sensorsDataUpdateHandler(AllSensorValuesMessage, QString)) );
	
	/**
	 * KCC TAB
	 * SIGNAL SLOT CONNECTIONS FOR KCC
	 * Signal slot connections for KCC ComboBox
	 */
	connect(Messenger, SIGNAL(rawImage(KRawImage, QString, boost::posix_time::ptime)), ui->KCCTab, SLOT(changeImage(KRawImage, QString, boost::posix_time::ptime)) );
	
	connect(Messenger, SIGNAL(visionDebugData(VisionDebugMessage, QString)), ui->KCCTab, SLOT(visionDebugData(VisionDebugMessage, QString)) );
	
	connect(Messenger, SIGNAL(GenericAckReceived(GenericACK, QString)), ui->KCCTab, SLOT(genericAckReceived(GenericACK, QString)) );
	
	connect(ui->KCCTab, SIGNAL(sendCameraCalibrationMessage(CameraCalibration)), Messenger, SLOT(KccPublishMessage(CameraCalibration)) );
	
	/**
	 * XML TAB
	 * SIGNAL SLOT CONNECTIONS FOR XML
	 * Signal slot connections for XML ComboBox
	 */
	connect(Messenger, SIGNAL(GenericAckReceived(GenericACK, QString)), ui->XMLTab, SLOT(genericAckReceived(GenericACK, QString)) );
	
	connect(ui->XMLTab, SIGNAL(sendConfigMessage(ExternalConfig)), Messenger, SLOT(XMLPublishMessage(ExternalConfig)) );
	
	/**
	 * COMMANDS TAB
	 * SIGNAL SLOT CONNECTIONS FOR COMMANDS
	 * Signal slot connections for Commands ComboBox
	 */
	connect(Messenger, SIGNAL(GenericAckReceived(GenericACK, QString)), ui->ComTab, SLOT(genericAckReceived(GenericACK, QString)) );
	
	connect(ui->ComTab, SIGNAL(sendExternalCommand(ExternalCommand)), Messenger, SLOT(CommandPublishMessage(ExternalCommand)) );
	
	/**
	 * GENERIC
	 * Signals-Slots for tab changes manipulation
	 */
	connect(ui->KMTabWidget, SIGNAL(currentChanged(int)), Messenger, SLOT(tabChangeHandler(int)) );
	ui->KMTabWidget->setCurrentIndex(1);
	
	// SIGNAL SLOT CONNECTIONS FOR MAIN WINDOW
	connect(ui->action_Quit, SIGNAL(triggered()), this, SLOT(quitKMonitor()) );
	setWindowState(Qt::WindowMaximized);
}

KMonitor::~KMonitor() { }

void KMonitor::closeEvent(QCloseEvent *event){
	std::cout << "KMonitor::quitKMonitor()" << std::endl;
	this->close();
	this->destroy();
	exit(0);
}

void KMonitor::quitKMonitor() {
	std::cout << "KMonitor::quitKMonitor()" << std::endl;
	this->close();
	this->destroy();
	exit(0);
}

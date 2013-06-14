#include "CommandsController.h"
#include "ui_CommandsController.h"
#include "core/architecture/configurator/Configurator.hpp"

using namespace std;
CommandsController::CommandsController(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::CommandsController) {
	
	ui->setupUi(this);
	availableCommandHosts = new HostsComboBox(ui->commandComboBox);
	connect(availableCommandHosts, SIGNAL(SubscriptionRequest(QString)), this, SLOT(SubscriptionHandler(QString)) );
	
	// SIMPLE ACTIONS
	connect(ui->sitPb, SIGNAL(clicked()), this, SLOT(sendSit()) );
	connect(ui->stiffnessOffPb, SIGNAL(clicked()), this, SLOT(sendStiffnessOff()) );
	connect(ui->shutdownPb, SIGNAL(clicked()), this, SLOT(sendShutDown()) );
	connect(ui->stopPb, SIGNAL(clicked()), this, SLOT(sendStopRobot()) );
	
	// GAMECONTROLLER
	connect(ui->initialPb, SIGNAL(clicked()), this, SLOT(sendInitial()) );
	connect(ui->readyPb, SIGNAL(clicked()), this, SLOT(sendReady()) );
	connect(ui->setPb, SIGNAL(clicked()), this, SLOT(sendSet()) );
	connect(ui->playPb, SIGNAL(clicked()), this, SLOT(sendPlay()) );
	connect(ui->penalizedPb, SIGNAL(clicked()), this, SLOT(sendPenalised()) );
	connect(ui->finishPb, SIGNAL(clicked()), this, SLOT(sendFinish()) );
	connect(ui->overridePb, SIGNAL(clicked()), this, SLOT(sendOverride()) );
	
	// LOCALIZATION
	connect(ui->initParPb, SIGNAL(clicked()), this, SLOT(sendLocReset()) );
	ui->xText->setText("0.0");
	ui->yText->setText("0.0");
	ui->phiText->setText("0.0");
	
	// ACTIONS
	connect(ui->actionPb, SIGNAL(clicked()), this, SLOT(sendAction()) );
	addActionComboBoxItems(Configurator::Instance().getDirectoryPath() + "xar");
	addActionComboBoxItems(Configurator::Instance().getDirectoryPath() + "kme");
	
	// STIFFNESS
	connect(ui->stiffSlider, SIGNAL(valueChanged(int)), this, SLOT(stiffSliderMoved(int)) );
	connect(ui->stiffEdit, SIGNAL(textChanged(QString)), this, SLOT(stiffTextChanged(QString)) );
	connect(ui->stiffSend, SIGNAL(clicked()), this, SLOT(sendStiffness()) );
	ui->stiffSlider->setSliderPosition(80);
	ui->bodyRb->setChecked(true);
	
	// HEAD ACTIONS
	connect(ui->stopheadPb, SIGNAL(clicked()), this, SLOT(sendHeadStop()) );
	connect(ui->smartscanheadPb, SIGNAL(clicked()), this, SLOT(sendHeadSmartScan()) );
	connect(ui->scanheadPb, SIGNAL(clicked()), this, SLOT(sendHeadScan()) );
	connect(ui->lookdownheadPb, SIGNAL(clicked()), this, SLOT(sendHeadLookDown()) );
	connect(ui->localizeheadPb, SIGNAL(clicked()), this, SLOT(sendHeadLocalize()) );
	connect(ui->localizafarheadPb, SIGNAL(clicked()), this, SLOT(sendHeadLocalizeFar()) );
	connect(ui->headSlider, SIGNAL(valueChanged(int)), this, SLOT(headSliderMoved(int)) );
	connect(ui->headEdit, SIGNAL(textChanged(QString)), this, SLOT(headTextChanged(QString)) );
	//connect(ui->stiffSend, SIGNAL(clicked()), this, SLOT(sendStiffness()));
	ui->headSlider->setSliderPosition(80);
	ui->gameGroup->setStyleSheet("QGroupBox {border: 2px solid gray; border-radius: 0px; padding-top: 10px;} QGroupBox::title { background-color: transparent; subcontrol-position: top left; padding:2 13px;}");
	ui->stiffGroup->setStyleSheet("QGroupBox {border: 2px solid gray; border-radius: 0px; padding-top: 10px;} QGroupBox::title { background-color: transparent; subcontrol-position: top left; padding:2 13px;}");
	ui->actionGroup->setStyleSheet("QGroupBox {border: 2px solid gray; border-radius: 0px; padding-top: 10px;} QGroupBox::title { background-color: transparent; subcontrol-position: top left; padding:2 13px;}");
	ui->varGroup->setStyleSheet("QGroupBox {border: 2px solid gray; border-radius: 0px; padding-top: 10px;} QGroupBox::title { background-color: transparent; subcontrol-position: top left; padding:2 13px;}");
	ui->localGroup->setStyleSheet("QGroupBox {border: 2px solid gray; border-radius: 0px; padding-top: 10px;} QGroupBox::title { background-color: transparent; subcontrol-position: top left; padding:2 13px;}");
	ui->headGroup->setStyleSheet("QGroupBox {border: 2px solid gray; border-radius: 0px; padding-top: 10px;} QGroupBox::title { background-color: transparent; subcontrol-position: top left; padding:2 13px;}");
	ui->statusText->setText("Waiting");
	// timer for the acks
	timer = new QTimer();
	timer->setInterval(50);
	connect(this->timer, SIGNAL(timeout()), this, SLOT(retransmitMessage()) );
	lastMessageACKed = true;
	numOfRetransmits = 0;
	timestamp = boost::posix_time::microsec_clock::universal_time();
}

void CommandsController::genericAckReceived(GenericACK ack, QString hostid) {
	if(boost::posix_time::from_iso_string(ack.messageid() ) == timestamp && !lastMessageACKed) {
		timer->stop();
		numOfRetransmits = 0;
		lastMessageACKed = true;

		if(ack.ownlock())
			ui->statusText->setText("succeeded");
		else
			ui->statusText->setText("Not lock owner");
	}
}

void CommandsController::sendInitial() {
	outmsg.clear_floatvars();
	outmsg.clear_stringvars();
	outmsg.set_commandid(ExternalCommand::SIMPLE_ACTIONS_SIZE + ExternalCommand::INITIAL);
	sendOutMsg();
}

void CommandsController::sendReady() {
	outmsg.clear_floatvars();
	outmsg.clear_stringvars();
	outmsg.set_commandid(ExternalCommand::SIMPLE_ACTIONS_SIZE + ExternalCommand::READY);
	sendOutMsg();
}

void CommandsController::sendSet() {
	outmsg.clear_floatvars();
	outmsg.clear_stringvars();
	outmsg.set_commandid(ExternalCommand::SIMPLE_ACTIONS_SIZE + ExternalCommand::SET);
	sendOutMsg();
}

void CommandsController::sendPlay() {
	outmsg.clear_floatvars();
	outmsg.clear_stringvars();
	outmsg.set_commandid(ExternalCommand::SIMPLE_ACTIONS_SIZE + ExternalCommand::PLAY);
	sendOutMsg();
}

void CommandsController::sendPenalised() {
	outmsg.clear_floatvars();
	outmsg.clear_stringvars();
	outmsg.set_commandid(ExternalCommand::SIMPLE_ACTIONS_SIZE + ExternalCommand::PENALISED);
	sendOutMsg();
}

void CommandsController::sendFinish() {
	outmsg.clear_floatvars();
	outmsg.clear_stringvars();
	outmsg.set_commandid(ExternalCommand::SIMPLE_ACTIONS_SIZE + ExternalCommand::FINISH);
	sendOutMsg();
}

void CommandsController::sendOverride() {
	outmsg.clear_floatvars();
	outmsg.clear_stringvars();
	outmsg.set_commandid(ExternalCommand::SIMPLE_ACTIONS_SIZE + ExternalCommand::OVERRIDE);
	sendOutMsg();
}

void CommandsController::sendShutDown() {
	outmsg.clear_floatvars();
	outmsg.clear_stringvars();
	outmsg.set_commandid(ExternalCommand::SHUTDOWN);
	sendOutMsg();
}

void CommandsController::sendStiffnessOff() {
	outmsg.clear_floatvars();
	outmsg.clear_stringvars();
	outmsg.set_commandid(ExternalCommand::STIFFNESS_OFF);
	sendOutMsg();
}

void CommandsController::sendStopRobot() {
	outmsg.clear_floatvars();
	outmsg.clear_stringvars();
	outmsg.set_commandid(ExternalCommand::STOP_ROBOT);
	sendOutMsg();
}

void CommandsController::sendSit() {
	outmsg.clear_floatvars();
	outmsg.clear_stringvars();
	outmsg.set_commandid(ExternalCommand::SIT);
	sendOutMsg();
}

void CommandsController::sendLocReset() {
	outmsg.clear_floatvars();
	outmsg.clear_stringvars();
	outmsg.set_commandid(ExternalCommand::SIMPLE_ACTIONS_SIZE + ExternalCommand::GAMECONTROLLER_SIZE + ExternalCommand::INIT_PARTICLES);
	outmsg.add_floatvars(ui->xText->text().toFloat());
	outmsg.add_floatvars(ui->yText->text().toFloat());
	outmsg.add_floatvars(ui->phiText->text().toFloat());
	sendOutMsg();
}

void CommandsController::sendAction() {
	outmsg.clear_floatvars();
	outmsg.clear_stringvars();
	outmsg.set_commandid(ExternalCommand::SIMPLE_ACTIONS_SIZE + ExternalCommand::GAMECONTROLLER_SIZE + ExternalCommand::LOCALIZATION_SIZE + ExternalCommand::EXECUTE_ACTION);
	outmsg.set_stringvars(ui->actionCb->currentText().toStdString());
	sendOutMsg();
}

void CommandsController::sendStiffness() {
	outmsg.clear_floatvars();
	outmsg.clear_stringvars();
	outmsg.set_commandid(ExternalCommand::SIMPLE_ACTIONS_SIZE + ExternalCommand::GAMECONTROLLER_SIZE + ExternalCommand::LOCALIZATION_SIZE + ExternalCommand::SET_STIFFNESS);

	if(ui->headRb->isChecked() == true)
		outmsg.set_stringvars("Head");
	else if(ui->larmRb->isChecked() == true)
		outmsg.set_stringvars("LArm");
	else if(ui->rarmRb->isChecked() == true)
		outmsg.set_stringvars("RArm");
	else if(ui->llegRb->isChecked() == true)
		outmsg.set_stringvars("LLeg");
	else if(ui->rlegRb->isChecked() == true)
		outmsg.set_stringvars("RLeg");
	else
		outmsg.set_stringvars("Body");

	outmsg.add_floatvars(ui->stiffEdit->text().toFloat());
	sendOutMsg();
}

void CommandsController::sendHeadScan() {
	sendHeadMessage(ExternalCommand::SCAN_AND_TRACK);
}

void CommandsController::sendHeadSmartScan() {
	sendHeadMessage(ExternalCommand::SMART_SCAN);
}

void CommandsController::sendHeadLocalize() {
	sendHeadMessage(ExternalCommand::LOCALIZE);
}

void CommandsController::sendHeadLocalizeFar() {
	sendHeadMessage(ExternalCommand::LOCALIZE_FAR);
}

void CommandsController::sendHeadLookDown() {
	sendHeadMessage(ExternalCommand::LOOK_DOWN);
}

void CommandsController::sendHeadStop() {
	sendHeadMessage(ExternalCommand::NOTHING);
}

void CommandsController::sendHeadMessage(int headCommandId) {
	outmsg.clear_floatvars();
	outmsg.clear_stringvars();
	outmsg.set_commandid(ExternalCommand::SIMPLE_ACTIONS_SIZE + ExternalCommand::GAMECONTROLLER_SIZE + ExternalCommand::LOCALIZATION_SIZE + ExternalCommand::MOTION_SIZE + headCommandId);
	outmsg.add_floatvars(ui->headEdit->text().toFloat());
	sendOutMsg();
}

void CommandsController::sendOutMsg() {
	if(lastMessageACKed) {
		// target host is be setting to the KGuiMessenger class
		ui->statusText->setText("Waiting");
		lastMessageACKed = false;
		timer->start();
		timestamp = boost::posix_time::microsec_clock::universal_time();
		string messageid = boost::posix_time::to_iso_string(timestamp);
		outmsg.set_messageid(messageid);
		lastmsg = outmsg;
		emit sendExternalCommand(outmsg);
	}
}

void CommandsController::retransmitMessage() {
	numOfRetransmits++;

	if(numOfRetransmits != MAX_RETRANSMITS)
		emit sendExternalCommand(lastmsg);
	else {
		numOfRetransmits = 0;
		timer->stop();
		timestamp = boost::posix_time::microsec_clock::universal_time();
		ui->statusText->setText("Retransmit failed");
		lastMessageACKed = true;
	}
}

void CommandsController::stiffTextChanged(QString newText) {
	ui->stiffSlider->blockSignals(true);
	float fvalue = newText.toFloat();
	int value = fvalue *(ui->stiffSlider->maximum());
	ui->stiffSlider->setSliderPosition(value);
	ui->stiffSlider->blockSignals(false);
}

void CommandsController::stiffSliderMoved(int value) {
	ui->stiffEdit->blockSignals(true);
	float fvalue =(float) value /(float)(ui->stiffSlider->maximum());
	QString ftext = "";
	ftext.setNum(fvalue, 'g', 3);
	ui->stiffEdit->setText(ftext);
	ui->stiffEdit->blockSignals(false);
}

void CommandsController::headTextChanged(QString newText) {
	ui->headSlider->blockSignals(true);
	float fvalue =(newText.toFloat() - minHeadSpeed) /(maxHeadSpeed - minHeadSpeed);
	int value = fvalue *(ui->headSlider->maximum());
	ui->headSlider->setSliderPosition(value);
	ui->headSlider->blockSignals(false);
}

void CommandsController::headSliderMoved(int value) {
	ui->headEdit->blockSignals(true);
	float fvalue =(float) value /(float)(ui->headSlider->maximum());
	fvalue = fvalue *(maxHeadSpeed - minHeadSpeed) + minHeadSpeed;
	QString ftext = "";
	ftext.setNum(fvalue, 'g', 3);
	ui->headEdit->setText(ftext);
	ui->headEdit->blockSignals(false);
}

void CommandsController::addActionComboBoxItems(string path) {
	boost::filesystem::path p(path);

	if(boost::filesystem::is_directory(p)) {
		vector<boost::filesystem::path> v;
		copy(boost::filesystem::directory_iterator(p), boost::filesystem::directory_iterator(), back_inserter(v));
		sort(v.begin(), v.end());

		for(vector<boost::filesystem::path>::iterator iter = v.begin() ; iter != v.end() ; ++iter) { //directory_iterator itr(p); itr != directory_iterator(); ++itr)
			if(boost::filesystem::is_regular_file(*iter)) {
				string filename = iter->string();
				size_t pos = filename.find_last_of("/");

				if(pos != string::npos)
					filename.erase(0, pos + 1);

				ui->actionCb->addItem(QString::fromStdString(filename), QVariant() );
			}
		}
	}
}

void CommandsController::changeToHost(QString data1) {
	availableCommandHosts->changeItem(data1);
}

void CommandsController::addComboBoxItem(QString data1, QString data2) {
	availableCommandHosts->addComboBoxItem(data1, data2);
}

void CommandsController::removeComboBoxItem(QString data1) {
	availableCommandHosts->removeComboBoxItem(data1);
}

void CommandsController::setGameStateInfo(QString data1, QString data2, QString data3) {
	availableCommandHosts->setGameStateInfo(data1, data2, data3);
}

void CommandsController::SubscriptionHandler(QString data1) {
	emit SubscriptionRequest(data1);
}


CommandsController::~CommandsController() {
	delete ui;
}


#include "KccCameraSettings.h"
#include "ui_KccCameraSettings.h"
#include "core/architecture/configurator/Configurator.hpp"
#include "tools/toString.h"
using namespace std;
KccCameraSettings::KccCameraSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::KccCameraSettings)
{
    ui->setupUi(this);

	connect(ui->gainSlider, SIGNAL(valueChanged(int)), this, SLOT(gainSliderMoved(int)) );
	connect(ui->gainEdit, SIGNAL(textChanged(QString)), this, SLOT(gainTextChanged(QString)) );
	connect(ui->contrastSlider, SIGNAL(valueChanged(int)), this, SLOT(contrastSliderMoved(int)) );
	connect(ui->contrastEdit, SIGNAL(textChanged(QString)), this, SLOT(contrastTextChanged(QString)) );
	connect(ui->redBalanceSlider, SIGNAL(valueChanged(int)), this, SLOT(redBalanceSliderMoved(int)) );
	connect(ui->redBalanceEdit, SIGNAL(textChanged(QString)), this, SLOT(redBalanceTextChanged(QString)) );
	connect(ui->blueBalanceSlider, SIGNAL(valueChanged(int)), this, SLOT(blueBalanceSliderMoved(int)) );
	connect(ui->blueBalanceEdit, SIGNAL(textChanged(QString)), this, SLOT(blueBalanceTextChanged(QString)) );
	connect(ui->greenGainSlider, SIGNAL(valueChanged(int)), this, SLOT(greenGainSliderMoved(int)) );
	connect(ui->greenGainEdit, SIGNAL(textChanged(QString)), this, SLOT(greenGainTextChanged(QString)) );

	connect(ui->pbSave, SIGNAL(clicked()), this, SLOT(saveToLocalConfigurator()) );
	connect(ui->pbSend, SIGNAL(clicked()), this, SLOT(sendCalibrationMessage()) );
	ui->gainSlider->setSliderPosition(atoi(Configurator::Instance().findValueForKey("camera.Gain").c_str()));
	ui->contrastSlider->setSliderPosition(atoi(Configurator::Instance().findValueForKey("camera.Contrast").c_str()));
	ui->greenGainSlider->setSliderPosition(atoi(Configurator::Instance().findValueForKey("camera.GreenChannelGain").c_str()));
	ui->redBalanceSlider->setSliderPosition(atoi(Configurator::Instance().findValueForKey("camera.RedBalance").c_str()));
	ui->blueBalanceSlider->setSliderPosition(atoi(Configurator::Instance().findValueForKey("camera.BlueBalance").c_str()));
	
	// timer for the acks
	timer = new QTimer();
	timer->setInterval(50);
	connect(this->timer, SIGNAL(timeout()), this, SLOT(retransmitMessage()) );
	lastMessageACKed = true;
	numOfRetransmits = 0;
	timestamp = boost::posix_time::microsec_clock::universal_time();
}

KccCameraSettings::~KccCameraSettings()
{
    delete ui;
}

void KccCameraSettings::genericAckReceived(GenericACK ack, QString hostid) {
	if(boost::posix_time::from_iso_string(ack.messageid() ) == timestamp && !lastMessageACKed) {
		timer->stop();
		numOfRetransmits = 0;
		lastMessageACKed = true;
		ui->pbSend->setEnabled(true);
	}
}

void KccCameraSettings::sendCalibrationMessage() {
	if(lastMessageACKed) {
		outmsg.set_gain(ui->gainSlider->value());
		outmsg.set_contrast(ui->contrastSlider->value());
		outmsg.set_redbalance(ui->redBalanceSlider->value());
		outmsg.set_bluebalance(ui->blueBalanceSlider->value());
		outmsg.set_greengain(ui->greenGainSlider->value());
		// target host is be setting to the KGuiMessenger class
		lastMessageACKed = false;
		timer->start();
		timestamp = boost::posix_time::microsec_clock::universal_time();
		string messageid = boost::posix_time::to_iso_string(timestamp);
		outmsg.set_messageid(messageid);
		lastmsg = outmsg;
		emit sendCameraCalibrationMessage(outmsg);
		ui->pbSend->setEnabled(false);
	}
}

void KccCameraSettings::retransmitMessage() {
	numOfRetransmits++;

	if(numOfRetransmits != MAX_RETRANSMITS){
		emit sendCameraCalibrationMessage(lastmsg);
	}else{
		numOfRetransmits = 0;
		timer->stop();
		timestamp = boost::posix_time::microsec_clock::universal_time();
		ui->pbSend->setEnabled(true);
		lastMessageACKed = true;
	}
}

void KccCameraSettings::saveToLocalConfigurator()
{
	vector<pair<string, string> > dataForWrite;
	pair<string, string> temp;
	temp.first = "camera.Gain";
	temp.second = _toString(ui->gainSlider->value());
	dataForWrite.push_back(temp);
	temp.first = "camera.Contrast";
	temp.second = _toString(ui->contrastSlider->value());
	dataForWrite.push_back(temp);
	temp.first = "camera.GreenChannelGain";
	temp.second = _toString(ui->greenGainSlider->value());
	dataForWrite.push_back(temp);
	temp.first = "camera.RedBalance";
	temp.second = _toString(ui->redBalanceSlider->value());
	dataForWrite.push_back(temp);
	temp.first = "camera.BlueBalance";
	temp.second = _toString(ui->blueBalanceSlider->value());
	dataForWrite.push_back(temp);

	Configurator::Instance().burstWrite(dataForWrite);
}

void KccCameraSettings::gainSliderMoved(int value)
{
	ui->gainEdit->blockSignals(true);
	QString text = QString::number(value);
	ui->gainEdit->setText(text);
	ui->gainEdit->blockSignals(false);
}

void KccCameraSettings::gainTextChanged(QString newText)
{
	ui->gainSlider->blockSignals(true);
	int value = newText.toInt();
	if(value < 0){
		value = 0;
	}else if(value > ui->gainSlider->maximum()){
		value = ui->gainSlider->maximum();
	}
	ui->gainSlider->setSliderPosition(value);
	ui->gainSlider->blockSignals(false);
}

void KccCameraSettings::contrastSliderMoved(int value)
{
	ui->contrastEdit->blockSignals(true);
	QString text = QString::number(value);
	ui->contrastEdit->setText(text);
	ui->contrastEdit->blockSignals(false);
}

void KccCameraSettings::contrastTextChanged(QString newText)
{
	ui->contrastSlider->blockSignals(true);
	int value = newText.toInt();
	if(value < 0){
		value = 0;
	}else if(value > ui->contrastSlider->maximum()){
		value = ui->contrastSlider->maximum();
	}
	ui->contrastSlider->setSliderPosition(value);
	ui->contrastSlider->blockSignals(false);
}

void KccCameraSettings::redBalanceSliderMoved(int value)
{
	ui->redBalanceEdit->blockSignals(true);
	QString text = QString::number(value);
	ui->redBalanceEdit->setText(text);
	ui->redBalanceEdit->blockSignals(false);
}

void KccCameraSettings::redBalanceTextChanged(QString newText)
{
	ui->redBalanceSlider->blockSignals(true);
	int value = newText.toInt();
	if(value < 0){
		value = 0;
	}else if(value > ui->redBalanceSlider->maximum()){
		value = ui->redBalanceSlider->maximum();
	}
	ui->redBalanceSlider->setSliderPosition(value);
	ui->redBalanceSlider->blockSignals(false);
}

void KccCameraSettings::blueBalanceSliderMoved(int value)
{
	ui->blueBalanceEdit->blockSignals(true);
	QString text = QString::number(value);
	ui->blueBalanceEdit->setText(text);
	ui->blueBalanceEdit->blockSignals(false);
}

void KccCameraSettings::blueBalanceTextChanged(QString newText)
{
	ui->blueBalanceSlider->blockSignals(true);
	int value = newText.toInt();
	if(value < 0){
		value = 0;
	}else if(value > ui->blueBalanceSlider->maximum()){
		value = ui->blueBalanceSlider->maximum();
	}
	ui->blueBalanceSlider->setSliderPosition(value);
	ui->blueBalanceSlider->blockSignals(false);
}

void KccCameraSettings::greenGainSliderMoved(int value)
{
	ui->greenGainEdit->blockSignals(true);
	QString text = QString::number(value);
	ui->greenGainEdit->setText(text);
	ui->greenGainEdit->blockSignals(false);
}

void KccCameraSettings::greenGainTextChanged(QString newText)
{
	ui->greenGainSlider->blockSignals(true);
	int value = newText.toInt();
	if(value < 0){
		value = 0;
	}else if(value > ui->greenGainSlider->maximum()){
		value = ui->greenGainSlider->maximum();
	}
	ui->greenGainSlider->setSliderPosition(value);
	ui->greenGainSlider->blockSignals(false);
}

void KccCameraSettings::closeEvent(QCloseEvent *event){
	emit iAmClosing();
	close();
}

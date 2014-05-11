#include "SensorsController.h"
#include "ui_SensorsController.h"

#include "hal/robot/nao/generic_nao/robot_consts.h"
#include "tools/toString.h"
#include <iostream>

using namespace std;

SensorsController::SensorsController(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::SensorsController) {
	
	ui->setupUi(this);
	availableLSDHosts = new HostsComboBox(ui->LSDComboBox);
	connect(availableLSDHosts, SIGNAL(SubscriptionRequest(QString)), this, SLOT(SubscriptionHandler(QString)) );
	
	ui->HY->setStyleSheet("border: 1px solid");
	ui->HP->setStyleSheet("border: 1px solid");
	
	ui->RHYP->setStyleSheet("border: 1px solid");
	ui->RHR->setStyleSheet("border: 1px solid");
	ui->RHP->setStyleSheet("border: 1px solid");
	ui->RKP->setStyleSheet("border: 1px solid");
	ui->RAP->setStyleSheet("border: 1px solid");
	ui->RAR->setStyleSheet("border: 1px solid");
	
	ui->LHYP->setStyleSheet("border: 1px solid");
	ui->LHR->setStyleSheet("border: 1px solid");
	ui->LHP->setStyleSheet("border: 1px solid");
	ui->LKP->setStyleSheet("border: 1px solid");
	ui->LAP->setStyleSheet("border: 1px solid");
	ui->LAR->setStyleSheet("border: 1px solid");
	
	ui->RSP->setStyleSheet("border: 1px solid");
	ui->RSR->setStyleSheet("border: 1px solid");
	ui->REY->setStyleSheet("border: 1px solid");
	ui->RER->setStyleSheet("border: 1px solid");

	ui->LSP->setStyleSheet("border: 1px solid");
	ui->LSR->setStyleSheet("border: 1px solid");
	ui->LEY->setStyleSheet("border: 1px solid");
	ui->LER->setStyleSheet("border: 1px solid");

	ui->AccX->setStyleSheet("border: 1px solid");
	ui->AccY->setStyleSheet("border: 1px solid");
	ui->AccZ->setStyleSheet("border: 1px solid");
	
	ui->LFsrFL->setStyleSheet("border: 1px solid");
	ui->LFsrRL->setStyleSheet("border: 1px solid");
	ui->LFsrFR->setStyleSheet("border: 1px solid");
	ui->LFsrRR->setStyleSheet("border: 1px solid");
	ui->RFsrFL->setStyleSheet("border: 1px solid");
	ui->RFsrRL->setStyleSheet("border: 1px solid");
	ui->RFsrFR->setStyleSheet("border: 1px solid");
	ui->RFsrRR->setStyleSheet("border: 1px solid");
	ui->SupportLeg->setStyleSheet("border: 1px solid");
}

void SensorsController::sensorsDataUpdateHandler(AllSensorValuesMessage asvm, QString hostId) {

	updateHeadJointsTable(asvm);
	updateLArmJointsTable(asvm);
	updateRArmJointsTable(asvm);
	updateLLegJointsTable(asvm);
	updateRLegJointsTable(asvm);
	updateInertialTable(asvm);
	updateFSRsTable(asvm);
}

void SensorsController::updateHeadJointsTable(AllSensorValuesMessage asvm) {

	SensorData HeadJoint;
	QString temp;
	HeadJoint.Clear();
	HeadJoint = asvm.jointdata(KDeviceLists::HEAD + KDeviceLists::YAW);
	ui->HY->setText(temp.setNum(HeadJoint.sensorvalue(), 'g', 6));

	HeadJoint.Clear();
	HeadJoint = asvm.jointdata(KDeviceLists::HEAD + KDeviceLists::PITCH);
	ui->HP->setText(temp.setNum( HeadJoint.sensorvalue(), 'g', 6));
}

void SensorsController::updateLArmJointsTable(AllSensorValuesMessage asvm) {
	SensorData ArmJoint;
	QString temp;
	ArmJoint.Clear();
	ArmJoint = asvm.jointdata(KDeviceLists::L_ARM + KDeviceLists::ELBOW_ROLL);
	ui->LER->setText(temp.setNum(ArmJoint.sensorvalue(), 'g', 6));

	ArmJoint.Clear();
	ArmJoint = asvm.jointdata(KDeviceLists::L_ARM + KDeviceLists::ELBOW_YAW);
	ui->LEY->setText(temp.setNum(ArmJoint.sensorvalue(), 'g', 6));

	ArmJoint.Clear();
	ArmJoint = asvm.jointdata(KDeviceLists::L_ARM + KDeviceLists::SHOULDER_PITCH);
	ui->LSP->setText(temp.setNum(ArmJoint.sensorvalue(), 'g', 6));

	ArmJoint.Clear();
	ArmJoint = asvm.jointdata(KDeviceLists::L_ARM + KDeviceLists::SHOULDER_ROLL);
	ui->LSR->setText(temp.setNum(ArmJoint.sensorvalue(), 'g', 6));
}


void SensorsController::updateRArmJointsTable(AllSensorValuesMessage asvm) {
	SensorData ArmJoint;
	QString temp;
	ArmJoint.Clear();
	ArmJoint = asvm.jointdata(KDeviceLists::R_ARM + KDeviceLists::ELBOW_ROLL);
	ui->RER->setText(temp.setNum(ArmJoint.sensorvalue(), 'g', 6));

	ArmJoint.Clear();
	ArmJoint = asvm.jointdata(KDeviceLists::R_ARM + KDeviceLists::ELBOW_YAW);
	ui->REY->setText(temp.setNum(ArmJoint.sensorvalue(), 'g', 6));

	ArmJoint.Clear();
	ArmJoint = asvm.jointdata(KDeviceLists::R_ARM + KDeviceLists::SHOULDER_PITCH);
	ui->RSP->setText(temp.setNum(ArmJoint.sensorvalue(), 'g', 6));

	ArmJoint.Clear();
	ArmJoint = asvm.jointdata(KDeviceLists::R_ARM + KDeviceLists::SHOULDER_ROLL);
	ui->RSR->setText(temp.setNum(ArmJoint.sensorvalue(), 'g', 6));
}

void SensorsController::updateLLegJointsTable(AllSensorValuesMessage asvm) {
	SensorData LegJoint;
	QString temp;
	LegJoint.Clear();
	LegJoint = asvm.jointdata(KDeviceLists::L_LEG + KDeviceLists::ANKLE_PITCH);
	ui->LAP->setText(temp.setNum(LegJoint.sensorvalue(), 'g', 6));

	LegJoint.Clear();
	LegJoint = asvm.jointdata(KDeviceLists::L_LEG + KDeviceLists::ANKLE_ROLL);
	ui->LAR->setText(temp.setNum(LegJoint.sensorvalue(), 'g', 6));

	LegJoint.Clear();
	LegJoint = asvm.jointdata(KDeviceLists::L_LEG + KDeviceLists::HIP_PITCH);
	ui->LHP->setText(temp.setNum(LegJoint.sensorvalue(), 'g', 6));

	LegJoint.Clear();
	LegJoint = asvm.jointdata(KDeviceLists::L_LEG + KDeviceLists::HIP_ROLL);
	ui->LHR->setText(temp.setNum(LegJoint.sensorvalue(), 'g', 6));

	LegJoint.Clear();
	LegJoint = asvm.jointdata(KDeviceLists::L_LEG + KDeviceLists::HIP_YAW_PITCH);
	ui->LHYP->setText(temp.setNum(LegJoint.sensorvalue(), 'g', 6));

	LegJoint.Clear();
	LegJoint = asvm.jointdata(KDeviceLists::L_LEG + KDeviceLists::KNEE_PITCH);
	ui->LKP->setText(temp.setNum(LegJoint.sensorvalue(), 'g', 6));
}

void SensorsController::updateRLegJointsTable(AllSensorValuesMessage asvm) {
	SensorData LegJoint;
	QString temp;
	LegJoint.Clear();
	LegJoint = asvm.jointdata(KDeviceLists::R_LEG + KDeviceLists::ANKLE_PITCH);
	ui->RAP->setText(temp.setNum(LegJoint.sensorvalue(), 'g', 6));

	LegJoint.Clear();
	LegJoint = asvm.jointdata(KDeviceLists::R_LEG + KDeviceLists::ANKLE_ROLL);
	ui->RAR->setText(temp.setNum(LegJoint.sensorvalue(), 'g', 6));

	LegJoint.Clear();
	LegJoint = asvm.jointdata(KDeviceLists::R_LEG + KDeviceLists::HIP_PITCH);
	ui->RHP->setText(temp.setNum(LegJoint.sensorvalue(), 'g', 6));

	LegJoint.Clear();
	LegJoint = asvm.jointdata(KDeviceLists::R_LEG + KDeviceLists::HIP_ROLL);
	ui->RHR->setText(temp.setNum(LegJoint.sensorvalue(), 'g', 6));

	LegJoint.Clear();
	LegJoint = asvm.jointdata(KDeviceLists::R_LEG + KDeviceLists::HIP_YAW_PITCH);
	ui->RHYP->setText(temp.setNum(LegJoint.sensorvalue(), 'g', 6));

	LegJoint.Clear();
	LegJoint = asvm.jointdata(KDeviceLists::R_LEG + KDeviceLists::KNEE_PITCH);
	ui->RKP->setText(temp.setNum(LegJoint.sensorvalue(), 'g', 6));
}

void SensorsController::updateInertialTable(AllSensorValuesMessage asvm) {
	SensorData Value;
	QString temp;

	Value.Clear();
	Value = asvm.sensordata(KDeviceLists::ACC + KDeviceLists::AXIS_X);
	ui->AccX->setText(temp.setNum(Value.sensorvalue(), 'g', 6));

	Value.Clear();
	Value = asvm.sensordata(KDeviceLists::ACC + KDeviceLists::AXIS_Y);
	ui->AccY->setText(temp.setNum(Value.sensorvalue(), 'g', 6));

	Value.Clear();
	Value = asvm.sensordata(KDeviceLists::ACC + KDeviceLists::AXIS_Z);
	ui->AccZ->setText(temp.setNum(Value.sensorvalue(), 'g', 6));

	Value.Clear();
	Value = asvm.computeddata(KDeviceLists::ANGLE + KDeviceLists::AXIS_X);
	ui->AngX->setText(temp.setNum(Value.sensorvalue(), 'g', 6));

	Value.Clear();
	Value = asvm.computeddata(KDeviceLists::ANGLE + KDeviceLists::AXIS_Y);
	ui->AngY->setText(temp.setNum(Value.sensorvalue(), 'g', 6));
}

void SensorsController::updateFSRsTable(AllSensorValuesMessage asvm) {
	SensorData Value;
	QString temp;
	Value.Clear();
	Value = asvm.sensordata(KDeviceLists::L_FSR + KDeviceLists::FSR_FL);
	ui->LFsrFL->setText(temp.setNum(Value.sensorvalue(), 'g', 6));

	Value.Clear();
	Value = asvm.sensordata(KDeviceLists::L_FSR + KDeviceLists::FSR_RL);
	ui->LFsrRL->setText(temp.setNum(Value.sensorvalue(), 'g', 6));

	Value.Clear();
	Value = asvm.sensordata(KDeviceLists::L_FSR + KDeviceLists::FSR_FR);
	ui->LFsrFR->setText(temp.setNum(Value.sensorvalue(), 'g', 6));

	Value.Clear();
	Value = asvm.sensordata(KDeviceLists::L_FSR + KDeviceLists::FSR_RR);
	ui->LFsrRR->setText(temp.setNum(Value.sensorvalue(), 'g', 6));

	Value.Clear();
	Value = asvm.sensordata(KDeviceLists::R_FSR + KDeviceLists::FSR_FL);
	ui->RFsrFL->setText(temp.setNum(Value.sensorvalue(), 'g', 6));

	Value.Clear();
	Value = asvm.sensordata(KDeviceLists::R_FSR + KDeviceLists::FSR_RL);
	ui->RFsrRL->setText(temp.setNum(Value.sensorvalue(), 'g', 6));

	Value.Clear();
	Value = asvm.sensordata(KDeviceLists::R_FSR + KDeviceLists::FSR_FR);
	ui->RFsrFR->setText(temp.setNum(Value.sensorvalue(), 'g', 6));

	Value.Clear();
	Value = asvm.sensordata(KDeviceLists::R_FSR + KDeviceLists::FSR_RR);
	ui->RFsrRR->setText(temp.setNum(Value.sensorvalue(), 'g', 6));

	Value.Clear();
	Value = asvm.computeddata(KDeviceLists::SUPPORT_LEG);

	float fvalue = Value.sensorvalue();
	
	if(fvalue == KDeviceLists::SUPPORT_LEG_NONE)
		ui->SupportLeg->setText("None");
	else if(fvalue == KDeviceLists::SUPPORT_LEG_LEFT)
		ui->SupportLeg->setText("Left");
	else if(fvalue == KDeviceLists::SUPPORT_LEG_RIGHT)
		ui->SupportLeg->setText("Right");
	else
		ui->SupportLeg->setText("Both");
}

void SensorsController::changeToHost(QString data1) {
	availableLSDHosts->changeItem(data1);
}

void SensorsController::addComboBoxItem(QString data1, QString data2) {
	availableLSDHosts->addComboBoxItem(data1, data2);
}

void SensorsController::removeComboBoxItem(QString data1) {
	availableLSDHosts->removeComboBoxItem(data1);
}

void SensorsController::setGameStateInfo(QString data1, QString data2, QString data3) {
	availableLSDHosts->setGameStateInfo(data1, data2, data3);
}

void SensorsController::SubscriptionHandler(QString data1) {
	emit SubscriptionRequest(data1);
}

SensorsController::~SensorsController() {
	delete ui;
}

#include "SensorsController.h"
#include "ui_SensorsController.h"

#include "hal/robot/generic_nao/robot_consts.h"
#include "tools/toString.h"
#include <iostream>

using namespace std;

SensorsController::SensorsController (QWidget *parent) :
	QWidget (parent),
	ui (new Ui::SensorsController) {
	ui->setupUi (this);
	availableLSDHosts = new HostsComboBox (ui->LSDComboBox);
	connect (availableLSDHosts, SIGNAL (SubscriptionRequest (QString) ), this, SLOT (SubscriptionHandler (QString) ) );
	headJointsBuffer.set_capacity (10);
	LArmJointsBuffer.set_capacity (10);
	RArmJointsBuffer.set_capacity (10);
	LLegJointsBuffer.set_capacity (10);
	RLegJointsBuffer.set_capacity (10);
	InertialBuffer.set_capacity (10);
	FSRsBuffer.set_capacity (10);
}

void SensorsController::sensorsDataUpdateHandler (AllSensorValuesMessage asvm, QString hostId) {
	clearComboLists();
	updateHeadJointsBuffer (asvm);
	updateLArmJointsBuffer (asvm);
	updateRArmJointsBuffer (asvm);
	updateLLegJointsBuffer (asvm);
	updateRLegJointsBuffer (asvm);
	updateInertialBuffer (asvm);
	updateFSRsBuffer (asvm);
	updateHeadJointsTable();
	updateLArmJointsTable();
	updateRArmJointsTable();
	updateLLegJointsTable();
	updateRLegJointsTable();
	updateInertialTable();
	updateFSRsTable();
}

void SensorsController::updateHeadJointsBuffer (AllSensorValuesMessage asvm) {
	SensorData HeadJoint;
	HeadJoints hj;
	HeadJoint.Clear();
	HeadJoint = asvm.jointdata (KDeviceLists::HEAD + KDeviceLists::YAW);
	hj.yaw = QString::fromStdString (_toString (HeadJoint.sensorvalue() ) );
	HeadJoint.Clear();
	HeadJoint = asvm.jointdata (KDeviceLists::HEAD + KDeviceLists::PITCH);
	hj.pitch = QString::fromStdString (_toString (HeadJoint.sensorvalue() ) );
	headJointsBuffer.push_front (hj);
}

void SensorsController::updateHeadJointsTable() {
	boost::circular_buffer<HeadJoints>::iterator it;

	for (it = headJointsBuffer.begin(); it != headJointsBuffer.end(); ++it) {
		ui->HYComboBox->addItem ( (*it).yaw);
		ui->HPComboBox->addItem ( (*it).pitch);
	}
}

void SensorsController::updateLArmJointsBuffer (AllSensorValuesMessage asvm) {
	SensorData ArmJoint;
	ArmJoints aj;
	ArmJoint.Clear();
	ArmJoint = asvm.jointdata (KDeviceLists::L_ARM + KDeviceLists::ELBOW_ROLL);
	aj.ElbowRoll = QString::fromStdString (_toString (ArmJoint.sensorvalue() ) );
	ArmJoint.Clear();
	ArmJoint = asvm.jointdata (KDeviceLists::L_ARM + KDeviceLists::ELBOW_YAW);
	aj.ElbowYaw = QString::fromStdString (_toString (ArmJoint.sensorvalue() ) );
	ArmJoint.Clear();
	ArmJoint = asvm.jointdata (KDeviceLists::L_ARM + KDeviceLists::SHOULDER_PITCH);
	aj.ShoulderPitch = QString::fromStdString (_toString (ArmJoint.sensorvalue() ) );
	ArmJoint.Clear();
	ArmJoint = asvm.jointdata (KDeviceLists::L_ARM + KDeviceLists::SHOULDER_ROLL);
	aj.ShoulderRoll = QString::fromStdString (_toString (ArmJoint.sensorvalue() ) );
	LArmJointsBuffer.push_front (aj);
}

void SensorsController::updateLArmJointsTable() {
	boost::circular_buffer<ArmJoints>::iterator it;

	for (it = LArmJointsBuffer.begin(); it != LArmJointsBuffer.end(); ++it) {
		ui->LSPComboBox->addItem ( (*it).ShoulderPitch);
		ui->LSRComboBox->addItem ( (*it).ShoulderRoll);
		ui->LEYComboBox->addItem ( (*it).ElbowYaw);
		ui->LERComboBox->addItem ( (*it).ElbowRoll);
	}
}

void SensorsController::updateRArmJointsBuffer (AllSensorValuesMessage asvm) {
	SensorData ArmJoint;
	ArmJoints aj;
	ArmJoint.Clear();
	ArmJoint = asvm.jointdata (KDeviceLists::R_ARM + KDeviceLists::ELBOW_ROLL);
	aj.ElbowRoll = QString::fromStdString (_toString (ArmJoint.sensorvalue() ) );
	ArmJoint.Clear();
	ArmJoint = asvm.jointdata (KDeviceLists::R_ARM + KDeviceLists::ELBOW_YAW);
	aj.ElbowYaw = QString::fromStdString (_toString (ArmJoint.sensorvalue() ) );
	ArmJoint.Clear();
	ArmJoint = asvm.jointdata (KDeviceLists::R_ARM + KDeviceLists::SHOULDER_PITCH);
	aj.ShoulderPitch = QString::fromStdString (_toString (ArmJoint.sensorvalue() ) );
	ArmJoint.Clear();
	ArmJoint = asvm.jointdata (KDeviceLists::R_ARM + KDeviceLists::SHOULDER_ROLL);
	aj.ShoulderRoll = QString::fromStdString (_toString (ArmJoint.sensorvalue() ) );
	RArmJointsBuffer.push_front (aj);
}

void SensorsController::updateRArmJointsTable() {
	boost::circular_buffer<ArmJoints>::iterator it;

	for (it = RArmJointsBuffer.begin(); it != RArmJointsBuffer.end(); ++it) {
		ui->RSPComboBox->addItem ( (*it).ShoulderPitch);
		ui->RSRComboBox->addItem ( (*it).ShoulderRoll);
		ui->REYComboBox->addItem ( (*it).ElbowYaw);
		ui->RERComboBox->addItem ( (*it).ElbowRoll);
	}
}

void SensorsController::updateLLegJointsBuffer (AllSensorValuesMessage asvm) {
	SensorData LegJoint;
	LegJoints lj;
	LegJoint.Clear();
	LegJoint = asvm.jointdata (KDeviceLists::L_LEG + KDeviceLists::ANKLE_PITCH);
	lj.AnklePitch = QString::fromStdString (_toString (LegJoint.sensorvalue() ) );
	LegJoint.Clear();
	LegJoint = asvm.jointdata (KDeviceLists::L_LEG + KDeviceLists::ANKLE_ROLL);
	lj.AnkleRoll = QString::fromStdString (_toString (LegJoint.sensorvalue() ) );
	LegJoint.Clear();
	LegJoint = asvm.jointdata (KDeviceLists::L_LEG + KDeviceLists::HIP_PITCH);
	lj.HipPitch = QString::fromStdString (_toString (LegJoint.sensorvalue() ) );
	LegJoint.Clear();
	LegJoint = asvm.jointdata (KDeviceLists::L_LEG + KDeviceLists::HIP_ROLL);
	lj.HipRoll = QString::fromStdString (_toString (LegJoint.sensorvalue() ) );
	LegJoint.Clear();
	LegJoint = asvm.jointdata (KDeviceLists::L_LEG + KDeviceLists::HIP_YAW_PITCH);
	lj.HipYawPitch = QString::fromStdString (_toString (LegJoint.sensorvalue() ) );
	LegJoint.Clear();
	LegJoint = asvm.jointdata (KDeviceLists::L_LEG + KDeviceLists::KNEE_PITCH);
	lj.KneePitch = QString::fromStdString (_toString (LegJoint.sensorvalue() ) );
	LLegJointsBuffer.push_front (lj);
}

void SensorsController::updateLLegJointsTable() {
	boost::circular_buffer<LegJoints>::iterator it;

	for (it = LLegJointsBuffer.begin(); it != LLegJointsBuffer.end(); ++it) {
		ui->LHYPComboBox->addItem ( (*it).HipYawPitch);
		ui->LHRComboBox->addItem ( (*it).HipRoll);
		ui->LHPComboBox->addItem ( (*it).HipPitch);
		ui->LKPComboBox->addItem ( (*it).KneePitch);
		ui->LAPComboBox->addItem ( (*it).AnklePitch);
		ui->LARComboBox->addItem ( (*it).AnkleRoll);
	}
}

void SensorsController::updateRLegJointsBuffer (AllSensorValuesMessage asvm) {
	SensorData LegJoint;
	LegJoints lj;
	LegJoint.Clear();
	LegJoint = asvm.jointdata (KDeviceLists::R_LEG + KDeviceLists::ANKLE_PITCH);
	lj.AnklePitch = QString::fromStdString (_toString (LegJoint.sensorvalue() ) );
	LegJoint.Clear();
	LegJoint = asvm.jointdata (KDeviceLists::R_LEG + KDeviceLists::ANKLE_ROLL);
	lj.AnkleRoll = QString::fromStdString (_toString (LegJoint.sensorvalue() ) );
	LegJoint.Clear();
	LegJoint = asvm.jointdata (KDeviceLists::R_LEG + KDeviceLists::HIP_PITCH);
	lj.HipPitch = QString::fromStdString (_toString (LegJoint.sensorvalue() ) );
	LegJoint.Clear();
	LegJoint = asvm.jointdata (KDeviceLists::R_LEG + KDeviceLists::HIP_ROLL);
	lj.HipRoll = QString::fromStdString (_toString (LegJoint.sensorvalue() ) );
	LegJoint.Clear();
	LegJoint = asvm.jointdata (KDeviceLists::R_LEG + KDeviceLists::HIP_YAW_PITCH);
	lj.HipYawPitch = QString::fromStdString (_toString (LegJoint.sensorvalue() ) );
	LegJoint.Clear();
	LegJoint = asvm.jointdata (KDeviceLists::R_LEG + KDeviceLists::KNEE_PITCH);
	lj.KneePitch = QString::fromStdString (_toString (LegJoint.sensorvalue() ) );
	RLegJointsBuffer.push_front (lj);
}

void SensorsController::updateRLegJointsTable() {
	boost::circular_buffer<LegJoints>::iterator it;

	for (it = RLegJointsBuffer.begin(); it != RLegJointsBuffer.end(); ++it) {
		ui->RHYPComboBox->addItem ( (*it).HipYawPitch);
		ui->RHRComboBox->addItem ( (*it).HipRoll);
		ui->RHPComboBox->addItem ( (*it).HipPitch);
		ui->RKPComboBox->addItem ( (*it).KneePitch);
		ui->RAPComboBox->addItem ( (*it).AnklePitch);
		ui->RARComboBox->addItem ( (*it).AnkleRoll);
	}
}

void SensorsController::updateInertialBuffer (AllSensorValuesMessage asvm) {
	SensorData Value;
	InertialValues iv;
	Value.Clear();
	Value = asvm.jointdata (KDeviceLists::ACC + KDeviceLists::AXIS_X);
	iv.AccXvalue = QString::fromStdString (_toString (Value.sensorvalue() ) );
	Value.Clear();
	Value = asvm.jointdata (KDeviceLists::ACC + KDeviceLists::AXIS_Y);
	iv.AccYvalue = QString::fromStdString (_toString (Value.sensorvalue() ) );
	Value.Clear();
	Value = asvm.jointdata (KDeviceLists::ACC + KDeviceLists::AXIS_Z);
	iv.AccZvalue = QString::fromStdString (_toString (Value.sensorvalue() ) );
	Value.Clear();
	Value = asvm.computeddata (KDeviceLists::ANGLE + KDeviceLists::AXIS_X);
	iv.angX = QString::fromStdString (_toString (Value.sensorvalue() ) );
	Value.Clear();
	Value = asvm.computeddata (KDeviceLists::ANGLE + KDeviceLists::AXIS_Y);
	iv.angY = QString::fromStdString (_toString (Value.sensorvalue() ) );
	InertialBuffer.push_front (iv);
}

void SensorsController::updateInertialTable() {
	boost::circular_buffer<InertialValues>::iterator it;

	for (it = InertialBuffer.begin(); it != InertialBuffer.end(); ++it) {
		ui->AccXComboBox->addItem ( (*it).AccXvalue);
		ui->AccYComboBox->addItem ( (*it).AccYvalue);
		ui->AccZComboBox->addItem ( (*it).AccZvalue);
		ui->AngXComboBox->addItem ( (*it).angX);
		ui->AngYComboBox->addItem ( (*it).angY);
	}
}

void SensorsController::updateFSRsBuffer (AllSensorValuesMessage asvm) {
	SensorData Value;
	FSRValues fv;
	Value.Clear();
	Value = asvm.jointdata (KDeviceLists::L_FSR + KDeviceLists::FSR_FL);
	fv.LFsrFL = QString::fromStdString (_toString (Value.sensorvalue() ) );
	Value.Clear();
	Value = asvm.jointdata (KDeviceLists::L_FSR + KDeviceLists::FSR_RL);
	fv.LFsrRL = QString::fromStdString (_toString (Value.sensorvalue() ) );
	Value.Clear();
	Value = asvm.jointdata (KDeviceLists::L_FSR + KDeviceLists::FSR_FR);
	fv.LFsrFR = QString::fromStdString (_toString (Value.sensorvalue() ) );
	Value.Clear();
	Value = asvm.jointdata (KDeviceLists::L_FSR + KDeviceLists::FSR_RR);
	fv.LFsrRR = QString::fromStdString (_toString (Value.sensorvalue() ) );
	Value.Clear();
	Value = asvm.jointdata (KDeviceLists::R_FSR + KDeviceLists::FSR_FL);
	fv.RFsrFL = QString::fromStdString (_toString (Value.sensorvalue() ) );
	Value.Clear();
	Value = asvm.jointdata (KDeviceLists::R_FSR + KDeviceLists::FSR_RL);
	fv.RFsrRL = QString::fromStdString (_toString (Value.sensorvalue() ) );
	Value.Clear();
	Value = asvm.jointdata (KDeviceLists::R_FSR + KDeviceLists::FSR_FR);
	fv.RFsrFR = QString::fromStdString (_toString (Value.sensorvalue() ) );
	Value.Clear();
	Value = asvm.jointdata (KDeviceLists::R_FSR + KDeviceLists::FSR_RR);
	fv.RFsrRR = QString::fromStdString (_toString (Value.sensorvalue() ) );
	FSRsBuffer.push_front (fv);
}

void SensorsController::updateFSRsTable() {
	boost::circular_buffer<FSRValues>::iterator it;

	for (it = FSRsBuffer.begin(); it != FSRsBuffer.end(); ++it) {
		ui->RFsrFLComboBox->addItem ( (*it).RFsrFL);
		ui->RFsrRLComboBox->addItem ( (*it).RFsrRL);
		ui->RFsrFRComboBox->addItem ( (*it).RFsrFR);
		ui->RFsrRRComboBox->addItem ( (*it).RFsrRR);
		ui->LFsrFLComboBox->addItem ( (*it).LFsrFL);
		ui->LFsrRLComboBox->addItem ( (*it).LFsrRL);
		ui->LFsrFRComboBox->addItem ( (*it).LFsrFR);
		ui->LFsrRRComboBox->addItem ( (*it).LFsrRR);
	}
}

void SensorsController::clearComboLists() {
	ui->HYComboBox->clear();	//0-1
	ui->HPComboBox->clear();
	ui->AccXComboBox->clear();	//2-6
	ui->AccYComboBox->clear();
	ui->AccZComboBox->clear();
	ui->AngXComboBox->clear();
	ui->AngYComboBox->clear();
	ui->RSPComboBox->clear();	//7-10
	ui->RSRComboBox->clear();
	ui->REYComboBox->clear();
	ui->RERComboBox->clear();
	ui->LSPComboBox->clear();	//11-14
	ui->LSRComboBox->clear();
	ui->LEYComboBox->clear();
	ui->LERComboBox->clear();
	ui->RHYPComboBox->clear();	//15-20
	ui->RHRComboBox->clear();
	ui->RHPComboBox->clear();
	ui->RKPComboBox->clear();
	ui->RAPComboBox->clear();
	ui->RARComboBox->clear();
	ui->LHYPComboBox->clear();  //21-26
	ui->LHRComboBox->clear();
	ui->LHPComboBox->clear();
	ui->LKPComboBox->clear();
	ui->LAPComboBox->clear();
	ui->LARComboBox->clear();
	ui->RFsrFLComboBox->clear();	//27-30
	ui->RFsrRLComboBox->clear();
	ui->RFsrFRComboBox->clear();
	ui->RFsrRRComboBox->clear();
	ui->LFsrFLComboBox->clear();	//31-34
	ui->LFsrRLComboBox->clear();
	ui->LFsrFRComboBox->clear();
	ui->LFsrRRComboBox->clear();
}

void SensorsController::changeToHost (QString data1) {
	availableLSDHosts->changeItem (data1);
}

void SensorsController::addComboBoxItem (QString data1, QString data2) {
	availableLSDHosts->addComboBoxItem (data1, data2);
}

void SensorsController::removeComboBoxItem (QString data1) {
	availableLSDHosts->removeComboBoxItem (data1);
}

void SensorsController::setGameStateInfo (QString data1, QString data2, QString data3) {
	availableLSDHosts->setGameStateInfo (data1, data2, data3);
}

void SensorsController::SubscriptionHandler (QString data1) {
	emit SubscriptionRequest (data1);
}

SensorsController::~SensorsController() {
	delete ui;
}

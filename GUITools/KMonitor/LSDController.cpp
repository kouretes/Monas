#include "LSDController.h"

#include "hal/robot/generic_nao/robot_consts.h"
#include "tools/toString.h"
#include <iostream>

using namespace std;

LSDController::LSDController(QList<QComboBox*> tablesList)
{
	parentTablesList.clear();
	parentTablesList = tablesList;

	headJointsBuffer.set_capacity(10);
	LArmJointsBuffer.set_capacity(10);
	RArmJointsBuffer.set_capacity(10);
	LLegJointsBuffer.set_capacity(10);
	RLegJointsBuffer.set_capacity(10);
	InertialBuffer.set_capacity(10);
	FSRsBuffer.set_capacity(10);
}

LSDController::~LSDController()
{
	boost::circular_buffer<HeadJoints>::iterator it;
	for(it=headJointsBuffer.begin(); it!=headJointsBuffer.end(); ++it)
	{
		//if ((*it))
		//	delete (*it);

	}
}

void LSDController::sensorsDataUpdateHandler(AllSensorValuesMessage asvm, QString hostId)
{
	if( currentHost == hostId)
	{
		clearComboLists(parentTablesList);

		updateHeadJointsBuffer(asvm);
		updateLArmJointsBuffer(asvm);
		updateRArmJointsBuffer(asvm);
		updateLLegJointsBuffer(asvm);
		updateRLegJointsBuffer(asvm);
		updateInertialBuffer(asvm);
		updateFSRsBuffer(asvm);

		updateHeadJointsTable();
		updateLArmJointsTable();
		updateRArmJointsTable();
		updateLLegJointsTable();
		updateRLegJointsTable();
		updateInertialTable();
		updateFSRsTable();

	}else
	{
		std::cout << "[67]Host hasn't been selected!" << hostId.toStdString() <<std::endl;
	}
}

void LSDController::updateHeadJointsBuffer(AllSensorValuesMessage asvm)
{
	SensorData HeadJoint;
	HeadJoints hj;

	HeadJoint.Clear();
	HeadJoint = asvm.jointdata(KDeviceLists::HEAD + KDeviceLists::YAW);
	hj.yaw = QString::fromStdString(_toString(HeadJoint.sensorvalue()));

	HeadJoint.Clear();
	HeadJoint = asvm.jointdata(KDeviceLists::HEAD + KDeviceLists::PITCH);
	hj.pitch = QString::fromStdString(_toString(HeadJoint.sensorvalue()));

	headJointsBuffer.push_front(hj);

}

void LSDController::updateHeadJointsTable()
{
	boost::circular_buffer<HeadJoints>::iterator it;
	for(it=headJointsBuffer.begin(); it!=headJointsBuffer.end(); ++it)
	{
		parentTablesList.at(0)->addItem((*it).yaw);
		parentTablesList.at(1)->addItem((*it).pitch);
	}
}

void LSDController::updateLArmJointsBuffer(AllSensorValuesMessage asvm)
{
	SensorData ArmJoint;
	ArmJoints aj;

	ArmJoint.Clear();
	ArmJoint = asvm.jointdata(KDeviceLists::L_ARM + KDeviceLists::ELBOW_ROLL);
	aj.ElbowRoll = QString::fromStdString(_toString(ArmJoint.sensorvalue()));

	ArmJoint.Clear();
	ArmJoint = asvm.jointdata(KDeviceLists::L_ARM + KDeviceLists::ELBOW_YAW);
	aj.ElbowYaw = QString::fromStdString(_toString(ArmJoint.sensorvalue()));

	ArmJoint.Clear();
	ArmJoint = asvm.jointdata(KDeviceLists::L_ARM + KDeviceLists::SHOULDER_PITCH);
	aj.ShoulderPitch = QString::fromStdString(_toString(ArmJoint.sensorvalue()));

	ArmJoint.Clear();
	ArmJoint = asvm.jointdata(KDeviceLists::L_ARM + KDeviceLists::SHOULDER_ROLL);
	aj.ShoulderRoll = QString::fromStdString(_toString(ArmJoint.sensorvalue()));

	LArmJointsBuffer.push_front(aj);

}

void LSDController::updateLArmJointsTable()
{
	boost::circular_buffer<ArmJoints>::iterator it;
	for(it= LArmJointsBuffer.begin(); it!=LArmJointsBuffer.end(); ++it)
	{
		parentTablesList.at(11)->addItem((*it).ShoulderPitch);
		parentTablesList.at(12)->addItem((*it).ShoulderRoll);
		parentTablesList.at(13)->addItem((*it).ElbowYaw);
		parentTablesList.at(14)->addItem((*it).ElbowRoll);
	}
}

void LSDController::updateRArmJointsBuffer(AllSensorValuesMessage asvm)
{
	SensorData ArmJoint;
	ArmJoints aj;

	ArmJoint.Clear();
	ArmJoint = asvm.jointdata(KDeviceLists::R_ARM + KDeviceLists::ELBOW_ROLL);
	aj.ElbowRoll = QString::fromStdString(_toString(ArmJoint.sensorvalue()));

	ArmJoint.Clear();
	ArmJoint = asvm.jointdata(KDeviceLists::R_ARM + KDeviceLists::ELBOW_YAW);
	aj.ElbowYaw = QString::fromStdString(_toString(ArmJoint.sensorvalue()));

	ArmJoint.Clear();
	ArmJoint = asvm.jointdata(KDeviceLists::R_ARM + KDeviceLists::SHOULDER_PITCH);
	aj.ShoulderPitch = QString::fromStdString(_toString(ArmJoint.sensorvalue()));

	ArmJoint.Clear();
	ArmJoint = asvm.jointdata(KDeviceLists::R_ARM + KDeviceLists::SHOULDER_ROLL);
	aj.ShoulderRoll = QString::fromStdString(_toString(ArmJoint.sensorvalue()));

	RArmJointsBuffer.push_front(aj);

}

void LSDController::updateRArmJointsTable()
{
	boost::circular_buffer<ArmJoints>::iterator it;
	for(it= RArmJointsBuffer.begin(); it!=RArmJointsBuffer.end(); ++it)
	{
		parentTablesList.at(7)->addItem((*it).ShoulderPitch);
		parentTablesList.at(8)->addItem((*it).ShoulderRoll);
		parentTablesList.at(9)->addItem((*it).ElbowYaw);
		parentTablesList.at(10)->addItem((*it).ElbowRoll);
	}
}

void LSDController::updateLLegJointsBuffer(AllSensorValuesMessage asvm)
{
	SensorData LegJoint;
	LegJoints lj;

	LegJoint.Clear();
	LegJoint = asvm.jointdata(KDeviceLists::L_LEG + KDeviceLists::ANKLE_PITCH);
	lj.AnklePitch = QString::fromStdString(_toString(LegJoint.sensorvalue()));

	LegJoint.Clear();
	LegJoint = asvm.jointdata(KDeviceLists::L_LEG + KDeviceLists::ANKLE_ROLL);
	lj.AnkleRoll = QString::fromStdString(_toString(LegJoint.sensorvalue()));

	LegJoint.Clear();
	LegJoint = asvm.jointdata(KDeviceLists::L_LEG + KDeviceLists::HIP_PITCH);
	lj.HipPitch = QString::fromStdString(_toString(LegJoint.sensorvalue()));

	LegJoint.Clear();
	LegJoint = asvm.jointdata(KDeviceLists::L_LEG + KDeviceLists::HIP_ROLL);
	lj.HipRoll = QString::fromStdString(_toString(LegJoint.sensorvalue()));

	LegJoint.Clear();
	LegJoint = asvm.jointdata(KDeviceLists::L_LEG + KDeviceLists::HIP_YAW_PITCH);
	lj.HipYawPitch = QString::fromStdString(_toString(LegJoint.sensorvalue()));

	LegJoint.Clear();
	LegJoint = asvm.jointdata(KDeviceLists::L_LEG + KDeviceLists::KNEE_PITCH);
	lj.KneePitch = QString::fromStdString(_toString(LegJoint.sensorvalue()));

	LLegJointsBuffer.push_front(lj);

}

void LSDController::updateLLegJointsTable()
{
	boost::circular_buffer<LegJoints>::iterator it;
	for(it= LLegJointsBuffer.begin(); it!=LLegJointsBuffer.end(); ++it)
	{
		parentTablesList.at(21)->addItem((*it).HipYawPitch);
		parentTablesList.at(22)->addItem((*it).HipRoll);
		parentTablesList.at(23)->addItem((*it).HipPitch);
		parentTablesList.at(24)->addItem((*it).KneePitch);
		parentTablesList.at(25)->addItem((*it).AnklePitch);
		parentTablesList.at(26)->addItem((*it).AnkleRoll);

	}
}

void LSDController::updateRLegJointsBuffer(AllSensorValuesMessage asvm)
{
	SensorData LegJoint;
	LegJoints lj;

	LegJoint.Clear();
	LegJoint = asvm.jointdata(KDeviceLists::R_LEG + KDeviceLists::ANKLE_PITCH);
	lj.AnklePitch = QString::fromStdString(_toString(LegJoint.sensorvalue()));

	LegJoint.Clear();
	LegJoint = asvm.jointdata(KDeviceLists::R_LEG + KDeviceLists::ANKLE_ROLL);
	lj.AnkleRoll = QString::fromStdString(_toString(LegJoint.sensorvalue()));

	LegJoint.Clear();
	LegJoint = asvm.jointdata(KDeviceLists::R_LEG + KDeviceLists::HIP_PITCH);
	lj.HipPitch = QString::fromStdString(_toString(LegJoint.sensorvalue()));

	LegJoint.Clear();
	LegJoint = asvm.jointdata(KDeviceLists::R_LEG + KDeviceLists::HIP_ROLL);
	lj.HipRoll = QString::fromStdString(_toString(LegJoint.sensorvalue()));

	LegJoint.Clear();
	LegJoint = asvm.jointdata(KDeviceLists::R_LEG + KDeviceLists::HIP_YAW_PITCH);
	lj.HipYawPitch = QString::fromStdString(_toString(LegJoint.sensorvalue()));

	LegJoint.Clear();
	LegJoint = asvm.jointdata(KDeviceLists::R_LEG + KDeviceLists::KNEE_PITCH);
	lj.KneePitch = QString::fromStdString(_toString(LegJoint.sensorvalue()));

	RLegJointsBuffer.push_front(lj);

}

void LSDController::updateRLegJointsTable()
{
	boost::circular_buffer<LegJoints>::iterator it;
	for(it= RLegJointsBuffer.begin(); it!=RLegJointsBuffer.end(); ++it)
	{
		parentTablesList.at(15)->addItem((*it).HipYawPitch);
		parentTablesList.at(16)->addItem((*it).HipRoll);
		parentTablesList.at(17)->addItem((*it).HipPitch);
		parentTablesList.at(18)->addItem((*it).KneePitch);
		parentTablesList.at(19)->addItem((*it).AnklePitch);
		parentTablesList.at(20)->addItem((*it).AnkleRoll);
	}
}

void LSDController::updateInertialBuffer(AllSensorValuesMessage asvm)
{
	SensorData Value;
	InertialValues iv;

	Value.Clear();
	Value = asvm.jointdata(KDeviceLists::ACC + KDeviceLists::AXIS_X);
	iv.AccXvalue = QString::fromStdString(_toString(Value.sensorvalue()));

	Value.Clear();
	Value = asvm.jointdata(KDeviceLists::ACC + KDeviceLists::AXIS_Y);
	iv.AccYvalue = QString::fromStdString(_toString(Value.sensorvalue()));


	Value.Clear();
	Value = asvm.jointdata(KDeviceLists::ACC + KDeviceLists::AXIS_Z);
	iv.AccZvalue = QString::fromStdString(_toString(Value.sensorvalue()));

	Value.Clear();
	Value = asvm.jointdata(KDeviceLists::ANGLE + KDeviceLists::AXIS_X);
	iv.angX = QString::fromStdString(_toString(Value.sensorvalue()));

	Value.Clear();
	Value = asvm.jointdata(KDeviceLists::ANGLE + KDeviceLists::AXIS_Y);
	iv.angY = QString::fromStdString(_toString(Value.sensorvalue()));

	InertialBuffer.push_front(iv);

}

void LSDController::updateInertialTable()
{
	boost::circular_buffer<InertialValues>::iterator it;
	for(it= InertialBuffer.begin(); it!=InertialBuffer.end(); ++it)
	{
		parentTablesList.at(2)->addItem((*it).AccXvalue);
		parentTablesList.at(3)->addItem((*it).AccYvalue);
		parentTablesList.at(4)->addItem((*it).AccZvalue);
		parentTablesList.at(5)->addItem((*it).angX);
		parentTablesList.at(6)->addItem((*it).angY);

	}
}

void LSDController::updateFSRsBuffer(AllSensorValuesMessage asvm)
{
	SensorData Value;
	FSRValues fv;

	Value.Clear();
	Value = asvm.jointdata(KDeviceLists::L_FSR + KDeviceLists::FSR_FL);
	fv.LFsrFL = QString::fromStdString(_toString(Value.sensorvalue()));


	Value.Clear();
	Value = asvm.jointdata(KDeviceLists::L_FSR + KDeviceLists::FSR_RL);
	fv.LFsrRL = QString::fromStdString(_toString(Value.sensorvalue()));

	Value.Clear();
	Value = asvm.jointdata(KDeviceLists::L_FSR + KDeviceLists::FSR_FR);
	fv.LFsrFR = QString::fromStdString(_toString(Value.sensorvalue()));

	Value.Clear();
	Value = asvm.jointdata(KDeviceLists::L_FSR + KDeviceLists::FSR_RR);
	fv.LFsrRR = QString::fromStdString(_toString(Value.sensorvalue()));

	Value.Clear();
	Value = asvm.jointdata(KDeviceLists::R_FSR + KDeviceLists::FSR_FL);
	fv.RFsrFL = QString::fromStdString(_toString(Value.sensorvalue()));


	Value.Clear();
	Value = asvm.jointdata(KDeviceLists::R_FSR + KDeviceLists::FSR_RL);
	fv.RFsrRL = QString::fromStdString(_toString(Value.sensorvalue()));

	Value.Clear();
	Value = asvm.jointdata(KDeviceLists::R_FSR + KDeviceLists::FSR_FR);
	fv.RFsrFR = QString::fromStdString(_toString(Value.sensorvalue()));

	Value.Clear();
	Value = asvm.jointdata(KDeviceLists::R_FSR + KDeviceLists::FSR_RR);
	fv.RFsrRR = QString::fromStdString(_toString(Value.sensorvalue()));

	FSRsBuffer.push_front(fv);

}

void LSDController::updateFSRsTable()
{
	boost::circular_buffer<FSRValues>::iterator it;
	for(it= FSRsBuffer.begin(); it!=FSRsBuffer.end(); ++it)
	{
		parentTablesList.at(27)->addItem((*it).RFsrFL);
		parentTablesList.at(28)->addItem((*it).RFsrRL);
		parentTablesList.at(29)->addItem((*it).RFsrFR);
		parentTablesList.at(30)->addItem((*it).RFsrRR);

		parentTablesList.at(31)->addItem((*it).LFsrFL);
		parentTablesList.at(32)->addItem((*it).LFsrRL);
		parentTablesList.at(33)->addItem((*it).LFsrFR);
		parentTablesList.at(34)->addItem((*it).LFsrRR);
	}
}

void LSDController::LSCSubscriptionHandler(QString hostId)
{
	if( currentHost != hostId)
	{
		currentHost = hostId;
		clearComboLists(parentTablesList);
	}

}

void LSDController::LSCUnsubscriptionHandler(QString hostId)
{
	if((hostId.isEmpty() && !currentHost.isEmpty()) || (currentHost == hostId && !hostId.isEmpty()))
	{
		currentHost.clear();
		clearComboLists(parentTablesList);
	}
}

void LSDController::clearComboLists(QList<QComboBox*> cList)
{
	QComboBox* item;

	for(int t = 0; t < cList.count();t++)
	{
		item = cList.at(t);
		item->clear();
	}
}

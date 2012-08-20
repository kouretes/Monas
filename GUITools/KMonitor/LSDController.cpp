#include "LSDController.h"

#include "hal/robot/generic_nao/robot_consts.h"
#include "tools/toString.h"
#include <iostream>

using namespace std;
//using namespace KDeviceLists;

LSDController::LSDController(QList<QTableWidget*> tablesList)
{
	parentTablesList.clear();
	parentTablesList = tablesList;

	headJointsBuffer.set_capacity(10);
	LArmJointsBuffer.set_capacity(10);
	RArmJointsBuffer.set_capacity(10);
	LLegJointsBuffer.set_capacity(10);
	RLegJointsBuffer.set_capacity(10);
	InertialBuffer.set_capacity(10);
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

		updateHeadJointsBuffer(asvm);
		updateLArmJointsBuffer(asvm);
		updateRArmJointsBuffer(asvm);
		updateLLegJointsBuffer(asvm);
		updateRLegJointsBuffer(asvm);
		updateInertialBuffer(asvm);

		updateHeadJointsTable();
		updateLArmJointsTable();
		updateRArmJointsTable();
		updateLLegJointsTable();
		updateRLegJointsTable();
		updateInertialTable();

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
	QTableWidget* headJointsTable = parentTablesList.at(0);
	QTableWidgetItem * tableItem;
	int raw = 0;

	boost::circular_buffer<HeadJoints>::iterator it;
	for(it=headJointsBuffer.begin(); it!=headJointsBuffer.end(); ++it)
	{
		for (int column=0; column< headJointsTable->columnCount(); column++)
		{
			tableItem = headJointsTable->item(raw, column);

			switch(column)
			{
				case 0:
					tableItem->setText((*it).yaw);
					break;
				case 1:
					tableItem->setText((*it).pitch);
					break;
			}

		}
		raw++;
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
	QTableWidget* JointsTable = parentTablesList.at(2);
	QTableWidgetItem * tableItem;
	int raw = 0;

	boost::circular_buffer<ArmJoints>::iterator it;
	for(it= LArmJointsBuffer.begin(); it!=LArmJointsBuffer.end(); ++it)
	{
		for (int column=0; column< JointsTable->columnCount(); column++)
		{
			tableItem = JointsTable->item(raw, column);

			switch(column)
			{
				case 0:
					tableItem->setText((*it).ShoulderPitch);
					break;
				case 1:
					tableItem->setText((*it).ShoulderRoll);
					break;
				case 2:
					tableItem->setText((*it).ElbowYaw);
					break;
				case 3:
					tableItem->setText((*it).ElbowRoll);
					break;
			}

		}
		raw++;
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
	QTableWidget* JointsTable = parentTablesList.at(3);
	QTableWidgetItem * tableItem;
	int raw = 0;

	boost::circular_buffer<ArmJoints>::iterator it;
	for(it= RArmJointsBuffer.begin(); it!=RArmJointsBuffer.end(); ++it)
	{
		for (int column=0; column< JointsTable->columnCount(); column++)
		{
			tableItem = JointsTable->item(raw, column);

			switch(column)
			{
				case 0:
					tableItem->setText((*it).ShoulderPitch);
					break;
				case 1:
					tableItem->setText((*it).ShoulderRoll);
					break;
				case 2:
					tableItem->setText((*it).ElbowYaw);
					break;
				case 3:
					tableItem->setText((*it).ElbowRoll);
					break;
			}

		}
		raw++;
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
	QTableWidget* JointsTable = parentTablesList.at(4);
	QTableWidgetItem * tableItem;
	int raw = 0;

	boost::circular_buffer<LegJoints>::iterator it;
	for(it= LLegJointsBuffer.begin(); it!=LLegJointsBuffer.end(); ++it)
	{
		for (int column=0; column< JointsTable->columnCount(); column++)
		{
			tableItem = JointsTable->item(raw, column);

			switch(column)
			{
				case 0:
					tableItem->setText((*it).HipYawPitch);
					break;
				case 1:
					tableItem->setText((*it).HipRoll);
					break;
				case 2:
					tableItem->setText((*it).HipPitch);
					break;
				case 3:
					tableItem->setText((*it).KneePitch);
					break;
				case 4:
					tableItem->setText((*it).AnklePitch);
					break;
				case 5:
					tableItem->setText((*it).AnkleRoll);
					break;
			}

		}
		raw++;
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
	QTableWidget* JointsTable = parentTablesList.at(5);
	QTableWidgetItem * tableItem;
	int raw = 0;

	boost::circular_buffer<LegJoints>::iterator it;
	for(it= RLegJointsBuffer.begin(); it!=RLegJointsBuffer.end(); ++it)
	{
		for (int column=0; column< JointsTable->columnCount(); column++)
		{
			tableItem = JointsTable->item(raw, column);

			switch(column)
			{
				case 0:
					tableItem->setText((*it).HipYawPitch);
					break;
				case 1:
					tableItem->setText((*it).HipRoll);
					break;
				case 2:
					tableItem->setText((*it).HipPitch);
					break;
				case 3:
					tableItem->setText((*it).KneePitch);
					break;
				case 4:
					tableItem->setText((*it).AnklePitch);
					break;
				case 5:
					tableItem->setText((*it).AnkleRoll);
					break;
			}

		}
		raw++;
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
	Value = asvm.jointdata(KDeviceLists::GYR + KDeviceLists::AXIS_X);
	iv.angX = QString::fromStdString(_toString(Value.sensorvalue()));

	Value.Clear();
	Value = asvm.jointdata(KDeviceLists::GYR + KDeviceLists::AXIS_Y);
	iv.angY = QString::fromStdString(_toString(Value.sensorvalue()));

	InertialBuffer.push_front(iv);

}

void LSDController::updateInertialTable()
{
	QTableWidget* JointsTable = parentTablesList.at(1);
	QTableWidgetItem * tableItem;
	int raw = 0;

	boost::circular_buffer<InertialValues>::iterator it;
	for(it= InertialBuffer.begin(); it!=InertialBuffer.end(); ++it)
	{
		for (int column=0; column< JointsTable->columnCount(); column++)
		{
			tableItem = JointsTable->item(raw, column);

			switch(column)
			{
				case 0:
					tableItem->setText((*it).AccZvalue);
					break;
				case 1:
					tableItem->setText((*it).AccXvalue);
					break;
				case 2:
					tableItem->setText((*it).AccYvalue);
					break;
				/*case 3:
					tableItem->setText((*it).KneePitch);
					break;*/
				case 4:
					tableItem->setText((*it).angX);
					break;
				case 5:
					tableItem->setText((*it).angY);
					break;
			}

		}
		raw++;
	}
}

void LSDController::LSCSubscriptionHandler(QString hostId)
{
	if( currentHost != hostId)
	{
		currentHost = hostId;
		uncheckAllTablesContents();
	}

}

void LSDController::LSCUnsubscriptionHandler(QString hostId)
{
	if((hostId.isEmpty() && !currentHost.isEmpty()) || (currentHost == hostId && !hostId.isEmpty()))
	{
		currentHost.clear();
		uncheckAllTablesContents();
	}
}

void LSDController::uncheckAllTablesContents()
{
	QTableWidget* JointsTable;
	QTableWidgetItem * tableItem;

	// ka8arise tous buffers
	for(int t = 0; t < parentTablesList.count();t++)
	{
		JointsTable = parentTablesList.at(t);

		for (int j=0; j< JointsTable->columnCount(); j++){
			for (int i=0; i< JointsTable->rowCount(); i++){

				tableItem = JointsTable->item(i, j);
				if (tableItem == 0){
					tableItem = new QTableWidgetItem(tr("%1").arg((i+1)*(j+1)));
					JointsTable->setItem(i, j, tableItem);
				}
				tableItem->setText(tr(""));
			}
		}
	}
}

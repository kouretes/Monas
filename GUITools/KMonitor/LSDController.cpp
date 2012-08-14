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
	//updateHeadJointsTable();
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

		udpateHeadJointsBuffer(asvm);
		updateHeadJointsTable();


	}else
	{
		std::cout << "[67]Host hasn't been selected!" << hostId.toStdString() <<std::endl;
	}
}

void LSDController::udpateHeadJointsBuffer(AllSensorValuesMessage asvm)
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
	if(currentHost == hostId)
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

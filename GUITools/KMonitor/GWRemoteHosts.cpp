#include "GWRemoteHosts.h"

#include <QTreeWidgetItem>
#include <QCheckBox>
#include <QString>
#include <QList>

#include "tools/toString.h"

#include <iostream>
using namespace std;

GWRemoteHosts::GWRemoteHosts(QTreeWidget *parent)
{
	parentTreeWidget = parent;

	parentTreeWidget->clear();
	GWRequests.clear();
}

void GWRemoteHosts::emergeAvailableHosts(KnownHosts newHosts)
{
	QTreeWidgetItem *item;
	QString hostId;
	QString hostName;

	removeDisconnectedHosts( newHosts );

	////////////////////////////////////////////

    const ::google::protobuf::RepeatedPtrField< HostEntry >& rf = newHosts.entrylist();
    ::google::protobuf::RepeatedPtrField< HostEntry >::const_iterator fit;

    for(fit=rf.begin();fit!=rf.end();++fit)
    {

            //cout <<"Tous pairnw ...."  <<  (*fit).hostid();

    		item = NULL;
    		hostId = QString::fromStdString(_toString(((*fit).hostid())));
    		hostName = QString::fromStdString(_toString(((*fit).hostname())));

    		item = GWhostFinder(hostId);
    		if( item == NULL)
    			addTreeWidgetItem(parentTreeWidget->topLevelItemCount(),hostId, hostName);
    }



	/////////////////////////////////////////////

	/*for (unsigned i=0; i < newHosts.name_size(); i++)
	{
		item = NULL;
		hostId = QString::fromStdString(_toString((newHosts.name(i))));

		item = GWhostFinder(hostId);
		if( item == NULL)
			addTreeWidgetItem(parentTreeWidget->topLevelItemCount(),hostId);
	}*/

	//printGWRequests();

}

void GWRemoteHosts::removeDisconnectedHosts( KnownHosts newHosts )
{
	QTreeWidgetItem *item;
	QCheckBox *checkbox;
	bool stillConnected;

	for (int it=0; it<parentTreeWidget->topLevelItemCount(); it++)
	{
		stillConnected = false;
		item = parentTreeWidget->topLevelItem(it);
		checkbox = (QCheckBox*) parentTreeWidget->itemWidget(item, 0);

		//////////////////////////////////////////////////////////////////////////
	    const ::google::protobuf::RepeatedPtrField< HostEntry >& rf = newHosts.entrylist();
	    ::google::protobuf::RepeatedPtrField< HostEntry >::const_iterator fit;

	    for(fit=rf.begin();fit!=rf.end();++fit)
	    {

			if (!QString::compare(checkbox->objectName(),QString::fromStdString(_toString(((*fit).hostid())))))
			{
				stillConnected = true;
				break;
			}
	    }


		/*for(unsigned i=0; i<newHosts.name_size(); i++)
		{
			if (!QString::compare(checkbox->objectName(),QString::fromStdString(_toString((newHosts.name(i))))))
			{
				stillConnected = true;
				break;
			}
		}*/

		if(!stillConnected)
			removeTreeWidgetItem(item);
	}

}

void GWRemoteHosts::removeTreeWidgetItem(QTreeWidgetItem* item)
{
	QCheckBox *checkBox = (QCheckBox*) parentTreeWidget->itemWidget(item, 0);

	for(int i = 0; i<GWRequests.count();i++)
	{
		if (GWRequests.at(i)->hostId == checkBox->objectName())
		{
			emit GWRHUnsubscriptionRequest(GWRequests.at(i)->hostId);
			emit GWRHOldHostRemoved(GWRequests.at(i)->hostId);
			GWRequests.removeAt(i);
		}
	}

	if(item->child(1) != NULL)
		delete(item->child(1));
	if(item->child(0) != NULL)
		delete(item->child(0));
	if(item != NULL)
	delete(item);

}

QTreeWidgetItem* GWRemoteHosts::GWhostFinder(QString hostId)
{
	QTreeWidgetItem* item;
	QCheckBox *checkbox;
	bool alreadyListed = false;

	for (int i=0; i<parentTreeWidget->topLevelItemCount(); i++)
	{
		item = parentTreeWidget->topLevelItem(i);
		checkbox = (QCheckBox*) parentTreeWidget->itemWidget(item, 0);

		if (checkbox->objectName() == hostId)
		{
			alreadyListed = true;
			break;
		}

	}

	if (alreadyListed)
	{
		return item;
	}else
	{
		return NULL;
	}

	//this->printGWRequests();
}

void GWRemoteHosts::addTreeWidgetItem(int position, QString hostId, QString hostName)
{
	QTreeWidgetItem *item;
	QTreeWidgetItem *subItem1;
	QTreeWidgetItem *subItem2;

	QCheckBox *checkBox;
	QCheckBox *subCheckBox1;
	QCheckBox *subCheckBox2;

	QIcon icon;
	QFont font;

	icon.addFile(QString::fromUtf8(":/KnownHostsIcons/chore_robot_on.png"), QSize(), QIcon::Normal, QIcon::Off);
	font.setPointSize(24);

	item = new QTreeWidgetItem(parentTreeWidget);
	subItem1 = new QTreeWidgetItem(item);
	subItem2 = new QTreeWidgetItem(item);

	checkBox = new QCheckBox();
	checkBox->setIcon(icon);
	checkBox->setIconSize(QSize(48, 48));
	checkBox->setTristate(false);
	checkBox->setObjectName(hostId);
	checkBox->setText(hostName); //

	subCheckBox1 = new QCheckBox();
	subCheckBox1->setTristate(false);
	subCheckBox1->setObjectName(QString("position,")+ hostId );
	subCheckBox1->setText(tr("Estimated Robot Position-Orientation"));

	subCheckBox2 = new QCheckBox();
	subCheckBox2->setTristate(false);
	subCheckBox2->setObjectName(tr("ball,")+ hostId );
	subCheckBox2->setText(tr("Estimated Ball Position"));

	parentTreeWidget->setItemWidget(parentTreeWidget->topLevelItem(position),0, checkBox);
	parentTreeWidget->setItemWidget(parentTreeWidget->topLevelItem(position)->child(0),0, subCheckBox1);
	parentTreeWidget->setItemWidget(parentTreeWidget->topLevelItem(position)->child(1),0, subCheckBox2);


	connect(checkBox,SIGNAL(stateChanged(int)),this, SLOT(mainCheckBoxHandler(int)));
	connect(subCheckBox1,SIGNAL(stateChanged(int)),this, SLOT(subCheckBox1Handler(int)));
	connect(subCheckBox2,SIGNAL(stateChanged(int)),this, SLOT(subCheckBox2Handler(int)));

	requestedElements *re = new Elements(hostId, hostName, false, false);
	GWRequests.append(re);

	//this->printGWRequests();
	emit GWRHNewHostAdded(hostId, hostName);

}

void GWRemoteHosts::mainCheckBoxHandler(int state)
{
	QTreeWidgetItem* subItem = NULL;
	QCheckBox* subCheckBox = NULL;

	QCheckBox* checkBox = (QCheckBox*)this->sender();
	QTreeWidgetItem* item = this->GWhostFinder(checkBox->objectName());

	for (int i=0; i<2; i++)
	{
		subItem = item->child(i);
		subCheckBox = (QCheckBox*) parentTreeWidget->itemWidget(subItem, 0);

		if(state == 0)
			subCheckBox->setChecked(false);
		else
			subCheckBox->setChecked(true);
	}



}

void GWRemoteHosts::subCheckBox1Handler(int state)
{
	QCheckBox* checkBox = (QCheckBox*)this->sender();
	QStringList list = checkBox->objectName().split(",");

	for(int i = 0; i<GWRequests.count();i++)
	{
		if (GWRequests.at(i)->hostId == list.at(1))
		{
			if (state == 0)
			{
				GWRequests.at(i)->setPositionRequest(false);
				emit GWRHSetRobotVisible(GWRequests.at(i)->hostId, false);

				if (!(GWRequests.at(i)->requestedBall))
					emit GWRHUnsubscriptionRequest(GWRequests.at(i)->hostId);
			}else
			{
				GWRequests.at(i)->setPositionRequest(true);
				emit GWRHSubscriptionRequest(GWRequests.at(i)->hostId);
				emit GWRHSetRobotVisible(GWRequests.at(i)->hostId, true);
			}

			break;
		}
	}

	//this->printGWRequests();
}

void GWRemoteHosts::subCheckBox2Handler(int state)
{
	QCheckBox* checkBox = (QCheckBox*)this->sender();
	QStringList list = checkBox->objectName().split(",");

	for(int i = 0; i<GWRequests.count();i++)
	{
		if (GWRequests.at(i)->hostId == list.at(1))
		{
			if (state == 0)
			{
				GWRequests.at(i)->setBallRequest(false);
				emit GWRHSetBallVisible(GWRequests.at(i)->hostId,false);

				if (!(GWRequests.at(i)->requestedPosition))
					emit GWRHUnsubscriptionRequest(GWRequests.at(i)->hostId);
			}else
			{
				GWRequests.at(i)->setBallRequest(true);
				emit GWRHSubscriptionRequest(GWRequests.at(i)->hostId);
				emit GWRHSetBallVisible(GWRequests.at(i)->hostId, true);
			}

			break;
		}


	}

	//this->printGWRequests();

}

void GWRemoteHosts::setGWRHGameStateInfo(GameStateMessage gsm, QString hostId)
{
	QTreeWidgetItem* item;
	QCheckBox *checkBox;
	QIcon icon;

	QString gsmQString = QString("");
	QString teamNumber = QString("");
	QString playerNumber = QString("");
	QString playerState = QString("");


	item = GWhostFinder(hostId);
	if( item != NULL)
	{
		checkBox = (QCheckBox*) parentTreeWidget->itemWidget(item, 0);

		if(gsm.team_color() == 0)
		{
			icon.addFile(QString::fromUtf8(":/KnownHostsIcons/robot_blue.png"), QSize(), QIcon::Normal, QIcon::Off);
			checkBox->setIcon(icon);
			checkBox->setIconSize(QSize(48, 48));

		}else if (gsm.team_color() == 1)
		{
			icon.addFile(QString::fromUtf8(":/KnownHostsIcons/robot_red.png"), QSize(), QIcon::Normal, QIcon::Off);
			checkBox->setIcon(icon);
			checkBox->setIconSize(QSize(48, 48));

		}

		playerNumber = QString::fromStdString(_toString((gsm.player_number())));
		teamNumber = QString::fromStdString(_toString((gsm.team_number())));

		if(gsm.player_state() == 0)
		{
			playerState = QString("INITIAL");

		}else if(gsm.player_state() == 1)
		{
			playerState = QString("READY");

		}else if(gsm.player_state() == 2)
		{
			playerState = QString("SET");

		}else if(gsm.player_state() == 3)
		{
			playerState = QString("PLAYING");

		}else if(gsm.player_state() == 4)
		{
			playerState = QString("FINISHED");

		}else if(gsm.player_state() == 5)
		{
			playerState = QString("PENALISED");
		}

		//find hostname and give it to me ...

		gsmQString = GWhostNameFinder(hostId) + QString(", ") + teamNumber + QString(", ") + playerNumber + QString(", ") + playerState;
		checkBox->setText(gsmQString) ;

		//for combo box manipulation
		emit LWRHGameStateMsgUpdate(icon, gsmQString, hostId);

	}

}

QString GWRemoteHosts::GWhostNameFinder(QString hostId)
{
	QString hostname = "";

	for(int i = 0; i<GWRequests.count();i++)
	{
		if (GWRequests.at(i)->hostId == hostId)
			hostname = GWRequests.at(i)->hostName;
	}

	return hostname;
}

void GWRemoteHosts::printGWRequests()
{
	std::cout << "~~~~~~~~~GWRemoteHosts::printGWRequests()~~~~~~~~~~~~~~" << std::endl;
	for(int i = 0; i<GWRequests.count();i++)
	{
		std::cout << "Host :: " << GWRequests.at(i)->hostId.toStdString()<< std::endl;
		std::cout << "Host :: " << GWRequests.at(i)->hostName.toStdString()<< std::endl;
		std::cout << "Position :: " << GWRequests.at(i)->requestedPosition<< std::endl;
		std::cout << "Ball :: " << GWRequests.at(i)->requestedBall<< std::endl << std::endl;
	}
	std::cout << "~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
}

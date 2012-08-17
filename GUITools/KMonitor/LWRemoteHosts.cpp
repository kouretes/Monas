#include "LWRemoteHosts.h"

#include <iostream>
using namespace std;

LWRemoteHosts::LWRemoteHosts(QComboBox *parent )
{
	parentComboBox = parent;

	parentComboBox->setIconSize(QSize(48, 48));
	parentComboBox->adjustSize();

	connect(parentComboBox, SIGNAL(activated(int)), this, SLOT(newLWRemoteHostSelected(int)));

	LWRequests.clear();
	myCurrentRequestedHost.clear();

	addComboBoxItem(tr(""), tr("  Please, select robot."));
	//printLWRequests();
}

LWRemoteHosts::~LWRemoteHosts()
{

}

void LWRemoteHosts::addComboBoxItem(QString hostId, QString hostName)
{
	QIcon icon;

	icon.addFile(QString::fromUtf8(":/KnownHostsIcons/chore_robot_on.png"), QSize(), QIcon::Normal, QIcon::Off);

	parentComboBox->addItem(icon, hostName);

	requestedLWElements *re = new LWElements(hostId, hostName, false);

	LWRequests.append(re);

	//printLWRequests();

}

void LWRemoteHosts::removeComboBoxItem(QString hostId)
{
	int hostIndex = -1;

	hostIndex = this->LWhostFinder(hostId);

	if(hostIndex != -1 && hostIndex <= parentComboBox->count())
	{
		emit LWRHUnsubscriptionRequest(hostId);

		if (LWRequests.at(hostIndex)->hostSelected)
			emit newLWRemoteHostSelected(0);

		parentComboBox->removeItem(hostIndex);
		LWRequests.removeAt(hostIndex);
	}

	//printLWRequests();

}

int LWRemoteHosts::LWhostFinder(QString hostId)
{
	int hostIndex = -1;

	for(int i = 0; i<LWRequests.count();i++)
	{
		if (LWRequests.at(i)->hostId == hostId)
		{
			hostIndex = i;
			break;
		}
	}

	return hostIndex;


}

void LWRemoteHosts::setLWRHGameStateInfo(QIcon icon, QString gsm, QString hostId)
{
	int hostIndex = this->LWhostFinder(hostId);

	if(hostIndex != -1 && hostIndex <= parentComboBox->count())
	{
		parentComboBox->setItemIcon(hostIndex, icon);
		parentComboBox->setItemText(hostIndex, gsm);

	}


}

void LWRemoteHosts::newLWRemoteHostSelected(int index)
{
/*	int oldHostIndex;

	oldHostIndex = LWhostFinder(myCurrentRequestedHost);
	if(oldHostIndex > -1 && oldHostIndex <= parentComboBox->count()){

		emit LWRHUnsubscriptionRequest(LWRequests.at(oldHostIndex)->hostId);
	}*/

	emit LWRHUnsubscriptionRequest(myCurrentRequestedHost);

	for(int i = 0; i<LWRequests.count();i++)
	{
		if(i == index)
		{
			LWRequests.at(i)->hostSelected = true;
			myCurrentRequestedHost = LWRequests.at(i)->hostId;

			if(!myCurrentRequestedHost.isEmpty()){
				std::cout << "LWRemoteHosts::newLWRemoteHostSelected :: KAnw subreq" << std::endl;
				emit LWRHSubscriptionRequest(LWRequests.at(i)->hostId);
			}
		}else
		{
			LWRequests.at(i)->hostSelected = false;
		}
	}

	//printLWRequests();
}

void LWRemoteHosts::printLWRequests()
{
	std::cout << "~~~~~~~~~LWRemoteHosts::printLWRequests()~~~~~~~~~~~~~~" << std::endl;
	for(int i = 0; i<LWRequests.count();i++)
	{
		std::cout << "Host :: " << LWRequests.at(i)->hostId.toStdString()<< std::endl;
		std::cout << "Requested :: " << LWRequests.at(i)->hostSelected << std::endl;
	}
	std::cout << "~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
}

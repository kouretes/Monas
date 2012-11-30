#include "HostsComboBox.h"

#include <iostream>
using namespace std;

HostsComboBox::HostsComboBox(QComboBox *parent )
{
	parentComboBox = parent;

	parentComboBox->setIconSize(QSize(48, 48));
	parentComboBox->adjustSize();

	connect(parentComboBox, SIGNAL(activated(int)), this, SLOT(newHostsComboBoxelected(int)));

	LWRequests.clear();
	myCurrentRequestedHost.clear();

	addComboBoxItem(tr(""), tr("  Please, select robot."));
	//printLWRequests();
}

HostsComboBox::~HostsComboBox()
{

}

void HostsComboBox::addComboBoxItem(QString hostId, QString hostName)
{
	QIcon icon;

	icon.addFile(QString::fromUtf8(":/KnownHostsIcons/chore_robot_on.png"), QSize(), QIcon::Normal, QIcon::Off);

	parentComboBox->addItem(icon, hostName);

	requestedLWElements *re = new LWElements(hostId, hostName, false);

	LWRequests.append(re);

	//printLWRequests();

}

void HostsComboBox::removeComboBoxItem(QString hostId)
{
	int hostIndex = -1;
	hostIndex = this->LWhostFinder(hostId);

	if(hostIndex != -1 && hostIndex <= parentComboBox->count())
	{
		if (LWRequests.at(hostIndex)->hostSelected){

			emit LWRHUnsubscriptionRequest(hostId);
			emit newHostsComboBoxelected(0);
		}

		parentComboBox->removeItem(hostIndex);
		LWRequests.removeAt(hostIndex);
	}

	//printLWRequests();

}

int HostsComboBox::LWhostFinder(QString hostId)
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

void HostsComboBox::setLWRHGameStateInfo(QString iconPath, QString gsm, QString hostId)
{
	int hostIndex = this->LWhostFinder(hostId);
	QIcon icon;
	icon.addFile(iconPath, QSize(), QIcon::Normal, QIcon::Off);
	if(hostIndex != -1 && hostIndex <= parentComboBox->count())
	{
		QString fixGsm = LWRequests.at(hostIndex)->hostName;
		fixGsm = fixGsm + gsm;
		parentComboBox->setItemIcon(hostIndex, icon);
		parentComboBox->setItemText(hostIndex, fixGsm);

	}


}

void HostsComboBox::newHostsComboBoxelected(int index)
{
	emit LWRHUnsubscriptionRequest(myCurrentRequestedHost);

	for(int i = 0; i<LWRequests.count();i++)
	{
		if(i == index)
		{
			LWRequests.at(i)->hostSelected = true;
			myCurrentRequestedHost = LWRequests.at(i)->hostId;

			if(!myCurrentRequestedHost.isEmpty())
				emit LWRHSubscriptionRequest(LWRequests.at(i)->hostId);

		}else
		{
			LWRequests.at(i)->hostSelected = false;
		}
	}

	//printLWRequests();
}

void HostsComboBox::printLWRequests()
{
	std::cout << "~~~~~~~~~HostsComboBox::printLWRequests()~~~~~~~~~~~~~~" << std::endl;
	for(int i = 0; i<LWRequests.count();i++)
	{
		std::cout << "Host :: " << LWRequests.at(i)->hostId.toStdString()<< std::endl;
		std::cout << "Requested :: " << LWRequests.at(i)->hostSelected << std::endl;
	}
	std::cout << "~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
}

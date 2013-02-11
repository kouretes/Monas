#include "HostsComboBox.h"

#include <iostream>
using namespace std;

HostsComboBox::HostsComboBox (QComboBox *parent ) {
	parentComboBox = parent;
	parentComboBox->setIconSize (QSize (48, 48) );
	parentComboBox->adjustSize();
	connect (parentComboBox, SIGNAL (activated (int) ), this, SLOT (newHostsComboBoxSelected (int) ) );
	Requests.clear();
	addComboBoxItem (tr (""), tr ("  Please, select robot.") );
}

HostsComboBox::~HostsComboBox() {
}

void HostsComboBox::addComboBoxItem (QString hostId, QString hostName) {
	QIcon icon;
	icon.addFile (QString::fromUtf8 (":/KnownHostsIcons/chore_robot_on.png"), QSize(), QIcon::Normal, QIcon::Off);
	parentComboBox->addItem (icon, hostName);
	requestedElements *re = new Elements (hostId, hostName, false);
	Requests.append (re);
}

void HostsComboBox::removeComboBoxItem (QString hostId) {
	int hostIndex = -1;
	hostIndex = this->HostFinder (hostId);

	if (hostIndex != -1 && hostIndex <= parentComboBox->count() ) {
		if (Requests.at (hostIndex)->hostSelected) {
			newHostsComboBoxSelected (0);
		}

		parentComboBox->removeItem (hostIndex);
		Requests.removeAt (hostIndex);
	}
}

int HostsComboBox::HostFinder (QString hostId) {
	int hostIndex = -1;

	for (int i = 0; i < Requests.count(); i++) {
		if (Requests.at (i)->hostId == hostId) {
			hostIndex = i;
			break;
		}
	}

	return hostIndex;
}

void HostsComboBox::setGameStateInfo (QString iconPath, QString gsm, QString hostId) {
	int hostIndex = this->HostFinder (hostId);
	QIcon icon;
	icon.addFile (iconPath, QSize(), QIcon::Normal, QIcon::Off);

	if (hostIndex != -1 && hostIndex <= parentComboBox->count() ) {
		QString fixGsm = Requests.at (hostIndex)->hostName;
		fixGsm = fixGsm + gsm;
		parentComboBox->setItemIcon (hostIndex, icon);
		parentComboBox->setItemText (hostIndex, fixGsm);
	}
}

void HostsComboBox::changeItem (QString hostId) {
	int index = this->HostFinder (hostId);
	parentComboBox->setCurrentIndex (index);

	for (int i = 0; i < Requests.count(); i++) {
		if (i == index) {
			Requests.at (i)->hostSelected = true;
		} else {
			Requests.at (i)->hostSelected = false;
		}
	}
}

void HostsComboBox::newHostsComboBoxSelected (int index) {
	for (int i = 0; i < Requests.count(); i++) {
		if (i == index) {
			Requests.at (i)->hostSelected = true;
			emit SubscriptionRequest (Requests.at (i)->hostId);
		} else {
			Requests.at (i)->hostSelected = false;
		}
	}
}

void HostsComboBox::printRequests() {
	std::cout << "~~~~~~~~~HostsComboBox::printRequests()~~~~~~~~~~~~~~" << std::endl;

	for (int i = 0; i < Requests.count(); i++) {
		std::cout << "Host :: " << Requests.at (i)->hostId.toStdString() << std::endl;
		std::cout << "Requested :: " << Requests.at (i)->hostSelected << std::endl;
	}

	std::cout << "~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
}

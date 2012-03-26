#include "LWElementList.h"

#include <iostream>
using namespace std;

LWElementList::LWElementList(QListWidget *parent )
{
	parentListWidget = parent;
	myCurrentLWRequestedHost = "";

	uncheckAllListElements();

	connect(parentListWidget, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(newListElementRequested(QListWidgetItem*)));

}

LWElementList::~LWElementList()
{

}


void LWElementList::newListElementRequested(QListWidgetItem* item)
{
	if(parentListWidget->row(item)==0)
	{
		if(item->checkState() == 0)
			emit LWRHSetRobotVisible(myCurrentLWRequestedHost, false);
		else
			emit LWRHSetRobotVisible(myCurrentLWRequestedHost, true);

	}else if(parentListWidget->row(item)==1)
	{
		if(item->checkState() == 0)
			emit LWRHSetBallVisible(myCurrentLWRequestedHost, false);
		else
			emit LWRHSetBallVisible(myCurrentLWRequestedHost, true);

	}else if(parentListWidget->row(item)==2)
	{
		if(item->checkState() == 0)
			emit LWRHSetVisionBallVisible(myCurrentLWRequestedHost, false);
		else
			emit LWRHSetVisionBallVisible(myCurrentLWRequestedHost, true);
	}
}

void LWElementList::uncheckAllListElements()
{
	QListWidgetItem* item;

	for (int i=0; i<parentListWidget->count(); i++)
	{
		item = parentListWidget->item(i);
		item->setCheckState(Qt::Unchecked);
	}
}

void LWElementList::LWELSubscriptionHandler(QString hostId)
{
	if(myCurrentLWRequestedHost != hostId)
	{
		myCurrentLWRequestedHost = hostId;
		uncheckAllListElements();
	}

}

void LWElementList::LWELUnsubscriptionHandler(QString hostId)
{
	if(myCurrentLWRequestedHost == hostId)
	{
		myCurrentLWRequestedHost = "";
		uncheckAllListElements();
	}
}

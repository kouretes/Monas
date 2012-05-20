#include "LMElementList.h"

#include <iostream>
using namespace std;

LMElementList::LMElementList(QListWidget *parent )
{
	parentListWidget = parent;
	myCurrentLMRequestedHost.clear();

	uncheckAllListElements();

	connect(parentListWidget, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(newListElementRequested(QListWidgetItem*)));

}

LMElementList::~LMElementList()
{

}

void LMElementList::newListElementRequested(QListWidgetItem* item)
{
	if(parentListWidget->row(item)==0 && !myCurrentLMRequestedHost.isEmpty())
	{
		if(item->checkState() == 0)
			emit LMRHSetObstaclesVisible(myCurrentLMRequestedHost, false);
		else
			emit LMRHSetObstaclesVisible(myCurrentLMRequestedHost, true);

	}/*else if(parentListWidget->row(item)==1)
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

	}*/
}

void LMElementList::uncheckAllListElements()
{
	QListWidgetItem* item;

	for (int i=0; i<parentListWidget->count(); i++)
	{
		item = parentListWidget->item(i);
		item->setCheckState(Qt::Unchecked);
	}
}

void LMElementList::LMELSubscriptionHandler(QString hostId)
{
	if(myCurrentLMRequestedHost != hostId)
	{
		myCurrentLMRequestedHost = hostId;
		uncheckAllListElements();
	}

}

void LMElementList::LMELUnsubscriptionHandler(QString hostId)
{
	if(myCurrentLMRequestedHost == hostId)
	{
		myCurrentLMRequestedHost.clear();
		uncheckAllListElements();
	}
}

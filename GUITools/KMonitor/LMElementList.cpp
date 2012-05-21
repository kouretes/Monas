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
	}
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

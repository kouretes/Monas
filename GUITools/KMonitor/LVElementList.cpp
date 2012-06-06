#include "LVElementList.h"

#include <iostream>
using namespace std;

LVElementList::LVElementList(QListWidget *parent )
{
	parentListWidget = parent;
	myCurrentLVRequestedHost.clear();

	uncheckAllListElements();

	connect(parentListWidget, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(newListElementRequested(QListWidgetItem*)));

}

LVElementList::~LVElementList()
{

}

void LVElementList::newListElementRequested(QListWidgetItem* item)
{
	if(!myCurrentLVRequestedHost.isEmpty())
	{
		if(parentListWidget->row(item)==0 )
		{
			if(item->checkState() == 0)
				emit LVRHSetRawImageVisible(myCurrentLVRequestedHost, false);
			else
				emit LVRHSetRawImageVisible(myCurrentLVRequestedHost, true);

		}else if(parentListWidget->row(item)== 1)
		{

		}
	}
}

void LVElementList::uncheckAllListElements()
{
	QListWidgetItem* item;

	for (int i=0; i<parentListWidget->count(); i++)
	{
		item = parentListWidget->item(i);
		item->setCheckState(Qt::Unchecked);
	}
}

void LVElementList::LVELSubscriptionHandler(QString hostId)
{
	if(myCurrentLVRequestedHost != hostId)
	{
		myCurrentLVRequestedHost = hostId;
		uncheckAllListElements();
	}
}

void LVElementList::LVELUnsubscriptionHandler(QString hostId)
{
	if(myCurrentLVRequestedHost == hostId)
	{
		myCurrentLVRequestedHost.clear();
		uncheckAllListElements();
	}
}

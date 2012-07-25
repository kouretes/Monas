#include "LVElementList.h"

#include <QCheckBox>

#include <iostream>
using namespace std;

LVElementList::LVElementList(QListWidget *parent )
{
	parentListWidget = parent;
	myCurrentLVRequestedHost.clear();

	rawImageRequested = false;
	segImageRequested = false;
	uncheckAllListElements();

	connect(parentListWidget, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(newListElementRequested(QListWidgetItem*)));

}

LVElementList::~LVElementList()
{

}

void LVElementList::newListElementRequested(QListWidgetItem* item)
{
	QCheckBox* checkBox = NULL;

	if(!myCurrentLVRequestedHost.isEmpty())
	{
		if(parentListWidget->row(item) == 0 )
		{
			if(item->checkState() == 0)
			{
				rawImageRequested = false;
				emit LVRHSetRawImageVisible(myCurrentLVRequestedHost, false);

			}else{
				rawImageRequested = true;
				if(segImageRequested)
					parentListWidget->item(1)->setCheckState(Qt::Unchecked);

				emit LVRHSetRawImageVisible(myCurrentLVRequestedHost, true);
			}

		}else if(parentListWidget->row(item) == 1)
		{
			if(item->checkState() == 0)
			{
				segImageRequested = false;
				emit LVRHSetSegImageVisible(myCurrentLVRequestedHost, false);

			}else
			{
				segImageRequested = true;
				if(rawImageRequested)
					parentListWidget->item(0)->setCheckState(Qt::Unchecked);

				emit LVRHSetSegImageVisible(myCurrentLVRequestedHost, true);
			}
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

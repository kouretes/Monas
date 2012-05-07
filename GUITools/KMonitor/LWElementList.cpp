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

	}else if(parentListWidget->row(item)==3)
	{
		if(item->checkState() == 0)
			emit LWRHSetVisionGoalPostsVisible(myCurrentLWRequestedHost, false);
		else
			emit LWRHSetVisionGoalPostsVisible(myCurrentLWRequestedHost, true);

	}else if(parentListWidget->row(item)==4)
	{
		if(item->checkState() == 0)
		{
			emit LWRHSetParticlesVisible(myCurrentLWRequestedHost, false);
		}
		else
		{
			emit LWRHSetParticlesVisible(myCurrentLWRequestedHost, true);
		}
	}else if(parentListWidget->row(item)==5)
	{
		if(item->checkState() == 0)
		{
			emit LWRHSetHFOVVisible(myCurrentLWRequestedHost, false);
		}
		else
		{
			emit LWRHSetHFOVVisible(myCurrentLWRequestedHost, true);
		}
	}else if(parentListWidget->row(item)==6)
	{
		if(item->checkState() == 0)
		{
			emit LWRHSetTraceVisible(myCurrentLWRequestedHost, false);
		}
		else
		{
			emit LWRHSetTraceVisible(myCurrentLWRequestedHost, true);
		}
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

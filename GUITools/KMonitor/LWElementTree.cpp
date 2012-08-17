#include "LWElementTree.h"

#include <iostream>
using namespace std;

LWElementTree::LWElementTree(QTreeWidget *parent )
{
	parentTreeWidget = parent;
	myCurrentLWRequestedHost.clear();

	un_checkAllTreeElements(Qt::Unchecked);

	connect(parentTreeWidget, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(newTreeElementRequested(QTreeWidgetItem*)));

}


LWElementTree::~LWElementTree()
{

}

void LWElementTree::newTreeElementRequested(QTreeWidgetItem* item)
{
	if(!myCurrentLWRequestedHost.isEmpty())
	{
		if(parentTreeWidget->itemAt(0,0)->indexOfChild(item) == 0 )
		{
			if(item->checkState(0) == 0)
				emit LWRHSetRobotVisible(myCurrentLWRequestedHost, false);
			else
				emit LWRHSetRobotVisible(myCurrentLWRequestedHost, true);

		}else if(parentTreeWidget->itemAt(0,0)->indexOfChild(item) == 1)
		{
			if(item->checkState(0) == 0)
				emit LWRHSetBallVisible(myCurrentLWRequestedHost, false);
			else
				emit LWRHSetBallVisible(myCurrentLWRequestedHost, true);

		}else if(parentTreeWidget->itemAt(0,0)->indexOfChild(item) == 2 )
		{
			if(item->checkState(0) == 0)
				emit LWRHSetVisionBallVisible(myCurrentLWRequestedHost, false);
			else
				emit LWRHSetVisionBallVisible(myCurrentLWRequestedHost, true);

		}else if(parentTreeWidget->itemAt(0,0)->indexOfChild(item) == 3 )
		{
			if(item->checkState(0) == 0)
				emit LWRHSetVisionGoalPostsVisible(myCurrentLWRequestedHost, false);
			else
				emit LWRHSetVisionGoalPostsVisible(myCurrentLWRequestedHost, true);

		}else if(parentTreeWidget->itemAt(0,0)->indexOfChild(item) == 4 )
		{
			if(item->checkState(0) == 0)
				emit LWRHSetParticlesVisible(myCurrentLWRequestedHost, false);
			else
				emit LWRHSetParticlesVisible(myCurrentLWRequestedHost, true);

		}else if(parentTreeWidget->itemAt(0,0)->indexOfChild(item) == 5 )
		{
			if(item->checkState(0) == 0)
				emit LWRHSetHFOVVisible(myCurrentLWRequestedHost, false);
			else
				emit LWRHSetHFOVVisible(myCurrentLWRequestedHost, true);

		}else if(parentTreeWidget->itemAt(0,0)->indexOfChild(item) == 6 )
		{
			if(item->checkState(0) == 0)
				emit LWRHSetTraceVisible(myCurrentLWRequestedHost, false);
			else
				emit LWRHSetTraceVisible(myCurrentLWRequestedHost, true);

		}else if(parentTreeWidget->itemAt(0,0)->indexOfChild(item) == 7 )
		{
			if(item->checkState(0) == 0)
				emit LWRHSetMWCmdVisible(myCurrentLWRequestedHost, false);
			else
				emit LWRHSetMWCmdVisible(myCurrentLWRequestedHost, true);

		}else
		{
			if(item->checkState(0) == 0)
				un_checkAllTreeElements(Qt::Unchecked);
			else
				un_checkAllTreeElements(Qt::Checked);
		}
	}
}

void LWElementTree::un_checkAllTreeElements(Qt::CheckState state)
{
	QTreeWidgetItem* item;

	for (int i=0; i<parentTreeWidget->itemAt(0,0)->childCount(); i++)
	{
		item = parentTreeWidget->itemAt(0,0)->child(i);
		item->setCheckState(0, state);
	}
	parentTreeWidget->itemAt(0,0)->setCheckState(0, state);
}

void LWElementTree::LWELSubscriptionHandler(QString hostId)
{
	if(myCurrentLWRequestedHost != hostId)
	{
		myCurrentLWRequestedHost = hostId;
		un_checkAllTreeElements(Qt::Unchecked);
	}
}

void LWElementTree::LWELUnsubscriptionHandler(QString hostId)
{
	if(myCurrentLWRequestedHost == hostId)
	{
		myCurrentLWRequestedHost.clear();
		un_checkAllTreeElements(Qt::Unchecked);
	}
}

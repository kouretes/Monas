#include "LMElementTree.h"

#include <iostream>
using namespace std;

LMElementTree::LMElementTree(QTreeWidget *parent )
{
	parentTreeWidget = parent;
	myCurrentLMRequestedHost.clear();

	un_checkAllTreeElements(Qt::Unchecked);

	connect(parentTreeWidget, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(newTreeElementRequested(QTreeWidgetItem*)));

}

LMElementTree::~LMElementTree()
{

}

void LMElementTree::newTreeElementRequested(QTreeWidgetItem* item)
{
	if(!myCurrentLMRequestedHost.isEmpty())
	{
		if(parentTreeWidget->itemAt(0,0)->indexOfChild(item) == 0 )
		{
			if(item->checkState(0) == 0)
				emit LMRHSetObstaclesVisible(myCurrentLMRequestedHost, false);
			else
				emit LMRHSetObstaclesVisible(myCurrentLMRequestedHost, true);

		}else if(parentTreeWidget->itemAt(0,0)->indexOfChild(item) == 1)
		{
			if(item->checkState(0) == 0)
				emit LMRHSetPathVisible(myCurrentLMRequestedHost, false);
			else
				emit LMRHSetPathVisible(myCurrentLMRequestedHost, true);

		}else if(parentTreeWidget->itemAt(0,0)->indexOfChild(item) == 2 )
		{
			if(item->checkState(0) == 0)
				emit LMRHSetTargCoordVisible(myCurrentLMRequestedHost, false);
			else
				emit LMRHSetTargCoordVisible(myCurrentLMRequestedHost, true);
		}else
		{
			if(item->checkState(0) == 0)
				un_checkAllTreeElements(Qt::Unchecked);
			else
				un_checkAllTreeElements(Qt::Checked);
		}
	}
}

void LMElementTree::un_checkAllTreeElements(Qt::CheckState state)
{
	QTreeWidgetItem* item;

	for (int i=0; i<parentTreeWidget->itemAt(0,0)->childCount(); i++)
	{
		item = parentTreeWidget->itemAt(0,0)->child(i);
		item->setCheckState(0, state);
	}
	parentTreeWidget->itemAt(0,0)->setCheckState(0, state);
}

void LMElementTree::LMELSubscriptionHandler(QString hostId)
{
	if(myCurrentLMRequestedHost != hostId)
	{
		myCurrentLMRequestedHost = hostId;
		un_checkAllTreeElements(Qt::Unchecked);
	}
}

void LMElementTree::LMELUnsubscriptionHandler(QString hostId)
{
	if((hostId.isEmpty() && !myCurrentLMRequestedHost.isEmpty()) || (myCurrentLMRequestedHost == hostId && !hostId.isEmpty()))
	{
		myCurrentLMRequestedHost.clear();
		un_checkAllTreeElements(Qt::Unchecked);
	}
}



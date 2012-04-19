#include "KGraphicsView.h"

KGraphicsView::KGraphicsView(QWidget* parent)
    : QGraphicsView(parent)
    , paintArea(0)
{
	this->paintArea = new KFieldScene(this);
	this->setScene(paintArea);
	this->setResizeAnchor(QGraphicsView::NoAnchor);
	this->setTransformationAnchor(QGraphicsView::NoAnchor);

}

KGraphicsView::~KGraphicsView()
{
}


void KGraphicsView::resizeEvent(QResizeEvent* event)
{
	//paintArea->resizeFieldScene(width()-20,height()-40);
	//paintArea->resizeFieldScene(width()-20, (width()-20)/1.37037) ;

	paintArea->resizeFieldScene((height()-40)*1.37, height()-40);
	QGraphicsView::resizeEvent(event);
}

void KGraphicsView::setKGFCGameStateInfo(GameStateMessage gsm, QString host)
{
	GraphicalRobotElement* element = paintArea->findGraphicalRobotItem(host);

	if(element !=NULL)
	{
		element->setCurrentGSM(gsm);

	}else
	{
		//std::cout << "KGraphicsView::setKGFCGameStateInfo:: Host hasn't been requested!" << host.toStdString() <<std::endl;
	}



}

void KGraphicsView::worldInfoUpdateHandler(WorldInfo nwim, QString host)
{
	GraphicalRobotElement* element = paintArea->findGraphicalRobotItem(host);

	if(element !=NULL)
	{
		element->setCurrentWIM(nwim);
		if (element->getGWSRobotVisible())
		{
			element->setRobotVisible(false);
			element->updateRobotRect();
			element->setRobotVisible(true);
		}

		if (element->getGWSBallVisible())
		{
			element->setBallVisible(false);
			element->updateBallRect();
			element->setBallVisible(true);
		}

		if (element->getGWSUnionistLineVisible())
		{
			element->setUnionistLineVisible(false);
			element->updateUnionistLineRect();
			element->setUnionistLineVisible(true);
		}

	}else
	{
		//std::cout << "[67]KGraphicsView::worldInfoUpdateHandler:: Host hasn't been requested!" << host.toStdString() <<std::endl;
	}

}

void KGraphicsView::GWSGVRobotVisible(QString host, bool visible)
{
	GraphicalRobotElement *robotElement = NULL;

	robotElement = paintArea->findGraphicalRobotItem(host );

	if(robotElement == NULL)
		robotElement = paintArea->newGraphicalRobotItem(host);

	if (robotElement != NULL)
	{
		robotElement->setGWSRobotVisible(visible);
		GWSGVUnionistLineVisible(robotElement);
		//paintArea->printRobotList();
	}else
	{
		std::cout << "[83] KGraphicsView::GWSGVRobotVisible : Fatal !" << std::endl;
	}

}

void KGraphicsView::GWSGVBallVisible(QString host, bool visible)
{
	GraphicalRobotElement *robotElement = NULL;

	robotElement = paintArea->findGraphicalRobotItem(host );

	if(robotElement == NULL)
		robotElement = paintArea->newGraphicalRobotItem(host);

	if (robotElement != NULL)
	{
		robotElement->setGWSBallVisible(visible);
		GWSGVUnionistLineVisible(robotElement);
		//paintArea->printRobotList();
	}else
	{
		std::cout << "[99] KGraphicsView::GWSGVBallVisible : Fatal !" << std::endl;
	}

}

void KGraphicsView::GWSGVUnionistLineVisible(GraphicalRobotElement *robotElement)
{

	if(robotElement->getGWSRobotVisible() && robotElement->getGWSBallVisible())
	{
		robotElement->setGWSUnionistLineVisible(true);
	}else
	{
		robotElement->setGWSUnionistLineVisible(false);

	}

}

void KGraphicsView::removeGraphicalElement(QString host)
{
	 paintArea->removeGraphicalRobotItem(host);
}

void KGraphicsView::LWSGVRobotVisible(QString host, bool visible)
{
	GraphicalRobotElement *robotElement = NULL;

	robotElement = paintArea->findGraphicalRobotItem(host );

	if(robotElement == NULL )
	{
		if(paintArea->getRobotList().count() != 0)
			removeGraphicalElement(paintArea->getRobotList().at(0)->getHostId());

		robotElement = paintArea->newGraphicalRobotItem(host);
	}

	if (robotElement != NULL)
	{
		robotElement->setGWSRobotVisible(visible);
		GWSGVUnionistLineVisible(robotElement);
		//paintArea->printRobotList();
	}else
	{
		std::cout << "[83] KGraphicsView::LWSGVRobotVisible : Fatal !" << std::endl;
	}

}

void KGraphicsView::LWSGVBallVisible(QString host, bool visible)
{
	GraphicalRobotElement *robotElement = NULL;

	robotElement = paintArea->findGraphicalRobotItem( host );

	if(robotElement == NULL )
	{
		if(paintArea->getRobotList().count() != 0)
			removeGraphicalElement(paintArea->getRobotList().at(0)->getHostId());

		robotElement = paintArea->newGraphicalRobotItem(host);
	}

	if (robotElement != NULL)
	{
		robotElement->setGWSBallVisible(visible);
		GWSGVUnionistLineVisible(robotElement);
		//paintArea->printRobotList();
	}else
	{
		std::cout << "[83] KGraphicsView::LWSGVRobotVisible : Fatal !" << std::endl;
	}

}

// !!!
void KGraphicsView::LWSGVVisionBallVisible(QString host, bool visible)
{
	GraphicalRobotElement *robotElement = NULL;

	robotElement = paintArea->findGraphicalRobotItem( host );

	if(robotElement == NULL )
	{
		if(paintArea->getRobotList().count() != 0)
			removeGraphicalElement(paintArea->getRobotList().at(0)->getHostId());

		robotElement = paintArea->newGraphicalRobotItem(host);
	}

	if (robotElement != NULL)
	{
		robotElement->setLWSVisionBallVisible(visible);

	}else
	{
		std::cout << "[214] KGraphicsView::LWSGVVisionBallVisible : Fatal !" << std::endl;
	}

	//paintArea->printRobotList();

}

void KGraphicsView::LWSGVVisionGoalPostsVisible(QString host, bool visible)
{
	GraphicalRobotElement *robotElement = NULL;

	robotElement = paintArea->findGraphicalRobotItem( host );

	if(robotElement == NULL )
	{
		if(paintArea->getRobotList().count() != 0)
			removeGraphicalElement(paintArea->getRobotList().at(0)->getHostId());

		robotElement = paintArea->newGraphicalRobotItem(host);
	}

	if (robotElement != NULL)
	{
		robotElement->setLWSYellowPostVisible(visible);
		robotElement->setLWSYellowLeftPostVisible(visible);
		robotElement->setLWSYellowRightPostVisible(visible);

	}else
	{
		std::cout << "[245] KGraphicsView::LWSGVVisionGoalPostsVisible : Fatal " << std::endl;
	}

	//paintArea->printRobotList();

}

void KGraphicsView::observationMessageUpdateHandler(ObservationMessage om, QString host)
{

	GraphicalRobotElement* element = paintArea->findGraphicalRobotItem(host);

	//paintArea->printRobotList();
	if(element != NULL)
	{
		element->setcurrentOBSM(om);

		if (element->getLWSVisionBallVisible())
		{
			element->setLWSVisionBallVisible(false);
			element->updateVisionBallRect(om);
			element->setLWSVisionBallVisible(true);
		}

		if (element->getLWSYellowPostVisible())
			element->setLWSYellowPostVisible(false);

		if (element->getLWSYellowLeftPostVisible())
			element->setLWSYellowLeftPostVisible(false);

		if (element->getLWSYellowRightPostVisible())
			element->setLWSYellowRightPostVisible(false);

		element->updateGoalPostsRect();

		element->setLWSYellowLeftPostVisible(true);
		element->setLWSYellowRightPostVisible(true);
		element->setLWSYellowPostVisible(true);

		//check ...
		element->getGREtimer()->start(500);
		std::cout << "Ksanampika gia 0.5 s !!" << std::endl;

	}else
	{
		//std::cout << "[67]KGraphicsView::worldInfoUpdateHandler:: Host hasn't been requested!" << host.toStdString() <<std::endl;
	}

}

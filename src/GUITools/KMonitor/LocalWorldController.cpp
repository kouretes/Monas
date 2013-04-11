#include "LocalWorldController.h"
#include "ui_LocalWorldController.h"

LocalWorldController::LocalWorldController(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::LocalWorldController) {
	
	ui->setupUi(this);
	availableLWHosts = new HostsComboBox(ui->comboBox);
	connect(availableLWHosts, SIGNAL(SubscriptionRequest(QString)), this, SLOT(SubscriptionHandler(QString)) );
	this->paintArea = new KFieldScene(ui->graphicsView);
	ui->graphicsView->setScene(paintArea);
	ui->graphicsView->setResizeAnchor(QGraphicsView::NoAnchor);
	ui->graphicsView->setTransformationAnchor(QGraphicsView::NoAnchor);
	un_checkAllTreeElements(Qt::Unchecked);
	connect(ui->checkTree, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(newTreeElementRequested(QTreeWidgetItem *)) );
}

LocalWorldController::~LocalWorldController() {
	delete ui;
}

void LocalWorldController::newTreeElementRequested(QTreeWidgetItem *item) {

	if(ui->checkTree->itemAt(0, 0)->indexOfChild(item) == 0) {
		if(item->checkState(0) == 0)
			LWSGVRobotVisible(myCurrentLWRequestedHost, false);
		else
			LWSGVRobotVisible(myCurrentLWRequestedHost, true);
    }
    else if(ui->checkTree->itemAt(0, 0)->indexOfChild(item) == 1) {
		if(item->checkState(0) == 0)
			LWSGVVarianceVisible(myCurrentLWRequestedHost, false);
		else
			LWSGVVarianceVisible(myCurrentLWRequestedHost, true);
    }
    else if(ui->checkTree->itemAt(0, 0)->indexOfChild(item) == 2) {
		if(item->checkState(0) == 0)
			LWSGVEkfMHypothesisVisible(myCurrentLWRequestedHost, false);
		else
			LWSGVEkfMHypothesisVisible(myCurrentLWRequestedHost, true);
	}
    else if(ui->checkTree->itemAt(0, 0)->indexOfChild(item) == 3) {
		if(item->checkState(0) == 0)
			LWSGVOdometryVisible(myCurrentLWRequestedHost, false);
		else
			LWSGVOdometryVisible(myCurrentLWRequestedHost, true);
    }
    else if(ui->checkTree->itemAt(0, 0)->indexOfChild(item) == 4) {
		if(item->checkState(0) == 0)
			LWSGVRobotTraceVisible(myCurrentLWRequestedHost, false);
		else
			LWSGVRobotTraceVisible(myCurrentLWRequestedHost, true);
	}
	else if(ui->checkTree->itemAt(0, 0)->indexOfChild(item) == 5) {
		if(item->checkState(0) == 0)
			LWSGVBallVisible(myCurrentLWRequestedHost, false);
		else
			LWSGVBallVisible(myCurrentLWRequestedHost, true);
	}
	else if(ui->checkTree->itemAt(0, 0)->indexOfChild(item) == 6) {
		if(item->checkState(0) == 0)
			LWSGVVisionBallVisible(myCurrentLWRequestedHost, false);
		else
			LWSGVVisionBallVisible(myCurrentLWRequestedHost, true);
	}
	else if(ui->checkTree->itemAt(0, 0)->indexOfChild(item) == 7) {
		if(item->checkState(0) == 0)
			LWSGVVisionGoalPostsVisible(myCurrentLWRequestedHost, false);
		else
			LWSGVVisionGoalPostsVisible(myCurrentLWRequestedHost, true);
	} 
	else if(ui->checkTree->itemAt(0, 0)->indexOfChild(item) == 8) {
		if(item->checkState(0) == 0)
			LWSGVParticlesVisible(myCurrentLWRequestedHost, false);
		else
			LWSGVParticlesVisible(myCurrentLWRequestedHost, true);
	} 
	else if(ui->checkTree->itemAt(0, 0)->indexOfChild(item) == 9) {
		if(item->checkState(0) == 0)
			LWSGVHFOVVisible(myCurrentLWRequestedHost, false);
		else
			LWSGVHFOVVisible(myCurrentLWRequestedHost, true);
	} 
	else if(ui->checkTree->itemAt(0, 0)->indexOfChild(item) == 10) {
		if(item->checkState(0) == 0)
			LWSGVMWCmdVisible(myCurrentLWRequestedHost, false);
		else
			LWSGVMWCmdVisible(myCurrentLWRequestedHost, true);
	} 
	else if(ui->checkTree->itemAt(0, 0)->indexOfChild(item) == 11) {
		if(item->checkState(0) == 0)
			LWSGVTeammatesVisible(myCurrentLWRequestedHost, false);
		else
			LWSGVTeammatesVisible(myCurrentLWRequestedHost, true);
	} 
	else if(ui->checkTree->itemAt(0, 0)->indexOfChild(item) == 12) {
		if(item->checkState(0) == 0)
			LWSGVFormationVisible(myCurrentLWRequestedHost, false);
		else
			LWSGVFormationVisible(myCurrentLWRequestedHost, true);
	} 
	else {
		if(item->checkState(0) == 0)
			un_checkAllTreeElements(Qt::Unchecked);
		else
			un_checkAllTreeElements(Qt::Checked);
	}
}


void LocalWorldController::setKGFCGameStateInfo(GameStateMessage gsm, QString host) {

	GraphicalRobotElement *element = paintArea->findGraphicalRobotItem(host);

	if(element != NULL)
		element->setCurrentGSM(gsm);
}

void LocalWorldController::EKFMHypothesisUpdateHandler(EKFMHypothesis ekfMHypothesisM, QString host) {

    GraphicalRobotElement *element = paintArea->findGraphicalRobotItem(host);
    
    if(element != NULL) {
        element->setCurrentEKFMHypothesisM(ekfMHypothesisM);
        if(element->getLWSEkfMHypothesisVisible()) { 
            element->setEkfMHypothesisVisible(false);
            element->updateEkfMHypothesisRects();
            element->setEkfMHypothesisVisible(true);
        }
    }
}

void LocalWorldController::OdometryUpdateHandler(OdometryInfoMessage odometryM, QString host) {

    GraphicalRobotElement *element = paintArea->findGraphicalRobotItem(host);
    
    if(element != NULL) {
        element->setCurrentOdometryM(odometryM);
        if(element->getLWSOdometryVisible()) { 
            element->setOdometryVisible(false);
            element->updateOdometryPolygon();
            element->setOdometryVisible(true);
        }
    }
}

void LocalWorldController::worldInfoUpdateHandler(WorldInfo nwim, QString host) {

	GraphicalRobotElement *element = paintArea->findGraphicalRobotItem(host);

	if(element != NULL) {
		element->setCurrentWIM(nwim);

		if(element->getLWSRobotVisible()) {
			element->setRobotVisible(false);
			element->updateRobotRect();
			element->setRobotVisible(true);
		}

		if(element->getLWSBallVisible()) {
			element->setBallVisible(false);
			element->updateBallRect();
			element->setBallVisible(true);
		}

		if(element->getLWSUnionistLineVisible()) {
			element->setUnionistLineVisible(false);
			element->updateUnionistLineRect();
			element->setUnionistLineVisible(true);
		}

        if(element->getLWSVarianceVisible()) {
            element->setVarianceVisible(false);
            element->updatePoseUncertaintyRect();
            element->setVarianceVisible(true);
        }
	}
}

void LocalWorldController::sharedWorldInfoUpdateHandler(SharedWorldInfo nswim, QString host) {

	GraphicalRobotElement *element = paintArea->findGraphicalRobotItem(host);

	if(element != NULL) {
		element->setCurrentSWIM(nswim);
		if(element->getLWSTeammatesVisible()) 
			element->updateTeammatesRects();
	}
}

void LocalWorldController::LWSGVUnionistLineVisible(GraphicalRobotElement *robotElement) {

	if(robotElement->getLWSRobotVisible() && robotElement->getLWSBallVisible())
		robotElement->setLWSUnionistLineVisible(true);
	else
		robotElement->setLWSUnionistLineVisible(false);
}

void LocalWorldController::removeGraphicalElement(QString host) {
	paintArea->removeGraphicalRobotItem(host);
}

void LocalWorldController::LWSGVRobotVisible(QString host, bool visible) {

	GraphicalRobotElement *robotElement = NULL;
	robotElement = paintArea->findGraphicalRobotItem(host);

	if(robotElement == NULL) {
		if(paintArea->getRobotList().count() != 0)
			removeGraphicalElement(paintArea->getRobotList().at(0)->getHostId());

		robotElement = paintArea->newGraphicalRobotItem(host);
	}

	if(robotElement != NULL) {
		robotElement->setLWSRobotVisible(visible);
		LWSGVUnionistLineVisible(robotElement);
	}
}

 	

void LocalWorldController::LWSGVVarianceVisible(QString host, bool visible) {

    GraphicalRobotElement *robotElement = NULL;
    robotElement = paintArea->findGraphicalRobotItem(host);

    if(robotElement == NULL) {
        if(paintArea->getRobotList().count() != 0)
            removeGraphicalElement(paintArea->getRobotList().at(0)->getHostId());
     	
        robotElement = paintArea->newGraphicalRobotItem(host);
    }

    if(robotElement != NULL)
        robotElement->setLWSVarianceVisible(visible);
}

void LocalWorldController::LWSGVOdometryVisible(QString host, bool visible) {

    GraphicalRobotElement *robotElement = NULL;
    robotElement = paintArea->findGraphicalRobotItem( host );

    if(robotElement == NULL) {
        if(paintArea->getRobotList().count() != 0)
            removeGraphicalElement(paintArea->getRobotList().at(0)->getHostId());
        
        robotElement = paintArea->newGraphicalRobotItem(host);
    }

    if(robotElement != NULL)
        robotElement->setLWSOdometryVisible(visible);
}

void LocalWorldController::LWSGVBallVisible(QString host, bool visible) {

	GraphicalRobotElement *robotElement = NULL;
	robotElement = paintArea->findGraphicalRobotItem(host);

	if(robotElement == NULL ) {
		if(paintArea->getRobotList().count() != 0)
			removeGraphicalElement(paintArea->getRobotList().at(0)->getHostId());

		robotElement = paintArea->newGraphicalRobotItem(host);
	}

	if(robotElement != NULL) {
		robotElement->setLWSBallVisible(visible);
		LWSGVUnionistLineVisible(robotElement);
	}
}

void LocalWorldController::LWSGVVisionBallVisible(QString host, bool visible) {

	GraphicalRobotElement *robotElement = NULL;
	robotElement = paintArea->findGraphicalRobotItem(host);

	if(robotElement == NULL ) {
		if(paintArea->getRobotList().count() != 0)
			removeGraphicalElement(paintArea->getRobotList().at(0)->getHostId());

		robotElement = paintArea->newGraphicalRobotItem(host);
	}

	if(robotElement != NULL)
		robotElement->setLWSVisionBallVisible(visible);
}

void LocalWorldController::LWSGVVisionGoalPostsVisible(QString host, bool visible) {

	GraphicalRobotElement *robotElement = NULL;
	robotElement = paintArea->findGraphicalRobotItem(host);

	if(robotElement == NULL ) {
		if(paintArea->getRobotList().count() != 0) 
			removeGraphicalElement(paintArea->getRobotList().at(0)->getHostId());

		robotElement = paintArea->newGraphicalRobotItem(host);
	}

	if(robotElement != NULL) {
		robotElement->setLWSYellowPostVisible(visible);
		robotElement->setLWSYellowLeftPostVisible(visible);
		robotElement->setLWSYellowRightPostVisible(visible);
	}
}

void LocalWorldController::observationMessageUpdateHandler(ObservationMessage om, QString host) {

	GraphicalRobotElement *element = paintArea->findGraphicalRobotItem(host);

	if(element != NULL) {
		if(element->getGREtimer()->isActive())
			element->clearVisionObservations();

		element->setCurrentOBSM(om);

		if(element->getLWSVisionBallVisible()) {
			element->setLWSVisionBallVisible(false);
			element->updateVisionBallRect(om);
			element->setLWSVisionBallVisible(true);
		}

		if(element->getLWSYellowPostVisible()) {
			element->setLWSYellowPostVisible(false);
			element->setLWSYellowLeftPostVisible(false);
			element->setLWSYellowRightPostVisible(false);
			element->updateGoalPostsRect();
			element->setLWSYellowLeftPostVisible(true);
			element->setLWSYellowRightPostVisible(true);
			element->setLWSYellowPostVisible(true);
		}

		if(element->getLWSHFOVVisible()) {
			element->setLWSHFOVVisible(false);
			element->updateHFOVRect();
			element->setLWSHFOVVisible(true);
		}

		element->getGREtimer()->start(500);
	}
}

void LocalWorldController::LWSGVParticlesVisible(QString host, bool visible) {

	GraphicalRobotElement *robotElement = NULL;
	robotElement = paintArea->findGraphicalRobotItem(host);

	if(robotElement == NULL) {
		if(paintArea->getRobotList().count() != 0)
			removeGraphicalElement(paintArea->getRobotList().at(0)->getHostId());

		robotElement = paintArea->newGraphicalRobotItem(host);
	}

	if(robotElement != NULL)
		robotElement->setLWSParticlesVisible(visible);
}

void LocalWorldController::localizationDataUpdateHandler(LocalizationDataForGUI debugData, QString host) {

	GraphicalRobotElement *element = paintArea->findGraphicalRobotItem(host);

	if(element != NULL) {
		if(element->getLWSParticlesVisible()) {
			element->setLWSParticlesVisible(false);
			element->updateParticlesRect(debugData);
			element->setLWSParticlesVisible(true);
		}
	}
}

void LocalWorldController::formationDataUpdateHandler(FormationDataForGUI debugData, QString host) {

	GraphicalRobotElement *element = paintArea->findGraphicalRobotItem(host);
	
	if(element != NULL) {
		if(element->getLWSFormationVisible()) { 
			element->setFormationVisible(false);
			element->updateFormationRects(debugData);
			element->setFormationVisible(true);
		}
	}
}

void LocalWorldController::LWSGVHFOVVisible(QString host, bool visible) {

	GraphicalRobotElement *robotElement = NULL;
	robotElement = paintArea->findGraphicalRobotItem(host);

	if(robotElement == NULL) {
		if(paintArea->getRobotList().count() != 0)
			removeGraphicalElement(paintArea->getRobotList().at(0)->getHostId());

		robotElement = paintArea->newGraphicalRobotItem(host);
	}

	if(robotElement != NULL)
		robotElement->setLWSHFOVVisible(visible);
}

void LocalWorldController::LWSGVRobotTraceVisible(QString host, bool visible) {

	GraphicalRobotElement *robotElement = NULL;
	robotElement = paintArea->findGraphicalRobotItem(host);

	if(robotElement == NULL) {
		if(paintArea->getRobotList().count() != 0)
			removeGraphicalElement(paintArea->getRobotList().at(0)->getHostId());

		robotElement = paintArea->newGraphicalRobotItem(host);
	}

	if(robotElement != NULL)
		robotElement->setLWSRobotTraceVisible(visible);
}

void LocalWorldController::LWSGVMWCmdVisible(QString host, bool visible) {

	GraphicalRobotElement *robotElement = NULL;
	robotElement = paintArea->findGraphicalRobotItem(host);

	if(robotElement == NULL) {
		if(paintArea->getRobotList().count() != 0)
			removeGraphicalElement(paintArea->getRobotList().at(0)->getHostId());

		robotElement = paintArea->newGraphicalRobotItem(host);
	}

	if(robotElement != NULL)
		robotElement->setLWSMWCmdVisible(visible);
	else
		std::cout << "[214] LocalWorldController::LWSGVMWCmdVisible : Fatal !" << std::endl;
}

void LocalWorldController::LWSGVTeammatesVisible(QString host, bool visible) {

	GraphicalRobotElement *robotElement = NULL;
	robotElement = paintArea->findGraphicalRobotItem(host);

	if(robotElement == NULL) {
		if(paintArea->getRobotList().count() != 0)
			removeGraphicalElement(paintArea->getRobotList().at(0)->getHostId());

		robotElement = paintArea->newGraphicalRobotItem(host);
	}

	if(robotElement != NULL)
		robotElement->setLWSTeammatesVisible(visible);
}

void LocalWorldController::LWSGVEkfMHypothesisVisible(QString host, bool visible) {

	GraphicalRobotElement *robotElement = NULL;
	robotElement = paintArea->findGraphicalRobotItem(host);

	if(robotElement == NULL ) {
		if(paintArea->getRobotList().count() != 0)
			removeGraphicalElement(paintArea->getRobotList().at(0)->getHostId());

		robotElement = paintArea->newGraphicalRobotItem(host);
	}

	if(robotElement != NULL)
		robotElement->setLWSEkfMHypothesisVisible(visible);
}


void LocalWorldController::LWSGVFormationVisible(QString host, bool visible) {

	GraphicalRobotElement *robotElement = NULL;
	robotElement = paintArea->findGraphicalRobotItem(host);
	
	if(robotElement == NULL ) {
		if(paintArea->getRobotList().count() != 0)
			removeGraphicalElement(paintArea->getRobotList().at(0)->getHostId());

		robotElement = paintArea->newGraphicalRobotItem(host);
	}

	if(robotElement != NULL)
		robotElement->setLWSFormationVisible(visible);
}

void LocalWorldController::motionCommandUpdateHandler(MotionWalkMessage wmot, QString host) {

	GraphicalRobotElement *element = paintArea->findGraphicalRobotItem(host);

	if(element != NULL) {
		if(element->getLWSMWCmdVisible()) {
			element->setLWSMWCmdVisible(false);
			element->updateMWCmdRect(wmot);
			element->setLWSMWCmdVisible(true);
			element->getMWCmdTimer()->start(3000);
		}
	}
}

void LocalWorldController::un_checkAllTreeElements(Qt::CheckState state) {

	QTreeWidgetItem *item;
	
	for(int i = 0 ; i < ui->checkTree->itemAt(0, 0)->childCount() ; i++) {
		item = ui->checkTree->itemAt(0, 0)->child(i);
		item->setCheckState(0, state);
	}

	ui->checkTree->itemAt(0, 0)->setCheckState(0, state);
}

void LocalWorldController::resizeEvent(QResizeEvent *event) {

	// keep the ratio
	float width =(ui->graphicsView->width() - 40);
	float height =(ui->graphicsView->width() - 40) / 1.5;

	if(height > ui->graphicsView->height()) {
		width =(ui->graphicsView->height() - 40)*1.5;
		height =(ui->graphicsView->height() - 40);
	}

	paintArea->resizeFieldScene(width, height);
}

void LocalWorldController::changeToHost(QString data1) {
	removeGraphicalElement(data1);
	myCurrentLWRequestedHost = data1;
	availableLWHosts->changeItem(data1);
}

void LocalWorldController::addComboBoxItem(QString data1, QString data2) {
	availableLWHosts->addComboBoxItem(data1, data2);
}

void LocalWorldController::removeComboBoxItem(QString data1) {
	availableLWHosts->removeComboBoxItem(data1);
}

void LocalWorldController::setGameStateInfo(QString data1, QString data2, QString data3) {
	availableLWHosts->setGameStateInfo(data1, data2, data3);
}

void LocalWorldController::SubscriptionHandler(QString data1) {
	myCurrentLWRequestedHost = data1;
	un_checkAllTreeElements(Qt::Unchecked);
	emit SubscriptionRequest(data1);
}

void LocalWorldController::changeEvent(QEvent *e) {
	QWidget::changeEvent(e);

	switch(e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

#include "KSonarsController.h"
#include "ui_KSonarsController.h"
#include "KMapScene.h"

KSonarsController::KSonarsController(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::KSonarsController) {
	
	ui->setupUi(this);
	availableSonarsHosts = new HostsComboBox(ui->SonarComboBox);
	connect(availableSonarsHosts, SIGNAL(SubscriptionRequest(QString)), this, SLOT(SubscriptionHandler(QString)) );
	un_checkAllTreeElements(Qt::Unchecked);
	connect(ui->checkTree, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(newTreeElementRequested(QTreeWidgetItem*)) );
	mapArea = new KMapScene(ui->graphicsView);
	ui->graphicsView->setScene(mapArea);
	ui->graphicsView->setResizeAnchor(QGraphicsView::AnchorViewCenter);
	ui->graphicsView->setTransformationAnchor(QGraphicsView::AnchorViewCenter);
	mapArea->resetKMapScene();

	if(ui->graphicsView->width() > ui->graphicsView->height())
		mapArea->resizeMapScene(ui->graphicsView->height() - 10);
	else
		mapArea->resizeMapScene(ui->graphicsView->width() - 10);
}

KSonarsController::~KSonarsController() {
	delete ui;
}


void KSonarsController::newTreeElementRequested(QTreeWidgetItem *item) {
	if(ui->checkTree->itemAt(0, 0)->indexOfChild(item) == 0) {
		if(item->checkState(0) == 0)
			mapArea->setLPMObstaclesVisible(false);
		else
			mapArea->setLPMObstaclesVisible(true);
	}
	else if(ui->checkTree->itemAt(0, 0)->indexOfChild(item) == 1) {
		if(item->checkState(0) == 0)
			mapArea->setLPMPathVisible(false);
		else
			mapArea->setLPMPathVisible(true);
	}
	else if(ui->checkTree->itemAt(0, 0)->indexOfChild(item) == 2) {
		if(item->checkState(0) == 0)
			mapArea->setLPMTargetCoordVisible(false);
		else
			mapArea->setLPMTargetCoordVisible(true);
	}
	else {
		if(item->checkState(0) == 0)
			un_checkAllTreeElements(Qt::Unchecked);
		else
			un_checkAllTreeElements(Qt::Checked);
	}
}

void KSonarsController::un_checkAllTreeElements(Qt::CheckState state) {
	QTreeWidgetItem *item;

	for(int i = 0 ; i < ui->checkTree->itemAt(0, 0)->childCount() ; i++) {
		item = ui->checkTree->itemAt(0, 0)->child(i);
		item->setCheckState(0, state);
	}

	ui->checkTree->itemAt(0, 0)->setCheckState(0, state);
}

void KSonarsController::resizeEvent(QResizeEvent *event) {

	if(ui->graphicsView->width() > ui->graphicsView->height())
		mapArea->resizeMapScene(ui->graphicsView->height() - 10);
	else
		mapArea->resizeMapScene(ui->graphicsView->width() - 10);
}

void KSonarsController::gridInfoUpdateHandler (GridInfo gim, QString hostId) {
	if(mapArea->cellsOfRadius != gim.cellsradius() || mapArea->cellsOfRing != gim.cellsring() || mapArea->pathLength != gim.pathlength()){
		mapArea->setupGrid(gim.cellsradius(), gim.cellsring(), gim.realgridlength(), gim.pathlength());
	}
	for (int ring = 0; ring < mapArea->cellsOfRadius; ring++) {
		for (int sector = 0; sector < mapArea->cellsOfRing; sector++) {
			mapArea->PolarGrid[ring][sector] = gim.gridcells (ring * gim.cellsring() + sector);
		}
	}
	if(gim.has_targetring()){
		mapArea->targetRing = gim.targetring ();
		mapArea->targetCell = gim.targetsector ();
		mapArea->targetOrient = gim.targetorientation ();
	}else{
		mapArea->targetRing = -1;
		mapArea->targetCell = -1;
		mapArea->targetOrient = -1;
	}
	if(gim.pathstepsring_size() != 0){
		for (int step = 0; step < gim.pathstepsring_size(); step++) {
			mapArea->pathR[step] = gim.pathstepsring (step);
			mapArea->pathS[step] = gim.pathstepssector (step);
			mapArea->pathO[step] = gim.pathstepsorientation (step);
		}
	}

	mapArea->totalVisits = gim.visitedring_size();
	if(gim.visitedring_size() != 0){
		for (int step = 0; step < gim.visitedring_size(); step++) {
			mapArea->pathR2[step] = gim.visitedring (step);
			mapArea->pathS2[step] = gim.visitedsector (step);
			mapArea->pathO2[step] = gim.visitedorientation (step);
		}
	}
	if (mapArea->getLPMObstaclesVisible() ) {
		mapArea->setPMObstaclesVisible (false);
		mapArea->updateObstacles (false);
		mapArea->updateArrow();
		mapArea->setPMObstaclesVisible(true);
	}

	if(mapArea->getLPMTargetCoordVisible()) {
		mapArea->setPMTargetCoordVisible(false);
		mapArea->updateTargetCoordinates();
		mapArea->setPMTargetCoordVisible(true);
	}

	if(mapArea->getLPMPathVisible()) {
		mapArea->setPMPathVisible(false);
		mapArea->updatePath();
		mapArea->setPMPathVisible(true);
	}
}

void KSonarsController::changeToHost(QString data1) {
	availableSonarsHosts->changeItem(data1);
}

void KSonarsController::addComboBoxItem(QString data1, QString data2) {
	availableSonarsHosts->addComboBoxItem(data1, data2);
}

void KSonarsController::removeComboBoxItem(QString data1) {
	availableSonarsHosts->removeComboBoxItem(data1);
}

void KSonarsController::setGameStateInfo(QString data1, QString data2, QString data3) {
	availableSonarsHosts->setGameStateInfo(data1, data2, data3);
}

void KSonarsController::SubscriptionHandler(QString data1) {
	un_checkAllTreeElements(Qt::Unchecked);
	mapArea->resetKMapScene();
	emit SubscriptionRequest(data1);
}

void KSonarsController::changeEvent(QEvent *e) {
	QWidget::changeEvent(e);

	switch(e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

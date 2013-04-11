#include "GlobalWorldController.h"
#include "ui_GlobalWorldController.h"

using namespace std;

GlobalWorldController::GlobalWorldController(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::GlobalWorldController) {
	
	ui->setupUi(this);
	GWRequests.clear();
	this->paintArea = new KFieldScene(ui->graphicsView);
	ui->graphicsView->setScene(paintArea);
	ui->graphicsView->setResizeAnchor(QGraphicsView::NoAnchor);
	ui->graphicsView->setTransformationAnchor(QGraphicsView::NoAnchor);
	paintArea->resizeFieldScene((ui->graphicsView->height() - 40) * 1.37, ui->graphicsView->height() - 40);
}

GlobalWorldController::~GlobalWorldController() {
	delete ui;
}

void GlobalWorldController::resizeEvent(QResizeEvent *event) {
	//Keep the ratio
	float width =(ui->graphicsView->width() - 40);
	float height =(ui->graphicsView->width() - 40) / 1.5;

	if(height > ui->graphicsView->height()) {
		width =(ui->graphicsView->height() - 40) * 1.5;
		height =(ui->graphicsView->height() - 40);
	}

	paintArea->resizeFieldScene(width, height);
}

void GlobalWorldController::setKGFCGameStateInfo(GameStateMessage gsm, QString host) {
	GraphicalRobotElement *element = paintArea->findGraphicalRobotItem(host);

	if(element != NULL)
		element->setCurrentGSM(gsm);
}

void GlobalWorldController::worldInfoUpdateHandler(WorldInfo nwim, QString host) {
	GraphicalRobotElement *element = paintArea->findGraphicalRobotItem(host);

	if(element != NULL) {
		element->setCurrentWIM(nwim);

		if(element->getGWSRobotVisible()) {
			element->setRobotVisible(false);
			element->updateRobotRect();
			element->setRobotVisible(true);
		}

		if(element->getGWSBallVisible()) {
			element->setBallVisible(false);
			element->updateBallRect();
			element->setBallVisible(true);
		}

		if(element->getGWSUnionistLineVisible()) {
			element->setUnionistLineVisible(false);
			element->updateUnionistLineRect();
			element->setUnionistLineVisible(true);
		}
	} 
	else {
		//std::cout << "[67]GlobalWorldController::worldInfoUpdateHandler:: Host hasn't been requested!" << host.toStdString() <<std::endl;
	}
}

void GlobalWorldController::GWSGVRobotVisible(QString host, bool visible) {
	GraphicalRobotElement *robotElement = NULL;
	robotElement = paintArea->findGraphicalRobotItem(host);

	if(robotElement == NULL)
		robotElement = paintArea->newGraphicalRobotItem(host);

	if(robotElement != NULL) {
		robotElement->setGWSRobotVisible(visible);
		GWSGVUnionistLineVisible(robotElement);
	} 
	else {
		//std::cout << "[83] GlobalWorldController::GWSGVRobotVisible : Fatal !" << std::endl;
	}
}

void GlobalWorldController::GWSGVBallVisible(QString host, bool visible) {
	GraphicalRobotElement *robotElement = NULL;
	robotElement = paintArea->findGraphicalRobotItem(host);

	if(robotElement == NULL)
		robotElement = paintArea->newGraphicalRobotItem(host);

	if(robotElement != NULL) {
		robotElement->setGWSBallVisible(visible);
		GWSGVUnionistLineVisible(robotElement);
		//paintArea->printRobotList();
	} 
	else
		std::cout << "[99] GlobalWorldController::GWSGVBallVisible : Fatal !" << std::endl;
}

void GlobalWorldController::GWSGVUnionistLineVisible(GraphicalRobotElement *robotElement) {
	if(robotElement->getGWSRobotVisible() && robotElement->getGWSBallVisible())
		robotElement->setGWSUnionistLineVisible(true);
	else
		robotElement->setGWSUnionistLineVisible(false);
}

void GlobalWorldController::removeOldItem(QString hostId) {
	QTreeWidgetItem *item = GWhostFinder(hostId);

	if(item != NULL) {
		QCheckBox *checkBox = (QCheckBox*)ui->hostTree->itemWidget(item, 0);

		for(int i = 0 ; i < GWRequests.count() ; i++) {
			if(GWRequests.at(i)->hostId == checkBox->objectName())
				GWRequests.removeAt(i);
		}

		if(item->child(1) != NULL)
			delete(item->child(1));

		if(item->child(0) != NULL)
			delete(item->child(0));

		delete(item);
	}

	paintArea->removeGraphicalRobotItem(hostId);
}

QTreeWidgetItem *GlobalWorldController::GWhostFinder(QString hostId) {
	QTreeWidgetItem *item;
	QCheckBox *checkbox;
	bool alreadyListed = false;

	for(int i = 0 ; i < ui->hostTree->topLevelItemCount() ; i++) {
		item = ui->hostTree->topLevelItem(i);
		checkbox = (QCheckBox*)ui->hostTree->itemWidget(item, 0);

		if(checkbox->objectName() == hostId) {
			alreadyListed = true;
			break;
		}
	}

	if(alreadyListed)
		return item;
	else
		return NULL;
}

void GlobalWorldController::addNewItem(QString hostId, QString hostName) {
	QTreeWidgetItem *item;
	QTreeWidgetItem *subItem1;
	QTreeWidgetItem *subItem2;
	int position = ui->hostTree->topLevelItemCount();
	QCheckBox *checkBox;
	QCheckBox *subCheckBox1;
	QCheckBox *subCheckBox2;
	QIcon icon;
	QFont font;
	icon.addFile(QString::fromUtf8(":/KnownHostsIcons/chore_robot_on.png"), QSize(), QIcon::Normal, QIcon::Off);
	font.setPointSize(24);
	item = new QTreeWidgetItem(ui->hostTree);
	subItem1 = new QTreeWidgetItem(item);
	subItem2 = new QTreeWidgetItem(item);
	checkBox = new QCheckBox();
	checkBox->setIcon(icon);
	checkBox->setIconSize(QSize(48, 48));
	checkBox->setTristate(false);
	checkBox->setObjectName(hostId);
	checkBox->setText(hostName);
	subCheckBox1 = new QCheckBox();
	subCheckBox1->setTristate(false);
	subCheckBox1->setObjectName(QString("position,") + hostId);
	subCheckBox1->setText(tr("Estimated Robot Pose"));
	subCheckBox2 = new QCheckBox();
	subCheckBox2->setTristate(false);
	subCheckBox2->setObjectName(tr("ball,") + hostId);
	subCheckBox2->setText(tr("Estimated Ball Position"));
	ui->hostTree->setItemWidget(ui->hostTree->topLevelItem(position), 0, checkBox);
	ui->hostTree->setItemWidget(ui->hostTree->topLevelItem(position)->child(0), 0, subCheckBox1);
	ui->hostTree->setItemWidget(ui->hostTree->topLevelItem(position)->child(1), 0, subCheckBox2);
	connect(checkBox, SIGNAL(stateChanged(int)), this, SLOT(mainCheckBoxHandler(int)) );
	connect(subCheckBox1, SIGNAL(stateChanged(int)), this, SLOT(subCheckBox1Handler(int)) );
	connect(subCheckBox2, SIGNAL(stateChanged(int)), this, SLOT(subCheckBox2Handler(int)) );
	requestedElements *re = new Elements(hostId, hostName, false, false);
	GWRequests.append(re);
}

void GlobalWorldController::mainCheckBoxHandler(int state) {
	QTreeWidgetItem *subItem = NULL;
	QCheckBox *subCheckBox = NULL;
	QCheckBox *checkBox =(QCheckBox *) this->sender();
	QTreeWidgetItem *item = this->GWhostFinder(checkBox->objectName());

	for(int i = 0 ; i < 2 ; i++) {
		subItem = item->child(i);
		subCheckBox = (QCheckBox*)ui->hostTree->itemWidget(subItem, 0);

		if(state == 0)
			subCheckBox->setChecked(false);
		else
			subCheckBox->setChecked(true);
	}
}

void GlobalWorldController::subCheckBox1Handler(int state) {
	QCheckBox *checkBox = (QCheckBox*)this->sender();
	QStringList list = checkBox->objectName().split(",");

	for(int i = 0 ; i < GWRequests.count() ; i++) {
		if(GWRequests.at(i)->hostId == list.at(1)) {
			if(state == 0) {
				GWRequests.at(i)->setPositionRequest(false);
				GWSGVRobotVisible(GWRequests.at(i)->hostId, false);

				if(!(GWRequests.at(i)->requestedBall))
					emit GWRHUnsubscriptionRequest(GWRequests.at(i)->hostId);
			} 
			else {
				GWRequests.at(i)->setPositionRequest(true);
				emit GWRHSubscriptionRequest(GWRequests.at(i)->hostId);
				GWSGVRobotVisible(GWRequests.at(i)->hostId, true);
			}
			break;
		}
	}
}

void GlobalWorldController::subCheckBox2Handler(int state) {
	QCheckBox *checkBox = (QCheckBox*)this->sender();
	QStringList list = checkBox->objectName().split(",");

	for(int i = 0 ; i < GWRequests.count() ; i++) {
		if(GWRequests.at(i)->hostId == list.at(1)) {
			if(state == 0) {
				GWRequests.at(i)->setBallRequest(false);
				GWSGVBallVisible(GWRequests.at(i)->hostId, false);

				if(!(GWRequests.at(i)->requestedPosition))
					emit GWRHUnsubscriptionRequest(GWRequests.at(i)->hostId);
			} 
			else {
				GWRequests.at(i)->setBallRequest(true);
				emit GWRHSubscriptionRequest(GWRequests.at(i)->hostId);
				GWSGVBallVisible(GWRequests.at(i)->hostId, true);
			}
			break;
		}
	}
}

void GlobalWorldController::updateGameState(QString iconPath, QString stateMSG, QString hostId) {
	QTreeWidgetItem *item;
	QCheckBox *checkBox;
	QIcon icon;
	item = GWhostFinder(hostId);

	if(item != NULL) {
		checkBox = (QCheckBox*)ui->hostTree->itemWidget(item, 0);
		icon.addFile(iconPath, QSize(), QIcon::Normal, QIcon::Off);
		checkBox->setIcon(icon);
		checkBox->setIconSize(QSize(48, 48));
		// find hostname and give it to me ...
		QString gsmQString = GWhostNameFinder(hostId) + stateMSG;
		checkBox->setText(gsmQString) ;
	}
}

QString GlobalWorldController::GWhostNameFinder(QString hostId) {
	QString hostname = "";

	for(int i = 0 ; i < GWRequests.count() ; i++) {
		if(GWRequests.at(i)->hostId == hostId)
			hostname = GWRequests.at(i)->hostName;
	}

	return hostname;
}


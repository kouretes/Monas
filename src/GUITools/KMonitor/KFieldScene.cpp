#include "KFieldScene.h"

#include <QtSvg/QGraphicsSvgItem>
#include <QGraphicsItem>
#include <QString>
#include <QFile>
#include <math.h>

#include "core/architecture/configurator/Configurator.hpp"

using namespace std;

KFieldScene::KFieldScene(QGraphicsView *parent) {

	this->parent = parent;
	this->RobotList.clear();
	
	loadXMLConfig();
	setBackgroundBrush(QBrush(QColor(0, 155, 0)));
	QPen penForWhiteLine(Qt::white);
	penForWhiteLine.setWidth(4);
	QPen penForYellowPost(Qt::yellow);
	penForYellowPost.setWidth(6);
	QPen penForRobotDirection(Qt::black);
	penForRobotDirection.setWidth(3);
	
	LSide = addRect(QRect(), penForWhiteLine, QBrush(Qt::transparent));
	RSide = addRect(QRect(), penForWhiteLine, QBrush(Qt::transparent));
	LSmallArea = addRect(QRect(), penForWhiteLine, QBrush(Qt::transparent));
	RSmallArea = addRect(QRect(), penForWhiteLine, QBrush(Qt::transparent));
	LGoalArea = addRect(QRect(), penForWhiteLine, QBrush(Qt::transparent));
	RGoalArea = addRect(QRect(), penForWhiteLine, QBrush(Qt::transparent));
	CCircle = addEllipse(QRect(), penForWhiteLine, QBrush(Qt::transparent));
	LCrossHPart = addLine(QLineF(), penForWhiteLine);
	LCrossVPart = addLine(QLineF(), penForWhiteLine);
	RCrossHPart = addLine(QLineF(), penForWhiteLine);
	RCrossVPart = addLine(QLineF(), penForWhiteLine);
	CCrossHPart = addLine(QLineF(), penForWhiteLine);
	LTPostCircle = addEllipse(QRect(), QPen(Qt::yellow), QBrush(Qt::yellow));
	LBPostCircle = addEllipse(QRect(), QPen(Qt::yellow), QBrush(Qt::yellow));
	RTPostCircle = addEllipse(QRect(), QPen(Qt::yellow), QBrush(Qt::yellow));
	RBPostCircle = addEllipse(QRect(), QPen(Qt::yellow), QBrush(Qt::yellow));
	LPostVPart = addLine(QLineF(), penForYellowPost);
	RPostVPart = addLine(QLineF(), penForYellowPost);
	setSvgItems();
	this->addItem(LTPost);
	this->addItem(LBPost);
	this->addItem(RTPost);
	this->addItem(RBPost);
	
	formationLabel = new QLabel("Ball Position:\nX:\nY:\n");
	this->addWidget(formationLabel);
	formationLabel->setStyleSheet("border: 2px solid #ffffff");
	formationLabel->adjustSize();
	QPalette pal = formationLabel->palette();
	pal.setColor(formationLabel->backgroundRole(), QColor(0,155,0));
	pal.setColor(formationLabel->foregroundRole(), Qt::black);
	formationLabel->setPalette(pal);
	formationLabel->setVisible(false);
}

KFieldScene::~KFieldScene() { }

GraphicalRobotElement *KFieldScene::newGraphicalRobotItem(QString hostId) {
	GraphicalRobotElement *robot = new GraphicalRobotElement(this, hostId);
	RobotList.append(robot);
	return robot;
}

void KFieldScene::removeGraphicalRobotItem(QString hostId) {
	GraphicalRobotElement *robot = findGraphicalRobotItem(hostId);

	if(robot != NULL) {
		for(int i = 0 ; i < RobotList.count() ; i++) {
			if(RobotList.at(i)->getHostId() == hostId) {
				RobotList.removeAt(i);
				break;
			}
		}

		delete robot;
	}
}

GraphicalRobotElement *KFieldScene::findGraphicalRobotItem(QString hostId) {
	bool exists = false;
	GraphicalRobotElement *robot = NULL;

	for(int i = 0 ; i < RobotList.count() ; i++) {
		if(RobotList.at(i)->getHostId() == hostId) {
			exists = true;
			robot = RobotList.at(i);
			break;
		}
	}

	if(exists)
		return robot;
	else
		return NULL;
}

void KFieldScene::printRobotList() {
	std::cout << "~~~~~~~~KFieldScene::printRobotList()~~~~~~~~" << std::endl;

	for(int i = 0 ; i < RobotList.count() ; i++) {
		std::cout << "Host :: " << RobotList.at(i)->getHostId().toStdString() << std::endl
		          << "Robot :: " << RobotList.at(i)->getGWSRobotVisible() << std::endl
		          << "Ball :: " << RobotList.at(i)->getGWSBallVisible() << std::endl
		          << "unionistLine :: " << RobotList.at(i)->getGWSUnionistLineVisible() << std::endl;
	}

	std::cout << "~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
}

void KFieldScene::loadXMLConfig() {

	float CarpetMaxX = atof(Configurator::Instance().findValueForKey("field.CarpetMaxX").c_str());
	float CarpetMinX = atof(Configurator::Instance().findValueForKey("field.CarpetMinX").c_str());
	float CarpetMaxY = atof(Configurator::Instance().findValueForKey("field.CarpetMaxY").c_str());
	float CarpetMinY = atof(Configurator::Instance().findValueForKey("field.CarpetMinY").c_str());
	float FieldMaxX = atof(Configurator::Instance().findValueForKey("field.FieldMaxX").c_str());
	float FieldMaxY = atof(Configurator::Instance().findValueForKey("field.FieldMaxY").c_str());
	float LeftPenaltyAreaMaxY = atof(Configurator::Instance().findValueForKey("field.LeftPenaltyAreaMaxY").c_str());
	float LeftPenaltyAreaMinY = atof(Configurator::Instance().findValueForKey("field.LeftPenaltyAreaMinY").c_str());
	float RightPenaltyAreaMaxX = atof(Configurator::Instance().findValueForKey("field.RightPenaltyAreaMaxX").c_str());
	float RightPenaltyAreaMinX = atof(Configurator::Instance().findValueForKey("field.RightPenaltyAreaMinX").c_str());
	float LeftGoalAreaMaxX = atof(Configurator::Instance().findValueForKey("field.LeftGoalAreaMaxX").c_str());
	float LeftGoalAreaMinX = atof(Configurator::Instance().findValueForKey("field.LeftGoalAreaMinX").c_str());
	float LeftGoalAreaMaxY = atof(Configurator::Instance().findValueForKey("field.LeftGoalAreaMaxY").c_str());
	float LeftGoalAreaMinY = atof(Configurator::Instance().findValueForKey("field.LeftGoalAreaMinY").c_str());
	float RightPenaltyMarkX = atof(Configurator::Instance().findValueForKey("field.RightPenaltyMarkX").c_str());
	float RightPenaltyMarkSize = atof(Configurator::Instance().findValueForKey("field.RightPenaltyMarkSize").c_str());
	float DiameterVPost = atof(Configurator::Instance().findValueForKey("field.DiameterVPost").c_str());
	float DiameterCCircle = atof(Configurator::Instance().findValueForKey("field.DiameterCCircle").c_str());
	
	float LSmallArea = 0.0f;
	float HSmallArea = 0.0f;
	float LGoalArea = 0.0f;
	float HGoalArea = 0.0f;
	float LCDistance = 0.0f;
	
	
	
	LSmallArea =(RightPenaltyAreaMaxX - RightPenaltyAreaMinX) * 1000;
	HSmallArea =(LeftPenaltyAreaMaxY - LeftPenaltyAreaMinY ) * 1000;
	LGoalArea =(LeftGoalAreaMaxX - LeftGoalAreaMinX ) * 1000;
	HGoalArea =(LeftGoalAreaMaxY - LeftGoalAreaMinY ) * 1000;
	LCDistance =(RightPenaltyAreaMaxX - RightPenaltyMarkX ) * 1000;
	DiameterVPost *= 1000;
	DiameterCCircle *= 1000;
	config.totalCarpetAreaWidth =(CarpetMaxX - CarpetMinX) * 1000;
	config.totalCarpetAreaHeight =(CarpetMaxY - CarpetMinY) * 1000;
	config.totalCarpetAreaMean =(config.totalCarpetAreaWidth + config.totalCarpetAreaHeight) / 2;
	config.xCentre = CarpetMaxX * 1000;
	config.yCentre = CarpetMaxY * 1000;
	config.xLLine =(CarpetMaxX - FieldMaxX) * 1000;
	config.yTLine =(CarpetMaxY - FieldMaxY) * 1000;
	config.xRLine = config.totalCarpetAreaWidth - config.xLLine;
	config.yBLine = config.totalCarpetAreaHeight - config.yTLine;
	config.xLSmallArea =(config.xLLine + LSmallArea);
	config.yTSmallArea = config.yTLine +((config.yBLine - config.yTLine - HSmallArea) / 2);
	config.xRSmallArea = config.xRLine - LSmallArea;
	config.yBSmallArea = config.yTSmallArea + HSmallArea ;
	config.xLGoalArea = config.xLLine - LGoalArea;
	config.yTGoalArea = config.yTSmallArea +(HSmallArea - HGoalArea) / 2;
	config.xRGoalArea = config.xRLine + LGoalArea;
	config.yBGoalArea = config.yTGoalArea + HGoalArea;
	config.wQCross = RightPenaltyMarkSize * 1000 / 4;
	config.xLCross = config.xLLine + LCDistance;
	config.xRCross = config.xRLine - LCDistance;
	config.radPost = DiameterVPost / 2;
	config.radCenterCircle = DiameterCCircle / 2;
}

void KFieldScene::setSvgItems() {

	QString path1  = ":/Posts3D/LBPost.svg";
	QString path2 = ":/Posts3D/LTPost.svg";
	QString path3 = ":/Posts3D/RTPost.svg";
	QString path4 = ":/Posts3D/RBPost.svg";

	if(!path1.isEmpty() && !path2.isEmpty() && !path3.isEmpty() && !path4.isEmpty()) {
		QFile file1(path1);
		QFile file2(path2);
		QFile file3(path3);
		QFile file4(path4);

		if(!file1.exists() || !file2.exists() || !file3.exists() || !file4.exists())
			return;

		LTPost = new QGraphicsSvgItem(file2.fileName());
		LBPost = new QGraphicsSvgItem(file1.fileName());
		RTPost = new QGraphicsSvgItem(file3.fileName());
		RBPost = new QGraphicsSvgItem(file4.fileName());
		LTPost->setScale(0.043);
		LBPost->setScale(0.043);
		RTPost->setScale(0.043);
		RBPost->setScale(0.043);
	}
}

void KFieldScene::resizeFieldScene(int width, int height) {

	float wCentre = width * config.xCentre / config.totalCarpetAreaWidth;
	float hCentre = height * config.yCentre / config.totalCarpetAreaHeight;
	float wLLine = width * config.xLLine / config.totalCarpetAreaWidth;
	float hTLine = height * config.yTLine / config.totalCarpetAreaHeight;
	float wRLine = width * config.xRLine / config.totalCarpetAreaWidth;
	float hBLine = height * config.yBLine / config.totalCarpetAreaHeight;
	float wLSmallArea = width * config.xLSmallArea / config.totalCarpetAreaWidth;
	float hTSmallArea = height * config.yTSmallArea / config.totalCarpetAreaHeight;
	float wRSmallArea = width * config.xRSmallArea / config.totalCarpetAreaWidth;
	float hBSmallArea = height * config.yBSmallArea / config.totalCarpetAreaHeight;
	float wLGoalArea = width * config.xLGoalArea / config.totalCarpetAreaWidth;
	float hTGoalArea = height * config.yTGoalArea / config.totalCarpetAreaHeight;
	float wRGoalArea = width * config.xRGoalArea / config.totalCarpetAreaWidth;
	float hBGoalArea = height * config.yBGoalArea / config.totalCarpetAreaHeight;
	float wCenterCross = width * config.wQCross / config.totalCarpetAreaWidth;
	float wLeftCross = width * config.xLCross / config.totalCarpetAreaWidth;
	float wRightCross = width * config.xRCross / config.totalCarpetAreaWidth;
	float radKickOffCircle =(height + width) / 2*config.radCenterCircle / config.totalCarpetAreaMean;
	float radPost =(height + width) / 2*config.radPost / config.totalCarpetAreaMean;
	
	setSceneRect(0, 0, width, height);
	
	LSide->setRect(wLLine, hTLine, wCentre - wLLine, hBLine - hTLine);
	RSide->setRect(wCentre, hTLine, wRLine - wCentre, hBLine - hTLine);
	LSmallArea->setRect(wLLine, hTSmallArea, wLSmallArea - wLLine, hBSmallArea - hTSmallArea);
	RSmallArea->setRect(wRSmallArea, hTSmallArea, wRLine - wRSmallArea, hBSmallArea - hTSmallArea);
	LGoalArea->setRect(wLGoalArea, hTGoalArea, wLLine - wLGoalArea, hBGoalArea - hTGoalArea);
	RGoalArea->setRect(wRLine, hTGoalArea, wRGoalArea - wRLine, hBGoalArea - hTGoalArea);
	CCircle->setRect(wCentre - radKickOffCircle, hCentre - radKickOffCircle, radKickOffCircle*2, radKickOffCircle*2);
	LCrossHPart->setLine(wLeftCross - wCenterCross, hCentre, wLeftCross + wCenterCross, hCentre);
	LCrossVPart->setLine(wLeftCross, hCentre - wCenterCross, wLeftCross, hCentre + wCenterCross);
	RCrossHPart->setLine(wRightCross - wCenterCross, hCentre, wRightCross + wCenterCross, hCentre);
	RCrossVPart->setLine(wRightCross, hCentre - wCenterCross, wRightCross, hCentre + wCenterCross);
	CCrossHPart->setLine(wCentre - wCenterCross, hCentre, wCentre + wCenterCross, hCentre);
	LTPostCircle->setRect(wLLine - radPost, hTGoalArea - radPost, radPost*2, radPost*2);
	LBPostCircle->setRect(wLLine - radPost, hBGoalArea - radPost, radPost*2, radPost*2);
	RTPostCircle->setRect(wRLine - radPost, hTGoalArea - radPost, radPost*2, radPost*2);
	RBPostCircle->setRect(wRLine - radPost, hBGoalArea - radPost, radPost*2, radPost*2);
	LPostVPart->setLine(wLLine - 15, hTGoalArea, wLLine - 15, hBGoalArea );
	RPostVPart->setLine(wRLine + 15, hTGoalArea, wRLine + 15, hBGoalArea );
	LBPost->setPos(wLLine - 24, hBGoalArea - 16);
	LTPost->setPos(wLLine - 24, hTGoalArea - 22);
	RTPost->setPos(wRLine - 6, hTGoalArea - 22);
	RBPost->setPos(wRLine - 6, hBGoalArea - 16);
	formationLabel->move(wRLine + 25, -15);
}

QRectF KFieldScene::rectFromFC(float xMiddle, float yMiddle, float width, float height) {
	return QRectF(
	           sceneRect().width() *(config.xCentre - width / 2 + xMiddle) / config.totalCarpetAreaWidth,
	           sceneRect().height() - sceneRect().height() *(config.yCentre + height / 2 + yMiddle) / config.totalCarpetAreaHeight,
	           sceneRect().width() * width / config.totalCarpetAreaWidth,
	           sceneRect().height() * height / config.totalCarpetAreaHeight);
}

QLineF KFieldScene::lineFromFCA(float x, float y, float degAngle, float size) {
	return QLineF(
	           sceneRect().width() *(config.xCentre + x) / config.totalCarpetAreaWidth,
	           sceneRect().height() - sceneRect().height() *(config.yCentre + y) / config.totalCarpetAreaHeight,
	           sceneRect().width() *(config.xCentre + x) / config.totalCarpetAreaWidth + sceneRect().width() * size * cos(degAngle) / config.totalCarpetAreaWidth,
	           sceneRect().height() -(sceneRect().height() *(config.yCentre + y) / config.totalCarpetAreaHeight + sceneRect().height() * size * sin(degAngle) / config.totalCarpetAreaHeight));
}

QPointF KFieldScene::pointFromFC(float x, float y){
    
    return QPointF(
       sceneRect().width() *(config.xCentre + x) / config.totalCarpetAreaWidth,
	   sceneRect().height() - sceneRect().height() *(config.yCentre + y) / config.totalCarpetAreaHeight);
}

QRectF KFieldScene::ballRectFromFC(WorldInfo *wim, float width, float height) {
	float a = cos(wim->myposition().phi());
	float b = sin(wim->myposition().phi());
	float xMiddle =(wim->myposition().x() + wim->balls(0).relativex()*a - wim->balls(0).relativey()*b) * 1000;
	float yMiddle =(wim->myposition().y() + wim->balls(0).relativex()*b + wim->balls(0).relativey()*a) * 1000;
	return QRectF(
	           sceneRect().width() *(config.xCentre - width / 2 + xMiddle) / config.totalCarpetAreaWidth,
	           sceneRect().height() - sceneRect().height() *(config.yCentre + height / 2 + yMiddle) / config.totalCarpetAreaHeight,
	           sceneRect().width() * width / config.totalCarpetAreaWidth,
	           sceneRect().height() * height / config.totalCarpetAreaHeight);
}

QLineF KFieldScene::unionistLineRectFromFC(WorldInfo *wim) {
	float x = wim->myposition().x()*1000;
	float y = wim->myposition().y()*1000;
	float a = cos(wim->myposition().phi());
	float b = sin(wim->myposition().phi());
	float xMiddle =(wim->myposition().x() + wim->balls(0).relativex()*a - wim->balls(0).relativey()*b) * 1000;
	float yMiddle =(wim->myposition().y() + wim->balls(0).relativex()*b + wim->balls(0).relativey()*a) * 1000;
	return lineRectFromFC(x, y, xMiddle, yMiddle);
}

QLineF KFieldScene::unionistLineRectFromFC(WorldInfo *wim, SharedWorldInfo *swim) {
	float x = wim->myposition().x() * 1000;
	float y = wim->myposition().y() * 1000;
	float xMiddle = swim->globalballs(0).x() * 1000;
	float yMiddle = swim->globalballs(0).y() * 1000;
	return lineRectFromFC(x, y, xMiddle, yMiddle);
}

QLineF KFieldScene::lineRectFromFC(float x1, float y1, float x2, float y2) {
	return QLineF(
	           sceneRect().width() *(config.xCentre + x1) / config.totalCarpetAreaWidth,
	           sceneRect().height() - sceneRect().height() *(config.yCentre + y1) / config.totalCarpetAreaHeight,
	           sceneRect().width() *(config.xCentre + x2) / config.totalCarpetAreaWidth,
	           sceneRect().height() - sceneRect().height() *(config.yCentre + y2) / config.totalCarpetAreaHeight);
}


QRectF KFieldScene::visionBallRect(BallObject bob, WorldInfo wim) {
	float xmiddle =(wim.myposition().x() + bob.dist()*cos((wim.myposition().phi() + bob.bearing())) )*1000;
	float ymiddle =(wim.myposition().y() + bob.dist()*sin((wim.myposition().phi() + bob.bearing())) )*1000;
	return rectFromFC( xmiddle, ymiddle, 75, 75);
}

QRectF KFieldScene::goalPostRectFromOBM(NamedObject *nob, WorldInfo *wim) {
	float xmiddle =(wim->myposition().x() + nob->distance()*cos((wim->myposition().phi() + nob->bearing())) )*1000;
	float ymiddle =(wim->myposition().y() + nob->distance()*sin((wim->myposition().phi() + nob->bearing())) )*1000;
	return rectFromFC(xmiddle, ymiddle, 125, 125);
}

QLineF KFieldScene::motionCmdRectFromFC(WorldInfo *wim, float cx, float cy) {
	float a = cos(wim->myposition().phi());
	float b = sin(wim->myposition().phi());
	float xMiddle =(wim->myposition().x() + cx*a - cy*b)*1000;
	float yMiddle =(wim->myposition().y() + cx*b + cy*a)*1000;
	return lineRectFromFC(wim->myposition().x()*1000, wim->myposition().y()*1000, xMiddle, yMiddle);
}


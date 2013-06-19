#include "KMapScene.h"

#include <QGraphicsPolygonItem>

#include <iostream>
using namespace std;


KMapScene::KMapScene(QGraphicsView *parent) {
	this->parent = parent;
	cellsList.clear();
	pathLineList.clear();
	pathEllipseList.clear();
	pathSmallLineList.clear();
	LPMPathVisible = false;
	LPMObstaclesVisible = false;
	LPMTargetCoordVisible = false;

	QPen penForBlackLine (Qt::black);
	penForBlackLine.setWidth (3);

	cellsOfRadius = 0;
	cellsOfRing = 0;
	moveStepInMeters = 0;
	turnStepInRads = 0;
	pathLength = 0;

	arrowBody = addLine (QLineF(), penForBlackLine);
	arrowLside = addLine (QLineF(), penForBlackLine);
	arrowRside = addLine (QLineF(), penForBlackLine);
	//arrowBody->setVisible (true);
	//arrowLside->setVisible (true);
	//arrowRside->setVisible (true);

	pathR = NULL;
	pathS = NULL;
	pathO = NULL;
	cellCenterX = NULL;
	cellCenterY = NULL;
	PolarGrid = NULL;
	gridImgH = NULL;
	gridImgV = NULL;
	targetBall = NULL;
	targetLine = NULL;
	
	setBackgroundBrush (QBrush (QColor (0, 155, 0) ) );
	
	QPen penForGreenLine (Qt::green);
	penForGreenLine.setWidth (2);
}

KMapScene::~KMapScene() {
}

void KMapScene::setupGrid(int cellsRad, int cellsRing, float gridLength, int pathSize){
	cellsOfRadius = cellsRad;
	cellsOfRing = cellsRing;
	
	//864 is a refered number of pixel, dont mint why just use it 
	moveStepInMeters = (((float)ImgSize/864))/(float)cellsOfRadius;
	//turn step in meters = 2 * pi / number of cells in each circle
	turnStepInRads = 2*M_PI/(float)cellsRing;
	totalVisits = 0;
	if(pathR != NULL){
		delete pathR;
		delete pathS;
		delete pathO;
		delete pathR2;
		delete pathS2;
		delete pathO2;
		
		for (int r = 0; r < cellsOfRadius; r++){
			delete PolarGrid[r];
			delete cellCenterY[r];
			delete cellCenterX[r];
		}
		delete PolarGrid;
		delete cellCenterX;
		delete cellCenterY;
		for (int r = 0; r < cellsOfRadius+1; r++){
			delete gridImgH[r];
			delete gridImgV[r];
		}
		delete gridImgV;
		delete gridImgH;
		delete targetBall;
		delete targetLine;
	}
	
	
	pathR = new int[pathSize];
	pathS = new int[pathSize];
	pathO = new int[pathSize];
	
	pathR2 = new int[2000];
	pathS2 = new int[2000];
	pathO2 = new int[2000];
	
	pathLength = pathSize;

	PolarGrid = new float*[cellsOfRadius];
	cellCenterX = new float*[cellsOfRadius];
	cellCenterY = new float*[cellsOfRadius];
	for (int r = 0; r < cellsOfRadius; r++){
		PolarGrid[r] = new float[cellsOfRing];
		cellCenterX[r] = new float[cellsOfRing];
		cellCenterY[r] = new float[cellsOfRing];
	}
	
	gridImgH = new int*[cellsOfRadius+1];
	gridImgV = new int*[cellsOfRadius+1];
	for (int r = 0; r < cellsOfRadius+1; r++){
		gridImgH[r] = new int[cellsOfRing];
		gridImgV[r] = new int[cellsOfRing];
	}
	
	for(int i = 0; i < cellsList.size(); i++){
		delete cellsList.at(i);
	}
	for(int i = 0; i < pathLineList.size(); i++){
		delete pathLineList.at(i);
	}
	for(int i = 0; i < pathEllipseList.size(); i++){
		delete pathEllipseList.at(i);
	}
	for(int i = 0; i < pathSmallLineList.size(); i++){
		delete pathSmallLineList.at(i);
	}
	for(int i = 0; i < visitedEllipseList.size(); i++){
		delete visitedEllipseList.at(i);
	}
	for(int i = 0; i < visitedSmallLineList.size(); i++){
		delete visitedSmallLineList.at(i);
	}
	
	cellsList.clear();
	pathLineList.clear();
	pathEllipseList.clear();
	pathSmallLineList.clear();
	visitedEllipseList.clear();
	visitedSmallLineList.clear();
	
	for (int r = 0; r < cellsOfRadius; r++){
		for (int s = 0; s < cellsOfRing; s++) {
			QGraphicsPolygonItem *cell = addPolygon (QPolygonF (QRectF() ), QPen (Qt::white), QBrush (Qt::transparent) );
			cellsList.append (cell);
		}
	}
	QPen penForBlueLine (Qt::blue);
	penForBlueLine.setWidth (2);
	//setup up in the background the path line and then in the foreground the robot positions
	for (int ways = 0; ways < pathLength; ways++) {
		QGraphicsLineItem *path = addLine (QLineF(), penForBlueLine);
		pathLineList.append (path);
	}
	QPen penForGreenLine (Qt::green);
	penForGreenLine.setWidth (4);
	for (int ways = 0; ways < pathLength; ways++) {
		QGraphicsEllipseItem *ellipse = addEllipse (QRect(), QPen (Qt::red), QBrush (Qt::blue) );
		pathEllipseList.append(ellipse);
		QGraphicsLineItem *smallLine = addLine (QLineF(), penForGreenLine );
		pathSmallLineList.append(smallLine);
	}
	for (int ways = 0; ways < 2000; ways++) {
		QGraphicsEllipseItem *ellipse = addEllipse (QRect(), QPen (Qt::green), QBrush (Qt::blue) );
		visitedEllipseList.append(ellipse);
	
	QPen black (Qt::black);
	black.setWidth (2);
		QGraphicsLineItem *smallLine = addLine (QLineF(), black );
		visitedSmallLineList.append(smallLine);
	}
	
	initCoordinates();
	initGrid();
	targetBall = addEllipse (QRect(), QPen (Qt::red), QBrush (Qt::red) );
	targetLine = addLine (QLineF(), penForGreenLine );
	updateObstacles(true);
}


void KMapScene::resetKMapScene() {
	setLPMObstaclesVisible(false);
	setLPMTargetCoordVisible(false);
	setLPMPathVisible(false);
}

void KMapScene::resizeMapScene (int size) {
	ImgSize = size;
	ImgShift = (ImgSize / 2);
	if(cellsOfRadius!=0){
		moveStepInMeters = (((float)ImgSize/864))/(float)cellsOfRadius;
	}

	initCoordinates();
	setSceneRect(0, 0, size, size);
	this->updateObstacles(true);
}

//initialize Polar grid
void KMapScene::initGrid() {
	for (int r = 0; r < cellsOfRadius; r++){
		for (int s = 0; s < cellsOfRing; s++){
			PolarGrid[r][s] = 0.5f;
		}
	}
}

void KMapScene::initCoordinates() {
	for (int r = 0; r < cellsOfRadius + 1; r++){
		for (int s = 0; s < cellsOfRing; s++){
			if (r == -1) {
				gridImgH[r][s] = ImgShift;
				gridImgV[r][s] = ImgShift;
			} else {
				//to movestepInmeters/2 mpenei dioti theloume na afisoume enan leuko kikllo sto kentro kai na ksekinaei to grid ligo meta opote epilegoume na einai toso
				//If you want help with this line please send me an e-mail: nikofinas@gmail.com
				gridImgH[r][s] = toGrid ( -KMath::toCartesianY ( ringToDistance (r) + moveStepInMeters/2, cellToTheta (s) - turnStepInRads/2 ));
				gridImgV[r][s] = toGrid ( -KMath::toCartesianX ( ringToDistance (r) + moveStepInMeters/2, cellToTheta (s) - turnStepInRads/2 ));
			}
		}
	}

	for (int r = 0; r < cellsOfRadius; r++){
		for (int s = 0; s < cellsOfRing; s++){
			if (r == -1) {
				cellCenterX[r][s] = 0.0;
				cellCenterY[r][s] = 0.0;
			} else {
				//To kentro kathe cell vriskete sto movestepinmeters/2 alla afou exoume idi proxorisei ta kelia kata movestep/2 gia na afisoume ton kiklo to kentro vriskete kathe movestepinmeters:D
				//If you want help with this line please send me an e-mail: nikofinas@gmail.com
				cellCenterX[r][s] = toGrid (  -KMath::toCartesianY ( ringToDistance (r) + moveStepInMeters, cellToTheta (s)/* - turnStepInRads/2*/));
				cellCenterY[r][s] = toGrid ( -KMath::toCartesianX ( ringToDistance (r) + moveStepInMeters, cellToTheta (s)/* - turnStepInRads/2*/));
			}
		}
	}
}


void KMapScene::setPMObstaclesVisible (bool visible) {
	for (int i = 0; i < cellsList.count(); i++) {
		cellsList.at (i)->setVisible (visible);
	}
	/*for(int i = 0; i < visitedEllipseList.count(); i++){
		visitedEllipseList.at(i)->setVisible (false);
	}
	for(int i = 0; i < visitedSmallLineList.count(); i++){
		visitedSmallLineList.at(i)->setVisible (false);
	}*/
}

void KMapScene::updateObstacles(bool initialization) {

	QGraphicsPolygonItem *cell;
	QVector<QPoint> curve1(0);
	int colorValue = 0, cellNum = 0;
	int r, s;

	for (r = 0; r < cellsOfRadius; r++) {
		for (s = 0; s < cellsOfRing; s++) {
			int sPlusOne = s==cellsOfRing-1 ? 0 : s+1;
			QPoint x0 ( gridImgH[r][s], gridImgV[r][s]);
			QPoint x1 ( gridImgH[r+1][s], gridImgV[r+1][s]);
			QPoint x2 ( gridImgH[r+1][sPlusOne], gridImgV[r+1][sPlusOne]);
			QPoint x3 ( gridImgH[r][sPlusOne], gridImgV[r][sPlusOne]);
			
			curve1.clear();
			curve1.append (x0);
			curve1.append (x1);
			curve1.append (x2);
			curve1.append (x3);
			colorValue = ColorMax - PolarGrid[r][s] * ColorMax;

			
			cell = cellsList.at (cellNum);

			cell->setPolygon (QPolygon (curve1) );
			cell->setBrush (QColor (colorValue, colorValue, colorValue) );

			cellNum++;
		}
	}
	
	QGraphicsEllipseItem *ellipse;
	QGraphicsLineItem *smallLine;
	QPoint toP (0, 0);
	int o;

	//setup up in the background the path line and then in the foreground the robot positions
	for (int ways = 0; ways < totalVisits; ways++) {
		r = pathR2[ways];
		s = pathS2[ways];	

		if(ways < totalVisits){
		
			ellipse = visitedEllipseList.at (ways);
			visitedEllipseList.at (ways)->setVisible(true);
			QPoint ellipseCenter;// ( cellCenterX[pathR[ways]][pathS[ways]] , cellCenterY[pathR[ways]][pathS[ways]] );
			
			if(r == 255 || s == 255){
				ellipseCenter.setX(toGrid(0));
				ellipseCenter.setY(toGrid(0));
			}else{
				ellipseCenter.setX(cellCenterX[r][s]);
				ellipseCenter.setY(cellCenterY[r][s]);
			}
			ellipse->setRect (ellipseCenter.x() - 5, ellipseCenter.y() - 5, 10, 10);
			//float color = 100.0f + 155.0f*((float)(ways+1)/(float)pathLength);
			ellipse->setBrush (QColor (Qt::transparent));
		
			int pix = 7;
			smallLine = visitedSmallLineList.at (ways);
			visitedSmallLineList.at (ways)->setVisible(true);
			int orientation = pathO2[ways];
			//orientation = orientation*2*M_PI/8;
			//cout << "Mpika " <<  toGrid (targetRing*moveStepInMeters)/2 << " " << toGrid (targetCell*moveStepInMeters)/2 << endl;
			float angle = orientation*2*M_PI/8.0f;
			int newX = -KMath::toCartesianY ( 10, angle);
			int newY = -KMath::toCartesianX ( 10, angle);
			toP.setX ( ellipseCenter.x() + newX);
			toP.setY (ellipseCenter.y() + newY );
			smallLine->setLine (ellipseCenter.x(), ellipseCenter.y(), toP.x(), toP.y() );
			//cout << ways << " " << orientation << endl;
		}
	}
}

void KMapScene::updateArrow() {
	QPoint center(ImgShift, ImgShift);
	QPoint fromP(center.x(), center.y() + ArrowOffset);
	QPoint toP(center.x(), center.y() - ArrowOffset);
	QPoint leftP(center.x() - ArrowOffset, center.y());
	QPoint rightP(center.x() + ArrowOffset, center.y());
	arrowBody->setLine(fromP.x(), fromP.y(), toP.x(), toP.y());
	arrowLside->setLine(leftP.x(), leftP.y(), toP.x(), toP.y());
	arrowRside->setLine(rightP.x(), rightP.y(), toP.x(), toP.y());
}


void KMapScene::setPMTargetCoordVisible (bool visible) {
	if(targetBall != NULL){
		targetBall->setVisible (visible);
		targetLine->setVisible (visible);
	}
}

void KMapScene::updateTargetCoordinates() {
	if(targetRing == -1){
		return;	
	}
	QPoint toP;
	QPoint ball ( cellCenterX[targetRing][targetCell] , cellCenterY[targetRing][targetCell] );
	
	targetBall->setRect(ball.x() - 5, ball.y() - 5, 10, 10);
	//targetBall->setRect(toGrid(0) - 5, toGrid(1) - 5, 10, 10);
	float angle = targetOrient;
	int newX = -KMath::toCartesianY ( 10, angle );
	int newY = -KMath::toCartesianX ( 10, angle );
	toP.setX ( ball.x() + newX);
	toP.setY (ball.y() + newY );
	targetLine->setLine (ball.x(), ball.y(), toP.x(), toP.y() );
}

void KMapScene::setPMPathVisible (bool visible) {
	for (int i = 0; i < pathLineList.count(); i++) {
		pathLineList.at (i)->setVisible (visible);
	}
	for (int i = 0; i < pathEllipseList.count(); i++) {
		pathEllipseList.at (i)->setVisible (visible);
	}
	for (int i = 0; i < pathSmallLineList.count(); i++) {
		pathSmallLineList.at (i)->setVisible (visible);
	}
}

void KMapScene::updatePath() {
	if(pathLength == 0){
		return;
	}
	QGraphicsLineItem *path;
	QGraphicsEllipseItem *ellipse;
	QGraphicsLineItem *smallLine;
	QPoint toP (0, 0), fromP (0, 0);
	int r, s, o;
	pathLineListRectReset();
	//setup up in the background the path line and then in the foreground the robot positions
	for (int ways = 0; ways < pathLength; ways++) {
		if (pathR[ways] == -1 && pathS[ways] == -1)
			break;

		r = pathR[ways];
		s = pathS[ways];	
		if(r == 255 || s == 255){
			toP.setX (toGrid(0));
			toP.setY (toGrid(0));
		}else{
			toP.setX (cellCenterX[r][s]);
			toP.setY (cellCenterY[r][s]);
		}

		if(ways == 0 || ways == 1){
			fromP.setX (toGrid(0));
			fromP.setY (toGrid(0));
		}else{
			fromP.setX (cellCenterX[pathR[ways-1]][pathS[ways-1]]);
			fromP.setY (cellCenterY[pathR[ways-1]][pathS[ways-1]]);
		}
		path = pathLineList.at (ways);
		path->setLine (fromP.x(), fromP.y(), toP.x(), toP.y() );
		if(ways < pathLength){
		
			ellipse = pathEllipseList.at (ways);
			QPoint ellipseCenter;// ( cellCenterX[pathR[ways]][pathS[ways]] , cellCenterY[pathR[ways]][pathS[ways]] );
			
			if(r == 255 || s == 255){
				ellipseCenter.setX(toGrid(0));
				ellipseCenter.setY(toGrid(0));
			}else{
				ellipseCenter.setX(cellCenterX[r][s]);
				ellipseCenter.setY(cellCenterY[r][s]);
			}
			ellipse->setRect (ellipseCenter.x() - 5, ellipseCenter.y() - 5, 10, 10);
			float color = 100.0f + 155.0f*((float)(ways+1)/(float)pathLength);
			ellipse->setBrush (QColor ((int) color, 0, 0) );
		
			int pix = 7;
			smallLine = pathSmallLineList.at (ways);
			int orientation = pathO[ways];
			//orientation = orientation*2*M_PI/8;
			//cout << "Mpika " <<  toGrid (targetRing*moveStepInMeters)/2 << " " << toGrid (targetCell*moveStepInMeters)/2 << endl;
			float angle = orientation*2*M_PI/8.0f;
			int newX = -KMath::toCartesianY ( 10, angle);
			int newY = -KMath::toCartesianX ( 10, angle);
			toP.setX ( ellipseCenter.x() + newX);
			toP.setY (ellipseCenter.y() + newY );
			smallLine->setLine (ellipseCenter.x(), ellipseCenter.y(), toP.x(), toP.y() );
			//cout << ways << " " << orientation << endl;
		}
	}
}

void KMapScene::pathLineListRectReset() {
	for(int i = 0 ; i < pathLineList.count() ; i++)
		pathLineList.at(i)->setLine(0, 0, 0, 0);
}

/*
 * Math functions
 */

float KMapScene::ringToDistance (int r) {
		return r * moveStepInMeters;
}

float KMapScene::cellToTheta (int s) {
	return KMath::wrapTo0_2Pi(s*turnStepInRads);
}

int KMapScene::toGrid (float x) {
	return int (round (ImgScale * x) + ImgShift);
}

#include "KMapScene.h"

#include <QGraphicsPolygonItem>

#include <iostream>
using namespace std;


KMapScene::KMapScene(KMapView* parent, QString hostId)
{
	this->parent = parent;
	currentHost = hostId;

	cellsList.clear();
	pathLineList.clear();

	LPMPathVisible = false;
	LPMObstaclesVisible = false;
	LPMTargetCoordVisible = false;

	QPen penForRedLine(Qt::red);
	penForRedLine.setWidth(3);

	QPen penForBlueLine(Qt::blue);
	penForBlueLine.setWidth(2);

	for (int r=0; r<TotalRings; r++)
		for (int s=0; s<N; s++)
		{
			QGraphicsPolygonItem* cell = addPolygon(QPolygonF(QRectF()),QPen(Qt::white),QBrush(Qt::transparent));
			cellsList.append(cell);
		}

	arrowBody = addLine(QLineF(),penForRedLine);
	arrowLside = addLine(QLineF(),penForRedLine);
	arrowRside = addLine(QLineF(),penForRedLine);

	for (int ways=0; ways<PathLength; ways++)
	{
		QGraphicsLineItem* path = addLine(QLineF(),penForBlueLine);
		pathLineList.append(path);
	}

	targetBall = addEllipse(QRect(),QPen(Qt::red),QBrush(Qt::red));
	targetLine = addLine(QLineF(),QPen(Qt::green));

	setBackgroundBrush(QBrush(QColor(155,0,0)));

}

KMapScene::~KMapScene()
{

}

void KMapScene::resizeMapScene(int size)
{
	present = 0;
	ImgSize = size;
	ImgShift = (ImgSize/2);

	initGrid();
	initCoordinates();

	setSceneRect(0,0,size,size);
}

//initialize Polar grid
void KMapScene::initGrid()
{
	present = 0;

	for (int k=0; k<2; k++)
		for (int r=0; r<TotalRings; r++)
			for (int s=0; s<N; s++)
				if (r == InnerRing)
					PolarGrid[k][r][s] = 0.0;
				else
					PolarGrid[k][r][s] = NoKnowledge;

}

void KMapScene::initCoordinates()
{
	for (int r=0; r<TotalRings+1; r++)
		for (int s=0; s<N; s++)
			if (r == InnerRing) {
				gridImgH[r][s] = ImgShift;
				gridImgV[r][s] = ImgShift;
			}
			else if (r == InnerRing+1) {
				gridImgV[r][s] = toGrid( toCartesianX( 0.5*RingDistance, StoT(s) ) );
				gridImgH[r][s] = toGrid( toCartesianY( 0.5*RingDistance, StoT(s) ) );
			}
			else {
				gridImgV[r][s] = toGrid( toCartesianX( RtoD(r), StoT(s) ) );
				gridImgH[r][s] = toGrid( toCartesianY( RtoD(r), StoT(s) ) );
			}

	for (int r=0; r<TotalRings; r++)
		for (int s=0; s<N; s++)
			if (r == InnerRing) {
				cellCenterX[r][s] = 0.0;
				cellCenterY[r][s] = 0.0;
			}
			else {
				cellCenterX[r][s] = toCartesianX( RtoD(r)+0.5*RingDistance, StoT(s)+0.5*SectorAngleRad );
				cellCenterY[r][s] = toCartesianY( RtoD(r)+0.5*RingDistance, StoT(s)+0.5*SectorAngleRad );
			}
}

void KMapScene::setPMObstaclesVisible(bool visible)
{
	for(unsigned i = 0; i<cellsList.count();i++)
	{
		if (visible == false)
		{
			cellsList.at(i)->setVisible(false);

		}else
		{
			cellsList.at(i)->setVisible(true);
		}
	}

	if (visible == false)
	{
		arrowBody->setVisible(false);
		arrowLside->setVisible(false);
		arrowRside->setVisible(false);
	}else
	{
		arrowBody->setVisible(true);
		arrowLside->setVisible(true);
		arrowRside->setVisible(true);
	}

}

void KMapScene::updateObstacles()
{
	QGraphicsPolygonItem* cell;
	QVector<QPoint> curve1(0);
	int colorValue = 0, cellNum = 0;
	int r, s;

	for (r=0; r<TotalRings ; r++)
	{
		for (s=0; s<N; s++)
		{
			QPoint x0( gridImgH[r][s], gridImgV[r][s]);
			QPoint x1( gridImgH[(r+1)][s], gridImgV[(r+1)][s]);
			QPoint x2( gridImgH[(r+1)][wrapTo(s+1, N)], gridImgV[(r+1)][wrapTo(s+1, N)]);
			QPoint x3( gridImgH[r][wrapTo(s+1, N)], gridImgV[r][wrapTo(s+1, N)]);

			curve1.clear();
			curve1.append(x0);
			curve1.append(x1);
			curve1.append(x2);
			curve1.append(x3);

			colorValue = ColorMax - PolarGrid[present][r][s]*ColorMax;

			cell = cellsList.at(cellNum);
			cell->setPolygon(QPolygon(curve1));

			if (r == InnerRing )
				cell->setBrush(QBrush(Qt::white));
			else
				cell->setBrush(QColor(colorValue,colorValue,colorValue));

			cellNum++;
		}
	}
}

void KMapScene::updateArrow()
{
	QPoint center(ImgShift, ImgShift);
	QPoint fromP(center.x(), center.y() + ArrowOffset);
	QPoint toP(center.x(), center.y() - ArrowOffset);
	QPoint leftP(center.x() - ArrowOffset, center.y());
	QPoint rightP(center.x() + ArrowOffset, center.y());

	arrowBody->setLine(fromP.x(), fromP.y(), toP.x(), toP.y());
	arrowLside->setLine(leftP.x(), leftP.y(), toP.x(), toP.y());
	arrowRside->setLine(rightP.x(), rightP.y(), toP.x(), toP.y());
}

void KMapScene::setPMTargetCoordVisible(bool visible)
{
	if (visible == false)
	{
		targetBall->setVisible(false);
		targetLine->setVisible(false);
	}else
	{
		targetBall->setVisible(true);
		targetLine->setVisible(true);
	}

}

void KMapScene::updateTargetCoordinates()
{
	QPoint toP;
	QPoint ball( toGrid(targetY), toGrid(targetX) );

	//cvCircle(img, ball, 3, red, 2, 8, 0);

	targetBall->setRect(ball.x()-4, ball.y()-4, 8, 8);

	int pix = 5;
	if (targetA > -(M_PI_4/2) && targetA <= (M_PI_4/2)){
		 toP.setX( ball.x() );
		 toP.setY( ball.y() - pix);
	}else if (targetA > (M_PI_4/2) && targetA <= 3*(M_PI_4/2)){
		toP.setX( ball.x() - pix);
		toP.setY( ball.y() - pix );
	}else if (targetA > 3*(M_PI_4/2) && targetA <= 5*(M_PI_4/2)){
		toP.setX( ball.x() -pix);
		toP.setY(ball.y() );
	}else if (targetA > 5*(M_PI_4/2) && targetA <= 7*(M_PI_4/2)){
		toP.setX( ball.x() - pix);
		toP.setY( ball.y() + pix );
	}else if (targetA > -3*(M_PI_4/2) && targetA <= -(M_PI_4/2)){
		toP.setX( ball.x() + pix);
		toP.setY(ball.y() - pix );
	}else if (targetA > -5*(M_PI_4/2) && targetA <= -3*(M_PI_4/2)){
		toP.setX(ball.x() + pix);
		toP.setY(ball.y());
	}else if (targetA > -7*(M_PI_4/2) && targetA <= -5*(M_PI_4/2)){
		toP.setX( ball.x() +pix);
		toP.setY(ball.y() + pix );
	}else{
		toP.setX( ball.x());
		toP.setY(ball.y() + pix );
	}


	targetLine->setLine(ball.x(), ball.y(), toP.x(), toP.y());
}

void KMapScene::setPMPathVisible(bool visible)
{
	for(unsigned i = 0; i<pathLineList.count();i++)
	{
		if (visible == false)
		{
			pathLineList.at(i)->setVisible(false);

		}else
		{
			pathLineList.at(i)->setVisible(true);
		}
	}

}

void KMapScene::updatePath()
{
	QGraphicsLineItem* path;
	QVector<QPoint> curve1(0);
	QPoint toP(0,0), fromP(0,0);
	int r, s;

	curve1.clear();
	for (int ways=0; ways<PathLength; ways++) {
		if (pathR[ways] == -1 && pathS[ways] == -1) break;
		r = pathR[ways];
		s = pathS[ways];

		if (r==InnerRing+1){
			fromP.setX( toGrid(toCartesianY(RtoD(r)+0.75*RingDistance, StoT(s)+0.5*SectorAngleRad)));
			fromP.setY( toGrid(toCartesianX(RtoD(r)+0.75*RingDistance, StoT(s)+0.5*SectorAngleRad)));
		}else{
			fromP.setX( toGrid(cellCenterY[r][s]));
			fromP.setY( toGrid(cellCenterX[r][s]));
		}

		/*curve1[0].x = gridImgH[r][s];
		curve1[0].y = gridImgV[r][s];
		curve1[1].x = gridImgH[(r+1)][s];
		curve1[1].y = gridImgV[(r+1)][s];
		curve1[2].x = gridImgH[(r+1)][wrapTo(s+1, N)];
		curve1[2].y = gridImgV[(r+1)][wrapTo(s+1, N)];
		curve1[3].x = gridImgH[r][wrapTo(s+1, N)];
		curve1[3].y = gridImgV[r][wrapTo(s+1, N)];*/

		QPoint x0(gridImgH[r][s], gridImgV[r][s]);
		QPoint x1(gridImgH[(r+1)][s], gridImgV[(r+1)][s]);
		QPoint x2(gridImgH[(r+1)][wrapTo(s+1, N)], gridImgV[(r+1)][wrapTo(s+1, N)]);
		QPoint x3(gridImgH[r][wrapTo(s+1, N)], gridImgV[r][wrapTo(s+1, N)]);

		curve1.clear();
		curve1.append(x0);
		curve1.append(x1);
		curve1.append(x2);
		curve1.append(x3);

		if (pathO[ways] == 0){
			//toP = cvPoint( (curve1[1].x + curve1[2].x )/2, (curve1[1].y + curve1[2].y )/2 );
			toP.setX((curve1[1].x() + curve1[2].x() )/2);
			toP.setY((curve1[1].y() + curve1[2].y() )/2);
		}else if (pathO[ways] == 1){
			//toP = cvPoint( curve1[2].x, curve1[2].y );
			toP.setX( curve1[2].x());
			toP.setY( curve1[2].y());
		}else if (pathO[ways] == 2){
			//toP = cvPoint( (curve1[2].x + curve1[3].x )/2, (curve1[2].y + curve1[3].y )/2 );
			toP.setX((curve1[2].x() + curve1[3].x() )/2);
			toP.setY((curve1[2].y() + curve1[3].y() )/2);
		}else if (pathO[ways] == 3){
			//toP = cvPoint(  curve1[3].x, curve1[3].y);
			toP.setX(curve1[3].x());
			toP.setY(curve1[3].y());
		}else if (pathO[ways] == 4){
			//toP = cvPoint( (curve1[0].x + curve1[3].x )/2, (curve1[0].y + curve1[3].y )/2 );
			toP.setX((curve1[0].x() + curve1[3].x() )/2);
			toP.setY((curve1[0].y() + curve1[3].y() )/2);
		}else if (pathO[ways] == 5){
			//toP = cvPoint(  curve1[0].x, curve1[0].y);
			toP.setX( curve1[0].x());
			toP.setY(curve1[0].y());
		}else if (pathO[ways] == 6){
			//toP = cvPoint( (curve1[1].x + curve1[0].x )/2, (curve1[1].y + curve1[0].y )/2 );
			toP.setX((curve1[1].x() + curve1[0].x() )/2);
			toP.setY((curve1[1].y() + curve1[0].y() )/2);
		}else if (pathO[ways] == 7){
			//toP = cvPoint(  curve1[1].x, curve1[1].y);
			toP.setX(curve1[1].x());
			toP.setY(curve1[1].y());
		}


		//cvLine( img, fromP, toP, blue, 1, CV_AA, 0);
		path = pathLineList.at(ways);
		path->setLine(fromP.x(), fromP.y(), toP.x(), toP.y());


		pathR[ways] = -1;
		pathS[ways] = -1;
		pathO[ways] = -1;
	}
}



/*********** Math functions ***********/

double KMapScene::wrapToPi(double angle) {
	while(angle < -M_PI) angle +=2.0*M_PI;
	while(angle > M_PI) angle -=2.0*M_PI;
	return (angle);
}

double KMapScene::wrapTo0_2Pi(double angle) {
	while (angle >= 2.0 * M_PI)
		angle -= 2.0 * M_PI;
	while (angle < 0.0)
		angle += 2.0 * M_PI;
	return angle;
}

double KMapScene::angleDiff(double a1, double a2) {
	return wrapToPi(wrapToPi(a1+M_PI-a2) - M_PI);
}

double KMapScene::toPolarD(double x, double y) {
	return sqrt(x*x + y*y);
}

double KMapScene::toPolarT(double x, double y) {
	return atan2(y, x);
}

double KMapScene::toCartesianX(double d, double t) {
	return d*cos(t);
}

double KMapScene::toCartesianY(double d, double t) {
	return d*sin(t);
}

int KMapScene::DtoR(double d) {
	if (d<0)
		return InnerRing;
	else
		return int( d / RingDistance ) + 1; // +1 is used to skip the InnerRing
}

int KMapScene::TtoS(double theta) {
	return int( wrapTo0_2Pi(theta + M_PI_2 + SectorShiftRad) / SectorAngleRad );
}

double KMapScene::RtoD(int r) {
	if (r==InnerRing)
		return 0.0;
	else
		return (r-1)*RingDistance;
}

double KMapScene::StoT(int s) {
	return wrapToPi(s*SectorAngleRad - M_PI_2 - SectorShiftRad);
}

int KMapScene::XYtoR(double x, double y) {
	return DtoR( toPolarD(x, y) );
}

int KMapScene::XYtoS(double x, double y) {
	return TtoS( toPolarT(x, y) );
}

int KMapScene::toGrid(double x) {
	return int(-round(ImgScale*x)+ImgShift);
}

int KMapScene::wrapTo(int n, int MAXN) {
	while (n<0) n += MAXN;
	return (n%MAXN);
}

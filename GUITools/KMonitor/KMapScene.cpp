#include "KMapScene.h"

#include <QGraphicsPolygonItem>

#include <iostream>
using namespace std;


KMapScene::KMapScene(KMapView* parent )//, QString hostId)
{
	this->parent = parent;
	cellsList.clear();

	//currentHost = hostId;

	QPen penForRedLine(Qt::red);
	penForRedLine.setWidth(3);

	for (int r=0; r<TotalRings; r++)
		for (int s=0; s<N; s++)
		{
			QGraphicsPolygonItem* cell = addPolygon(QPolygonF(QRectF()),QPen(Qt::white),QBrush(Qt::transparent));
			cellsList.append(cell);
		}

	arrowBody = addLine(QLineF(),penForRedLine);
	arrowLside = addLine(QLineF(),penForRedLine);
	arrowRside = addLine(QLineF(),penForRedLine);

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

	for (int ring=0; ring < TotalRings; ring++)
		for (int sector=0; sector < N; sector++){
			PolarGrid[0][ring][sector] = sector*0.01;
		}
	targetX = 0;
	targetY = 0;
	targetA = 0;

	for (int step = 0; step < PathLength; step++){
		pathR[step] = 0;
		pathS[step] = 0;
		pathO[step] = 0;
	}

	updateObstacles();
	updateArrow();
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

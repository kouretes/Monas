#include "KRobotMap.h"

#include <iostream>
using namespace std;

KRobotMap::KRobotMap(KLabel* parent, QString hostId)
	: parentLabel(0)
{
	this->parentLabel = parent;

	currentHost = hostId;
	obstaclesVisible = false;
	targetCoordVisible = true;	//TEST
	pathVisible = true;		//TEST

	if (this->parentLabel->width()>this->parentLabel->height())
		ImgSize = this->parentLabel->height()-10;
	else
		ImgSize = this->parentLabel->width()-10;

	ImgShift = (ImgSize/2);
	present = 0;

	img = cvCreateImage(cvSize(ImgSize, ImgSize), IPL_DEPTH_8U, 3);
	initGrid();
	initCoordinates();

	QImage* image = this->IplImage2QImage(img);
	this->parentLabel->setPixmap(QPixmap::fromImage((*image)));
	this->parentLabel->setAlignment(Qt::AlignHCenter);
	//this->parentLabel->setScaledContents (true);
}

KRobotMap::~KRobotMap()
{
	if (img)
		delete img;
}

void KRobotMap::resizeRobotMap(int size)
{

}

//initialize Polar grid
void KRobotMap::initGrid() {
	present = 0;

	for (int k=0; k<2; k++)
		for (int r=0; r<TotalRings; r++)
			for (int s=0; s<N; s++)
				if (r == InnerRing)
					PolarGrid[k][r][s] = 0.0;
				else
					PolarGrid[k][r][s] = NoKnowledge;

}

void KRobotMap::initCoordinates() {
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

QImage* KRobotMap::IplImage2QImage(IplImage *iplImg)
{
	int h = iplImg->height;
	int w = iplImg->width;
	int channels = iplImg->nChannels;
	QImage *qimg = new QImage(w, h, QImage::Format_ARGB32);
	char *data = iplImg->imageData;

	for (int y = 0; y < h; y++, data += iplImg->widthStep)
	{
		for (int x = 0; x < w; x++)
		{
			char r, g, b, a = 0;
			if (channels == 1)
			{
				r = data[x * channels];
				g = data[x * channels];
				b = data[x * channels];
			}
			else if (channels == 3 || channels == 4)
			{
				r = data[x * channels + 2];
				g = data[x * channels + 1];
				b = data[x * channels];
			}

			if (channels == 4)
			{
				a = data[x * channels + 3];
				qimg->setPixel(x, y, qRgba(r, g, b, a));
			}
			else
			{
				qimg->setPixel(x, y, qRgb(r, g, b));
			}
		}
	}
	return qimg;
}

void KRobotMap::cvDrawGrid() {
	int colorValue = 0;
	CvPoint curve1[4];
	CvPoint* curveArr = {curve1};
	int nCurvePts = {4};
	int nCurves = 1;
	int isCurveClosed=1;
	int lineWidth=8;
	int r, s;
	CvPoint center, toP, fromP, leftP, rightP;

	CvScalar white = cvScalar(ColorMax,ColorMax,ColorMax);
	CvScalar grey = cvScalar(ColorMax/2,ColorMax/2,ColorMax/2);
	CvScalar blue = cvScalar(ColorMax, 0, 0);
	CvScalar black = cvScalar(0, 0, 0);
	CvScalar paintColor = grey;
	CvScalar red = cvScalar(0, 0, ColorMax);
	CvScalar green = cvScalar(0, ColorMax, 0);

	if(obstaclesVisible){

		for (r=0; r<TotalRings; r++) {
			for (s=0; s<N; s++) {
				curve1[0].x = gridImgH[r][s];
				curve1[0].y = gridImgV[r][s];
				curve1[1].x = gridImgH[(r+1)][s];
				curve1[1].y = gridImgV[(r+1)][s];
				curve1[2].x = gridImgH[(r+1)][wrapTo(s+1, N)];
				curve1[2].y = gridImgV[(r+1)][wrapTo(s+1, N)];
				curve1[3].x = gridImgH[r][wrapTo(s+1, N)];
				curve1[3].y = gridImgV[r][wrapTo(s+1, N)];

				colorValue = ColorMax - PolarGrid[present][r][s]*ColorMax;
				paintColor = cvScalar(colorValue, colorValue, colorValue);

				if (r == InnerRing) {
					paintColor = cvScalar(ColorMax, ColorMax, ColorMax);
					cvFillPoly( img, &curveArr,&nCurvePts, nCurves, paintColor );
				}
				else {
					cvFillPoly( img, &curveArr, &nCurvePts, nCurves, paintColor );
					cvPolyLine( img,&curveArr, &nCurvePts, nCurves, 1, white, isCurveClosed, lineWidth, 0 );
				}
			}
		}
	}

	/**************************************************************************/
	/*if(targetCoordVisible){

		CvPoint ball = cvPoint( toGrid(targetY), toGrid(targetX) );
		cvCircle(img, ball, 3, red, 2, 8, 0);

		int pix = 5;
		if (targetA > -(M_PI_4/2) && targetA <= (M_PI_4/2))
			toP = cvPoint( ball.x , ball.y - pix);
		else if (targetA > (M_PI_4/2) && targetA <= 3*(M_PI_4/2))
			toP = cvPoint( ball.x - pix, ball.y - pix );
		else if (targetA > 3*(M_PI_4/2) && targetA <= 5*(M_PI_4/2))
			toP = cvPoint( ball.x -pix, ball.y );
		else if (targetA > 5*(M_PI_4/2) && targetA <= 7*(M_PI_4/2))
			toP = cvPoint( ball.x - pix, ball.y + pix );


		else if (targetA > -3*(M_PI_4/2) && targetA <= -(M_PI_4/2))
			toP = cvPoint( ball.x + pix, ball.y - pix );
		else if (targetA > -5*(M_PI_4/2) && targetA <= -3*(M_PI_4/2))
			toP = cvPoint( ball.x+pix, ball.y);
		else if (targetA > -7*(M_PI_4/2) && targetA <= -5*(M_PI_4/2))
			toP = cvPoint( ball.x +pix, ball.y + pix );
		else
			toP = cvPoint( ball.x , ball.y + pix );


		cvLine( img, ball, toP, green, 2, CV_AA, 0);
	}

	************************************************************************

	if(pathVisible){

		for (int ways=0; ways<PathLength; ways++) {
			if (pathR[ways] == -1 && pathS[ways] == -1) break;
			r = pathR[ways];
			s = pathS[ways];

			if (r==InnerRing+1)
				fromP = cvPoint( toGrid(toCartesianY(RtoD(r)+0.75*RingDistance, StoT(s)+0.5*SectorAngleRad)), toGrid(toCartesianX(RtoD(r)+0.75*RingDistance, StoT(s)+0.5*SectorAngleRad)) );
			else
				fromP = cvPoint( toGrid(cellCenterY[r][s]), toGrid(cellCenterX[r][s]) );

			curve1[0].x = gridImgH[r][s];
			curve1[0].y = gridImgV[r][s];
			curve1[1].x = gridImgH[(r+1)][s];
			curve1[1].y = gridImgV[(r+1)][s];
			curve1[2].x = gridImgH[(r+1)][wrapTo(s+1, N)];
			curve1[2].y = gridImgV[(r+1)][wrapTo(s+1, N)];
			curve1[3].x = gridImgH[r][wrapTo(s+1, N)];
			curve1[3].y = gridImgV[r][wrapTo(s+1, N)];

			if (pathO[ways] == 0)
				toP = cvPoint( (curve1[1].x + curve1[2].x )/2, (curve1[1].y + curve1[2].y )/2 );
			else if (pathO[ways] == 1)
				toP = cvPoint( curve1[2].x, curve1[2].y );
			else if (pathO[ways] == 2)
				toP = cvPoint( (curve1[2].x + curve1[3].x )/2, (curve1[2].y + curve1[3].y )/2 );
			else if (pathO[ways] == 3)
				toP = cvPoint(  curve1[3].x, curve1[3].y);
			else if (pathO[ways] == 4)
				toP = cvPoint( (curve1[0].x + curve1[3].x )/2, (curve1[0].y + curve1[3].y )/2 );
			else if (pathO[ways] == 5)
				toP = cvPoint(  curve1[0].x, curve1[0].y);
			else if (pathO[ways] == 6)
				toP = cvPoint( (curve1[1].x + curve1[0].x )/2, (curve1[1].y + curve1[0].y )/2 );
			else if (pathO[ways] == 7)
				toP = cvPoint(  curve1[1].x, curve1[1].y);

			cvLine( img, fromP, toP, blue, 1, CV_AA, 0);
			pathR[ways] = -1;
			pathS[ways] = -1;
			pathO[ways] = -1;
		}
	}*/

	/**************************************************************************/
	//draw the arrow
	center = cvPoint(ImgShift, ImgShift);
	fromP  = cvPoint(center.x, center.y + ArrowOffset);
	toP    = cvPoint(center.x, center.y - ArrowOffset);
	leftP  = cvPoint(center.x - ArrowOffset, center.y);
	rightP = cvPoint(center.x + ArrowOffset, center.y);

	cvLine( img, fromP,  toP, blue, 2, CV_AA, 0);
	cvLine( img, leftP,  toP, blue, 2, CV_AA, 0);
	cvLine( img, rightP, toP, blue, 2, CV_AA, 0);

	//return img;
}

void KRobotMap::updateRobotMap()
{
	QImage* image;

	cvDrawGrid();

	image = this->IplImage2QImage(img);
	this->parentLabel->setPixmap(QPixmap::fromImage((*image)));
	this->parentLabel->setAlignment(Qt::AlignHCenter);
	//this->parentLabel->setScaledContents (true);


}

/*********** Math functions ***********/

double KRobotMap::wrapToPi(double angle) {
	while(angle < -M_PI) angle +=2.0*M_PI;
	while(angle > M_PI) angle -=2.0*M_PI;
	return (angle);
}

double KRobotMap::wrapTo0_2Pi(double angle) {
	while (angle >= 2.0 * M_PI)
		angle -= 2.0 * M_PI;
	while (angle < 0.0)
		angle += 2.0 * M_PI;
	return angle;
}

double KRobotMap::angleDiff(double a1, double a2) {
	return wrapToPi(wrapToPi(a1+M_PI-a2) - M_PI);
}

double KRobotMap::toPolarD(double x, double y) {
	return sqrt(x*x + y*y);
}

double KRobotMap::toPolarT(double x, double y) {
	return atan2(y, x);
}

double KRobotMap::toCartesianX(double d, double t) {
	return d*cos(t);
}

double KRobotMap::toCartesianY(double d, double t) {
	return d*sin(t);
}

int KRobotMap::DtoR(double d) {
	if (d<0)
		return InnerRing;
	else
		return int( d / RingDistance ) + 1; // +1 is used to skip the InnerRing
}

int KRobotMap::TtoS(double theta) {
	return int( wrapTo0_2Pi(theta + M_PI_2 + SectorShiftRad) / SectorAngleRad );
}

double KRobotMap::RtoD(int r) {
	if (r==InnerRing)
		return 0.0;
	else
		return (r-1)*RingDistance;
}

double KRobotMap::StoT(int s) {
	return wrapToPi(s*SectorAngleRad - M_PI_2 - SectorShiftRad);
}

int KRobotMap::XYtoR(double x, double y) {
	return DtoR( toPolarD(x, y) );
}

int KRobotMap::XYtoS(double x, double y) {
	return TtoS( toPolarT(x, y) );
}

int KRobotMap::toGrid(double x) {
	return int(-round(ImgScale*x)+ImgShift);
}

int KRobotMap::wrapTo(int n, int MAXN) {
	while (n<0) n += MAXN;
	return (n%MAXN);
}

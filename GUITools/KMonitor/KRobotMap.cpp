#include "KRobotMap.h"

KRobotMap::KRobotMap(KLabel* parentscene)
	: parentLabel(0)
{
	this->parentLabel = parentscene;

}

KRobotMap::~KRobotMap()
{}

void KRobotMap::resizeRobotMap(int size)
{
	QImage* image;

	ImgSize = size;
	ImgShift = (ImgSize/2);
	//arrowOffset=10;
	present = 0;
	img = cvCreateImage(cvSize(ImgSize, ImgSize), IPL_DEPTH_8U, 3);
	initGrid();
	initCoordinates();

	image = this->IplImage2QImage(img);
	this->parentLabel->setPixmap(QPixmap::fromImage((*image)));

	//this->parentLabel->setAlignment(Qt::AlignHCenter);
	this->parentLabel->setScaledContents (true);
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

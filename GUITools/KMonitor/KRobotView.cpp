#include "KRobotView.h"

#include <QImage>

#include <iostream>
using namespace std;

KRobotView::KRobotView(KLabel* parent, QString hostId)
	: parentLabel(0)
{
	parentLabel = parent;
	currentHost = hostId;

	LRVRawImageVisible = false;
}

KRobotView::~KRobotView()
{
}

void KRobotView::resizeRobotView(int size)
{
	//this->parentLabel->setAlignment(Qt::AlignHCenter);
}

void KRobotView::resetKRobotView(QString hostId)
{
	currentHost = hostId;

	setLRVRawImageVisible(false);
}

void KRobotView::setRVRawImageVisible(bool visible)
{
	if (visible == false)
	{
		this->parentLabel->setPixmap(QPixmap());
		this->parentLabel->setAlignment(Qt::AlignHCenter);
	}
}

void KRobotView::updateRobotView(KRawImage rawImage)
{
	QImage* image;

	image = this->YUVPixelFormat2RGB32(rawImage);
	this->parentLabel->setPixmap(QPixmap::fromImage(*image));
	this->parentLabel->setAlignment(Qt::AlignHCenter);

}

QImage* KRobotView::YUVPixelFormat2RGB32(KRawImage rawImage)
{
	const  double A=1.4075, B=0.3455, C=0.7169, D=1.7790;
	int startofBlock;
	unsigned char y, cu, cv;
	int r, g, b, u, v;

	int widthInPixels = rawImage.width();
	int widthmult2 = widthInPixels*2;
	int heightInPixels = rawImage.height();
	int channels = rawImage.bytes_per_pix();
	const char *data = rawImage.image_rawdata().data();

	QImage *qimg = new QImage(widthInPixels, heightInPixels, QImage::Format_ARGB32);

	for (int hCoord = 0; hCoord < heightInPixels; hCoord++)
	{
		for (int wCoord = 0; wCoord < widthInPixels; wCoord++)
		{
			if(channels == 2)
			{
				y = *(data + hCoord*widthmult2 + (wCoord<<1));//Y is right where we want it
				//a block is a yuyv sequence, and from that block extract the second (Y) and 4th byte (V)
				startofBlock =hCoord * widthmult2 + ((wCoord>>1)<<2); //every 2 pixels (i/2) swap block (size of block=4)
				// cout<<"sob"<<endl;
				cu=*(data + startofBlock+1);
				// cout<<"u"<<endl;
				cv= *(data + startofBlock+3);

				u=cu-128;
				v=cv-128;
				//cout<<y<<" "<<u<<" "<< v<<" " <<endl;
				r=y+A*v;
				g=y-B*u-C*v;
				b=y+D*u;
				//cout<<r<<" "<<g<<" "<< b<<endl;
				r=r<0?0:r;
				g=g<0?0:g;
				b=b<0?0:b;
				r=r>255?255:r;
				g=g>255?255:g;
				b=b>255?255:b;

				qimg->setPixel(wCoord, hCoord, qRgb(r,g,b));
			}else
			{
				// what ??
			}
		}
	}
	return qimg;
}


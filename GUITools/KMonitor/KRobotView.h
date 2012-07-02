#ifndef KROBOTVIEW_H_
#define KROBOTVIEW_H_

#include "KLabel.h"
#include "activities/Vision/KSegmentator.h"
using namespace std;

class KLabel;

class KRobotView
{

public:
	KRobotView(KLabel* parent, QString hostId);
	~KRobotView();

	void resizeRobotView(int size);

	QString getCurrentHost(){ return currentHost; }
	void resetKRobotView(QString hostId);

	void setLRVRawImageVisible(bool visible){ LRVRawImageVisible = visible; setRVRawImageVisible(visible);}
	bool getLRVRawImageVisible(){return LRVRawImageVisible;}
	void setRVRawImageVisible(bool visible);
	void updateRawRobotView(KRawImage rawImage);

	void setLRVSegImageVisible(bool visible){ LRVSegImageVisible = visible; setRVSegImageVisible(visible);}
	bool getLRVSegImageVisible(){return LRVSegImageVisible;}
	void setRVSegImageVisible(bool visible);
	void updateSegRobotView(KRawImage rawImage);

private:
	QImage* YUVPixelFormat2RGB32(KRawImage rawImage);
	void loadXMLConfigParameters(std::string fname);
	QImage* YUV2RGBPlusPixSegmentation(KRawImage rawImage);

	KLabel* parentLabel;
	QString currentHost;

	bool LRVRawImageVisible;
	bool LRVSegImageVisible;

	unsigned char orangeColor;// = 'o';
	unsigned char greenColor;// = 'g';
	unsigned char yellowColor;// = 'y';
	unsigned char whiteColor;// = 'w';
	unsigned char redColor;// = 'r';
	unsigned char blueColor;// = 'u';//From magic the gathering :P
	unsigned char blackColor;// = 'b';

	struct QYuv{
		unsigned char y,u,v;
		//Custom operator because is needed for the map<>
		bool operator<(const QYuv& QYuv1) const { return (int((y<<16)|(u<<8)|v)) < (int((QYuv1.y<<16)|(QYuv1.u<<8)|QYuv1.v));}
	};

	map<unsigned char,QRgb> basicSegColors;
	KSegmentator *yuvColorTable;
};

#endif /* KROBOTVIEW_H_ */

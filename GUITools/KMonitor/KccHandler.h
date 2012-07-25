#ifndef KCCHANDLER_H
#define KCCHANDLER_H

#include <QWidget>
#include <vector>
#include <map>
#include <QScrollArea>
#include <string>
#include <QDir>
#include <QDebug>
#include <QColor>
#include <QRgb>
#include <math.h>
#include <QFileDialog>
#include <QScrollBar>
#include <QString>
#include <fstream>
#include "KccLabel.h"
#include "LWRemoteHosts.h"
#include "messages/Kimage.pb.h"
#include "activities/Vision/KSegmentator.h"
using namespace std;

namespace Ui {
    class KccHandler;
}

class KccHandler : public QWidget
{
    Q_OBJECT
	struct QYuv{
		unsigned char y,u,v;
		//Custom operator because is needed for the map<>
		bool operator<(const QYuv& QYuv1) const { return (int((y<<16)|(u<<8)|v)) < (int((QYuv1.y<<16)|(QYuv1.u<<8)|QYuv1.v));}
	};
public:
	static const unsigned int MAX_UNDO = 10;
	static const float gammaExposure = 0.45;
	unsigned char orangeColor;// = 'o';
	unsigned char greenColor;// = 'g';
	unsigned char yellowColor;// = 'y';
	unsigned char whiteColor;// = 'w';
	unsigned char redColor;// = 'r';
	unsigned char blueColor;// = 'u';//From magic the gathering :P
	unsigned char blackColor;// = 'b';

public:
    explicit KccHandler(QWidget *parent = 0);
    ~KccHandler();

    Ui::KccHandler* getKccUi() {return ui;}

private:
	void adjustScrollBar(QScrollBar *scrollBar, double factor);
	void transformYUVtoRGB(const char *yuvImage, QImage *rgbImage);
	int distance(QYuv a,QYuv b);
	unsigned char adjustY(unsigned char y);
	unsigned char adjustU(unsigned char u);
	unsigned char adjustV(unsigned char v);
signals:
	void NewHostAdded(QString,QString);
	void OldHostRemoved(QString);
	void GameStateMsgUpdate(QIcon,QString,QString);
	void LWRHSubscriptionRequest(QString);
	void LWRHUnsubscriptionRequest(QString);
public slots:
	void addComboBoxItem(QString, QString);
	void removeComboBoxItem(QString);
	void setLWRHGameStateInfo(QIcon, QString, QString);
	void SubscriptionHandler(QString);
	void UnsubscriptionHandler(QString);
    void clickedImage(QMouseEvent* ev);
	void pbOrangePressed();
	void pbGreenPressed();
	void pbYellowPressed();
    void pbWhitePressed();
	void pbBluePressed();
	void pbRedPressed();
    void pbBlackPressed();
	void undoPressed();
	void pbSnapshotPressed();
	void segSave();
	void segOpen();
	void clearColorTable();
	void realZoomIn();
	void realZoomOut();
	void segZoomIn();
	void segZoomOut();
	void changeImage(KRawImage rawImage, QString hostId);

private:

	double A, B, C, D;
	int widthInPixels;
	int heightInPixels;
	int widthmult2;
	float lumaScale;
	float curLuminance;
	KSegmentator *yuvColorTable,*yuvColorTableOld;
	
	QYuv yuvRealImage[480][640];
	map<unsigned char,QRgb> basicSegColors;

	unsigned char choosedColor;

	vector<map<QYuv,unsigned char> > undoVector;
	bool takeSnapshot;
    KccLabel* realImL,* segImL;
    LWRemoteHosts* availableKCCHosts;
	QScrollArea *scrollImage,*scrollSeg;

	double rScale,iScale;
	double zoomInScale,zoomOutScale;
	QImage segImage,realImage;
    Ui::KccHandler *ui;
};

#endif // KCCHANDLER_H

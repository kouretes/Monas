#ifndef KCAMVIEW_H
#define KCAMVIEW_H

#include <QWidget>
#include <QListWidgetItem>
#include <QCheckBox>
#include <map>
#include <iostream>
#include <fstream>
#include "HostsComboBox.h"
#include "messages/Kimage.pb.h"
#include "activities/Vision/KSegmentator.h"
namespace Ui {
    class KCamView;
}

class KCamView : public QWidget
{
    Q_OBJECT

public:
    explicit KCamView(QWidget *parent = 0);
    ~KCamView();
    
signals:
	void NewHostAdded(QString,QString);
	void OldHostRemoved(QString);
	void GameStateMsgUpdate(QString,QString,QString);
	void LWRHSubscriptionRequest(QString);
	void LWRHUnsubscriptionRequest(QString);

public slots:
	void addComboBoxItem(QString, QString);
	void removeComboBoxItem(QString);
	void setLWRHGameStateInfo(QString, QString, QString);
	void SubscriptionHandler(QString);
	void UnsubscriptionHandler(QString);
	void kRawImageUpdateHandler(KRawImage, QString);
	
private slots:
	void newListElementRequested(QListWidgetItem* item);
protected:
    void changeEvent(QEvent *e);

private:
    HostsComboBox* availableCamHosts;

	QString myCurrentLVRequestedHost;

    bool rawImageRequested;
    bool segImageRequested;
    
    Ui::KCamView *ui;
    
    QImage* YUVPixelFormat2RGB32(KRawImage rawImage);
	void loadXMLConfigParameters(std::string fname);
	QImage* YUV2RGBPlusPixSegmentation(KRawImage rawImage);

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
	
	KRawImage currentImage;
	std::map<unsigned char,QRgb> basicSegColors;
	KSegmentator *yuvColorTable;
};

#endif // KCAMVIEW_H

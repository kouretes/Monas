#ifndef KROBOTVIEW_H_
#define KROBOTVIEW_H_

#include "KLabel.h"

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
	void updateRobotView(KRawImage rawImage);

//protected:
//public slots:
//signals:
private:
	QImage* YUVPixelFormat2RGB32(KRawImage rawImage);

	KLabel* parentLabel;
	QString currentHost;

	bool LRVRawImageVisible;
};

#endif /* KROBOTVIEW_H_ */

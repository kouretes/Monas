#ifndef KLABEL_H_
#define KLABEL_H_

#include <QLabel>

#include "messages/Kimage.pb.h"

class KRobotView;

class KLabel : public QLabel
{
	Q_OBJECT

public:
	KLabel(QWidget* parent = 0);
	~KLabel();

protected:
	void resizeEvent(QResizeEvent* event);

public slots:
	void resetRobotView(QString);

	void LVRawImageVisible(QString, bool);

	void kRawImageUpdateHandler(KRawImage rawImage, QString);

//signals:
private:
	KRobotView* robotView;

};

#endif /* KLABEL_H_ */

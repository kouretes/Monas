#ifndef KCCLABEL_H
#define KCCLABEL_H

#include <QLabel>
#include <QMouseEvent>
#include <QPainter>

#include "messages/Debug.pb.h"
#include <boost/date_time/posix_time/posix_time.hpp>

class KccLabel : public QLabel {
	Q_OBJECT
	
public:
	KccLabel(QWidget *parent);
	~KccLabel() {}
	
	QPixmap latestImgPix, latestImgLinesPix;
	boost::posix_time::ptime latestTimestamp;

signals:
	void clicked(QMouseEvent *event);

public slots:
	void updateLabel(VisionDebugMessage vdm);
	
protected:
	void mousePressEvent(QMouseEvent *event);

};

#endif

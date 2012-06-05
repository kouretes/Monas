#ifndef KMAPVIEW_H_
#define KMAPVIEW_H_

#include <QGraphicsView>

#include "messages/ObstacleAvoidanceMessage.pb.h"

class KMapScene;

class KMapView : public QGraphicsView
{
	Q_OBJECT

public:
	KMapView(QWidget* parent = 0);
	~KMapView();

public slots:
	void resetRobotMap(QString);

	void LMObstaclesVisible(QString, bool);
	void LMPathVisible(QString, bool);
	void LMTargetCoordVisible(QString, bool);

	void gridInfoUpdateHandler(GridInfo, QString);

protected:
	void resizeEvent(QResizeEvent* event);

private:

	KMapScene* mapArea;

};

#endif /* KMAPVIEW_H_ */

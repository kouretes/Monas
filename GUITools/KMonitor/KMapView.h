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
	void removeRobotMap(QString hostId);
	void LMObstaclesVisible(QString hostId, bool visible);
	void LMPathVisible(QString hostId, bool visible);
	void LMTargetCoordVisible(QString hostId, bool visible);

	void gridInfoUpdateHandler(GridInfo gim, QString hostId);

protected:
	void resizeEvent(QResizeEvent* event);

private:

	KMapScene* mapArea;

};

#endif /* KMAPVIEW_H_ */

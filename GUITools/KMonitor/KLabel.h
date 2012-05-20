#ifndef KLABEL_H_
#define KLABEL_H_

#include <QLabel>

#include "messages/ObstacleAvoidanceMessage.pb.h"

class KRobotMap;

class KLabel : public QLabel
{
	Q_OBJECT

public:
	KLabel(QWidget* parent = 0);
	~KLabel();

protected:
	void resizeEvent(QResizeEvent* event);

public slots:
	void removeRobotMap(QString);

	void LMObstaclesVisible(QString, bool);
	void gridInfoUpdateHandler(GridInfo, QString);

//signals:
private:
	KRobotMap* polarMap;

};

#endif /* KLABEL_H_ */

#ifndef KGRAPHICSVIEW_H_
#define KGRAPHICSVIEW_H_

#include <QGraphicsView>

#include "messages/Gamecontroller.pb.h"
#include "messages/VisionObservations.pb.h"
#include "GraphicalRobotElement.h"
#include "KFieldScene.h"


class GraphicalRobotElement;
class KFieldScene;

class KGraphicsView : public QGraphicsView
{
	Q_OBJECT

public:
	KGraphicsView(QWidget* parent = 0);
	~KGraphicsView();

protected:
	void resizeEvent(QResizeEvent* event);

public slots:
	void setKGFCGameStateInfo(GameStateMessage gsm, QString host);
	void worldInfoUpdateHandler(WorldInfo nwim, QString host);
	void observationMessageUpdateHandler(ObservationMessage om, QString host);
	void localizationDataUpdateHandler(LocalizationDataForGUI debugData, QString host);

	void GWSGVRobotVisible(QString host, bool visible);
	void GWSGVBallVisible(QString host, bool visible);

	void removeGraphicalElement(QString host);

	void LWSGVRobotVisible(QString host, bool visible);
	void LWSGVBallVisible(QString host, bool visible);

	void LWSGVVisionBallVisible(QString host, bool visible);
	void LWSGVVisionGoalPostsVisible(QString host, bool visible);

	void LWSGVParticlesVisible(QString host, bool visible);

signals:
	void forceTimeOut();

private:
	void GWSGVUnionistLineVisible(GraphicalRobotElement *robotElement);

	KFieldScene* paintArea;

};
#endif /* KGRAPHICSVIEW_H_ */

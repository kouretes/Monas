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
	void setKGFCGameStateInfo(GameStateMessage, QString);
	void worldInfoUpdateHandler(WorldInfo, QString);
	void observationMessageUpdateHandler(ObservationMessage, QString);
	void localizationDataUpdateHandler(LocalizationDataForGUI, QString);
	void headYawJointUpdateHandler(float, QString);
	void motionCommandUpdateHandler(MotionWalkMessage, QString);

	void GWSGVRobotVisible(QString, bool);
	void GWSGVBallVisible(QString, bool);

	void removeGraphicalElement(QString);

	void LWSGVRobotVisible(QString, bool);
	void LWSGVBallVisible(QString, bool);

	void LWSGVVisionBallVisible(QString, bool);
	void LWSGVVisionGoalPostsVisible(QString, bool);

	void LWSGVParticlesVisible(QString, bool);
	void LWSGVHFOVVisible(QString, bool);
	void LWSGVTraceVisible(QString, bool);

	void LWSGVMWCmdVisible(QString, bool);

signals:
	void forceTimeOut();

private:
	void GWSGVUnionistLineVisible(GraphicalRobotElement *robotElement);

	KFieldScene* paintArea;

};
#endif /* KGRAPHICSVIEW_H_ */

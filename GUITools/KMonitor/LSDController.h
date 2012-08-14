#ifndef LSDCONTROLLER_H_
#define LSDCONTROLLER_H_

#include <QTableWidget>
#include <QList>

#include "messages/SensorsMessage.pb.h"
#include <boost/circular_buffer.hpp>

typedef struct joints{

	QString yaw;
	QString pitch;

}HeadJoints;


class LSDController : public QTableWidget
{
	Q_OBJECT

public:
	LSDController(QList<QTableWidget*> tablesList);
	~LSDController();

/*
protected:
	void resizeEvent(QResizeEvent* event);

public slots:
	void setKGFCGameStateInfo(GameStateMessage, QString);
	void worldInfoUpdateHandler(WorldInfo, QString);
	void observationMessageUpdateHandler(ObservationMessage, QString);
	void localizationDataUpdateHandler(LocalizationDataForGUI, QString);
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

*/

public slots:
	void LSCSubscriptionHandler(QString);
	void LSCUnsubscriptionHandler(QString);

	void sensorsDataUpdateHandler(AllSensorValuesMessage, QString);

private:
	void uncheckAllTablesContents();
	void udpateHeadJointsBuffer(AllSensorValuesMessage);
	void updateHeadJointsTable();



	boost::circular_buffer<HeadJoints> headJointsBuffer;

	QList<QTableWidget*> parentTablesList;
	QString currentHost;

};
#endif /* LSDCONTROLLER_H_ */

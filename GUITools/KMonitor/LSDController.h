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

typedef struct joints1{

	QString ShoulderPitch;
	QString ShoulderRoll;
	QString ElbowYaw;
	QString ElbowRoll;

}ArmJoints;

typedef struct joints2{

	QString HipYawPitch;
	QString HipRoll;
	QString HipPitch;
	QString KneePitch;
	QString AnklePitch;
	QString AnkleRoll;

}LegJoints;

typedef struct values{

	QString AccZvalue;
	QString AccXvalue;
	QString AccYvalue;
	QString angX;
	QString angY;
	//QString AnkleRoll;

}InertialValues;


class LSDController : public QTableWidget
{
	Q_OBJECT

public:
	LSDController(QList<QTableWidget*> tablesList);
	~LSDController();

public slots:
	void LSCSubscriptionHandler(QString);
	void LSCUnsubscriptionHandler(QString);

	void sensorsDataUpdateHandler(AllSensorValuesMessage, QString);

private:
	void uncheckAllTablesContents();
	void updateHeadJointsBuffer(AllSensorValuesMessage);
	void updateLArmJointsBuffer(AllSensorValuesMessage asvm);
	void updateRArmJointsBuffer(AllSensorValuesMessage asvm);
	void updateLLegJointsBuffer(AllSensorValuesMessage asvm);
	void updateRLegJointsBuffer(AllSensorValuesMessage asvm);
	void updateInertialBuffer(AllSensorValuesMessage asvm);
	void updateHeadJointsTable();
	void updateLArmJointsTable();
	void updateRArmJointsTable();
	void updateLLegJointsTable();
	void updateRLegJointsTable();
	void updateInertialTable();



	boost::circular_buffer<HeadJoints> headJointsBuffer;
	boost::circular_buffer<ArmJoints> LArmJointsBuffer;
	boost::circular_buffer<ArmJoints> RArmJointsBuffer;
	boost::circular_buffer<LegJoints> LLegJointsBuffer;
	boost::circular_buffer<LegJoints> RLegJointsBuffer;
	boost::circular_buffer<InertialValues> InertialBuffer;

	QList<QTableWidget*> parentTablesList;
	QString currentHost;

};
#endif /* LSDCONTROLLER_H_ */

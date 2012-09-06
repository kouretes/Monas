#ifndef LSDCONTROLLER_H_
#define LSDCONTROLLER_H_

#include <QComboBox>
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

}InertialValues;

typedef struct morevalues{

	QString LFsrFL;
	QString LFsrRL;
	QString LFsrFR;
	QString LFsrRR;
	QString RFsrFL;
	QString RFsrRL;
	QString RFsrFR;
	QString RFsrRR;

}FSRValues;


class LSDController : public QComboBox
{
	Q_OBJECT

public:
	LSDController(QList<QComboBox*> tablesList);
	~LSDController();

public slots:
	void LSCSubscriptionHandler(QString);
	void LSCUnsubscriptionHandler(QString);

	void sensorsDataUpdateHandler(AllSensorValuesMessage, QString);

private:
	void clearComboLists(QList<QComboBox*> cList);
	void updateHeadJointsBuffer(AllSensorValuesMessage);
	void updateLArmJointsBuffer(AllSensorValuesMessage asvm);
	void updateRArmJointsBuffer(AllSensorValuesMessage asvm);
	void updateLLegJointsBuffer(AllSensorValuesMessage asvm);
	void updateRLegJointsBuffer(AllSensorValuesMessage asvm);
	void updateInertialBuffer(AllSensorValuesMessage asvm);
	void updateFSRsBuffer(AllSensorValuesMessage asvm);
	void updateHeadJointsTable();
	void updateLArmJointsTable();
	void updateRArmJointsTable();
	void updateLLegJointsTable();
	void updateRLegJointsTable();
	void updateInertialTable();
	void updateFSRsTable();



	boost::circular_buffer<HeadJoints> headJointsBuffer;
	boost::circular_buffer<ArmJoints> LArmJointsBuffer;
	boost::circular_buffer<ArmJoints> RArmJointsBuffer;
	boost::circular_buffer<LegJoints> LLegJointsBuffer;
	boost::circular_buffer<LegJoints> RLegJointsBuffer;
	boost::circular_buffer<InertialValues> InertialBuffer;
	boost::circular_buffer<FSRValues> FSRsBuffer;

	QList<QComboBox*> parentTablesList;
	QString currentHost;

};
#endif /* LSDCONTROLLER_H_ */

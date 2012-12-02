#ifndef SENSORSCONTROLLER_H
#define SENSORSCONTROLLER_H

#include <QComboBox>
#include <QList>
#include <QWidget>

#include "HostsComboBox.h"
#include "messages/SensorsMessage.pb.h"
#include <boost/circular_buffer.hpp>

namespace Ui {
class SensorsController;
}

class SensorsController : public QWidget {
	Q_OBJECT

public:
	explicit SensorsController (QWidget *parent = 0);
	~SensorsController();

	Ui::SensorsController *getLSDControllerUi() {
		return ui;
	}

signals:
	void SubscriptionRequest (QString);
public slots:
	void addComboBoxItem (QString, QString);
	void removeComboBoxItem (QString);
	void setGameStateInfo (QString, QString, QString);
	void changeToHost (QString);
	void SubscriptionHandler (QString);

	void sensorsDataUpdateHandler (AllSensorValuesMessage, QString);

private:
	void clearComboLists();
	void updateHeadJointsBuffer (AllSensorValuesMessage);
	void updateLArmJointsBuffer (AllSensorValuesMessage asvm);
	void updateRArmJointsBuffer (AllSensorValuesMessage asvm);
	void updateLLegJointsBuffer (AllSensorValuesMessage asvm);
	void updateRLegJointsBuffer (AllSensorValuesMessage asvm);
	void updateInertialBuffer (AllSensorValuesMessage asvm);
	void updateFSRsBuffer (AllSensorValuesMessage asvm);
	void updateHeadJointsTable();
	void updateLArmJointsTable();
	void updateRArmJointsTable();
	void updateLLegJointsTable();
	void updateRLegJointsTable();
	void updateInertialTable();
	void updateFSRsTable();


	HostsComboBox *availableLSDHosts;
	Ui::SensorsController *ui;
private:
	typedef struct joints {

		QString yaw;
		QString pitch;

	} HeadJoints;

	typedef struct joints1 {

		QString ShoulderPitch;
		QString ShoulderRoll;
		QString ElbowYaw;
		QString ElbowRoll;

	} ArmJoints;

	typedef struct joints2 {

		QString HipYawPitch;
		QString HipRoll;
		QString HipPitch;
		QString KneePitch;
		QString AnklePitch;
		QString AnkleRoll;

	} LegJoints;

	typedef struct values {

		QString AccZvalue;
		QString AccXvalue;
		QString AccYvalue;
		QString angX;
		QString angY;

	} InertialValues;

	typedef struct morevalues {

		QString LFsrFL;
		QString LFsrRL;
		QString LFsrFR;
		QString LFsrRR;
		QString RFsrFL;
		QString RFsrRL;
		QString RFsrFR;
		QString RFsrRR;

	} FSRValues;
	boost::circular_buffer<HeadJoints> headJointsBuffer;
	boost::circular_buffer<ArmJoints> LArmJointsBuffer;
	boost::circular_buffer<ArmJoints> RArmJointsBuffer;
	boost::circular_buffer<LegJoints> LLegJointsBuffer;
	boost::circular_buffer<LegJoints> RLegJointsBuffer;
	boost::circular_buffer<InertialValues> InertialBuffer;
	boost::circular_buffer<FSRValues> FSRsBuffer;
};
#endif /* SensorsController_H_ */

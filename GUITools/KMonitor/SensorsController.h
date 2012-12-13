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
	void updateHeadJointsTable(AllSensorValuesMessage asvm);
	void updateLArmJointsTable(AllSensorValuesMessage asvm);
	void updateRArmJointsTable(AllSensorValuesMessage asvm);
	void updateLLegJointsTable(AllSensorValuesMessage asvm);
	void updateRLegJointsTable(AllSensorValuesMessage asvm);
	void updateInertialTable(AllSensorValuesMessage asvm);
	void updateFSRsTable(AllSensorValuesMessage asvm);


	HostsComboBox *availableLSDHosts;
	Ui::SensorsController *ui;
private:

};
#endif /* SensorsController_H_ */

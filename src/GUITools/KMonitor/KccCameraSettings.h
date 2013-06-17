#ifndef KCCCAMERASETTINGS_H
#define KCCCAMERASETTINGS_H

#include <QDialog>
#include <QString>
#include <QTimer>
#include <map>
#include <string>
#include <vector>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "messages/ExternalCommunication.pb.h"

namespace Ui {
    class KccCameraSettings;
}

class KccCameraSettings : public QDialog
{
    Q_OBJECT

public:
    explicit KccCameraSettings(QWidget *parent = 0);
    ~KccCameraSettings();

signals:
	void iAmClosing();
	void sendCameraCalibrationMessage(CameraCalibration msg);

public slots:
	void saveToLocalConfigurator();
	void gainSliderMoved(int);
	void gainTextChanged(QString);
	void contrastSliderMoved(int);
	void contrastTextChanged(QString);
	void redBalanceSliderMoved(int);
	void redBalanceTextChanged(QString);
	void blueBalanceSliderMoved(int);
	void blueBalanceTextChanged(QString);
	void greenGainSliderMoved(int);
	void greenGainTextChanged(QString);
	void closeEvent(QCloseEvent *event);
	void genericAckReceived(GenericACK ack, QString hostid);
	void sendCalibrationMessage();
	void retransmitMessage();

private:
    Ui::KccCameraSettings *ui;
	QTimer *timer;
	CameraCalibration outmsg, lastmsg;
	static const int MAX_RETRANSMITS = 20;
	// variables for the ack method
	bool lastMessageACKed;
	int numOfRetransmits;
	boost::posix_time::ptime timestamp;
};

#endif // KCCCAMERASETTINGS_H

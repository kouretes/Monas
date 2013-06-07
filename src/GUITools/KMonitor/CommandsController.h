#ifndef COMMANDSCONTROLLER_H
#define COMMANDSCONTROLLER_H

#include <QWidget>
#include <vector>
#include <map>
#include <string>
#include <QDebug>
#include <QTimer>
#include <QList>
#include <QFileDialog>
#include <QString>
#include <fstream>
#include <QGroupBox>

#include <boost/filesystem.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "HostsComboBox.h"

#include "messages/ExternalCommunication.pb.h"

/**
 * Code for the commander handler GUI tab of KMonitor
 * @author <a href="mailto:nikofinas@gmail.com">Nikos Kofinas</a> aka eldr4d, 2012 kouretes team
 * \file XMLHandler.h
*/

namespace Ui {
	class CommandsController;
}

class CommandsController : public QWidget {
	Q_OBJECT

public:
	explicit CommandsController(QWidget *parent = 0);
	~CommandsController();

signals:
	void SubscriptionRequest(QString);
	void sendExternalCommand(ExternalCommand msg);

public slots:
	void addComboBoxItem(QString, QString);
	void removeComboBoxItem(QString);
	void setGameStateInfo(QString, QString, QString);
	void changeToHost(QString);
	void SubscriptionHandler(QString);
	void genericAckReceived(GenericACK ack, QString hostid);

private slots:
	void retransmitMessage();
	void sendAction();
	void sendLocReset();
	void sendStopRobot();
	void sendShutDown();
	void sendStiffnessOff();
	void sendSit();
	void sendInitial();
	void sendReady();
	void sendSet();
	void sendPlay();
	void sendPenalised();
	void sendFinish();
	void sendOverride();
	void sendStiffness();
	void sendHeadScan();
	void sendHeadSmartScan();
	void sendHeadLocalize();
	void sendHeadLocalizeFar();
	void sendHeadLookDown();
	void sendHeadStop();
	void stiffSliderMoved(int);
	void stiffTextChanged(QString);
	void headSliderMoved(int);
	void headTextChanged(QString);

private:
	void addActionComboBoxItems(std::string path);
	void sendHeadMessage(int headCommandId);
	void sendOutMsg();

private:
	static const float maxHeadSpeed = 1.8;
	static const float minHeadSpeed = 0.5;
	static const int MAX_RETRANSMITS = 20;
	Ui::CommandsController *ui;
	QTimer *timer;
	HostsComboBox *availableCommandHosts;

	ExternalCommand outmsg, lastmsg;

	// variables for the ack method
	bool lastMessageACKed;
	int numOfRetransmits;
	boost::posix_time::ptime timestamp;
};

#endif // COMMANDSCONTROLLER_H

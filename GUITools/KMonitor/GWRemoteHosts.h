#ifndef GWREMOTEHOSTS_H_
#define GWREMOTEHOSTS_H_

#include <QTreeWidget>

#include "messages/Network.pb.h"
#include "messages/Gamecontroller.pb.h"

typedef struct Elements{

	QString hostId;
	QString hostName;
	bool requestedPosition;
	bool requestedBall;

	Elements()
	{
			hostId = QString::QString("");
			hostName = QString::QString("");
			requestedPosition = false;
			requestedBall = false;
	}

	Elements(QString host,QString name, bool position, bool ball)
	{
		hostId = host;
		hostName = name;
		requestedPosition = position;
		requestedBall = ball;
	}

	void setPositionRequest(bool request)
	{
		requestedPosition = request;
	}

	void setBallRequest(bool request)
	{
		requestedBall = request;
	}

}requestedElements;

class GWRemoteHosts : public QObject
{
    Q_OBJECT

public:
    GWRemoteHosts(QTreeWidget *parent = 0);

public slots:
	void emergeAvailableHosts(KnownHosts newHosts);
	void setGWRHGameStateInfo(GameStateMessage gsm, QString hostId);

signals:
	void GWRHSubscriptionRequest(QString);
	void GWRHUnsubscriptionRequest(QString);
	void GWRHSetRobotVisible(QString, bool);
	void GWRHSetBallVisible(QString, bool);

	// for combo Box manipulation
	void GWRHNewHostAdded(QString, QString);
	void GWRHOldHostRemoved(QString);
	void LWRHGameStateMsgUpdate(QIcon, QString, QString);

private slots:
	void mainCheckBoxHandler(int state);
	void subCheckBox1Handler(int state);
	void subCheckBox2Handler(int state);

private:
	QString GWhostNameFinder(QString hostId);
	void removeDisconnectedHosts( KnownHosts newHosts );
	void removeTreeWidgetItem(QTreeWidgetItem* item);
	void addTreeWidgetItem(int position, QString hostId, QString hostName);
	QTreeWidgetItem* GWhostFinder(QString hostId);
	void printGWRequests();


	QList<requestedElements*> GWRequests;
	QTreeWidget *parentTreeWidget;

};
#endif /* GWREMOTEHOSTS_H_ */

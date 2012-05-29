#ifndef LWREMOTEHOSTS_H
#define LWREMOTEHOSTS_H

#include <QtGui/QComboBox>
#include <QString>

typedef struct LWElements{

	QString hostId;
	QString hostName;
	bool hostSelected;


	LWElements()
	{
			hostId = QString::QString("");
			hostName = QString::QString("");
			hostSelected = false;
	}

	LWElements(QString host,QString name, bool selected)
	{
		hostId = host;
		hostName = name;
		hostSelected = selected;
	}

}requestedLWElements;

class LWRemoteHosts : public QComboBox
{
    Q_OBJECT

public:
    LWRemoteHosts(QComboBox *parent = 0);
    ~LWRemoteHosts();

public slots:
	void addComboBoxItem(QString hostId, QString hostName);
	void removeComboBoxItem(QString hostId);
	void setLWRHGameStateInfo(QIcon icon, QString gsm, QString hostId);

signals:
	void LWRHSubscriptionRequest(QString);
	void LWRHUnsubscriptionRequest(QString);

private slots:
	void newLWRemoteHostSelected(int index);

private:
	void printLWRequests();
	int LWhostFinder(QString hostId);


    QComboBox* parentComboBox;
    QList<requestedLWElements*> LWRequests;
    QString myCurrentRequestedHost;


};

#endif // LWREMOTEHOSTS_H

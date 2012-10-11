#ifndef HOSTSCOMBOBOX_H
#define HOSTSCOMBOBOX_H

#include <QtGui/QComboBox>
#include <QString>



class HostsComboBox : public QComboBox
{
    Q_OBJECT

public:
    HostsComboBox(QComboBox *parent = 0);
    ~HostsComboBox();

public slots:
	void addComboBoxItem(QString hostId, QString hostName);
	void removeComboBoxItem(QString hostId);
	void setLWRHGameStateInfo(QString iconPath, QString gsm, QString hostId);

signals:
	void LWRHSubscriptionRequest(QString);
	void LWRHUnsubscriptionRequest(QString);

private slots:
	void newHostsComboBoxelected(int index);

private:
	typedef struct LWElements{

		QString hostId;
		QString hostName;
		bool hostSelected;


		LWElements()
		{
			hostId = QString("");
			hostName = QString("");
			hostSelected = false;
		}

		LWElements(QString host,QString name, bool selected)
		{
			hostId = host;
			hostName = name;
			hostSelected = selected;
		}

	}requestedLWElements;

	void printLWRequests();
	int LWhostFinder(QString hostId);


    QComboBox* parentComboBox;
    QList<requestedLWElements*> LWRequests;
    QString myCurrentRequestedHost;
};

#endif // HostsComboBox_H

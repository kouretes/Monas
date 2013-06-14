#ifndef HOSTSCOMBOBOX_H
#define HOSTSCOMBOBOX_H

#ifdef QT_VERSION_5
#include <QtWidgets/QComboBox>
#else
#include <QtGui/QComboBox>
#endif
#include <QString>


class HostsComboBox : public QComboBox {
	Q_OBJECT

public:
	HostsComboBox(QComboBox *parent = 0);
	~HostsComboBox();

public:
	void addComboBoxItem(QString hostId, QString hostName);
	void removeComboBoxItem(QString hostId);
	void setGameStateInfo(QString iconPath, QString gsm, QString hostId);
	void changeItem(QString hostId);

signals:
	void SubscriptionRequest(QString);

private slots:
	void newHostsComboBoxSelected(int index);

private:

	typedef struct Elements {

		QString hostId;
		QString hostName;
		bool hostSelected;

		Elements() {
			hostId = QString("");
			hostName = QString("");
			hostSelected = false;
		}

		Elements(QString host, QString name, bool selected) {
			hostId = host;
			hostName = name;
			hostSelected = selected;
		}

	} requestedElements;

	void printRequests();
	int HostFinder(QString hostId);

	QComboBox *parentComboBox;
	QList<requestedElements*> Requests;
};

#endif // HostsComboBox_H

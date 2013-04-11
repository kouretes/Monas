#ifndef XMLHandler_H
#define XMLHandler_H

#include <QWidget>
#include <vector>
#include <map>
#include <string>
#include <QDebug>
#include <QTimer>
#include <QList>
#include <QFileDialog>
#include <math.h>
#include <QString>
#include <fstream>
#include <QTreeWidgetItem>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "HostsComboBox.h"
#include "core/architecture/XmlManager/XmlManager.h"
#include "messages/GUICommunication.pb.h"

/**
 * Code for the xml handler GUI tab of KMonitor
 * @author <a href="mailto:nikofinas@gmail.com">Nikos Kofinas</a> aka eldr4d, 2012 Kouretes Team
 * \file XMLHandler.h
 */

namespace Ui {
	class XMLHandler;
}

class XMLHandler : public QWidget {
	Q_OBJECT

public:
	explicit XMLHandler(QWidget *parent = 0);
	~XMLHandler();

	Ui::XMLHandler* getXMLUi() {
		return ui;
	}

signals:
	void SubscriptionRequest(QString);
	void sendConfigMessage(ExternalConfig msg);

public slots:
	void addComboBoxItem(QString, QString);
	void removeComboBoxItem(QString);
	void setGameStateInfo(QString, QString, QString);
	void changeToHost(QString);
	void SubscriptionHandler(QString);
	void genericAckReceived(GenericACK ack, QString hostid);

private slots:
	void retransmitMessage();
	void pbHandOfPressed();
	void sendPressed();
	void editItemOrNot(QTreeWidgetItem *item, int col);
	void itemChanged(QTreeWidgetItem *item, int col);
	void selectActivities(QTreeWidgetItem *item, int col);
	void changeCt();
	
private:
	void addChildsRecursive(QTreeWidgetItem *parent, QString name, QString data, XmlManagerNode *currentNode, std::string currentKey);
	void addAttributeChild(QTreeWidgetItem *parent, QString name, QString data, std::string currentKey);
	void initializeActivitiesTree();
	void updateTreeStructure(std::string headID, std::string bodyID);
	void updateXMLFiles();

	static const int MAX_RETRANSMITS = 15;

	HostsComboBox *availableXMLHosts;
	Ui::XMLHandler *ui;
	QTimer *timer;
	XmlManager xmlStructure;

	std::string headid;
	std::string bodyid;

	std::map<std::string, std::string> changes;
	QString	oldValue;

	int oldChecksum;

	ExternalConfig outmsg, lastmsg;

	// variables for the ack method
	bool lastMessageACKed;
	int numOfRetransmits;
	boost::posix_time::ptime timestamp;
};

#endif // XMLHandler_H

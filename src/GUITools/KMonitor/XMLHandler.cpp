#include "XMLHandler.h"
#include "ui_XMLHandler.h"
#include "tools/toString.h"
#include <sstream>

using namespace std;

XMLHandler::XMLHandler(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::XMLHandler) {
	
	ui->setupUi(this);
	availableXMLHosts = new HostsComboBox(ui->XMLComboBox);
	connect(availableXMLHosts, SIGNAL(SubscriptionRequest(QString)), this, SLOT(SubscriptionHandler(QString)) );
	connect(ui->pushHandOff, SIGNAL(clicked()), this, SLOT(pbHandOfPressed()) );
	connect(ui->sendpb, SIGNAL(clicked()), this, SLOT(sendPressed()) );
	connect(ui->changePb, SIGNAL(clicked()), this, SLOT(changeCt()) );
	ui->sendpb->setEnabled(false);
	
	// initialize main tree
	connect(ui->mainTree, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(editItemOrNot(QTreeWidgetItem *, int)) );
	connect(ui->mainTree, SIGNAL(itemChanged(QTreeWidgetItem *, int)), this, SLOT(itemChanged(QTreeWidgetItem *, int)) );
	ui->mainTree->blockSignals(true);
	ui->mainTree->setColumnCount(3);
	ui->mainTree->setHeaderLabels(QStringList() << "Name" << "Value" << "key");
	QTreeWidgetItem *item = new QTreeWidgetItem(ui->mainTree);
	item->setText(0, "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	item->setText(1, "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	item->setText(2, "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

	for(int i = 0 ; i < 3 ; i++) 
		ui->mainTree->resizeColumnToContents(i);

	ui->mainTree->takeTopLevelItem(0);
	ui->mainTree->blockSignals(false);
	
	// initialize activities tree
	ui->activitiesTree->setHeaderLabel("Reset activities list");
	ui->activitiesTree->resizeColumnToContents(1);
	ui->activitiesTree->takeTopLevelItem(0);
	connect(ui->activitiesTree, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(selectActivities(QTreeWidgetItem *, int)) );
	ui->status->setText("");
	ui->status2->setText("Waiting for handoff");
	updateTreeStructure("", "");
	
	// timer for the acks
	timer = new QTimer();
	timer->setInterval(200);
	connect(this->timer, SIGNAL(timeout()), this, SLOT(retransmitMessage()) );
	bodyid = "";
	headid = "";
	lastMessageACKed = true;
	numOfRetransmits = 0;
	timestamp = boost::posix_time::microsec_clock::universal_time();
}

void XMLHandler::updateTreeStructure(string headID, string bodyID) {
	ui->mainTree->blockSignals(true);
	ui->mainTree->clear();
	Configurator::Instance().initConfigurator("../../../../config", headID, bodyID);

	for(map<string, vector<ConfigNode> >::iterator kit = Configurator::Instance().root.kids.begin() ; kit != Configurator::Instance().root.kids.end() ; ++kit) {
		addChildsRecursive(ui->mainTree->invisibleRootItem(), QString((*kit).first.c_str()), QString(" "), &((*kit).second.front()), (*kit).first.c_str() );
	}

	initializeActivitiesTree();
	changes.clear();
	ui->ctLabel->setText(QString::fromStdString(Configurator::Instance().findValueForKey("vision.SegmentationBottom")) );
	ui->mainTree->blockSignals(false);
}

void XMLHandler::addChildsRecursive(QTreeWidgetItem *parent, QString name, QString data, ConfigNode *currentNode, string currentKey) {
	QTreeWidgetItem *item = new QTreeWidgetItem(parent);
	item->setText(0, name);
	item->setText(1, data);
	item->setText(2, QString::fromStdString(currentKey) );
	item->setFlags(item->flags() | Qt::ItemIsEditable);

	for(map<string, string>::iterator attr = currentNode->attributes.begin() ; attr != currentNode->attributes.end() ; ++attr) {
		string tempKey = currentKey;
		tempKey.append(".$" +(*attr).first);
		addAttributeChild(item, QString::fromStdString("$" +(*attr).first), QString::fromStdString((*attr).second), tempKey);
	}

	for(map<string, vector<ConfigNode> >::iterator kit = currentNode->kids.begin() ; kit != currentNode->kids.end() ; ++kit) {
		int kidNum = 0;

		for(vector<ConfigNode>::iterator vit =(*kit).second.begin() ; vit !=(*kit).second.end() ; ++vit) {
			QString text = " ";

			if((*vit).text.size() != 0) {
				text = QString::fromStdString((*vit).text);
			}

			string tempKey = currentKey;
			tempKey.append(string(".") + string((*kit).first) + string("~") + _toString(kidNum));
			kidNum++;
			addChildsRecursive(item, QString::fromStdString((*kit).first), text, &(*vit), tempKey);
		}
	}
}

void XMLHandler::addAttributeChild(QTreeWidgetItem *parent, QString name, QString data, string currentKey) {
	QTreeWidgetItem *item = new QTreeWidgetItem(parent);
	item->setText(0, name);
	item->setText(1, data);
	item->setText(2, QString::fromStdString(currentKey) );
	item->setFlags(item->flags() | Qt::ItemIsEditable);
}

void XMLHandler::initializeActivitiesTree() {
	int numOfAgents = Configurator::Instance().numberOfNodesForKey("agents.agent");
	ui->activitiesTree->clear();

	for(int i = 0 ; i < numOfAgents ; i++) {
		if(Configurator::Instance().findValueForKey(string("agents.agent~") + string(_toString(i) ) + ".$Enable").compare("1") != 0) {
			continue;
		}

		QTreeWidgetItem *parent = new QTreeWidgetItem(ui->activitiesTree);
		QString agentName = QString::fromStdString(Configurator::Instance().findValueForKey(string("agents.agent~") + string(_toString(i)) + ".name"));
		parent->setText(0, agentName);
		parent->setFlags(parent->flags() & ~Qt::ItemIsSelectable);
		int numOfActivities = Configurator::Instance().numberOfNodesForKey(string("agents.agent~") + string(_toString(i) ) + ".activity");

		for(int j = 0 ; j < numOfActivities ; j++) {
			if(Configurator::Instance().findValueForKey(string("agents.agent~") + string(_toString(i) ) + string(".activity~") + _toString(j) + ".$Enable").compare("1") == 0) {
				QTreeWidgetItem *child = new QTreeWidgetItem(parent);
				child->setFlags(child->flags() & ~Qt::ItemIsSelectable);
				QString activityName = QString::fromStdString(Configurator::Instance().findValueForKey(string("agents.agent~") + string(_toString(i)) + string(".activity~") + _toString(j)));
				child->setText(0, activityName);
				child->setText(1, "0");
			}
		}
	}

	int numOfStatecharts = Configurator::Instance().numberOfNodesForKey("agents.statechart");

	if(numOfStatecharts != 0) {
		QTreeWidgetItem *parent = new QTreeWidgetItem(ui->activitiesTree);
		QString statechart = "Statecharts";
		parent->setText(0, statechart);
		parent->setFlags(parent->flags() & ~Qt::ItemIsSelectable);
		int enableStates = 0;

		for(int i = 0 ; i < numOfStatecharts ; i++) {
		
			if(Configurator::Instance().findValueForKey(string("agents.statechart~") + _toString(i) + ".$Enable").compare("1") != 0)
				continue;

			enableStates++;
			QTreeWidgetItem *child = new QTreeWidgetItem(parent);
			child->setFlags(child->flags() & ~Qt::ItemIsSelectable);
			QString statechartName = QString::fromStdString(Configurator::Instance().findValueForKey(string("agents.statechart~") + _toString(i)));
			child->setText(0, statechartName);
			child->setText(1, "0");
		}

		if(enableStates == 0)
			ui->activitiesTree->takeTopLevelItem(ui->activitiesTree->topLevelItemCount() - 1);
	}

	ui->activitiesTree->expandAll();
	ui->activitiesTree->setItemsExpandable(false);
	ui->activitiesTree->setRootIsDecorated(false);
}

void XMLHandler::pbHandOfPressed() {
	outmsg.clear_updatexml();
	outmsg.clear_resetactivities();
	outmsg.clear_file();
	
	// target host is be setting to the KGuiMessenger class
	lastMessageACKed = false;
	ui->pushHandOff->setEnabled(false);
	timer->start();
	timestamp = boost::posix_time::microsec_clock::universal_time();
	string messageid = boost::posix_time::to_iso_string(timestamp);
	outmsg.set_messageid(messageid);
	outmsg.set_handoffrequest(true);
	lastmsg = outmsg;
	emit sendConfigMessage(outmsg);
}

void XMLHandler::genericAckReceived(GenericACK ack, QString hostid) {

	if(boost::posix_time::from_iso_string(ack.messageid() ) == timestamp && !lastMessageACKed) {
		timer->stop();
		numOfRetransmits = 0;
		lastMessageACKed = true;

		if(ack.ownlock()) {
			if(ack.has_handshaking()) {
				HandShake hs = ack.handshaking();
				headid = hs.headid();
				bodyid = hs.bodyid();
				updateTreeStructure(headid, bodyid);
				oldChecksum = Configurator::Instance().getChecksum();

				if(oldChecksum == hs.checksum()) {
					ui->status->setText("succeeded");
					ui->status2->setText("");
					ui->sendpb->setEnabled(true);
				} 
				else {
					ui->status->setText("different checksum");
					ui->sendpb->setEnabled(false);
				}
			} 
			else {
				ui->status2->setText("success");
				ui->sendpb->setEnabled(true);
			}

			updateXMLFiles();
			ui->ctLabel->setText(QString::fromStdString(Configurator::Instance().findValueForKey("vision.SegmentationBottom") ) );
		} 
		else {
			ui->status->setText("Locked owned by other GUI");
			ui->status2->setText("Waiting for handoff");
			ui->pushHandOff->setEnabled(true);
			ui->sendpb->setEnabled(false);
		}
	}
}

void XMLHandler::retransmitMessage() {

	numOfRetransmits++;

	if(numOfRetransmits != MAX_RETRANSMITS) {
		emit sendConfigMessage(lastmsg);
	}
	else {
		numOfRetransmits = 0;
		timer->stop();
		ui->pushHandOff->setEnabled(true);
		ui->sendpb->setEnabled(false);
		ui->status->setText("failed");
		ui->status2->setText("hand off required");
		lastMessageACKed = true;
		timestamp = boost::posix_time::microsec_clock::universal_time();
	}
}

void XMLHandler::editItemOrNot(QTreeWidgetItem *item, int col) {
	
	ui->mainTree->blockSignals(true);

	if(col == 1 && item->text(col) != " ") {
		oldValue = item->text(col);
		item->setFlags(item->flags() | Qt::ItemIsEditable);
	} 
	else
		item->setFlags(item->flags() & ~Qt::ItemIsEditable);

	ui->mainTree->blockSignals(false);
}

void XMLHandler::selectActivities(QTreeWidgetItem *item, int col) {

	if(item->childCount() != 0) {
		for(int i = 0 ; i != item->childCount() ; i++) {
			item->child(i)->setBackground(0, *(new QBrush(Qt::green) ) );
			item->child(i)->setText(1, "1");
		}
	} 
	else {
		if(item->text(1) == "0") {
			item->setBackground(0, *(new QBrush(Qt::green)));
			item->setText(1, "1");
		} 
		else {
			item->setBackground(0, item->background(1));
			item->setText(1, "0");
		}
	}
}

void XMLHandler::itemChanged(QTreeWidgetItem *item, int col) {

	if(item->text(1) != "" && item->text(1) != " ")
		changes[item->text(2).toStdString() ] = item->text(1).toStdString();
	else
		item->setText(1, oldValue);
}

void XMLHandler::sendPressed() {

	outmsg.clear_updatexml();
	outmsg.clear_resetactivities();
	outmsg.clear_file();
	int i = 0;

	for(map<string, string>::iterator iter = changes.begin() ; iter != changes.end() ; iter++) {
		outmsg.add_updatexml();
		outmsg.mutable_updatexml(i)->set_keyword(iter->first);
		outmsg.mutable_updatexml(i)->set_value(iter->second);
		i++;
	}

	QTreeWidgetItem *first = ui->activitiesTree->invisibleRootItem();

	if(first->childCount() != 0) {
		for(int i = 0 ; i != first->childCount() ; i++) {
			for(int j = 0 ; j != first->child(i)->childCount() ; j++) {
				if(first->child(i)->child(j)->text(1).compare("1") == 0)
					outmsg.add_resetactivities(first->child(i)->child(j)->text(0).toStdString());
			}
		}
	}

	if(ui->ctCB->isChecked()) {
		string path = Configurator::Instance().getDirectoryPath() + "colortables/";
		path.append(ui->ctLabel->text().toStdString() );
		string filepath = "colortables/" + ui->ctLabel->text().toStdString();
		ifstream ctfile(path.c_str());

		if(ctfile.is_open()) {
			string strfile;
			ctfile.seekg(0, ios::end);
			strfile.reserve(ctfile.tellg());
			ctfile.seekg(0, ios::beg);
			strfile.assign((istreambuf_iterator<char>(ctfile)), istreambuf_iterator<char>());
			ctfile.close();
			outmsg.mutable_file()->set_file(strfile);
			outmsg.mutable_file()->set_filepath(filepath);
			ui->ctCB->setChecked(false);
		}
	}

	if((outmsg.updatexml_size() != 0 || outmsg.resetactivities_size() != 0 || outmsg.has_file() ) && lastMessageACKed) {
		
		// target host is be setting to the KGuiMessenger class
		lastMessageACKed = false;
		ui->sendpb->setEnabled(false);
		ui->status2->setText("waiting");
		timer->start();
		outmsg.set_handoffrequest(false);
		timestamp = boost::posix_time::microsec_clock::universal_time();
		string messageid = boost::posix_time::to_iso_string(timestamp);
		outmsg.set_messageid(messageid);
		lastmsg = outmsg;
		emit sendConfigMessage(outmsg);
	}
}

void XMLHandler::updateXMLFiles() {

	if(changes.size() != 0) {
		vector<pair<string, string> > dataForWrite;

		for(map<string, string>::iterator iter = changes.begin() ; iter != changes.end() ; iter++) {
			pair<string, string> temp;
			temp.first = iter->first;
			temp.second = iter->second;
			dataForWrite.push_back(temp);
		}

		changes.clear();
		Configurator::Instance().burstWrite(dataForWrite);
	}
}

void XMLHandler::changeCt() {
	string path = Configurator::Instance().getDirectoryPath() + "colortables";
	QString filename = QFileDialog::getOpenFileName(this, tr("Choose Segmentation File"), QString::fromStdString(path), tr("Segmentation Files(*.conf)"));

	if(filename != "") {
		int found = filename.toStdString().find_last_of("/");
		string strfilename = filename.toStdString().substr(found + 1);
		ui->ctLabel->setText(QString::fromStdString(strfilename));
	}
}

void XMLHandler::changeToHost(QString data1) {
	availableXMLHosts->changeItem(data1);
}

void XMLHandler::addComboBoxItem(QString data1, QString data2) {
	availableXMLHosts->addComboBoxItem(data1, data2);
}

void XMLHandler::removeComboBoxItem(QString data1) {
	availableXMLHosts->removeComboBoxItem(data1);
}

void XMLHandler::setGameStateInfo(QString data1, QString data2, QString data3) {
	availableXMLHosts->setGameStateInfo(data1, data2, data3);
}

void XMLHandler::SubscriptionHandler(QString data1) {
	ui->pushHandOff->setEnabled(true);
	ui->sendpb->setEnabled(false);
	ui->status->setText("failed");
	ui->status2->setText("hand off required");
	emit SubscriptionRequest(data1);
}

XMLHandler::~XMLHandler() {
	delete ui;
}

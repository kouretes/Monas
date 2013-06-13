#include "KGUIMessenger.h"

#include "tools/toString.h"

#include "core/architecture/messaging/network/multicastpoint.hpp"
#include "core/architecture/messaging/MessageBuffer.hpp"
#include "core/architecture/messaging/TopicTree.hpp"
#include "core/architecture/configurator/Configurator.hpp"

#include "hal/robot/generic_nao/robot_consts.h"

using std::string;
using namespace std;

KGUIMessenger::KGUIMessenger() : multicast(NULL), timer(NULL) {

	string multicastip;
	unsigned int port;
	unsigned maxpayload;
	unsigned beacon_interval;

	
    multicastip = Configurator::Instance().findValueForKey("network.multicast_ip");
	port = atoi(Configurator::Instance().findValueForKey("network.multicast_port").c_str());
    maxpayload = atoi(Configurator::Instance().findValueForKey("network.maxpayload").c_str());
    beacon_interval = atoi(Configurator::Instance().findValueForKey("network.beacon_interval").c_str());
    cout << "Initiating multicast network at address: " << multicastip << ":" << port << std::endl;
	KNetwork::MulticastPoint *m = new KNetwork::MulticastPoint(multicastip, maxpayload);
	m->setCleanupAndBeacon(beacon_interval);
	m->attachTo(*this);

	if(m->startEndPoint(multicastip, port) == false) {
		delete m;
		_exit(-1);
	}

	multicast = m;

	timer = new QTimer();
	timer->setInterval(50);
	connect(this->timer, SIGNAL(timeout()), this, SLOT(allocateReceivedMessages()) );
	timer->start();
	currentKMonitorTab = 0;
	myGWRequestedHosts.clear();
	myKMonitorRequestedHost.clear();
}

KGUIMessenger::~KGUIMessenger() {
	if(multicast != NULL)
		delete multicast;
}

MessageBuffer *KGUIMessenger::makeWriteBuffer(std::string const &s) {
	size_t newid = pubsubRegistry.registerNew(s);
	MessageBuffer *new_msg_buf = new MessageBuffer(newid);
	return new_msg_buf;
}

MessageBuffer *KGUIMessenger::makeReadBuffer(std::string const &s) {
	size_t newid = pubsubRegistry.registerNew(s);
	MessageBuffer *new_msg_buf = new MessageBuffer(newid);
	return new_msg_buf;
}

void KGUIMessenger::allocateReceivedMessages() {

	QString currentRHost;
	std::vector<msgentry> incomingMessages = multicast->getWriteBuffer()->remove();

	for(unsigned i = 0 ; i < incomingMessages.size() ; i++) {
		if(incomingMessages.at(i).msg != NULL) {
			currentRHost.clear();
			currentRHost = QString::fromStdString(_toString(incomingMessages.at(i).host) );

			if(incomingMessages.at(i).msg->GetTypeName() == "KnownHosts") {
				KnownHosts myRemoteHosts;
				myRemoteHosts.Clear();
				myRemoteHosts.CopyFrom(*(incomingMessages.at(i).msg));
				updateKnownHosts(myRemoteHosts);
			} 
			else if(incomingMessages.at(i).msg->GetTypeName() == "KRawImage" && myKMonitorRequestedHost == currentRHost) {
				KRawImage rawimg;
				rawimg.Clear();
				rawimg.CopyFrom(*(incomingMessages.at(i).msg));
				if(myKMonitorRequestedHost == currentRHost)
					emit rawImage(rawimg, currentRHost);
			} 
			else if(incomingMessages.at(i).msg->GetTypeName() == "GameStateMessage") {
				GameStateMessage gsm;
				gsm.Clear();
				gsm.CopyFrom(*(incomingMessages.at(i).msg));
				updateGameState(gsm, currentRHost);
				emit gameStateMessageUpdate(gsm, currentRHost);
			} 
			else if(incomingMessages.at(i).msg->GetTypeName() == "WorldInfo" && (myGWRequestedHosts.contains(currentRHost) ||(myKMonitorRequestedHost == currentRHost)) ) {
				WorldInfo wim;
				wim.Clear();
				wim.CopyFrom(*(incomingMessages.at(i).msg));
				emit worldInfoUpdate(wim, currentRHost);
			} 
			else if(incomingMessages.at(i).msg->GetTypeName() == "SharedWorldInfo" && (myGWRequestedHosts.contains(currentRHost) 
																								||(myKMonitorRequestedHost == currentRHost)) ) {
				SharedWorldInfo swim;
				swim.Clear();
				swim.CopyFrom(*(incomingMessages.at(i).msg));
				emit sharedWorldInfoUpdate(swim, currentRHost);
			}
			else if(incomingMessages.at(i).msg->GetTypeName() == "ObservationMessage" && myKMonitorRequestedHost == currentRHost) {
				ObservationMessage om;
				om.Clear();
				om.CopyFrom(*(incomingMessages.at(i).msg));
				emit obsmsgUpdate(om, currentRHost);
			} 
			else if(incomingMessages.at(i).msg->GetTypeName() == "LocalizationDataForGUI" && myKMonitorRequestedHost == currentRHost) {
				LocalizationDataForGUI debugGUI;
				debugGUI.Clear();
				debugGUI.CopyFrom(*(incomingMessages.at(i).msg));
				emit localizationDataUpdate(debugGUI, currentRHost);
			}  
            else if(incomingMessages.at(i).msg->GetTypeName() == "EKFMHypothesis" && myKMonitorRequestedHost == currentRHost) {
				EKFMHypothesis ekfmHypothesisM;
				ekfmHypothesisM.Clear();
				ekfmHypothesisM.CopyFrom(*(incomingMessages.at(i).msg));
				emit EKFMHypothesisUpdate(ekfmHypothesisM, currentRHost);
			}
            else if(incomingMessages.at(i).msg->GetTypeName() == "OdometryInfoMessage" && myKMonitorRequestedHost == currentRHost) {
				OdometryInfoMessage odometryM;
				odometryM.Clear();
				odometryM.CopyFrom(*(incomingMessages.at(i).msg));
				emit OdometryUpdate(odometryM, currentRHost);
			}
			else if(incomingMessages.at(i).msg->GetTypeName() == "FormationDataForGUI" && myKMonitorRequestedHost == currentRHost) {
				FormationDataForGUI debugGUI;
				debugGUI.Clear();
				debugGUI.CopyFrom(*(incomingMessages.at(i).msg));
				emit formationDataUpdate(debugGUI, currentRHost);
			} 
			else if(incomingMessages.at(i).msg->GetTypeName() == "MotionWalkMessage" && myKMonitorRequestedHost == currentRHost) {
				MotionWalkMessage mwm;
				mwm.Clear();
				mwm.CopyFrom(*(incomingMessages.at(i).msg));
				emit motionCommandUpdate(mwm, currentRHost);
			} 
			else if(incomingMessages.at(i).msg->GetTypeName() == "GridInfo" && myKMonitorRequestedHost == currentRHost) {
				GridInfo ngim;
				ngim.Clear();
				ngim.CopyFrom(*(incomingMessages.at(i).msg));
				emit gridInfoUpdate(ngim, currentRHost);
			} 
			else if(incomingMessages.at(i).msg->GetTypeName() == "AllSensorValuesMessage" && myKMonitorRequestedHost == currentRHost) {
				AllSensorValuesMessage asvm;
				asvm.Clear();
				asvm.CopyFrom(*(incomingMessages.at(i).msg));
				emit sensorsDataUpdate(asvm, currentRHost);
			} 
			else if(incomingMessages.at(i).msg->GetTypeName() == "GenericACK" && myKMonitorRequestedHost == currentRHost) {
				GenericACK gack;
				gack.Clear();
				gack.CopyFrom(*(incomingMessages.at(i).msg));
				emit GenericAckReceived(gack, currentRHost);
			}
		} 
		else
			std::cout << "Null msg from host " << incomingMessages.at(i).host << std::endl;
	}
}

void KGUIMessenger::updateKnownHosts(KnownHosts myRemoteHosts) {

	const ::google::protobuf::RepeatedPtrField<HostEntry>& rf = myRemoteHosts.entrylist();
	::google::protobuf::RepeatedPtrField<HostEntry>::const_iterator fit;
	vec tempHosts;

	for(fit = rf.begin() ; fit != rf.end() ; ++fit) {
		tempHosts.push_back((*fit).hostid());
		vec::iterator found = find(hostIds.begin(), hostIds.end(), (*fit).hostid());

		if(found != hostIds.end())
			hostIds.erase(found);
		else
			emit addHost(QString::fromStdString(_toString((*fit).hostid())), QString::fromStdString(_toString((*fit).hostname())) );
	}

	for(vec::iterator iter = hostIds.begin() ; iter < hostIds.end() ; iter++) {
		updateSubscription("global", msgentry::UNSUBSCRIBE_ALL_TOPIC, (*iter));
		emit removeHost(QString::fromStdString(_toString(*iter)));

		if(myKMonitorRequestedHost.toUInt() == *iter)
			myKMonitorRequestedHost.clear();
	}

	hostIds.clear();
	hostIds = tempHosts;
}

void KGUIMessenger::updateGameState(GameStateMessage gsm, QString hostId) {
	QString iconFile;

	if(gsm.team_color() == 0)
		iconFile = QString::fromUtf8(":/KnownHostsIcons/robot_blue.png");
	else if(gsm.team_color() == 1)
		iconFile = QString::fromUtf8(":/KnownHostsIcons/robot_red.png");

	QString playerNumber = QString::fromStdString(_toString((gsm.player_number())));
	QString teamNumber = QString::fromStdString(_toString((gsm.team_number())));
	QString playerState;

	switch(gsm.player_state()) {
	case PLAYER_INITIAL:
		playerState = QString("INITIAL");
		break;
	case PLAYER_READY:
		playerState = QString("READY");
		break;
	case PLAYER_SET:
		playerState = QString("SET");
		break;
	case PLAYER_PLAYING:
		playerState = QString("PLAYING");
		break;
	case PLAYER_FINISHED:
		playerState = QString("FINISHED");
		break;
	case PLAYER_PENALISED:
		playerState = QString("PENALISED");
		break;
	case PLAYER_DEAD:
		playerState = QString("DEAD");
		break;
	case PLAYER_LOG:
		playerState = QString("LOG");
		break;
	case PLAYER_DEMO:
		playerState = QString("DEMO");
		break;
	}

	QString stateMSG = QString(", ") + teamNumber + QString(", ") + playerNumber + QString(", ") + playerState;
	emit updateGameState(iconFile, stateMSG, hostId);
}

void KGUIMessenger::updateSubscription(std::string const &topic , msgentry::msgclass_t where, std::size_t host) {

	if(multicast->getReadBuffer() == NULL || multicast->getWriteBuffer() == NULL)
		return;

	msgentry nmsg;
	nmsg.topic = Topics::Instance().getId(topic);
	nmsg.host = host;
	nmsg.msgclass = where;
	multicast->getReadBuffer()->add(nmsg);
}

void KGUIMessenger::GWSubscriptionHandler(QString hostId) {
	myGWRequestedHosts << hostId;
	myGWRequestedHosts.removeDuplicates();
	tabChangeHandler(currentKMonitorTab);
}

void KGUIMessenger::GWUnsubscriptionHandler(QString hostId) {
	myGWRequestedHosts.removeOne(hostId);
	tabChangeHandler(currentKMonitorTab);
}

void KGUIMessenger::SubscriptionHandler(QString hostId) {
	emit changeAllTabsToHost(hostId);
	myKMonitorRequestedHost = hostId;
	tabChangeHandler(currentKMonitorTab);
}

void KGUIMessenger::XMLPublishMessage(ExternalConfig message) {
	message.set_targethost(myKMonitorRequestedHost.toUInt());
	msgentry nmsg;
	google::protobuf::Message *newptr = message.New();
	newptr->CopyFrom(message);
	nmsg.msg.reset(newptr);
	nmsg.host = msgentry::HOST_ID_LOCAL_HOST;
	nmsg.topic = Topics::Instance().getId("external");
	nmsg.msgclass = msgentry::SIGNAL;
	multicast->getReadBuffer()->add(nmsg);
}

void KGUIMessenger::CommandPublishMessage(ExternalCommand message) {
	message.set_targethost(myKMonitorRequestedHost.toUInt() );
	msgentry nmsg;
	google::protobuf::Message *newptr = message.New();
	newptr->CopyFrom(message);
	nmsg.msg.reset(newptr);
	nmsg.host = msgentry::HOST_ID_LOCAL_HOST;
	nmsg.topic = Topics::Instance().getId("external");
	nmsg.msgclass = msgentry::SIGNAL;
	multicast->getReadBuffer()->add(nmsg);
}

void KGUIMessenger::tabChangeHandler(int currentTab) {
	QString hostId;
	currentKMonitorTab = currentTab;
	updateSubscription("global", msgentry::UNSUBSCRIBE_ALL_TOPIC, msgentry::HOST_ID_ANY_HOST);

	if((myKMonitorRequestedHost.isEmpty() && currentTab != 0) ||(myGWRequestedHosts.isEmpty() && currentTab == 0))
		return;

	switch(currentTab) {

	case 0: // Global World State ((un-)sub to worldstate is absolutely defined by user's prefs)
		for(unsigned i = 0 ; i < myGWRequestedHosts.count() ; i++ ) {
			hostId = myGWRequestedHosts.at(i);
			updateSubscription("worldstate", msgentry::SUBSCRIBE_ON_TOPIC, hostId.toUInt());
		}
		break;	
	case 1: // Local World State
		updateSubscription("worldstate", msgentry::SUBSCRIBE_ON_TOPIC, myKMonitorRequestedHost.toUInt());
		updateSubscription("vision", msgentry::SUBSCRIBE_ON_TOPIC, myKMonitorRequestedHost.toUInt());
		updateSubscription("debug", msgentry::SUBSCRIBE_ON_TOPIC, myKMonitorRequestedHost.toUInt());
		updateSubscription("motion", msgentry::SUBSCRIBE_ON_TOPIC, myKMonitorRequestedHost.toUInt());
		break;	
	case 2: // Local Polar Map
		updateSubscription("pathplanning", msgentry::SUBSCRIBE_ON_TOPIC, myKMonitorRequestedHost.toUInt());
		break;
	case 3:	// Local Robot View
		updateSubscription("image", msgentry::SUBSCRIBE_ON_TOPIC, myKMonitorRequestedHost.toUInt());
		break;
	case 4:	// Local Sensors Data
		updateSubscription("sensors", msgentry::SUBSCRIBE_ON_TOPIC, myKMonitorRequestedHost.toUInt());
		break;
	case 5: // Kcc
		updateSubscription("image", msgentry::SUBSCRIBE_ON_TOPIC, myKMonitorRequestedHost.toUInt());
	case 6: // Xml
		updateSubscription("external", msgentry::SUBSCRIBE_ON_TOPIC, myKMonitorRequestedHost.toUInt());
		break;
	case 7: // Commands
		updateSubscription("external", msgentry::SUBSCRIBE_ON_TOPIC, myKMonitorRequestedHost.toUInt());
		break;
	}
}

void KGUIMessenger::printKnownHosts(KnownHosts hosts) {

	std::cout <<  "###### KGUIMessenger::KNOWNHOSTS ######" << std::endl;
	const ::google::protobuf::RepeatedPtrField< HostEntry>& rf = hosts.entrylist();
	::google::protobuf::RepeatedPtrField< HostEntry >::const_iterator fit;

	for(fit = rf.begin() ; fit != rf.end() ; ++fit) {
		cout << "My host id :: "  << (*fit).hostid() << endl;
		cout << "My hostname :: "  << (*fit).hostname() << endl;
	}

	std::cout <<  "#######################################" << std::endl << std::endl;
}

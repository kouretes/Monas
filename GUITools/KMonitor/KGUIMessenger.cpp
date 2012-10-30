#include "KGUIMessenger.h"

#include "tools/toString.h"
#include "tools/XMLConfig.h"
#include "architecture/archConfig.h"

#include "architecture/narukom/network/multicastpoint.hpp"
#include "architecture/narukom/pub_sub/message_buffer.h"
#include "architecture/narukom/pub_sub/topicTree.h"
#include "hal/robot/generic_nao/robot_consts.h"

using std::string;
using namespace std;

KGUIMessenger::KGUIMessenger() : multicast(NULL), timer(NULL)
{

	XMLConfig xmlconfig(ArchConfig::Instance().GetConfigPrefix()+"/network.xml");
	string multicastip;
	unsigned int port;
	unsigned maxpayload;
	unsigned beacon_interval;
	if(xmlconfig.QueryElement("multicast_ip", multicastip) &&
		xmlconfig.QueryElement("multicast_port", port) &&
		xmlconfig.QueryElement("maxpayload", maxpayload) &&
		xmlconfig.QueryElement("beacon_interval", beacon_interval) )
	{
		cout<<"Initiating multicast network at address: "<<multicastip<<":"<<port<<std::endl;
		KNetwork::MulticastPoint *m=new KNetwork::MulticastPoint(multicastip,maxpayload);
		m->setCleanupAndBeacon(beacon_interval);
		m->attachTo(*this);
		if(m->startEndPoint(multicastip,port)==false)
		{
			delete m;
			_exit(-1);
		}
		multicast=m;
	}
	timer = new QTimer();
	timer->setInterval(50);

	connect(this->timer, SIGNAL(timeout()), this, SLOT(allocateReceivedMessages()));

	timer->start();

	myGWRequestedHosts.clear();
	myLWRequestedHost.clear();
	myLMRequestedHost.clear();
	myLVRequestedHost.clear();
	myKccRequestedHost.clear();
	myLSRequestedHost.clear();
	myXMLRequestedHost.clear();
}

KGUIMessenger::~KGUIMessenger()
{
	if ( multicast != NULL )
		delete multicast;
}

MessageBuffer* KGUIMessenger::makeWriteBuffer(std::string const& s)
{
	size_t newid=pubsubRegistry.registerNew(s);

	MessageBuffer* new_msg_buf = new MessageBuffer ( newid);

	return new_msg_buf;
}

MessageBuffer* KGUIMessenger::makeReadBuffer(std::string const& s)
{
	size_t newid=pubsubRegistry.registerNew(s);

	MessageBuffer* new_msg_buf = new MessageBuffer ( newid);

	return new_msg_buf;
}

void KGUIMessenger::allocateReceivedMessages()
{
	//QString currentRHost = "";
	//TODO change it everywhere gypsy
	QString currentRHost;

	std::vector<msgentry> incomingMessages = multicast->getWriteBuffer()->remove();

	for( unsigned i=0; i<incomingMessages.size(); i++ )
	{
		if ( incomingMessages.at(i).msg != NULL )
		{
			currentRHost.clear();
			currentRHost = QString::fromStdString(_toString(incomingMessages.at(i).host));
			//std::cout << currentRHost.toStdString() << "::" << incomingMessages.at(i).msg->GetTypeName() << std::endl;

			if( incomingMessages.at(i).msg->GetTypeName() == "KnownHosts" )
			{
				KnownHosts myRemoteHosts;
				myRemoteHosts.Clear();
				myRemoteHosts.CopyFrom(*(incomingMessages.at(i).msg));
		
				updateKnownHosts(myRemoteHosts);
			}
			else if (incomingMessages.at(i).msg->GetTypeName()=="KRawImage" && (myLVRequestedHost == currentRHost || myKccRequestedHost == currentRHost))
			{
				//std::cout << "incomingMessages == KRawImage " << std::endl;
				KRawImage rawimg;
				rawimg.Clear();
				rawimg.CopyFrom(*(incomingMessages.at(i).msg));

				if(myLVRequestedHost == currentRHost && myKccRequestedHost == currentRHost)
				{
					emit rawImageUpdate(rawimg, currentRHost);
					emit KCCRawImageUpdate(rawimg, currentRHost);
				}
				else if(myKccRequestedHost == currentRHost)
					emit KCCRawImageUpdate(rawimg, currentRHost);
				else
					emit rawImageUpdate(rawimg, currentRHost);

			}
			else if (incomingMessages.at(i).msg->GetTypeName()=="GameStateMessage")
			{
				//std::cout << "incomingMessages == GameStateMessage " << std::endl;
				GameStateMessage gsm;
				gsm.Clear();
				gsm.CopyFrom(*(incomingMessages.at(i).msg));

				updateGameState(gsm, currentRHost);
				emit gameStateMessageUpdate(gsm, currentRHost);
			}
			else if (incomingMessages.at(i).msg->GetTypeName()=="WorldInfo" && (myGWRequestedHosts.contains(currentRHost)||(myLWRequestedHost ==currentRHost)))
			{
				//std::cout << "incomingMessages == WorldInfo " << std::endl;
				WorldInfo wim;
				wim.Clear();
				wim.CopyFrom(*(incomingMessages.at(i).msg));

				emit worldInfoUpdate(wim, currentRHost);
			}
			else if (incomingMessages.at(i).msg->GetTypeName()=="SharedWorldInfo" && (myGWRequestedHosts.contains(currentRHost)||(myLWRequestedHost ==currentRHost)))
			{
				//std::cout << "incomingMessages == WorldInfo " << std::endl;
				SharedWorldInfo swim;
				swim.Clear();
				swim.CopyFrom(*(incomingMessages.at(i).msg));

				emit sharedWorldInfoUpdate(swim, currentRHost);
			}
			else if (incomingMessages.at(i).msg->GetTypeName()=="ObservationMessage" && myLWRequestedHost == currentRHost)
			{
				//std::cout << "incomingMessages == ObservationMessage " << std::endl;
				ObservationMessage om;
				om.Clear();
				om.CopyFrom(*(incomingMessages.at(i).msg));

				emit obsmsgUpdate(om, currentRHost);
			}
			else if (incomingMessages.at(i).msg->GetTypeName()=="LocalizationDataForGUI" && myLWRequestedHost == currentRHost)
			{
				LocalizationDataForGUI debugGUI;
				debugGUI.Clear();
				debugGUI.CopyFrom(*(incomingMessages.at(i).msg));

				emit localizationDataUpdate(debugGUI, currentRHost);
			}
			else if (incomingMessages.at(i).msg->GetTypeName()=="MotionWalkMessage" && myLWRequestedHost == currentRHost)
			{
				MotionWalkMessage mwm;
				mwm.Clear();
				mwm.CopyFrom(*(incomingMessages.at(i).msg));

				emit motionCommandUpdate(mwm, currentRHost);
			}
			else if (incomingMessages.at(i).msg->GetTypeName()=="GridInfo" && myLMRequestedHost == currentRHost)
			{
				GridInfo ngim;
				ngim.Clear();
				ngim.CopyFrom(*(incomingMessages.at(i).msg));

				emit gridInfoUpdate(ngim, currentRHost);
			}
			else if (incomingMessages.at(i).msg->GetTypeName()=="AllSensorValuesMessage" && myLSRequestedHost == currentRHost)
			{
				AllSensorValuesMessage asvm;
				asvm.Clear();
				asvm.CopyFrom(*(incomingMessages.at(i).msg));

				emit sensorsDataUpdate(asvm, currentRHost);
			}
			else if (incomingMessages.at(i).msg->GetTypeName()=="GenericACK" && myXMLRequestedHost == currentRHost){
				GenericACK gack;
				gack.Clear();
				gack.CopyFrom(*(incomingMessages.at(i).msg));
				
				emit xmlGenericAckReceived(gack, currentRHost);
			}

		}
		else
		{
			std::cout << "Null msg from host " << incomingMessages.at(i).host << std::endl;
		}
	}
}

void KGUIMessenger::updateKnownHosts(KnownHosts myRemoteHosts){
	const ::google::protobuf::RepeatedPtrField< HostEntry >& rf = myRemoteHosts.entrylist();
    ::google::protobuf::RepeatedPtrField< HostEntry >::const_iterator fit;
	vec tempHosts;
    for(fit=rf.begin();fit!=rf.end();++fit)
    {
		tempHosts.push_back((*fit).hostid());
		vec::iterator found = find(hostIds.begin(), hostIds.end(), (*fit).hostid());
		if(found != hostIds.end()){
			hostIds.erase(found);
		}else{
			emit addHost(QString::fromStdString(_toString((*fit).hostid())), QString::fromStdString(_toString((*fit).hostname())));
		}
    }
    for(vec::iterator iter = hostIds.begin(); iter < hostIds.end(); iter++){
    	emit removeHost(QString::fromStdString(_toString(*iter)));
    }
    hostIds.clear();
    hostIds = tempHosts;
}

void KGUIMessenger::updateGameState(GameStateMessage gsm, QString hostId){
	QString iconFile;
	if(gsm.team_color() == 0)
	{
		iconFile = QString::fromUtf8(":/KnownHostsIcons/robot_blue.png");
	}else if (gsm.team_color() == 1)
	{
		iconFile = QString::fromUtf8(":/KnownHostsIcons/robot_red.png");
	}
	QString playerNumber = QString::fromStdString(_toString((gsm.player_number())));
	QString teamNumber = QString::fromStdString(_toString((gsm.team_number())));
	QString playerState;
	switch(gsm.player_state())
	{
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

void KGUIMessenger::updateSubscription(std::string const& topic , msgentry::msgclass_t where, std::size_t host)
{
	if(multicast->getReadBuffer()==NULL||multicast->getWriteBuffer()==NULL)
		return;
		
	msgentry nmsg;
	nmsg.topic=Topics::Instance().getId(topic);
	nmsg.host=host;
	nmsg.msgclass=where;

	multicast->getReadBuffer()->add(nmsg);
}

void KGUIMessenger::GWRHSubscriptionHandler(QString hostId)
{
	updateSubscription("worldstate",msgentry::SUBSCRIBE_ON_TOPIC, hostId.toUInt());

	myGWRequestedHosts << hostId;
	myGWRequestedHosts.removeDuplicates();

	//printMyGWRequestedHosts();
}

void KGUIMessenger::GWRHUnsubscriptionHandler(QString hostId)
{
	updateSubscription("worldstate",msgentry::UNSUBSCRIBE_ON_TOPIC, hostId.toUInt());
	myGWRequestedHosts.removeOne(hostId);

	//printMyGWRequestedHosts();
}

void KGUIMessenger::LWRHSubscriptionHandler(QString hostId)
{
	updateSubscription("worldstate",msgentry::SUBSCRIBE_ON_TOPIC,hostId.toUInt());
	updateSubscription("vision",msgentry::SUBSCRIBE_ON_TOPIC,hostId.toUInt());
	updateSubscription("debug",msgentry::SUBSCRIBE_ON_TOPIC,hostId.toUInt());
	updateSubscription("motion",msgentry::SUBSCRIBE_ON_TOPIC,hostId.toUInt());

	myLWRequestedHost = hostId;
	//printMyGWRequestedHosts();
}

void KGUIMessenger::LWRHUnsubscriptionHandler(QString hostId)
{
	if((hostId.isEmpty() && !myLWRequestedHost.isEmpty()) || (myLWRequestedHost == hostId && !hostId.isEmpty()))
	{
		updateSubscription("worldstate",msgentry::UNSUBSCRIBE_ON_TOPIC, myLWRequestedHost.toUInt());
		updateSubscription("vision",msgentry::UNSUBSCRIBE_ON_TOPIC, myLWRequestedHost.toUInt());
		updateSubscription("debug",msgentry::UNSUBSCRIBE_ON_TOPIC, myLWRequestedHost.toUInt());
		updateSubscription("motion",msgentry::UNSUBSCRIBE_ON_TOPIC, myLWRequestedHost.toUInt());

		myLWRequestedHost.clear();
	}
}

void KGUIMessenger::LMRHSubscriptionHandler(QString hostId)
{
	updateSubscription("obstacle",msgentry::SUBSCRIBE_ON_TOPIC,hostId.toUInt());
	myLMRequestedHost = hostId;
}

void KGUIMessenger::LMRHUnsubscriptionHandler(QString hostId)
{

	if((hostId.isEmpty() && !myLMRequestedHost.isEmpty()) || (myLMRequestedHost == hostId && !hostId.isEmpty()))
	{
		updateSubscription("obstacle",msgentry::UNSUBSCRIBE_ON_TOPIC, myLMRequestedHost.toUInt());
		myLMRequestedHost.clear();
	}

}

void KGUIMessenger::LVRHSubscriptionHandler(QString hostId)
{
	updateSubscription("image",msgentry::SUBSCRIBE_ON_TOPIC,hostId.toUInt());
	myLVRequestedHost = hostId;
}

void KGUIMessenger::LVRHUnsubscriptionHandler(QString hostId)
{
	if((hostId.isEmpty() && !myLVRequestedHost.isEmpty()) || (myLVRequestedHost == hostId && !hostId.isEmpty()))
	{
		updateSubscription("image",msgentry::UNSUBSCRIBE_ON_TOPIC,myLVRequestedHost.toUInt());
		myLVRequestedHost.clear();
	}

}

void KGUIMessenger::LSRHSubscriptionHandler(QString hostId)
{
	updateSubscription("sensors",msgentry::SUBSCRIBE_ON_TOPIC,hostId.toUInt());
	myLSRequestedHost = hostId;

}

void KGUIMessenger::LSRHUnsubscriptionHandler(QString hostId)
{
	if((hostId.isEmpty() && !myLSRequestedHost.isEmpty()) || (myLSRequestedHost == hostId && !hostId.isEmpty()))
	{
		updateSubscription("sensors",msgentry::UNSUBSCRIBE_ON_TOPIC,myLSRequestedHost.toUInt());
		myLSRequestedHost.clear();
	}

}

void KGUIMessenger::KCCRHSubscriptionHandler(QString hostId)
{
	updateSubscription("image",msgentry::SUBSCRIBE_ON_TOPIC,hostId.toUInt());
	myKccRequestedHost = hostId;
}

void KGUIMessenger::KCCRHUnsubscriptionHandler(QString hostId)
{
	if((hostId.isEmpty() && !myKccRequestedHost.isEmpty()) || (myKccRequestedHost == hostId && !hostId.isEmpty()))
	{
		updateSubscription("image",msgentry::UNSUBSCRIBE_ON_TOPIC,myKccRequestedHost.toUInt());
		myKccRequestedHost.clear();
	}
}


void KGUIMessenger::XMLRHSubscriptionHandler(QString hostId){
		updateSubscription("external",msgentry::SUBSCRIBE_ON_TOPIC,hostId.toUInt());
		myXMLRequestedHost = hostId;
}


void KGUIMessenger::XMLRHUnsubscriptionHandler(QString hostId)
{
	if((hostId.isEmpty() && !myXMLRequestedHost.isEmpty()) || (myXMLRequestedHost == hostId && !hostId.isEmpty()))
	{
		updateSubscription("external",msgentry::UNSUBSCRIBE_ON_TOPIC,myXMLRequestedHost.toUInt());
		myXMLRequestedHost.clear();
	}
}


void KGUIMessenger::XMLPublishMessage(ExternalConfig message)
{
	message.set_targethost(myXMLRequestedHost.toUInt());
	
	msgentry nmsg;
	google::protobuf::Message * newptr = message.New();
	newptr->CopyFrom(message);
	nmsg.msg.reset(newptr);
	nmsg.host = msgentry::HOST_ID_LOCAL_HOST;
	nmsg.topic = Topics::Instance().getId("external");
	nmsg.msgclass = msgentry::SIGNAL;
	multicast->getReadBuffer()->add(nmsg);
}

void KGUIMessenger::tabChangeHandler(int currentTab)
{
	QString hostId;

	updateSubscription("global",msgentry::UNSUBSCRIBE_ALL_TOPIC, msgentry::HOST_ID_ANY_HOST);
	switch(currentTab)
	{
		// Global World State 	((un-)sub to worldstate is absolutely defined by user's prefs )
		case 0:
			for(unsigned i=0; i< myGWRequestedHosts.count(); i++ ){
				hostId = myGWRequestedHosts.at(i);
				updateSubscription("worldstate",msgentry::SUBSCRIBE_ON_TOPIC,hostId.toUInt());
			}
			break;

		// Local World State
		case 1:
			if (!myLWRequestedHost.isEmpty())
			{
				updateSubscription("worldstate",msgentry::SUBSCRIBE_ON_TOPIC,myLWRequestedHost.toUInt());
				updateSubscription("vision",msgentry::SUBSCRIBE_ON_TOPIC,myLWRequestedHost.toUInt());
				updateSubscription("debug",msgentry::SUBSCRIBE_ON_TOPIC,myLWRequestedHost.toUInt());
				updateSubscription("motion",msgentry::SUBSCRIBE_ON_TOPIC,myLWRequestedHost.toUInt());
			}
			break;

		// Local Polar Map
		case 2:
			if(!myLMRequestedHost.isEmpty())
				updateSubscription("obstacle",msgentry::SUBSCRIBE_ON_TOPIC,myLMRequestedHost.toUInt());

			break;

		// Local Robot View
		case 3:
			if (!myLVRequestedHost.isEmpty())
				updateSubscription("image",msgentry::SUBSCRIBE_ON_TOPIC, myLVRequestedHost.toUInt());

			break;

		// Local Sensors Data
		case 4:
			if (!myLSRequestedHost.isEmpty())
				updateSubscription("sensors",msgentry::SUBSCRIBE_ON_TOPIC, myLSRequestedHost.toUInt());

			break;

		// Kcc
		case 5:
			if (!myKccRequestedHost.isEmpty())
				updateSubscription("image",msgentry::SUBSCRIBE_ON_TOPIC, myKccRequestedHost.toUInt());
		// Xml
		case 6:
			if (!myXMLRequestedHost.isEmpty())
				updateSubscription("external",msgentry::SUBSCRIBE_ON_TOPIC, myXMLRequestedHost.toUInt());
			break;
	}

}

void KGUIMessenger::printKnownHosts(KnownHosts hosts)
{
	std::cout <<  "#####KGUIMessenger::KNOWNHOSTS#######" << std::endl;

    const ::google::protobuf::RepeatedPtrField< HostEntry>& rf = hosts.entrylist();
    ::google::protobuf::RepeatedPtrField< HostEntry >::const_iterator fit;

    for(fit=rf.begin();fit!=rf.end();++fit)
    {
    	cout <<"To hostid mou :: "  <<  (*fit).hostid() << endl;
        cout <<"To hostname mou :: "  <<  (*fit).hostname() << endl;
    }

	std::cout <<  "###################" << std::endl << std::endl;
}

void KGUIMessenger::printMyGWRequestedHosts()
{
	QString str;
	std::cout <<  "##KGUI::printMyGWRequestedHosts()##" << std::endl;

	for(unsigned i=0; i< myGWRequestedHosts.count(); i++ )
	{
		str = myGWRequestedHosts.at(i);
		std::cout <<  str.toStdString() << std::endl;

	}
	std::cout <<  "Kai o LWRH mou :: " << myLWRequestedHost.toStdString() << std::endl;
	std::cout <<  "###################################" << std::endl << std::endl;
}

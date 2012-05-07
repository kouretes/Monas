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
		m->startEndPoint(multicastip,port);
		multicast=m;
	}

	timer = new QTimer();
	timer->setInterval(50);

	connect(this->timer, SIGNAL(timeout()), this, SLOT(allocateReceivedMessages()));

	timer->start();

	myGWRequestedHosts.clear();
	myLWRequestedHost.clear();

	updateSubscription("worldstate",msgentry::SUBSCRIBE_ON_TOPIC,msgentry::HOST_ID_ANY_HOST);
	updateSubscription("vision",msgentry::SUBSCRIBE_ON_TOPIC,msgentry::HOST_ID_ANY_HOST);
	updateSubscription("debug",msgentry::SUBSCRIBE_ON_TOPIC,msgentry::HOST_ID_ANY_HOST);
	updateSubscription("sensors",msgentry::SUBSCRIBE_ON_TOPIC,msgentry::HOST_ID_ANY_HOST);

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

			if( incomingMessages.at(i).msg->GetTypeName() == "KnownHosts" )
			{
				KnownHosts myRemoteHosts;
				myRemoteHosts.Clear();
				myRemoteHosts.CopyFrom(*(incomingMessages.at(i).msg));

				//printKnownHosts(myRemoteHosts);
				emit knownHostsUpdate( myRemoteHosts );
				//printMyGWRequestedHosts();
			}
			else if (incomingMessages.at(i).msg->GetTypeName()=="GameStateMessage")
			{
				GameStateMessage gsm;
				gsm.Clear();
				gsm.CopyFrom(*(incomingMessages.at(i).msg));

				emit gameStateMessageUpdate(gsm, currentRHost);
			}
			else if (incomingMessages.at(i).msg->GetTypeName()=="WorldInfo" && (myGWRequestedHosts.contains(currentRHost)||(myLWRequestedHost ==currentRHost)))
			{
				WorldInfo wim;
				wim.Clear();
				wim.CopyFrom(*(incomingMessages.at(i).msg));

				emit worldInfoUpdate(wim, currentRHost);
			}
			else if (incomingMessages.at(i).msg->GetTypeName()=="ObservationMessage" && myLWRequestedHost == currentRHost)
			{
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
			else if (incomingMessages.at(i).msg->GetTypeName()=="AllSensorValuesMessage" && myLWRequestedHost == currentRHost)
			{
				AllSensorValuesMessage asvm;
				SensorData HeadYaw;
				float targetYaw;

				asvm.Clear();
				HeadYaw.Clear();
				targetYaw = 0.f;

				asvm.CopyFrom(*(incomingMessages.at(i).msg));
				HeadYaw = asvm.jointdata(KDeviceLists::HEAD + KDeviceLists::YAW);
				targetYaw = HeadYaw.sensorvalue();

				emit headYawJointUpdate(targetYaw, currentRHost);
			}
		}
		else
		{
			std::cout << "Null msg from host " << incomingMessages.at(i).host << std::endl;
		}
	}
}

void KGUIMessenger::updateSubscription(std::string const& topic , msgentry::msgclass_t where, std::size_t host)
{
	//cout<<"Check -2"<<endl;
	if(multicast->getReadBuffer()==NULL||multicast->getWriteBuffer()==NULL)
		return;
	//cout<<"Check -1"<<endl;

	msgentry nmsg;
	nmsg.topic=Topics::Instance().getId(topic);
	nmsg.host=host;
	nmsg.msgclass=where;

	multicast->getReadBuffer()->add(nmsg);
}

void KGUIMessenger::GWRHSubscriptionHandler(QString hostId)
{
	myGWRequestedHosts << hostId;
	myGWRequestedHosts.removeDuplicates();

	//printMyGWRequestedHosts();

}

void KGUIMessenger::GWRHUnsubscriptionHandler(QString hostId)
{
	myGWRequestedHosts.removeOne(hostId);
	//printMyGWRequestedHosts();
}

void KGUIMessenger::LWRHSubscriptionHandler(QString hostId)
{
	myLWRequestedHost = hostId;
	//printMyGWRequestedHosts();

}

void KGUIMessenger::LWRHUnsubscriptionHandler(QString hostId)
{
	if(myLWRequestedHost == hostId)
		myLWRequestedHost = "";
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

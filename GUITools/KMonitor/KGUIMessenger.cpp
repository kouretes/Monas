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

/*	updateSubscription("worldstate",msgentry::SUBSCRIBE_ON_TOPIC,msgentry::HOST_ID_ANY_HOST);
	updateSubscription("vision",msgentry::SUBSCRIBE_ON_TOPIC,msgentry::HOST_ID_ANY_HOST);
	updateSubscription("image",msgentry::SUBSCRIBE_ON_TOPIC,msgentry::HOST_ID_ANY_HOST);
	updateSubscription("debug",msgentry::SUBSCRIBE_ON_TOPIC,msgentry::HOST_ID_ANY_HOST);
	updateSubscription("motion",msgentry::SUBSCRIBE_ON_TOPIC,msgentry::HOST_ID_ANY_HOST);
	updateSubscription("obstacle",msgentry::SUBSCRIBE_ON_TOPIC,msgentry::HOST_ID_ANY_HOST);*/
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
			else if (incomingMessages.at(i).msg->GetTypeName()=="KRawImage" && (myLVRequestedHost == currentRHost || myKccRequestedHost == currentRHost))
			{
				//std::cout << "incomingMessages == KRawImage " << std::endl;
				KRawImage rawimg;
				rawimg.Clear();
				rawimg.CopyFrom(*(incomingMessages.at(i).msg));

				//todo fix this better
				if(myLVRequestedHost == currentRHost)
					emit rawImageUpdate(rawimg, currentRHost);

				else if(myKccRequestedHost == currentRHost)
					emit KCCRawImageUpdate(rawimg, currentRHost);
				else
				{
					emit rawImageUpdate(rawimg, currentRHost);
					emit KCCRawImageUpdate(rawimg, currentRHost);
				}

			}
			else if (incomingMessages.at(i).msg->GetTypeName()=="GameStateMessage")
			{
				//std::cout << "incomingMessages == GameStateMessage " << std::endl;
				GameStateMessage gsm;
				gsm.Clear();
				gsm.CopyFrom(*(incomingMessages.at(i).msg));

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
	if(myLWRequestedHost == hostId)
	{
		updateSubscription("worldstate",msgentry::UNSUBSCRIBE_ON_TOPIC,hostId.toUInt());
		updateSubscription("vision",msgentry::UNSUBSCRIBE_ON_TOPIC,hostId.toUInt());
		updateSubscription("debug",msgentry::UNSUBSCRIBE_ON_TOPIC,hostId.toUInt());
		updateSubscription("motion",msgentry::UNSUBSCRIBE_ON_TOPIC,hostId.toUInt());

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
	if(myLMRequestedHost == hostId)
	{
		updateSubscription("obstacle",msgentry::UNSUBSCRIBE_ON_TOPIC,hostId.toUInt());
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
	if(myLVRequestedHost == hostId)
	{
		updateSubscription("image",msgentry::UNSUBSCRIBE_ON_TOPIC,hostId.toUInt());
		myLVRequestedHost.clear();
	}

}

void KGUIMessenger::KCCRHSubscriptionHandler(QString hostId)
{
	updateSubscription("image",msgentry::SUBSCRIBE_ON_TOPIC,hostId.toUInt());
	myKccRequestedHost = hostId;
}

void KGUIMessenger::KCCRHUnsubscriptionHandler(QString hostId)
{
	if(myKccRequestedHost == hostId)
	{
		updateSubscription("image",msgentry::UNSUBSCRIBE_ON_TOPIC,hostId.toUInt());
		myKccRequestedHost.clear();
	}
}

void KGUIMessenger::tabChangeHandler(int currentTab)
{
	switch(currentTab)
	{
		// Global World State
		case 0:
			if (!myLVRequestedHost.isEmpty())
				updateSubscription("image",msgentry::UNSUBSCRIBE_ON_TOPIC, myLVRequestedHost.toUInt());
			if (!myKccRequestedHost.isEmpty())
				updateSubscription("image",msgentry::UNSUBSCRIBE_ON_TOPIC, myKccRequestedHost.toUInt());

			break;

		// Local World State
		case 1:
			if (!myLVRequestedHost.isEmpty())
				updateSubscription("image",msgentry::UNSUBSCRIBE_ON_TOPIC, myLVRequestedHost.toUInt());
			if (!myKccRequestedHost.isEmpty())
				updateSubscription("image",msgentry::UNSUBSCRIBE_ON_TOPIC, myKccRequestedHost.toUInt());

			break;

		// Local Polar Map
		case 2:
			if (!myLVRequestedHost.isEmpty())
				updateSubscription("image",msgentry::UNSUBSCRIBE_ON_TOPIC, myLVRequestedHost.toUInt());
			if (!myKccRequestedHost.isEmpty())
				updateSubscription("image",msgentry::UNSUBSCRIBE_ON_TOPIC, myKccRequestedHost.toUInt());

			break;

		// Local Robot View
		case 3:
			if (!myKccRequestedHost.isEmpty())
				updateSubscription("image",msgentry::UNSUBSCRIBE_ON_TOPIC, myKccRequestedHost.toUInt());
			if (!myLVRequestedHost.isEmpty())
				updateSubscription("image",msgentry::SUBSCRIBE_ON_TOPIC, myLVRequestedHost.toUInt());

			break;

		// Kcc
		case 4:
			if (!myLVRequestedHost.isEmpty())
				updateSubscription("image",msgentry::UNSUBSCRIBE_ON_TOPIC, myLVRequestedHost.toUInt());
			if (!myKccRequestedHost.isEmpty())
				updateSubscription("image",msgentry::SUBSCRIBE_ON_TOPIC, myKccRequestedHost.toUInt());

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
          /*  QString curr = QString::fromStdString(_toString((*fit).hostid()));
            std::cout <<"QString"<<  curr.toStdString() << std::endl;
            std::cout <<"uint"<<  curr.toUInt() << std::endl;*/

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

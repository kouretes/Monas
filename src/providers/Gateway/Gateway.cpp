#include "Gateway.h"

#include "tools/logger.h"
#include "tools/toString.h"
#include <boost/crc.hpp> 

#include <fstream>
#include <streambuf>

using namespace std;

PROVIDER_REGISTER(Gateway);


void Gateway::UserInit()
{
#ifdef RUN_ON_NAO
	_xml = XmlNode(ArchConfig::Instance().GetConfigPrefix(), KRobotConfig::Instance().getConfig(KDeviceLists::Interpret::HEAD_ID)
	               , KRobotConfig::Instance().getConfig(KDeviceLists::Interpret::BODY_ID), true);
#else
	_xml = XmlNode(ArchConfig::Instance().GetConfigPrefix(), "hi", "bi", true);
#endif
	_blk.updateSubscription("communication", msgentry::SUBSCRIBE_ON_TOPIC);
	_blk.updateSubscription("external", msgentry::SUBSCRIBE_ON_TOPIC, msgentry::HOST_ID_ANY_HOST);
	localHostId = 0;
	lockId = 0;
	locked = false;
	Logger::Instance().WriteMsg("Gateway", "Initialized", Logger::ExtraInfo);
}



int Gateway::Execute()
{
	_blk.process_messages();
	_blk.publish_all();

	//cout << _blk.getMyHostId() << endl;
	
	h = _blk.readState<KnownHosts>("communication");
	
	if(!h.get() || (h && h->entrylist_size() == 0))
	{
		return 0;
	}
	else
	{
		if(localHostId == 0){
			localHostId = h->localhost().hostid();
		}
		const ::google::protobuf::RepeatedPtrField< HostEntry >& rf = h->entrylist();
		::google::protobuf::RepeatedPtrField< HostEntry >::const_iterator fit;
		if(locked){
			bool found = false;
			for(fit = rf.begin(); fit != rf.end(); ++fit){
				if(lockId == (*fit).hostid()){
					found = true;
					break;
				}
			}
			if(!found)
				locked = false;
		}
		for(fit = rf.begin(); fit != rf.end(); ++fit)
		{

			boost::shared_ptr<const ExternalConfig> inmsg = _blk.readSignal<ExternalConfig> ("external", (*fit).hostid());
			if (inmsg.get() != NULL && inmsg->targethost() == localHostId)
			{
				bool lockOwner = true;
				if(locked == false){
					lockId = (*fit).hostid();
					locked = true;
				}else if(lockId != (*fit).hostid()){
					lockOwner = false; //Locked in other gui
				}
				outmsg.clear_handshaking();
				updateXMLMsg.clear_updatexml();
				resetActMsg.clear_resetactivities();
				if(lockOwner){
					map<uint32_t,string>::iterator iter = timeouts.find((*fit).hostid());
					//We must always send back ack but we must do changes only if the message is new and not retrasmit
					bool freshMessage = false;
					if(iter == timeouts.end() || (*iter).second.compare(inmsg->messageid()) != 0)
					{
						freshMessage = true;
					}
					
					//Handshaking message
					if(inmsg->handoffrequest() == true){
						outmsg.mutable_handshaking()->set_headid(_xml.getHeadID());
						outmsg.mutable_handshaking()->set_bodyid(_xml.getBodyID());
						outmsg.mutable_handshaking()->set_checksum(_xml.getChecksum());
					}
				

					if(freshMessage){
						vector<pair<string,string> > dataForWrite;
						for(int i=0; i < inmsg->updatexml_size(); i++){
							updateXMLMsg.add_updatexml();
							updateXMLMsg.mutable_updatexml(i)->set_keyword(inmsg->updatexml(i).keyword());
							updateXMLMsg.mutable_updatexml(i)->set_value(inmsg->updatexml(i).value());
							pair<string,string> temp;
							temp.first = inmsg->updatexml(i).keyword();
							temp.second = inmsg->updatexml(i).value();
							dataForWrite.push_back(temp);
						}
						if(updateXMLMsg.updatexml_size() != 0){
							_xml.burstWrite(dataForWrite);
							msgentry nmsg;
							google::protobuf::Message * newptr = updateXMLMsg.New();
							newptr->CopyFrom(updateXMLMsg);
							nmsg.msg.reset(newptr);
							nmsg.host = msgentry::HOST_ID_LOCAL_HOST;
							nmsg.topic = Topics::Instance().getId("external");
							nmsg.msgclass = msgentry::SIGNAL;
							this->publish(nmsg);
						}
					
						if(inmsg->has_file()){
							ofstream fout;
							fout.open( ( ArchConfig::Instance().GetConfigPrefix() + inmsg->file().filepath()).c_str(), ios::trunc | ios::out);
							if (fout.is_open()){
								fout.write(inmsg->file().file().c_str(),inmsg->file().file().size());
								fout.close();
							}else
								Logger::Instance().WriteMsg("Gateway", "Wrong file path", Logger::ExtraInfo);
						}
						
						for(int i=0; i < inmsg->resetactivities_size(); i++){
							resetActMsg.add_resetactivities(inmsg->resetactivities(i));
						}
						if(resetActMsg.resetactivities_size() != 0){					
							msgentry nmsg;
							google::protobuf::Message * newptr = resetActMsg.New();
							newptr->CopyFrom(resetActMsg);
							nmsg.msg.reset(newptr);
							nmsg.host = msgentry::HOST_ID_LOCAL_HOST;
							nmsg.topic = Topics::Instance().getId("external");
							nmsg.msgclass = msgentry::SIGNAL;
							this->publish(nmsg);
						}
					}
				}
				
				timeouts[(*fit).hostid()] = inmsg->messageid();
				outmsg.set_hostid((*fit).hostid());
				outmsg.set_messageid(inmsg->messageid());
				outmsg.set_ownlock(lockOwner);
				
				msgentry nmsg;
				google::protobuf::Message * newptr = outmsg.New();
				newptr->CopyFrom(outmsg);
				nmsg.msg.reset(newptr);
				nmsg.host = msgentry::HOST_ID_LOCAL_HOST;
				nmsg.topic = Topics::Instance().getId("external");
				nmsg.msgclass = msgentry::SIGNAL;
				this->publish(nmsg);
			}
		}

	}
	return 0;
}


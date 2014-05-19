#include "Gateway.h"

#include "core/include/Logger.hpp"
#include <boost/crc.hpp>


#include "tools/toString.h"
#include <fstream>
#include <streambuf>

using namespace std;

PROVIDER_REGISTER (Gateway);


void Gateway::UserInit() {
	_blk.updateSubscription ("communication", Messaging::MessageEntry::SUBSCRIBE_ON_TOPIC);
	_blk.updateSubscription ("external", Messaging::MessageEntry::SUBSCRIBE_ON_TOPIC, Messaging::MessageEntry::HOST_ID_ANY_HOST);
	_blk.updateSubscription ("worldstate", Messaging::MessageEntry::SUBSCRIBE_ON_TOPIC);
	localHostId = 0;
	lockId = 0;
	locked = false;
	LogEntry(LogLevel::ExtraInfo,GetName())<< "Initialized";
}



int Gateway::Execute() {
	_blk.process_messages();
	_blk.publish_all();
	//cout << _blk.getMyHostId() << endl;
	h = _blk.readState<KnownHosts> ("communication");
	gsm = _blk.readState<GameStateMessage> ("worldstate");

	if (!h.get() || (h && h->entrylist_size() == 0) ) {
		return 0;
	} else {
		if (localHostId == 0) {
			localHostId = h->localhost().hostid();
		}

		const ::google::protobuf::RepeatedPtrField< HostEntry >& rf = h->entrylist();

		::google::protobuf::RepeatedPtrField< HostEntry >::const_iterator fit;

		if (locked) {
			bool found = false;

			for (fit = rf.begin(); fit != rf.end(); ++fit) {
				if (lockId == (*fit).hostid() ) {
					found = true;
					break;
				}
			}

			if (!found) {
				locked = false;
			}
		}

		for (fit = rf.begin(); fit != rf.end(); ++fit) {
			//Config PART
			processExternalConfig ( (*fit).hostid() );
			//Command part
			processExternalCommands ( (*fit).hostid() );
			//Camera part
			processCameraCalibration ( (*fit).hostid() );
		}
	}

	return 0;
}

void Gateway::processExternalConfig (uint32_t incomingHostId) {
	boost::shared_ptr<const ExternalConfig> ecmsg = _blk.readSignal<ExternalConfig> ("external", incomingHostId);

	if (ecmsg.get() != NULL && ecmsg->targethost() == localHostId) {
		outmsg.clear_handshaking();
		bool lockOwner = true;

		if (locked == false) {
			lockId = incomingHostId;
			locked = true;
		} else if (lockId != incomingHostId) {
			lockOwner = false; //Locked in other gui
		}

		if (lockOwner) {
			resetActMsg.clear_resetactivities();
			map<uint32_t, string>::iterator iter = ectimeouts.find (incomingHostId);
			//We must always send back ack but we must do changes only if the message is new and not retrasmit
			bool freshMessage = false;

			if (iter == ectimeouts.end() || (*iter).second.compare (ecmsg->messageid() ) != 0) {
				freshMessage = true;
			}

			//Handshaking message
			if (ecmsg->handoffrequest() == true) {
				outmsg.mutable_handshaking()->set_headid (Configurator::Instance().getHeadID() );
				outmsg.mutable_handshaking()->set_bodyid (Configurator::Instance().getBodyID() );
				outmsg.mutable_handshaking()->set_checksum (Configurator::Instance().getChecksum() );
			}

			if (freshMessage) {
				vector<pair<string, string> > dataForWrite;

				for (int i = 0; i < ecmsg->updateconfig_size(); i++) {
					//Prepare to write to files
					pair<string, string> temp;
					temp.first = ecmsg->updateconfig (i).keyword();
					temp.second = ecmsg->updateconfig (i).value();
					dataForWrite.push_back (temp);
				}

				//Write to the files and publish the message
				if (dataForWrite.size() != 0) {
					Configurator::Instance().burstWrite (dataForWrite);
				}

				if (ecmsg->has_file() ) {
					ofstream fout;
					fout.open ( (Configurator::Instance().getDirectoryPath() + ecmsg->file().filepath() ).c_str(), ios::trunc | ios::out);

					if (fout.is_open() ) {
						fout.write (ecmsg->file().file().c_str(), ecmsg->file().file().size() );
						fout.close();
					} else {
						LogEntry(LogLevel::Info,GetName())<< "Wrong file path, Can't save external file";
					}
				}

				for (int i = 0; i < ecmsg->resetactivities_size(); i++) {
					resetActMsg.add_resetactivities (ecmsg->resetactivities (i) );
				}

				if (resetActMsg.resetactivities_size() != 0) {
					publishSignal (resetActMsg, "architecture");
				}
			}
		}

		ectimeouts[incomingHostId] = ecmsg->messageid();
		outmsg.set_hostid (incomingHostId);
		outmsg.set_messageid (ecmsg->messageid() );
		outmsg.set_ownlock (lockOwner);
		publishSignal (outmsg, "external");
	}
}

void Gateway::processExternalCommands (uint32_t incomingHostId) {
	boost::shared_ptr<const ExternalCommand> commsg = _blk.readSignal<ExternalCommand> ("external", incomingHostId );

	if (commsg.get() != NULL && commsg->targethost() == localHostId) {
		outmsg.clear_handshaking();
		bool lockOwner = true;

		if (locked == false) {
			lockId = incomingHostId;
			locked = true;
		} else if (lockId != incomingHostId ) {
			lockOwner = false; //Locked in other gui
		}

		if (lockOwner) {
			map<uint32_t, string>::iterator iter = comtimeouts.find ( incomingHostId );
			//We must always send back ack but we must do changes only if the message is new and not retrasmit
			bool freshMessage = false;

			if (iter == comtimeouts.end() || (*iter).second.compare (commsg->messageid() ) != 0) {
				freshMessage = true;
			}

			if (freshMessage) {
				int commandID = commsg->commandid();
				if(commandID < ExternalCommand::SIMPLE_ACTIONS_SIZE){
					int tempId = commandID;
					processSimpleActionsCommand(tempId);
				}else if(commandID < ExternalCommand::SIMPLE_ACTIONS_SIZE + ExternalCommand::GAMECONTROLLER_SIZE){

					int tempId = commandID - ExternalCommand::SIMPLE_ACTIONS_SIZE;

					processGameControllerCommand(tempId);
				}else if(commandID < ExternalCommand::SIMPLE_ACTIONS_SIZE + ExternalCommand::GAMECONTROLLER_SIZE + ExternalCommand::LOCALIZATION_SIZE){

					//int tempId = commandID - ExternalCommand::SIMPLE_ACTIONS_SIZE - ExternalCommand::GAMECONTROLLER_SIZE;

					processLocalizationCommand(commsg->floatvars(0), commsg->floatvars(1), commsg->floatvars(2));
				}else if(commandID < ExternalCommand::SIMPLE_ACTIONS_SIZE + ExternalCommand::GAMECONTROLLER_SIZE + ExternalCommand::LOCALIZATION_SIZE + ExternalCommand::MOTION_SIZE){

					int tempId = commandID - ExternalCommand::SIMPLE_ACTIONS_SIZE - ExternalCommand::GAMECONTROLLER_SIZE - ExternalCommand::LOCALIZATION_SIZE;
					string action = commsg->has_stringvars() ? commsg->stringvars() : "";
					float value = commsg->floatvars_size() == 1 ? commsg->floatvars(0) : 0;
					processMotionCommand(tempId, action, value);
				}else if(commandID < ExternalCommand::SIMPLE_ACTIONS_SIZE + ExternalCommand::GAMECONTROLLER_SIZE + ExternalCommand::LOCALIZATION_SIZE + ExternalCommand::MOTION_SIZE + ExternalCommand::HEAD_SIZE){
					int tempId = commandID - ExternalCommand::SIMPLE_ACTIONS_SIZE - ExternalCommand::GAMECONTROLLER_SIZE - ExternalCommand::LOCALIZATION_SIZE - ExternalCommand::MOTION_SIZE;
					float value = commsg->floatvars_size() == 1 ? commsg->floatvars(0) : 0;
					processHeadCommand(tempId, value);
				}else{
					LogEntry(LogLevel::Info,GetName())<<  "Unknown External Command ID";
				}
			}
		}

		comtimeouts[ incomingHostId ] = commsg->messageid();
		outmsg.set_hostid ( incomingHostId );
		outmsg.set_messageid (commsg->messageid() );
		outmsg.set_ownlock (lockOwner);
		publishSignal (outmsg, "external");
	}
}

void Gateway::processGameControllerCommand(int commandID){
	if(commandID == ExternalCommand::INITIAL){
		new_gsm.CopyFrom(*gsm);
		new_gsm.set_previous_player_state(new_gsm.player_state());
		new_gsm.set_penalty(PENALTY_NONE);
		new_gsm.set_game_state(STATE_INITIAL);
		new_gsm.set_player_state(PLAYER_INITIAL);
		publishState(new_gsm, "worldstate");
	}else if(commandID == ExternalCommand::READY){
		new_gsm.CopyFrom(*gsm);
		new_gsm.set_previous_player_state(new_gsm.player_state());
		new_gsm.set_penalty(PENALTY_NONE);
		new_gsm.set_game_state(STATE_READY);
		new_gsm.set_player_state(PLAYER_READY);
		publishState(new_gsm, "worldstate");
	}else if(commandID == ExternalCommand::SET){
		new_gsm.CopyFrom(*gsm);
		new_gsm.set_previous_player_state(new_gsm.player_state());
		new_gsm.set_penalty(PENALTY_NONE);
		new_gsm.set_game_state(STATE_SET);
		new_gsm.set_player_state(PLAYER_SET);
		publishState(new_gsm, "worldstate");
	}else if(commandID == ExternalCommand::PLAY){
		new_gsm.CopyFrom(*gsm);
		new_gsm.set_previous_player_state(new_gsm.player_state());
		new_gsm.set_penalty(PENALTY_NONE);
		new_gsm.set_game_state(STATE_PLAYING);
		new_gsm.set_player_state(PLAYER_PLAYING);
		publishState(new_gsm, "worldstate");
	}else if(commandID == ExternalCommand::PENALISED){
		new_gsm.CopyFrom(*gsm);
		new_gsm.set_previous_player_state(new_gsm.player_state());
		new_gsm.set_penalty(PENALTY_MANUAL);
		new_gsm.set_game_state(STATE_PLAYING);
		new_gsm.set_player_state(PLAYER_PENALISED);
		publishState(new_gsm, "worldstate");
	}else if(commandID == ExternalCommand::FINISH){
		new_gsm.CopyFrom(*gsm);
		new_gsm.set_previous_player_state(new_gsm.player_state());
		new_gsm.set_penalty(PENALTY_NONE);
		new_gsm.set_game_state(STATE_FINISHED);
		new_gsm.set_player_state(PLAYER_FINISHED);
		publishState(new_gsm, "worldstate");
	}else if(commandID == ExternalCommand::OVERRIDE){
		new_gsm.CopyFrom(*gsm);
		if(gsm->override_state() == OVERRIDE_ENABLED){
			new_gsm.set_override_state(OVERRIDE_DISABLED);
		}else{
			new_gsm.set_override_state(OVERRIDE_ENABLED);
		}
		publishState(new_gsm, "worldstate");
	}
}

void Gateway::processSimpleActionsCommand(int commandID){
	if(commandID == ExternalCommand::SHUTDOWN){
		LogEntry(LogLevel::FatalError,GetName())<< "SysCall: Shutdown robot";
		SysCall::_Shutdown();
	}else if(commandID == ExternalCommand::STIFFNESS_OFF){
		new_gsm.CopyFrom(*gsm);
		new_gsm.set_override_state(OVERRIDE_DROPDEAD);
		publishState(new_gsm, "worldstate");
	}else if(commandID == ExternalCommand::STOP_ROBOT){
		new_gsm.CopyFrom(*gsm);
		new_gsm.set_penalty(PENALTY_MANUAL);
		new_gsm.set_player_state(PLAYER_PENALISED);
		publishState(new_gsm, "worldstate");
	}else if(commandID == ExternalCommand::SIT){
		amot.set_command("PoseInitial.xar");
		publishSignal(amot, "motion");
	}
}

void Gateway::processMotionCommand(int commandID, string svalue, float value){
	if(commandID == ExternalCommand::EXECUTE_ACTION){
		amot.set_command(svalue);
		publishSignal(amot, "motion");
	}else if(commandID == ExternalCommand::SET_STIFFNESS){
		msm.set_chain(svalue);
		msm.set_value(value);
		publishSignal(msm, "motion");
	}
}

void Gateway::processLocalizationCommand(float x, float y, float phi){
	lrm.set_type(LocalizationResetMessage::MANUAL);
	lrm.set_kickoff(false);
	lrm.set_xpos(x);
	lrm.set_ypos(y);
	lrm.set_phipos(phi);
	publishSignal(lrm, "worldstate");
}

void Gateway::processHeadCommand(int commandID, float value){
	hcm.mutable_task()->set_speed(value);
	if(commandID == ExternalCommand::NOTHING){
		hcm.mutable_task()->set_action(HeadControlMessage::NOTHING);
	}else if(commandID == ExternalCommand::LOOK_DOWN){
		hcm.mutable_task()->set_action(HeadControlMessage::FROWN);
	}else if(commandID == ExternalCommand::SCAN_AND_TRACK){
		hcm.mutable_task()->set_action(HeadControlMessage::SCAN_AND_TRACK_FOR_BALL);
	}else if(commandID == ExternalCommand::LOCALIZE){
		hcm.mutable_task()->set_action(HeadControlMessage::LOCALIZE);
	}else if(commandID == ExternalCommand::LOCALIZE_FAR){
		hcm.mutable_task()->set_action(HeadControlMessage::LOCALIZE_FAR);
	}else if(commandID == ExternalCommand::SMART_SCAN){
		hcm.mutable_task()->set_action(HeadControlMessage::SMART_SELECT);
	}else if(commandID == ExternalCommand::JUST_SCAN){
		hcm.mutable_task()->set_action(HeadControlMessage::SCAN);
	}else if(commandID == ExternalCommand::GOALIE_LOCALIZE){
		hcm.mutable_task()->set_action(HeadControlMessage::GOALIE_LOCALIZE_CLOSE);
	}else{
		hcm.mutable_task()->set_action(HeadControlMessage::NOTHING);
	}
	publishState(hcm, "behavior");
}

void Gateway::processCameraCalibration (uint32_t incomingHostId) {
	boost::shared_ptr<const CameraCalibration> eccmsg = _blk.readSignal<CameraCalibration> ("external", incomingHostId);

	if (eccmsg.get() != NULL && eccmsg->targethost() == localHostId) {
		bool lockOwner = true;

		if (locked == false) {
			lockId = incomingHostId;
			locked = true;
		} else if (lockId != incomingHostId) {
			lockOwner = false; //Locked in other gui
		}

		if (lockOwner) {
			map<uint32_t, string>::iterator iter = ecameratimeouts.find (incomingHostId);
			//We must always send back ack but we must do changes only if the message is new and not retrasmit
			bool freshMessage = false;

			if (iter == ecameratimeouts.end() || (*iter).second.compare (eccmsg->messageid() ) != 0) {
				freshMessage = true;
			}

			if (freshMessage) {
				bool changes = false;
				vector<pair<string, string> > dataForWrite;
				pair<string, string> temp;
				temp.first = "camera.Gain";
				temp.second = _toString(eccmsg->gain());
				if(eccmsg->gain() != (unsigned int)atoi(Configurator::Instance().findValueForKey("camera.Gain").c_str())){
					changes = true;
				}
				dataForWrite.push_back(temp);
				temp.first = "camera.Contrast";
				temp.second = _toString(eccmsg->contrast());
				if(eccmsg->contrast() != (unsigned int)atoi(Configurator::Instance().findValueForKey("camera.Contrast").c_str())){
					changes = true;
				}
				dataForWrite.push_back(temp);
				temp.first = "camera.GreenChannelGain";
				temp.second = _toString(eccmsg->greengain());
				if(eccmsg->greengain() != (unsigned int)atoi(Configurator::Instance().findValueForKey("camera.GreenChannelGain").c_str())){
					changes = true;
				}
				dataForWrite.push_back(temp);
				temp.first = "camera.RedBalance";
				temp.second = _toString(eccmsg->redbalance());
				if(eccmsg->redbalance() != (unsigned int)atoi(Configurator::Instance().findValueForKey("camera.RedBalance").c_str())){
					changes = true;
				}
				dataForWrite.push_back(temp);
				temp.first = "camera.BlueBalance";
				temp.second = _toString(eccmsg->bluebalance());
				if(eccmsg->bluebalance() != (unsigned int)atoi(Configurator::Instance().findValueForKey("camera.BlueBalance").c_str())){
					changes = true;
				}
				dataForWrite.push_back(temp);

				if(changes){
					ccm.set_readconfiguration(true);
					publishSignal(ccm, "image");
					Configurator::Instance().burstWrite (dataForWrite);
				}

			}
		}

		ecameratimeouts[incomingHostId] = eccmsg->messageid();
		outmsg.set_hostid (incomingHostId);
		outmsg.set_messageid (eccmsg->messageid() );
		outmsg.set_ownlock (lockOwner);
		publishSignal (outmsg, "external");
	}
}

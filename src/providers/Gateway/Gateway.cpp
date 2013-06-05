#include "Gateway.h"

#include "tools/logger.h"
#include "tools/toString.h"
#include <boost/crc.hpp>

#include <fstream>
#include <streambuf>

using namespace std;

PROVIDER_REGISTER (Gateway);


void Gateway::UserInit() {
	_blk.updateSubscription ("communication", msgentry::SUBSCRIBE_ON_TOPIC);
	_blk.updateSubscription ("external", msgentry::SUBSCRIBE_ON_TOPIC, msgentry::HOST_ID_ANY_HOST);
	_blk.updateSubscription ("worldstate", msgentry::SUBSCRIBE_ON_TOPIC);
	localHostId = 0;
	lockId = 0;
	locked = false;
	Logger::Instance().WriteMsg ("Gateway", "Initialized", Logger::ExtraInfo);
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
			//XML PART
			processExternalConfig ( (*fit).hostid() );
			//Command part
			processExternalCommands ( (*fit).hostid() );
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
			updateXMLMsg.clear_updatexml();
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

				for (int i = 0; i < ecmsg->updatexml_size(); i++) {
					//Add to msg to informe the world about the changes
					updateXMLMsg.add_updatexml();
					updateXMLMsg.mutable_updatexml (i)->set_keyword (ecmsg->updatexml (i).keyword() );
					updateXMLMsg.mutable_updatexml (i)->set_value (ecmsg->updatexml (i).value() );
					//Prepare to write to files
					pair<string, string> temp;
					temp.first = ecmsg->updatexml (i).keyword();
					temp.second = ecmsg->updatexml (i).value();
					dataForWrite.push_back (temp);
				}

				//Write to the files and publish the message
				if (updateXMLMsg.updatexml_size() != 0) {
					Configurator::Instance().burstWrite (dataForWrite);
					publishSignal (updateXMLMsg, "external");
				}

				if (ecmsg->has_file() ) {
					ofstream fout;
					fout.open ( (ArchConfig::Instance().GetConfigPrefix() + ecmsg->file().filepath() ).c_str(), ios::trunc | ios::out);

					if (fout.is_open() ) {
						fout.write (ecmsg->file().file().c_str(), ecmsg->file().file().size() );
						fout.close();
					} else {
						Logger::Instance().WriteMsg ("Gateway", "Wrong file path, Can't save external file", Logger::Info);
					}
				}

				for (int i = 0; i < ecmsg->resetactivities_size(); i++) {
					resetActMsg.add_resetactivities (ecmsg->resetactivities (i) );
				}

				if (resetActMsg.resetactivities_size() != 0) {
					publishSignal (resetActMsg, "external");
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

					int tempId = commandID - ExternalCommand::SIMPLE_ACTIONS_SIZE - ExternalCommand::GAMECONTROLLER_SIZE;

					processLocalizationCommand(tempId, commsg->floatvars(0), commsg->floatvars(1), commsg->floatvars(2));
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
					Logger::Instance().WriteMsg ("Gateway", "Unknown External Command ID", Logger::Info);
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
		Logger::Instance().WriteMsg("Gateway", "SysCall: Shutdown robot", Logger::FatalError);
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

void Gateway::processLocalizationCommand(int commandID, float x, float y, float phi){
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
	}else{
		hcm.mutable_task()->set_action(HeadControlMessage::NOTHING);
	}
	publishState(hcm, "behavior");
}

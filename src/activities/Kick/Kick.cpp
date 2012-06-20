
#include "Kick.h"

ACTIVITY_REGISTER(Kick);

int Kick::Execute() {
	Logger::Instance().WriteMsg(GetName(),  " execute", Logger::Info);

	gsm = _blk.readState<GameStateMessage>("worldstate");
	wimsg  = _blk.readData<WorldInfo>("worldstate");

	if(!readConf)
		readGoalConfiguration(ArchConfig::Instance().GetConfigPrefix() +"/Features.xml");
//#ifdef RETURN_TO_POSITION
		//rpm->set_goalietopos(true);
		//_blk.publishSignal(*rpm, "behavior");
//#endif

	if(gsm!=0 && gsm->game_state()!=PLAYER_PLAYING){
	//	Logger::Instance().WriteMsg("MyLeftKick",  " No********************************", Logger::Info);
		return 0;
	}
	//Logger::Instance().WriteMsg("MyLeftKick",  " Execute********************************", Logger::Info);
	if (wimsg==0 || wimsg->balls_size()==0){
		//Logger::Instance().WriteMsg("TrCond_0_3_2_3_4_6_2TOMyLeftKick", "FALSE NO OBSM", Logger::Info);
		return 0;
	}
	if (wimsg && wimsg->balls_size()!=0)
		by = wimsg->balls(0).relativey();
	#ifdef PENALTY_ON
		if (by > 0.0)
			amot->set_command("KickForwardLeft.xar"); //LeftKick
		else
			amot->set_command("KickForwardRight.xar"); //RightKick
		_blk.publishSignal(*amot, "motion");
		return 0;
	#endif
	double loppgb = anglediff2(atan2(oppGoalLeftY - wimsg->myposition().y(), oppGoalLeftX - wimsg->myposition().x()), wimsg->myposition().phi());
	double roppgb = anglediff2(atan2(oppGoalRightY - wimsg->myposition().y(), oppGoalRightX - wimsg->myposition().x()), wimsg->myposition().phi());
	double cone = anglediff2(loppgb, roppgb);
	double oppgb = wrapToPi(roppgb + cone/2.0);

	if ( (oppgb <= M_PI_4) && (oppgb > -M_PI_4) ) {
		orientation = 0;
	} else if ( (oppgb > M_PI_4) && (oppgb <= (M_PI-M_PI_4) ) ) {
		orientation = 1;
	} else if ( (oppgb > (M_PI-M_PI_4) ) || (oppgb <= -(M_PI-M_PI_4) ) ) {
		orientation = 2;
	} else if ( (oppgb <= -M_PI_4 ) && (oppgb > -(M_PI-M_PI_4) ) ) {
		orientation = 3;
	}
	
	if (orientation == 0) {
		if (by > 0.0)
			amot->set_command("KickForwardLeft.xar"); //LeftKick
		else
			amot->set_command("KickForwardRight.xar"); //RightKick
	} else if (orientation == 3) {
		amot->set_command("KickSideLeftStable.xar"); //"HardLeftSideKick"
	} else if (orientation == 1) {
		amot->set_command("KickSideRightStable.xar"); //"HardRightSideKick"
	} else if (orientation == 2) {

		if (by > 0.0){
			amot->set_command("KickSideLeftStable.xar"); //LeftKick
		//	amot->set_command("KickBackLeftPierris.xar"); //LeftBackHigh_carpet
		}else{
			amot->set_command("KickSideRightStable.xar"); //LeftKick
		//	amot->set_command("KickBackRightPierris.xar"); //RightBackHigh_carpet
		}

	} else {
		if (by > 0.0)
			amot->set_command("KickSideLeftStable.xar");
		else
			amot->set_command("KickSideRightStable.xar");
	}
	_blk.publishSignal(*amot, "motion");
	_blk.publish_all();
	return 0;
}

void Kick::UserInit () {

	_blk.updateSubscription("worldstate", msgentry::SUBSCRIBE_ON_TOPIC);

	amot = new MotionActionMessage();
	rpm = new ReturnToPositionMessage();
	orientation = 0;
	readConf = false;
	by = 0.0;
	ownGoalX = 0.0; ownGoalY = 0.0; oppGoalX = 0.0; oppGoalY = 0.0;
	ownGoalLeftX = 0.0; ownGoalLeftY = 0.0; ownGoalRightX = 0.0; ownGoalRightY = 0.0;
	oppGoalLeftX = 0.0; oppGoalLeftY = 0.0; oppGoalRightX = 0.0; oppGoalRightY = 0.0;
}

std::string Kick::GetName () {
	return "Kick";
}



bool Kick::readGoalConfiguration(const std::string& file_name) {

	TiXmlDocument doc2(file_name.c_str());
	bool loadOkay = doc2.LoadFile();
	if (!loadOkay) {
		Logger::Instance().WriteMsg("Behavior",  " readGoalConfiguration: cannot read file " + file_name , Logger::Info);
		return false;
	}

	TiXmlNode * Ftr;
	TiXmlElement * Attr;
	double x, y;
	std::string ID;

	for (Ftr = doc2.FirstChild()->NextSibling(); Ftr != 0; Ftr = Ftr->NextSibling()) {
		if(Ftr->ToComment() == NULL){
			Attr = Ftr->ToElement();
			Attr->Attribute("x", &x);
			Attr->Attribute("y", &y);
			ID = Attr->Attribute("ID");
			if (ID == "YellowGoal"){
				oppGoalX = x;
				oppGoalY = y;
				ownGoalX = -oppGoalX;
				ownGoalY = -oppGoalY;
			}
			if (ID == "YellowLeft"){
				oppGoalLeftX = x;
				oppGoalLeftY = y;
				ownGoalLeftX = -oppGoalLeftX;
				ownGoalLeftY = -oppGoalLeftY;
			}
			if (ID == "YellowRight"){
				oppGoalRightX = x;
				oppGoalRightY = y;
				ownGoalRightX = -oppGoalRightX;
				ownGoalRightY = -oppGoalRightY;
			}
		}
	}
	return true;
}

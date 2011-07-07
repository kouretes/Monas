
#include "Kick.h"

namespace {
    ActivityRegistrar<Kick>::Type temp("Kick");
}

int Kick::Execute() {
	gsm = _blk->readState<GameStateMessage>("behavior");
	wimsg  = _blk->readData<WorldInfo>("behavior");
	pnm = _blk->readState<PlayerNumberMessage>("behavior");
	
	if(!readConf)
		readGoalConfiguration(ArchConfig::Instance().GetConfigPrefix() +"/Features.xml");
//#ifdef RETURN_TO_POSITION
		//rpm->set_goalietopos(true);
		//_blk->publishSignal(*rpm, "behavior");
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
	//}else if ( wimsg->balls(0).relativey() > -0.01 ){
		//amot->set_command("LeftKick");
		//_blk->publishSignal(*amot, "motion");
		////Logger::Instance().WriteMsg("Kick", "LeftKick", Logger::Info);
		//return 0;
	//}else{
		//amot->set_command("RightKick");
		//_blk->publishSignal(*amot, "motion");
	////	Logger::Instance().WriteMsg("Kick", "RightKick", Logger::Info);
		//return 0;
	//}
	if(pnm&&pnm->team_side()==TEAM_RED){
		oppGoalX = blueGoalX;
		oppGoalY = blueGoalY;
		
	}else if(pnm&&pnm->team_side()==TEAM_BLUE){
		oppGoalX = yellowGoalX;
		oppGoalY = yellowGoalY;
	}

	float ogb = k.anglediff2(atan2(oppGoalY - wimsg->myposition().y(), oppGoalX - wimsg->myposition().x()), wimsg->myposition().phi());

	if ((fabs(ogb) <= +45 * TO_RAD) && (fabs(ogb) > -45 * TO_RAD)) {
		orientation = 0;
	} else if ((fabs(ogb) > +45 * TO_RAD) && (fabs(ogb) <= +135 * TO_RAD)) {
		orientation = 1;
	} else if ((fabs(ogb) > +135 * TO_RAD) || (fabs(ogb) <= -135 * TO_RAD)) {
		orientation = 2;
	} else if ((fabs(ogb) <= -45 * TO_RAD) && (fabs(ogb) > -135 * TO_RAD)) {
		orientation = 3;
	}
	if (wimsg==0 || wimsg->balls_size()!=0)
		by = wimsg->balls(0).relativey();
	if (orientation == 0) {
		if (by > 0.0)
			amot->set_command("KickForwardLeft.xar"); //LeftKick
		else
			amot->set_command("KickForwardRight.xar"); //RightKick
	} else if (orientation == 3) {
		amot->set_command("KickSideLeftPierris.xar"); //"HardLeftSideKick"
	} else if (orientation == 1) {
		amot->set_command("KickSideRightPierris.xar"); //"HardRightSideKick"
	} else if (orientation == 2) {
		if (by > 0.0)
			amot->set_command("KickBackLeftPierris.xar"); //LeftBackHigh_carpet
		else
			amot->set_command("KickBackRightPierris.xar"); //RightBackHigh_carpet
	} else {
		if (by > 0.0)
			amot->set_command("KickSideLeftFast.xar");
		else
			amot->set_command("KickSideRightFast.xar");
	}
	_blk->publishSignal(*amot, "motion");
	return 0;
}

void Kick::UserInit () {
	_blk->subscribeTo("behavior",0);
	amot = new MotionActionMessage();
	rpm = new ReturnToPositionMessage();
	orientation = 0;
	readConf = false;
	by = 0.0;
	blueGoalX = 0.0;
	blueGoalY = 0.0;
	yellowGoalX = 0.0;
	yellowGoalY= 0.0;
	oppGoalX =0.0;
	oppGoalY = 0.0;
	oppGoalY = 0.0;
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
	string ID;

	for (Ftr = doc2.FirstChild()->NextSibling(); Ftr != 0; Ftr = Ftr->NextSibling()) {
		Attr = Ftr->ToElement();
		Attr->Attribute("x", &x);
		Attr->Attribute("y", &y);
		ID = Attr->Attribute("ID");

		if (ID == "SkyblueGoal"){
			blueGoalX = x/1000.0;
			blueGoalY = y/1000.0;
		}
		if (ID == "YellowGoal"){
			yellowGoalX = x/1000.0;
			yellowGoalY = y/1000.0;
		}
	}

	readConf = true;
	return true;
}

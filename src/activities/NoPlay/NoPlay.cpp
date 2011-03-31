
#include "NoPlay.h"
#include "architecture/archConfig.h"
#include "tools/logger.h"
#include "tools/toString.h"
#include "tools/XML.h"
#include "tools/XMLConfig.h"
#include "messages/RoboCupGameControlData.h"
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
namespace {
    ActivityRegistrar<NoPlay>::Type temp("NoPlay");
}

int NoPlay::Execute() {
	
	Logger::Instance().WriteMsg("NoPlay",  " Execute", Logger::Info);

	gsm = _blk->readState<GameStateMessage> ("behavior");
		if(!readConf)
			readRobotConfiguration(ArchConfig::Instance().GetConfigPrefix() + "/robotConfig.xml", kickOff);
		
		if(gsm.get()==0 ){
			Logger::Instance().WriteMsg("NoPlay",  " NO GSM", Logger::Info);
			bhmsg->set_headaction(DONOTHING);
		}else if(gsm->player_state()==PLAYER_PLAYING){
			cal = false;
			Logger::Instance().WriteMsg("NoPlay",  " PLAYER_PLAYING", Logger::Info);	
			return 0;
		}else{
			switch(gsm->player_state()){
				case PLAYER_PENALISED:
					Logger::Instance().WriteMsg("NoPlay",  " playerpenalised", Logger::Info);
					//if(prevaction!=CALIBRATE)
					velocityWalk(0.0, 0.0, 0.0, 1);
					curraction = CALIBRATE;						
					pmsg->set_posx(initX);
					pmsg->set_posy(initY);
					pmsg->set_theta(initPhi);
					_blk->publishState(*pmsg, "behavior");
					Logger::Instance().WriteMsg("NoPlay",  " publish pos", Logger::Info);
					rpm->set_goalietopos(true);
					_blk->publishSignal(*rpm, "behavior");
					Logger::Instance().WriteMsg("NoPlay",  " publish return to pos", Logger::Info);
				break;
				case PLAYER_SET:
					Logger::Instance().WriteMsg("NoPlay",  " playerset", Logger::Info);
					velocityWalk(0,0,0,1);
					curraction = DONOTHING;
				break;
				case PLAYER_READY:
					kickOff = gsm->kickoff();
					kcm->set_kickoff(kickOff);
					_blk->publishState(*kcm, "behavior");
					Logger::Instance().WriteMsg("NoPlay",  " playerready", Logger::Info);
					curraction = CALIBRATE;
				break;
				case PLAYER_INITIAL:
					Logger::Instance().WriteMsg("NoPlay",  " playerinitial", Logger::Info);
					velocityWalk( 0,0,0,1);
					curraction = CALIBRATE;
				break;
			}
		
						
	}
	
	bhmsg->set_headaction(curraction);	

	prevaction = curraction;
	_blk->publishSignal(*bhmsg, "behavior");
	Logger::Instance().WriteMsg("NoPlay",  " bgainw", Logger::Info);
			return 0;

}

void NoPlay::UserInit () {
	_blk->subscribeTo("behavior", 0);
	curraction = DONOTHING;
	prevaction = DONOTHING;
	cal = false;
	kickOff = false;
	readConf = false;
	posX = 0.30;
	posY = 0.30;
	bhmsg = new BToHeadMessage();
	kcm = new KickOffMessage();
	pmsg = new PositionMessage();
	rpm = new ReturnToPositionMessage();
	initX = 0.0;
	initY = 0.0;
	initPhi = 0.0; 
	//wmot = new MotionWalkMessage();
	wmot.add_parameter(0.0f);
	wmot.add_parameter(0.0f);
	wmot.add_parameter(0.0f);
	wmot.add_parameter(0.0f);
	}

std::string NoPlay::GetName () {
	return "NoPlay";
}

bool NoPlay::readRobotConfiguration(const std::string& file_name, bool kickoff) {
	readConf=true;
	XML config(file_name);
	typedef std::vector<XMLNode<std::string, float, std::string> > NodeCont;
	NodeCont teamPositions, robotPosition ;
	pnm = _blk->readState<PlayerNumberMessage>("behavior");
	Logger::Instance().WriteMsg("NoPlay",  " readConf "  , Logger::Info);
	int side = 1;
	initPhi = 0;
	if (pnm.get()==0)
		return false;
	
	if (kickoff)
		teamPositions = config.QueryElement<std::string, float, std::string>( "kickOff" );
	else
		teamPositions = config.QueryElement<std::string, float, std::string>( "noKickOff" );
	if( teamPositions.size()!=0)
		robotPosition = config.QueryElement<std::string, float, std::string>( "robot", &(teamPositions[0]) );
	Logger::Instance().WriteMsg("NoPlay",  " teamPo size" +_toString( teamPositions.size())+ "robotPos size" + _toString( robotPosition.size())  , Logger::Info);
    for ( NodeCont::iterator it = robotPosition.begin(); it != robotPosition.end(); it++ ) {
		Logger::Instance().WriteMsg("NoPlay",  " it "  , Logger::Info);
		if(it->attrb["number"] == 1 ){// pnm->player_number()){
			if(pnm->team_side()==TEAM_BLUE){
				side=-1;
				initPhi = 180*TO_RAD;
			}
			initX = side*(it->attrb["posx"]);
			initY = side*(it->attrb["posy"]);
	Logger::Instance().WriteMsg("NoPlay",  " readConf INIT X "+ _toString(initX) +" INITY "+_toString(initY) + " INITPHI " + _toString(initPhi)  , Logger::Info);
			pmsg->set_posx(initX);
			pmsg->set_posy(initY);
			pmsg->set_theta(initPhi); 
			_blk->publishState(*pmsg, "behavior");
			return true;
		}
        
	}
	return true;
}

void NoPlay::velocityWalk(double x, double y, double th, double f) {
	//Logger::Instance().WriteMsg("Aproachball",  " VelocityWalk", Logger::Info);
	wmot.set_command("setWalkTargetVelocity");
	wmot.set_parameter(0, x);
	wmot.set_parameter(1, y);
	wmot.set_parameter(2, th);
	wmot.set_parameter(3, f);
	_blk->publishSignal(wmot, "motion");
}

void NoPlay::littleWalk(double x, double y, double th) {
	//cout << x << " y " << y << " th " << th<<endl;
	wmot.set_command("walkTo");
	wmot.set_parameter(0, x);
	wmot.set_parameter(1, y);
	wmot.set_parameter(2, th);
	_blk->publishSignal(wmot, "motion");
}

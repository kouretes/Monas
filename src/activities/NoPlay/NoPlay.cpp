
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
using namespace std;

int NoPlay::Execute() {

	Logger::Instance().WriteMsg(GetName(),  " Execute", Logger::Info);
	gsm = _blk->readState<GameStateMessage> ("behavior");

	if(!readConf)
		readRobotConfiguration(ArchConfig::Instance().GetConfigPrefix() + "/robotConfig.xml");
	if(gsm.get()==0 ){
		Logger::Instance().WriteMsg(GetName(),  " NO GSM", Logger::Info);
		curraction = DONOTHING; //CALIBRATE;
		//if(prevaction!=CALIBRATE){
			//bhmsg->set_headaction(curraction);
		//}
		//else{
			//bhmsg->set_headaction(DONOTHING);
		//}
		prevaction = curraction;
		_blk->publishSignal(*bhmsg, "behavior");
		//Logger::Instance().WriteMsg(GetName(),  " bgainw", Logger::Info);
		return 0;

	}else if(gsm->player_state()==PLAYER_PLAYING){
	//	cal = false;
	//	Logger::Instance().WriteMsg(GetName(),  " PLAYER_PLAYING", Logger::Info);
//	_blk->publish_all();
		return 0;
	}

	currstate = gsm->player_state();
	teamColor = gsm->team_color();
	switch(currstate){
		case PLAYER_PENALISED:
		//	Logger::Instance().WriteMsg(GetName(),  " playerpenalised", Logger::Info);
			bhmsg->set_headaction(DONOTHING);
			curraction = DONOTHING;
			if(lastMove<= boost::posix_time::microsec_clock::universal_time()){
				velocityWalk(0.0f, 0.0f, 0.0f, 1.0f);
				lastMove = boost::posix_time::microsec_clock::universal_time()+boost::posix_time::seconds(5);
			}

			pmsg->set_posx(initX[0][teamColor]);
			pmsg->set_posy(initY[0][teamColor]);
			pmsg->set_theta(initPhi[0][teamColor]);
			_blk->publishState(*pmsg, "behavior");
	//		Logger::Instance().WriteMsg(GetName(),  " publish pos", Logger::Info);
			#ifdef PENALTY_ON
				;
			#else
			rpm->set_goalietopos(true);
			_blk->publishSignal(*rpm, "behavior");
			#endif
//			Logger::Instance().WriteMsg(GetName(),  " publish return to pos", Logger::Info);

		//goToPosition(initX, initY, initPhi);
		break;
		case PLAYER_SET:
		//	Logger::Instance().WriteMsg(GetName(),  " playerset", Logger::Info);

		//	if(lastMove<= boost::posix_time::microsec_clock::universal_time()){
			//	velocityWalk(0.0f, 0.0f, 0.0f, 1.0f);
				//lastMove = boost::posix_time::microsec_clock::universal_time()+boost::posix_time::seconds(5);
			//}
			if (prevstate!=PLAYER_SET){
				amot.set_command("Init.xar");
				_blk->publishSignal(amot, "motion");
				}
			curraction = SCANFORPOST;
			bhmsg->set_headaction(curraction);
		break;
		case PLAYER_READY:
			kickOff = gsm->kickoff();
			kcm->set_kickoff(kickOff);
			_blk->publishState(*kcm, "behavior");

			if(kickOff){	//in 0 position of the table kickoff positions
				pmsg->set_posx(initX[0][teamColor]);
				pmsg->set_posy(initY[0][teamColor]);
				pmsg->set_theta(initPhi[0][teamColor]);
			}else{	//in 1 position of the table not in kickoff positions
				pmsg->set_posx(initX[1][teamColor]);
				pmsg->set_posy(initY[1][teamColor]);
				pmsg->set_theta(initPhi[1][teamColor]);
			}
			_blk->publishState(*pmsg, "behavior");
			//Logger::Instance().WriteMsg(GetName(),  " playerready", Logger::Info);
			curraction = SCANFORPOST;
			bhmsg->set_headaction(curraction);
		break;
		case PLAYER_INITIAL:
		//	Logger::Instance().WriteMsg(GetName(),  " playerinitial", Logger::Info);
	//		velocityWalk(0,0,0,1);
			if(prevstate!=PLAYER_INITIAL){
				firstInit = boost::posix_time::microsec_clock::universal_time();
			}
			if(firstInit + boost::posix_time::seconds(3) <=boost::posix_time::microsec_clock::universal_time()){
				curraction = CALIBRATE;
				if(prevaction!=CALIBRATE){
				//	Logger::Instance().WriteMsg(GetName(),  " playerinitial CALIBRATE", Logger::Info);
					calibrate_time = boost::posix_time::microsec_clock::universal_time();
					bhmsg->set_headaction(curraction);
				}
				else{
					if(calibrate_time+boost::posix_time::seconds(15) <boost::posix_time::microsec_clock::universal_time()){
						bhmsg->set_headaction(curraction);
						calibrate_time = boost::posix_time::microsec_clock::universal_time();
					}else
						bhmsg->set_headaction(DONOTHING);
		//			Logger::Instance().WriteMsg(GetName(),  " playerinitial DONOTHING", Logger::Info);
				}
			}else
				curraction = DONOTHING;
		break;
	}

	prevaction = curraction;
	prevstate = currstate;
	_blk->publishSignal(*bhmsg, "behavior");
	//Logger::Instance().WriteMsg(GetName(),  " bgainw", Logger::Info);
	_blk->publish_all();
	return 0;

}

void NoPlay::UserInit () {
	_blk->updateSubscription("behavior", msgentry::SUBSCRIBE_ON_TOPIC);
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
	myPosX = 0.0;
	myPosY = 0.0;
	myPhi = 0.0;
	teamColor = TEAM_BLUE;
	playernum = -1;
	lastMove =  boost::posix_time::microsec_clock::universal_time();
	lastObsm =  boost::posix_time::microsec_clock::universal_time();
	firstInit =  boost::posix_time::microsec_clock::universal_time();
	calibrate_time =  boost::posix_time::microsec_clock::universal_time();
	currstate = PLAYER_PENALISED;
	prevstate = PLAYER_PENALISED;
	//wmot = new MotionWalkMessage();
	wmot.add_parameter(0.0f);
	wmot.add_parameter(0.0f);
	wmot.add_parameter(0.0f);
	wmot.add_parameter(0.0f);
	}

std::string NoPlay::GetName () {
	return "NoPlay";
}

bool NoPlay::readRobotConfiguration(const std::string& file_name) {

	playernum =-1;
	if(gsm!=0)
		playernum = gsm->player_number();

	if(playernum==-1){
		//Logger::Instance().WriteMsg(GetName(), " Invalid player number " , Logger::Error);
		return false;
	}

	readConf = true;
	XML config(file_name);
	typedef std::vector<XMLNode<std::string, float, std::string> > NodeCont;
	NodeCont teamPositions, robotPosition ;
	//Logger::Instance().WriteMsg(GetName(), " readConf " , Logger::Info);


	for (int i = 0; i < 2; i++) //KICKOFF==0, NOKICKOFF == 1
	{
		string kickoff=(i==0)?"KickOff":"noKickOff";
		bool found = false;

		teamPositions = config.QueryElement<std::string, float, std::string>(kickoff);

		if (teamPositions.size() != 0)
		robotPosition = config.QueryElement<std::string, float, std::string>("robot", &(teamPositions[0]));

		// Logger::Instance().WriteMsg(GetName(), " teamPo size" + _toString(teamPositions.size()) + "robotPos size" + _toString(robotPosition.size()), Logger::Info);

		for (NodeCont::iterator it = robotPosition.begin(); it != robotPosition.end(); it++)
		{
		// Logger::Instance().WriteMsg(GetName(), " it ", Logger::Info);
			if (it->attrb["number"] == playernum)///////////////////////////////////////////
			{
			initPhi[i][TEAM_BLUE] = 0;
			if((it->attrb["posx"])!=0.0)
				initX[i][TEAM_BLUE] = -1*(it->attrb["posx"]);
			else
				initX[i][TEAM_BLUE] = (it->attrb["posx"]);

			if((it->attrb["posy"])!=0.0)
				initY[i][TEAM_BLUE] = -1*(it->attrb["posy"]);
			else
				initY[i][TEAM_BLUE] = (it->attrb["posy"]);

			initPhi[i][TEAM_RED] = 180 * TO_RAD;
			initX[i][TEAM_RED] = (it->attrb["posx"]);
			initY[i][TEAM_RED] = (it->attrb["posy"]);

		//	Logger::Instance().WriteMsg(GetName(), " readConf TEAM_BLUE INIT X "+ kickoff + " "+ _toString(initX[i][TEAM_BLUE]) + " INITY " + _toString(initY[i][TEAM_BLUE]) + " INITPHI " + _toString(initPhi[i][TEAM_BLUE]), Logger::Info);
		//	Logger::Instance().WriteMsg(GetName(), " readConf TEAM_RED INIT X "+ kickoff + " "+ _toString(initX[i][TEAM_RED]) + " INITY " + _toString(initY[i][TEAM_RED]) + " INITPHI " + _toString(initPhi[i][TEAM_RED]), Logger::Info);

			found = true;
			}
		}

		if(!found)
		{
		Logger::Instance().WriteMsg(GetName(), " Unable to find initial " + kickoff+ " position for player number " + _toString(playernum) , Logger::Error);
		readConf = false;
		}
	}
	return readConf;

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

void NoPlay::goToPosition(float x, float y, float phi){

	curraction = SCANFORPOST;
	wimsg = _blk->readData<WorldInfo>("behavior");
	if(wimsg.get()!=0){
		myPosX = wimsg->myposition().x();
		myPosY = wimsg->myposition().y();
		myPhi = wimsg->myposition().phi();
		Logger::Instance().WriteMsg(GetName(),  " X "+ _toString(myPosX) +" Y "+_toString(myPosY) + " PHI " + _toString(myPhi)  , Logger::Info);
	}
	float relativeX, relativeY, relativePhi;
	relativeX = rotation(x, -y, myPhi) - myPosX;
	relativeY = rotation(y, y, myPhi) - myPosY;

	float velx, vely;
	float angle = anglediff2(atan2(y -myPosY, x - myPosX), myPhi);
	relativePhi = anglediff2(phi,myPhi);
	vely = 0.8*sin(angle);
	velx = 0.8*cos(angle);

		if(velx>1)
			velx=1;
		else if(velx<-1)
			velx = -1;

		if(vely>1)
			vely=1;
		else if(vely<-1)
			vely = -1;

	//Logger::Instance().WriteMsg(GetName(),  " velx" + _toString(velx)+ " vel y " + _toString(vely), Logger::Info);
	if(lastMove <= boost::posix_time::microsec_clock::universal_time()){
			velocityWalk(velx, vely, 0.1*relativePhi, 1.0);
		lastMove = boost::posix_time::microsec_clock::universal_time() + boost::posix_time::milliseconds(500);
	}

	bhmsg->set_headaction(curraction);
	return;
}


float NoPlay::rotation(float a, float b, float theta){
	//Logger::Instance().WriteMsg("Rotation",  " Entered", Logger::Info);
	return a*cos(theta) + b*sin(theta);


}

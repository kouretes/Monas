#include "LBehavior.h"
/*
#include "hal/robot/generic_nao/kAlBroker.h"

#include "alxplatform.h"
#include <albroker.h>
#include <alproxy.h>
#include <almemoryfastaccess.h>
#include "almotionproxy.h"
#include "almemoryproxy.h"
#include "albrokermanager.h"
#include "alvalue.h"*/

#include "tools/logger.h"
#include "tools/toString.h"
#include "messages/RoboCupGameControlData.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include "hal/robot/generic_nao/robot_consts.h"


using namespace boost::posix_time;


namespace {
	ActivityRegistrar<LBehavior>::Type temp("LBehavior");
}
using namespace std;

LBehavior::LBehavior()        {
}

double wrapToPi(double angle)
{
	while (angle > M_PI)
		angle -= 2.0 * M_PI;
	while (angle < -M_PI)
		angle += 2.0 * M_PI;
	return angle;
}
double wrapTo2Pi(double angle)
{
	while (angle > 2.0 * M_PI)
		angle -= 2.0 * M_PI;
	while (angle < -2.0 * M_PI)
		angle += 2.0 * M_PI;
	return angle;
}
double wrapTo0_2Pi(double angle)
{
	while (angle > 2.0 * M_PI)
		angle -= 2.0 * M_PI;
	while (angle < 0)
		angle += 2.0 * M_PI;
	return angle;
}

double anglediff2(double a1, double a2)
{
	return wrapToPi(wrapToPi(a1 + M_PI - a2) - M_PI);
}

double anglediff(double a1, double a2)
{
	return fabs(wrapTo0_2Pi(a1 + M_PI - a2) - M_PI);
}



void LBehavior::UserInit() {
	_blk->subscribeTo("vision", 0);
	_blk->subscribeTo("sensors", 0);
	_blk->subscribeTo("behavior", 0);
	_blk->subscribeTo("obstacle", 0);

	wmot = new MotionWalkMessage();
	//wmot->set_topic("motion");
	wmot->add_parameter(0.0f);
	wmot->add_parameter(0.0f);
	wmot->add_parameter(0.0f);
	wmot->add_parameter(0.0f);

	hmot = new MotionHeadMessage();
	//hmot->set_topic("motion");
	//hmot->add_parameter(0.0f);
	//hmot->add_parameter(0.0f);

	leftright = 1;
	headpos = 0;
	hmot->set_command("setHead");
	hmot->add_parameter(0.0f);
	hmot->add_parameter(-0.66322512);


	amot = new MotionActionMessage();
	for (int i = 0; i < 2; i++)     	// i: kick-off
		for (int j = 0; j < 2; j++){	// j: team color
			initX[i][j] = 0.0;
			initY[i][j] = 0.0;
			initPhi[i][j] = 0.0;
		}

	locReset = new LocalizationResetMessage();

	ballfound = 0;
	readRobotConf = false;

	scanforball = true;
	startscan = true;

	calibrated = 0;

	stopped = true;
	play = false;
	kickoff = false;
	cX=0;cY=0;ct=0;

	//hjsm = 0;
	//bmsg = 0;
	//gsm = 0;
	//obsm = 0;
	//om = 0;

	readytokick = false;
	back = 0;
	direction = 1;
	turning = false;
	count = 0;
	obstacleFront = false;
	gameState = PLAYER_INITIAL;

	teamColor = TEAM_BLUE;
	orientation = 0;

	srand(time(0));

	lastwalk = microsec_clock::universal_time();

	lastrun  = lastball = lastwalk;
	lastkick=lastrun-seconds(4);

	readConfiguration(ArchConfig::Instance().GetConfigPrefix() + "/team_config.xml");
	readRobotConfiguration(ArchConfig::Instance().GetConfigPrefix() + "/robotConfig.xml");
	Logger::Instance().WriteMsg("LBehavior", "Initialized", Logger::Info);
}

int LBehavior::MakeTrackBallAction() {

	/*float overshootfix = 0.50;
	float cx = bmsg->cx();
	float cy = bmsg->cy();
	*/
	//if (fabs(cx) > 0.015 || fabs(cy) > 0.015) {
		//hmot->set_command("setHead");
		//hmot->set_parameter(0, bmsg->referenceyaw() - overshootfix * cx);
		//hmot->set_parameter(1, bmsg->referencepitch() - overshootfix * cy);
		hmot->set_command("setHead");
        hmot->set_parameter(0, bmsg->referenceyaw());
		hmot->set_parameter(1,  bmsg->referencepitch());
		_blk->publishSignal(*hmot, "motion");
		cout<<"Track step"<<endl;
	//}
	return 1;
}


void LBehavior::test() {

	if (om!=0) {
		Logger::Instance().WriteMsg("LBehavior", "Obstacle - Direction: " + _toString(om->direction()), Logger::Info);
		if (om->direction() == 0)
			velocityWalk(0.0, 0.0, 1.0, 1.0);
	}
	else
		velocityWalk(1.0, 0.0, 0.0, 1.0);
}

int LBehavior::Execute() {

	read_messages();

	ptime now=microsec_clock::microsec_clock::universal_time();
	ptime prelastrun=lastrun;
	ptime lastrun=now;
	if (gsm != 0) {
		Logger::Instance().WriteMsg("LBehavior", " Player_state " + _toString(gsm->player_state()), Logger::ExtraExtraInfo);
		gameState = gsm->player_state();
		teamColor = gsm->team_color();

		if (gameState == PLAYER_PLAYING) {
			if (calibrated == 2) {
				play = true;
				//littleWalk(0.01,0.0,0.0,1);
			}
			else if (calibrated == 0) {
				calibrate();
			}
			else if (calibrated == 1) {
				// wait
			}
			/*if (calibrated == 0)
			{
				calibrate();
			}
			int p = ((kickoff = gsm->kickoff()))?0:1;
			gotoPosition(initX[p][teamColor],initY[p][teamColor],initPhi[p][teamColor]);
			play = false;
			return 0;*/

		}
		else if (gameState == PLAYER_INITIAL) {
			play = false;
		}
		else if (gameState == PLAYER_READY) {
			if (calibrated == 0)
			{
				calibrate();
			}
			int p = ((kickoff = gsm->kickoff()))?0:1;
			gotoPosition(initX[p][teamColor],initY[p][teamColor],initPhi[p][teamColor]);
			play = false;
			return 0;
		}
		else if (gameState == PLAYER_SET) {
			play = false;
			calibrate();

		}
		else if (gameState == PLAYER_FINISHED) {
			play = false;
		}
		else if (gameState == PLAYER_PENALISED) {
			play = false;
			_blk->publishSignal(*locReset, "behavior");
			calibrate();
			velocityWalk(0.0,0.0,0.0,1);

		}
	}

	if (gameState == PLAYER_PLAYING) {
		if (calibrated == 2) {
			play = true;
		}
		else if (calibrated == 0) {
			calibrate();
		}

	}
	if (play) {

		if (bmsg != 0) {
			Logger::Instance().WriteMsg("LBehavior", "BallTrackMessage", Logger::ExtraExtraInfo);
			if (bmsg->radius() > 0) { //This means that a ball was found
				scanforball = false; //if you are scanning for ball please stop now
				back = 0;
				MakeTrackBallAction();
				lastball = now;
				ballfound = 1;
			} else {
				if (lastball+seconds(4)<now)
					ballfound = 0;
			}
		}
		//return 0;
		//Logger::Instance().WriteMsg("LBehavior", "ballfound Value: " + _toString(ballfound), Logger::ExtraInfo);

		//float X=0.0, Y=0.0, theta=0.0;
		float bd=0.0, bx=0.0, by=0.0, bb=0.0;
		float posx=0.117, posy=0.04;
		//static float lastx=0,lasty=0;

		if(wim != 0 && !turning){
			if (wim->balls_size() > 0) {
				int side=1;
				bx = wim->balls(0).relativex()+wim->balls(0).relativexspeed()*(now-wimtime).total_milliseconds()/1000.0;
				by = wim->balls(0).relativey()+wim->balls(0).relativeyspeed()*(now-wimtime).total_milliseconds()/1000.0;
				bd = sqrt(pow(bx,2)+pow(by,2));
				bb = atan2(by,bx);

				side = (bb > 0) ? 1 : -1;
				Logger::Instance().WriteMsg("LBehavior", "Measurements - Distance: " + _toString(bd) + "  Bearing: " + _toString(bb) + "  BX: " + _toString(bx) + "  BY: " + _toString(by), Logger::Info);

				if (ballfound==1) {

					readytokick = true;

					if ( fabs( bx - posx ) > 0.015  || fabs( by - (side*posy) ) > 0.015) {
						//Y = gainFine * ( by - (side*posy) );
						readytokick = false;
					}
					float offsety=side*posy;

					if (!readytokick) {
					   // if(abs(bd>0.6))
						//{

							static float X=0,Y=0,th=0,f=0.2;
							//X=(bx-posx)*2;
							X=(bx-posx)*3;
							X=X>0?X:X-0.001;
							X=X>1?1:X;
							X=X<-1?-1:X;
							//Y=(by-offsety)*1.6;
							Y=(by-offsety)*4;
	//
	//						lastx=bx;
	//						lasty=by;

							if(bd>0.30)
							{
								if(bx<0)
									th=0.2 *Y;
								else
									th=0.1 *Y;

							}
							else
							{
								th=-0.06*by*(Y>0?-1:1);
							}

							Y=Y>0?Y+0.01:Y-0.01;
							Y=Y>1?1:Y;
							Y=Y<-1?-1:Y;
							f=1;

							th=th>1?1:th;
							th=th<-1?-1:th;
							velocityWalk(X,Y,th,f);
					}
				}
			} else {
				scanforball = true;
				ballfound = 0;
			}
		} else {
			readytokick = false;
		}

		/* Ready to take action */
		if (readytokick && !turning) {
			if (by > 0.0) {
				amot->set_command("KickForwardLeft.xar"); //bb>0  "KickSideLeftFast.xar"
				direction = -1;
			}
			else {
				amot->set_command("KickForwardRight.xar"); //"KickSideRightFast.xar"
				direction = +1;
			}
			_blk->publishSignal(*amot, "motion");

			kickoff = false;

			lastkick=microsec_clock::universal_time()+seconds(4);

			scanforball = true;
			readytokick = false;
		}

		if ( (ballfound == 0) && !readytokick && !turning ) {
			if (!scanforball) {
				startscan = true;
				velocityWalk(0.0, 0.0, 0.0, 1.0);
			}
			scanforball = true;
		}

		if (scanforball ) {

			HeadYaw= allsm->jointdata(KDeviceLists::HEAD+KDeviceLists::YAW);
			HeadPitch= allsm->jointdata(KDeviceLists::HEAD+KDeviceLists::PITCH);
			HeadScanStep();
		}

	} else if (!play) {   // Non-Play state
		velocityWalk(0.0, 0.0, 0.0, 1.0);
	}

	return 0;
}




void LBehavior::HeadScanStep() {

	float  blue1y,blue1p,blue2y,blue2p;
	blue1y=0.75;
	blue1p=0.38;
	blue2y=0;
	blue2p=-0.55;
	float green1y,green1p,green2y,green2p;
	green1y=1.45;
	green1p=-0.42;
	green2y=0;
	green2p=0.35;
	float red1y,red1p,red2y,red2p;
	red1y=1.8;
	red1p=-0.39;
	red2y=0;
	red2p=-0.6;
	static enum {BLUE,RED,GREEN} state=BLUE;
	static enum {START,MIDDLE,END} phase=START;

	if (startscan) {





		//BE CAREFULL the max sign is according to sensors values (max maybe negative! :p)
		ysign=HeadYaw.sensorvalue()>0?1:-1; //Side
		targetYaw=blue1y*ysign;
		targetPitch=blue1p;
		state=BLUE;
		phase=START;

		hmot->set_command("setHead");
		hmot->set_parameter(0, targetYaw);
		hmot->set_parameter(1, targetPitch);
		_blk->publishSignal(*hmot, "motion");
		waiting=0;

		startscan=false;
		return;

	}
	waiting++;
	if( (fabs(targetPitch-HeadPitch.sensorvalue())<=OVERSH &&fabs(targetYaw -HeadYaw.sensorvalue())<=OVERSH )
		|| waiting>=WAITFOR)
	{

		waiting=0;
		if(phase==START)
		{
			phase=MIDDLE;
			switch(state)
			{
			case BLUE:
				targetYaw=blue2y;
				targetPitch=blue2p;
				break;
			case GREEN:
				targetYaw=green2y;
				targetPitch=green2p;
				break;
			case RED:
				targetYaw=red2y;
				targetPitch=red2p;
				break;
			}

		}
		else if(phase==MIDDLE)
		{
			ysign=-ysign;
			phase=END;
			switch(state)
			{
			case BLUE:
				targetYaw=blue1y*ysign;
				targetPitch=blue1p;
				break;
			case GREEN:
				targetYaw=green1y*ysign;
				targetPitch=green1p;
				break;
			case RED:
				targetYaw=red1y*ysign;
				targetPitch=red1p;
				break;
			}
		}
		else
		{
			phase=START;
			switch(state)
			{
			case BLUE:
				state=GREEN;
				targetYaw=green1y*ysign;
				targetPitch=green1p;
				break;
			case GREEN:
				state=RED;
				targetYaw=red1y*ysign;
				targetPitch=red1p;
				break;
			case RED:
				state=BLUE;
				targetYaw=blue1y*ysign;
				targetPitch=blue1p;
				break;
			}
		}

		hmot->set_command("setHead");
		hmot->set_parameter(0, targetYaw);
		hmot->set_parameter(1, targetPitch);
		_blk->publishSignal(*hmot, "motion");

	}
	return ;

}

void LBehavior::read_messages() {

	//if (gsm != 0) delete gsm;
	//if (bmsg != 0) delete bmsg;
	//if (hjsm != 0) delete hjsm;
	//if (obsm != 0) delete obsm;
	//if (om != 0) delete om;

	gsm  = _blk->readState<GameStateMessage> ("behavior");
	bmsg = _blk->readSignal<BallTrackMessage> ("vision");
	allsm = _blk->readData<AllSensorValuesMessage> ("sensors");
	//obsm = _blk->readSignal<ObservationMessage> ("vision");
	om   = _blk->readSignal<ObstacleMessage> ("obstacle");
	wim  = _blk->readData<WorldInfo> ("behavior","localhost",&wimtime);


	//Logger::Instance().WriteMsg("LBehavior", "read_messages ", Logger::ExtraExtraInfo);
	boost::shared_ptr<const CalibrateCam> c= _blk->readState<CalibrateCam> ("vision");
	if (c != NULL) {
		if (c->status() == 1)
			calibrated = 2;
	}
}

double LBehavior::Rand()
{
    return rand() / double(RAND_MAX);
}

void LBehavior::velocityWalk(double ix, double iy, double it, double f)
{
	double x, y, t;
	x = ix;
	y = iy;
	t = it;

	wmot->set_command("setWalkTargetVelocity");

	if ( (x==0.0) && (y==0.0) && (t==0.0) ) {
		cX = 0.0;
		cY = 0.0;
		ct = 0.0;
	}
	else {
		if( lastwalk + milliseconds(200) > microsec_clock::universal_time() )
			return;
		x = x>+1.0?+1.0:x;
		x = x<-1.0?-1.0:x;
		y = y>+1.0?+1.0:y;
		y = y<-1.0?-1.0:y;
		t = t>+1.0?+1.0:t;
		t = t<-1.0?-1.0:t;
		cX = 0.75*cX+0.25*x;
		cY = 0.75*cY+0.25*y;
		ct = 0.75*ct+0.25*t;
	}

	wmot->set_parameter(0, cX);
	wmot->set_parameter(1, cY);
	wmot->set_parameter(2, ct);
	wmot->set_parameter(3, f);
	_blk->publishSignal(*wmot, "motion");
	lastwalk = microsec_clock::universal_time();
}


void LBehavior::littleWalk(double x, double y, double th)
{
	wmot->set_command("walkTo");
	wmot->set_parameter(0, x);
	wmot->set_parameter(1, y);
	wmot->set_parameter(2, th);
	_blk->publishSignal(*wmot, "motion");
}

void LBehavior::calibrate()
{
	CalibrateCam v;
	v.set_status(0);
	_blk->publishState(v, "vision");
	calibrated = 1;
}
/*
 * this function set's player number in global variable playernum
 * and initial team color in global variable teamColor
 *
 * This function is used in UserInit function
 *
 * */

bool LBehavior::readConfiguration(const std::string& file_name) {
	XMLConfig config(file_name);

	playernum = -1;
	if (!config.QueryElement("player", playernum))
		Logger::Instance().WriteMsg("LBehavior", "Configuration file has no player, setting to default value: " + _toString(playernum), Logger::Error);

	//If color is changed default configuration color does need to be changed
	std::string color = "blue";
	teamColor = TEAM_BLUE;
	if (!config.QueryElement("default_team_color", color))
		Logger::Instance().WriteMsg("LBehavior", "Configuration file has no team_color, setting to default value: " + color, Logger::Error);
	if (color == "blue")
		teamColor = TEAM_BLUE;
	else if (color == "red")
		teamColor = TEAM_RED;
	else
		Logger::Instance().WriteMsg("LBehavior", "Undefined color in configuration, setting to default value: " + color, Logger::Error);

	return true;
}

/*
 * this function set's initial robot's position:
 * in x axis : initX in meters
 * in y axis : initY in meters
 * in rotation : initPhi in rads
 *
 *
 * This function is used in PLAYER_READY state
 *  */
bool LBehavior::readRobotConfiguration(const std::string& file_name) {
	if(playernum==-1){
		Logger::Instance().WriteMsg("LBehavior",  " Invalid player number "  , Logger::Error);
		return false;
	}
	readRobotConf = true;
	XML config(file_name);
	typedef std::vector<XMLNode<std::string, float, std::string> > NodeCont;
	NodeCont teamPositions, robotPosition ;
	Logger::Instance().WriteMsg("LBehavior",  " readConf "  , Logger::Info);
	int side = 1;

	for (int i = 0; i < 2; i++)  //KICKOFF==0, NOKICKOFF == 1
	{
		string kickoff=(i==0)?"KickOff":"noKickOff";
		bool found = false;

		teamPositions = config.QueryElement<std::string, float, std::string>(kickoff);

		if (teamPositions.size() != 0)
			robotPosition = config.QueryElement<std::string, float, std::string>("robot", &(teamPositions[0]));

//		Logger::Instance().WriteMsg("LBehavior", " teamPo size" + _toString(teamPositions.size()) + "robotPos size" + _toString(robotPosition.size()), Logger::Info);

		for (NodeCont::iterator it = robotPosition.begin(); it != robotPosition.end(); it++)
		{
//			Logger::Instance().WriteMsg("LBehavior", " it ", Logger::Info);
			if (it->attrb["number"] == playernum)
			{
				initPhi[i][TEAM_BLUE] = 0;
				initX[i][TEAM_BLUE] = -1*(it->attrb["posx"]);
				initY[i][TEAM_BLUE] = -1*(it->attrb["posy"]);

				initPhi[i][TEAM_RED] = 180 * TO_RAD;
				initX[i][TEAM_RED] = (it->attrb["posx"]);
				initY[i][TEAM_RED] = (it->attrb["posy"]);

				Logger::Instance().WriteMsg("LBehavior", " readConf TEAM_BLUE INIT X "+ kickoff + " "+ _toString(initX[i][TEAM_BLUE]) + " INITY " + _toString(initY[i][TEAM_BLUE]) + " INITPHI " + _toString(initPhi[i][TEAM_BLUE]), Logger::Info);
				Logger::Instance().WriteMsg("LBehavior", " readConf TEAM_RED INIT X "+ kickoff + " "+ _toString(initX[i][TEAM_RED]) + " INITY " + _toString(initY[i][TEAM_RED]) + " INITPHI " + _toString(initPhi[i][TEAM_RED]), Logger::Info);

				found = true;
			}
		}
		if(!found)
		{
			Logger::Instance().WriteMsg("LBehavior",  " Unable to find initial " + kickoff+ " position for player number " + _toString(playernum) , Logger::Error);
			readRobotConf = false;
		}
	}
	return readRobotConf;
}

//Goes to position directly
void LBehavior::gotoPosition(float target_x,float target_y, float target_phi)
{
	float robot_x,robot_y,robot_phi,robot_confidence;
	if(wim.get() != 0){
		robot_x = wim->myposition().x();
		robot_y = wim->myposition().y();
		robot_phi = wim->myposition().phi();
		robot_confidence = wim->myposition().confidence();
	}else{
		Logger::Instance().WriteMsg("LBehavior",  "No WorldInfo Message" , Logger::Error);
		return;
	}


	// Head Scan simple step
	bool headscan = true;
	if (headscan)
	{
//		if (count % 100 == 2)
//		{
			hmot->set_command("setHead");
//			//hmot.set_parameter(0,0.0f);
//			hmot->set_parameter(1, -0.66322512);
//		}

		if (fabs(headpos) > 2.08) // 1.3
			leftright *= -1;

		headpos += 0.1 * leftright;

		hmot->set_parameter(0, headpos);
		Logger::Instance().WriteMsg(GetName(), "pitch " + _toString(headpos), Logger::Info);
		float abspos = fabs(headpos);
		if(abspos<1.57)
			hmot->set_parameter(1, (0.145 * fabs(headpos)) - 0.752);
		else
			hmot->set_parameter(1, (-0.0698 * (fabs(headpos)-1.57)) - 0.52);
		_blk->publishSignal(*hmot, "motion");

	}

//	{
//		if (count % 400 == 0)
//			if (count % 100 == 0)
//			{
//				bhmsg->set_headaction(SCANFORBALL);
//				_blk->publishState(*bhmsg, "behavior");
//			} else
//			{
//				bhmsg->set_headaction(SCANFORPOST);
//				_blk->publishState(*bhmsg, "behavior");
//			}
//
//	}


	//Go to target simple behavior
	float Robot2Target_bearing = anglediff2(atan2(target_y - robot_y, target_x - robot_x), robot_phi);
	float Distance2Target = sqrt((target_x-robot_x)*(target_x-robot_x)+(target_y-robot_y)*(target_y-robot_y));

//	cout<<"Distance2Target:"<<Distance2Target<<endl;

	//cout << "Robot2Target_bearing*TO_DEG  " << Robot2Target_bearing * TO_DEG << endl;
	//cout << atan2(target.y - AgentPosition.y, target.x - AgentPosition.x) << endl;
	//cout << AgentPosition.theta << endl << endl;



	float VelX, VelY, Rot, freq;

	//TRy to get the robot to the desired position ...

	VelX =  cos(Robot2Target_bearing);
	VelY =  sin(Robot2Target_bearing);


	if(Distance2Target<0.3)
	{
		VelX/=2.0;
		VelY/=2.0;
		Rot = anglediff2(target_phi, robot_phi)*0.5 ;
		freq=Distance2Target;
	}
	else if(fabs(Robot2Target_bearing)>1)
	{
		VelX/=4.0;
		VelY/=4.0;
		Rot=Robot2Target_bearing*0.4;
		freq = 0.5;
	}
	else
	{
		Rot = Robot2Target_bearing*0.1 ;
		freq=1;
	}

	velocityWalk(VelX, VelY, Rot, freq);
}

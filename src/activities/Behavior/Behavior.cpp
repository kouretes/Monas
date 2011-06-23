#include "Behavior.h"

#include <math.h>
#include "tools/logger.h"
#include "tools/toString.h"
#include "messages/RoboCupGameControlData.h"
#include "hal/robot/generic_nao/robot_consts.h"

#define DISTANCE_2(dx,dy) (sqrt(dx * dx + dy*dy))

using namespace boost::posix_time;

namespace {
	ActivityRegistrar<Behavior>::Type temp("Behavior");
}
using namespace std;

Behavior::Behavior() {

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


void Behavior::UserInit() {
	readConfiguration(ArchConfig::Instance().GetConfigPrefix() + "/team_config.xml");
	
	_blk->subscribeTo("vision", 0);
	_blk->subscribeTo("sensors", 0);
	_blk->subscribeTo("behavior", 0);
	_blk->subscribeTo("obstacle", 0);

	wmot = new MotionWalkMessage();
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

	ballfound = 0;
	readRobotConf = false;

	scanforball = true;
	startscan = true;
	calibrated = 0;

	stopped = true;
	play = false;
	kickoff = false;
	cX=0.0;cY=0.0;ct=0.0;
	bd=0.0; bb=0.0; bx=0.0; by=0.0; posx=0.0; posy=0.0;
	side = +1;

	readytokick = false;
	back = 0;
	direction = 1;
	count = 0;
	obstacleFront = false;
	gameState = PLAYER_INITIAL;
	teamColor = TEAM_BLUE;
	orientation = 0;

	srand(time(0));

	lastmove = microsec_clock::universal_time();
	lastball = microsec_clock::universal_time();
	lastwalk = microsec_clock::universal_time();

	readConfiguration(ArchConfig::Instance().GetConfigPrefix() + "/team_config.xml");
	readRobotConfiguration(ArchConfig::Instance().GetConfigPrefix() + "/robotConfig.xml");

	Logger::Instance().WriteMsg("Behavior", "Initialized: My number is " + _toString(playerNumber) + " and my color is " + _toString(teamColor), Logger::Info);
}

void Behavior::mgltest() {

	if (om!=0) {
		Logger::Instance().WriteMsg("Behavior", "Obstacle - Direction: " + _toString(om->direction()), Logger::Info);
		if (om->direction() == 0)
			velocityWalk(0.0, 0.0, 1.0, 1.0);
	}
	else
		velocityWalk(1.0, 0.0, 0.0, 1.0);
}

int Behavior::Execute() {

	read_messages();
	
	GetGameState();

	if ( (gameState == PLAYER_READY) || (gameState == PLAYER_SET) || (gameState == PLAYER_PLAYING) ) {
		if (calibrated == 0) {
			calibrate();
			return 0;
		}
		if (calibrated == 1)
			return 0;
	}

	if (gameState == PLAYER_PLAYING) {

		CheckForBall();
		CheckForGoals();
		
		readytokick = false;
		
		if (ballfound==1 && microsec_clock::universal_time()<lastball + seconds(5)) {

			posx=0.12, posy=0.03; // Desired ball position for kick
			double epsx = 0.02, epsy = 0.02; // Desired precision
			if ( fabs( bx-posx ) < epsx  && fabs( by-(side*posy) ) < epsy ) {
				readytokick = true;
				Kick(side);
				direction = (side == +1)?-1:+1;
			}
			
			if (!readytokick) {
				if (orientation == 1)
					side = -1;
				else if (orientation == 3) 
					side = +1;
				static double X=0.0, Y=0.0, t=0.0, f=1.0, gain = 1.0;
				double maxd = fmaxf(fabs(bx-posx),fabs(by-side*posy));
				f    = fminf(1.0, 0.4+(maxd/0.5));
				gain = fminf(1.0, 0.0+(maxd/0.5));
				X = gain*(bx-posx)/maxd;
				Y = gain*(by-side*posy)/maxd;
				t = gain*(bb/3.14);
				velocityWalk(X,Y,t,f);
			}
		}else{
			scanforball = true;
			ballfound = 0;
		}

		if (ballfound == 0) {
			if (!scanforball) {
				startscan = true;
				velocityWalk(0.0,0.0,0.0,1.0);
				lastmove = microsec_clock::universal_time();
			}
			scanforball = true;
			if (lastmove + seconds(5) < microsec_clock::universal_time()) {
				littleWalk(direction * 0.20, 0.0, direction * 60 * TO_RAD);
				lastmove = microsec_clock::universal_time();
			}
			if (scanforball)
				HeadScanStep();
		}
	} 
	else if (gameState == PLAYER_READY) {
		;
	}
	else if (gameState == PLAYER_SET) {
		CheckForBall();
		if (ballfound==0)
			HeadScanStep();
	}

	return 0;
}


void Behavior::CheckForBall() {

	if (bmsg != 0) {
		Logger::Instance().WriteMsg("Behavior", "BallTrackMessage", Logger::ExtraExtraInfo);
		if (bmsg->radius() > 0) { 
			scanforball = false; 
			MakeTrackBallAction();
			lastball=microsec_clock::universal_time();
		}
	}
	//Logger::Instance().WriteMsg("Behavior", "ballfound Value: " + _toString(ballfound), Logger::ExtraInfo);
	
	if(wim != 0){
		if (wim->balls_size() > 0) {
			scanforball = false; //be sure to stop scanning
			ballfound = 1;
			int side=1;
			bx = wim->balls(0).relativex();
			by = wim->balls(0).relativey();
			bd = sqrt(pow(bx,2)+pow(by,2));
			bb = atan2(by,bx);
			side = (bb > 0) ? 1 : -1;
		}else{
			scanforball = true;
			ballfound = 0;
		}
	}
}


int Behavior::MakeTrackBallAction() {

	hmot->set_command("setHead");
	hmot->set_parameter(0, bmsg->referenceyaw());
	hmot->set_parameter(1,  bmsg->referencepitch());
	_blk->publishSignal(*hmot, "motion");
		
	return 1;
}

void Behavior::HeadScanStep() {

	static float s=(YAWMIN-YAWMAX)/(PITCHMIN-PITCHMAX);
	HeadYaw= allsm->jointdata(KDeviceLists::HEAD+KDeviceLists::YAW);
	HeadPitch= allsm->jointdata(KDeviceLists::HEAD+KDeviceLists::PITCH);

	if (startscan) {
		//BE CAREFULL the max sign is according to sensors values (max maybe negative! :p)
		ysign=HeadYaw.sensorvalue()>0?1:-1; //Side
		//Crop to limits
		targetPitch=HeadPitch.sensorvalue();
		targetYaw=HeadYaw.sensorvalue();
		targetPitch=(targetPitch>=PITCHMAX)?PITCHMAX:targetPitch;
		targetPitch=(targetPitch<=PITCHMIN)?PITCHMIN:targetPitch;
		float yawlim=s*(targetPitch-PITCHMAX)+YAWMAX;
		targetYaw+=ysign*YAWSTEP;
		targetYaw=fabs(targetYaw)>=yawlim?ysign*yawlim:targetYaw;
		if(fabs(targetYaw)>=yawlim)
			ysign=-ysign;
		psign=1;//Down
		hmot->set_command("setHead");
		hmot->set_parameter(0, targetYaw);
		hmot->set_parameter(1, targetPitch);
		_blk->publishSignal(*hmot, "motion");
		waiting=0;
		startscan=false;
	}
	waiting++;
	if( (fabs(targetPitch-HeadPitch.sensorvalue())<=OVERSH && fabs(targetYaw -HeadYaw.sensorvalue())<=OVERSH )
		|| waiting>=WAITFOR)
	{
		waiting=0;
		float yawlim=s*(targetPitch-PITCHMAX)+YAWMAX;
		if(fabs(fabs(targetYaw)-yawlim)<=OVERSH)
		{
			targetPitch+=psign*PITCHSTEP;
			targetPitch=(targetPitch>=PITCHMAX)?PITCHMAX:targetPitch;
			targetPitch=(targetPitch<=PITCHMIN)?PITCHMIN:targetPitch;
			if(targetPitch>=PITCHMAX)
				psign=-1;
			else if(targetPitch<=PITCHMIN)
				psign=1;
		}
		else
		{
			targetYaw+=ysign*YAWSTEP;
			targetYaw=fabs(targetYaw)>=yawlim?ysign*yawlim:targetYaw;
			if(fabs(targetYaw)>=yawlim)
				ysign=-ysign;
		}

		hmot->set_command("setHead");
		hmot->set_parameter(0, targetYaw);
		hmot->set_parameter(1, targetPitch);
		_blk->publishSignal(*hmot, "motion");
	}
	return ;
}

void Behavior::Kick(int side) {

	if ( kickoff ) {
		if ( (mglRand()<0.5) && !obstacleFront ) {
			littleWalk(0.2, 0.0, 0.0);
		} else {
			if (side == 1) 
				amot->set_command("KickSideLeftSoft.xar");
			else 
				amot->set_command("KickSideRightSoft.xar");
			_blk->publishSignal(*amot, "motion");
		}
		kickoff = false;
	}
	else {
		if (orientation == 0) {
			if (by > 0.0)
				amot->set_command("KickForwardLeft.xar"); //LeftKick
			else
				amot->set_command("KickForwardRight.xar"); //RightKick
		} else if (orientation == 3) {
			amot->set_command("KickSideLeftFast.xar"); //"HardLeftSideKick"
			direction = -1;
		} else if (orientation == 1) {
			amot->set_command("KickSideRightFast.xar"); //"HardRightSideKick"
			direction = +1;
		} else if (orientation == 2) {
			if (by > 0.0)
				amot->set_command("KickBackLeft.xar"); //LeftBackHigh_carpet
			else
				amot->set_command("KickBackRight.xar"); //RightBackHigh_carpet
		} else {
			if (by > 0.0)
				amot->set_command("KickSideLeftFast.xar");
			else
				amot->set_command("KickSideRightFast.xar");
		}
		_blk->publishSignal(*amot, "motion");
	}
}


void Behavior::GetGameState()
{
	if (gsm != 0) {
		Logger::Instance().WriteMsg("Behavior", " Player_state " + _toString(gsm->player_state()), Logger::ExtraExtraInfo);
		int prevGameState = gameState;
		gameState = gsm->player_state();
		teamColor = gsm->team_color();

		if (gameState == PLAYER_PLAYING) {
			if (prevGameState == PLAYER_PENALISED)
				calibrated = 0;
		}
		else if (gameState == PLAYER_INITIAL) {
			if (gameState != prevGameState)
				calibrated = 0;
		}
		else if (gameState == PLAYER_READY) {
			if (gameState != prevGameState) {
				velocityWalk(0.0, 0.0, 0.0, 1.0);
				calibrated = 0;
			}
			if (calibrated == 0)
			{
				calibrate();
			}
			int p = ((kickoff = gsm->kickoff()))?0:1;
			gotoPosition(initX[p][teamColor],initY[p][teamColor],initPhi[p][teamColor]);
			play = false;
			//return 0;
		}
		else if (gameState == PLAYER_SET) {
			kickoff = gsm->kickoff();
			if (gameState != prevGameState) 
				velocityWalk(0.0, 0.0, 0.0, 1.0);
		}
		else if (gameState == PLAYER_FINISHED) {
			;
		}
		else if (gameState == PLAYER_PENALISED) {
			kickoff = gsm->kickoff();
		}
	}
}


void Behavior::read_messages() 
{
	gsm  = _blk->readState<GameStateMessage> ("behavior");
	bmsg = _blk->readSignal<BallTrackMessage> ("vision");
	allsm = _blk->readData<AllSensorValuesMessage> ("sensors");
	//obsm = _blk->readSignal<ObservationMessage> ("vision");
	om   = _blk->readSignal<ObstacleMessage> ("obstacle");
	wim  = _blk->readData<WorldInfo> ("behavior");

	Logger::Instance().WriteMsg("Behavior", "read_messages ", Logger::ExtraExtraInfo);
	boost::shared_ptr<const CalibrateCam> c= _blk->readState<CalibrateCam> ("vision");
	if (c != NULL) {
		if (c->status() == 1)
			calibrated = 2;
	}
}


double Behavior::mglRand()
{
    return rand() / double(RAND_MAX);
}


void Behavior::velocityWalk(double ix, double iy, double it, double f)
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
		cX = 0.25*cX+0.75*x;
		cY = 0.25*cY+0.75*y;
		ct = 0.25*ct+0.75*t;
	}

	wmot->set_parameter(0, cX);
	wmot->set_parameter(1, cY);
	wmot->set_parameter(2, ct);
	wmot->set_parameter(3, f);
	_blk->publishSignal(*wmot, "motion");
	lastwalk = microsec_clock::universal_time();
}


void Behavior::littleWalk(double x, double y, double th)
{
	wmot->set_command("walkTo");
	wmot->set_parameter(0, x);
	wmot->set_parameter(1, y);
	wmot->set_parameter(2, th);
	_blk->publishSignal(*wmot, "motion");
}


void Behavior::calibrate()
{
	CalibrateCam v;
	v.set_status(0);
	_blk->publishState(v, "vision");
	calibrated = 1;
}

void Behavior::CheckForGoals() 
{
//	if (obsm != 0) {
//
//		if (obsm->regular_objects_size() > 0) {
//			if (((obsm->regular_objects(0).object_name().find("Skyblue",0) != std::string::npos) && (teamColor == TEAM_RED)) ||
//			    ((obsm->regular_objects(0).object_name().find("Yellow",0) != std::string::npos) && (teamColor == TEAM_BLUE))) {
//				double ogb = obsm->regular_objects(0).bearing();
//				if (fabs(ogb) < +45 * TO_RAD) {
//					orientation = 0;
//				} else if (ogb > +45 * TO_RAD) {
//					orientation = 1;
//				} else if (ogb < -45 * TO_RAD) {
//					orientation = 3;
//				}
//			}
//
//			if (((obsm->regular_objects(0).object_name().find("Yellow",0) != std::string::npos) && (teamColor == TEAM_RED)) ||
//			    ((obsm->regular_objects(0).object_name().find("Skyblue",0) != std::string::npos) && (teamColor == TEAM_BLUE))) {
//				double mgb = obsm->regular_objects(0).bearing();
//				if (fabs(mgb) < +45 * TO_RAD) {
//					orientation = 2;
//				} else if (mgb > +45 * TO_RAD) {
//					orientation = 3;
//				} else if (mgb < -45 * TO_RAD) {
//					orientation = 1;
//				}
//			}
//			Logger::Instance().WriteMsg("Behavior", "Observation: " + obsm->regular_objects(0).object_name() + " Team Color " + _toString(teamColor) + " Orientation: " + _toString(orientation), Logger::Info);
//		}
//	}
}


bool Behavior::readConfiguration(const std::string& file_name) {
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
bool Behavior::readRobotConfiguration(const std::string& file_name) {
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
void Behavior::gotoPosition(float target_x,float target_y, float target_phi)
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
	float Robot2Target_distance = DISTANCE_2(target_x-robot_x,target_y-robot_y);

	//cout << "Robot2Target_bearing*TO_DEG  " << Robot2Target_bearing * TO_DEG << endl;
	//cout << atan2(target.y - AgentPosition.y, target.x - AgentPosition.x) << endl;
	//cout << AgentPosition.theta << endl << endl;

	float speed;

	if (robot_confidence > 50)
		speed = 1;
	else
		speed = 0.8;

	if (Robot2Target_distance < 0.2)
		speed *= 0.2;

	float VelX, VelY, Rot, freq;

	//TRy to get the robot to the desired position ...

	VelX = speed * cos(Robot2Target_bearing);
	VelY = speed * sin(Robot2Target_bearing);
	Rot = anglediff2(target_phi, robot_phi) * 0.3; // Robot2Target_bearing * 0.3;
	freq = 1;

	if (Robot2Target_distance < 0.3)
		freq *= Robot2Target_distance / 0.3;

	velocityWalk(VelX, VelY, Rot, freq);
}


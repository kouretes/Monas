#include "Behavior.h"

#include <math.h>
#include "tools/logger.h"
#include "tools/toString.h"
#include "messages/RoboCupGameControlData.h"
#include "hal/robot/generic_nao/robot_consts.h"

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
	blueGoalX = 0.0, blueGoalY = 0.0, yellowGoalX = 0.0, yellowGoalY = 0.0;

	locReset = new LocalizationResetMessage();


	ballfound = 0;
	readRobotConf = false;

	scanforball = true;
	startscan = true;
	calibrated = 0;

	forball = 0;
	forpost = 0;

	kickoff = false;
	cX=0.0;cY=0.0;ct=0.0;
	bd=0.0; bb=0.0; bx=0.0; by=0.0; posx=0.0; posy=0.0;
	side = +1;
	robot_x = 0.0, robot_y = 0.0, robot_phi = 0.0, robot_confidence = 1.0;

	readytokick = false;
	back = 0;
	direction = 1;
	count = 0;
	obstacleFront = false;
	gameState = PLAYER_INITIAL;
	teamColor = TEAM_BLUE;
	if (teamColor == TEAM_BLUE){
		oppGoalX = yellowGoalX;
		oppGoalY = yellowGoalY;
		ownGoalX = blueGoalX;
		ownGoalY = blueGoalY;
	}
	else {
		oppGoalX = blueGoalX;
		oppGoalY = blueGoalY;
		ownGoalX = yellowGoalX;
		ownGoalY = yellowGoalY;
	}
	orientation = 0;

	srand(time(0));

	lastmove = microsec_clock::universal_time();
	lastball = microsec_clock::universal_time();
	lastwalk = microsec_clock::universal_time();
	lastplay = microsec_clock::universal_time();

	readConfiguration(ArchConfig::Instance().GetConfigPrefix() + "/team_config.xml");
	readRobotConfiguration(ArchConfig::Instance().GetConfigPrefix() + "/robotConfig.xml");
	LoadFeaturesXML(ArchConfig::Instance().GetConfigPrefix() + "/Features.xml");

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
	GetPosition();

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
		UpdateOrientation();
		
		readytokick = false;
		
		if (ballfound==1) {

			side = (bb > 0) ? 1 : -1;
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
		}

		if (ballfound == 0) {
			if (!scanforball) {
				startscan = true;
				velocityWalk(0.0,0.0,0.0,1.0);
				lastmove = microsec_clock::universal_time();
			}
			scanforball = true;
			if (lastmove + seconds(5) < microsec_clock::universal_time()) {
				littleWalk(direction * 0.20, 0.0, direction * 30 * TO_RAD);
				lastmove = microsec_clock::universal_time();
			}
			if (scanforball){
				if(forball%111 != 0){
					HeadScanStep();
					forball++;
				} else {
					HighHeadScanStep(1.8);
					forpost++;
					if (forpost%9 == 0){
						forball = 1;
						forpost = 0;
						startscan = true;
					}
				}
			}

		}
	} 
	else if (gameState == PLAYER_READY) {
		if (calibrated == 0)
		{
			calibrate();
		}
		int p = (kickoff)?0:1;
		gotoPosition(initX[p][teamColor],initY[p][teamColor],initPhi[p][teamColor]);
		return 0;
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
			ballfound = 1;
		} else {
			if (lastball+seconds(4)<microsec_clock::universal_time())
				ballfound = 0;
		}
	}
	//Logger::Instance().WriteMsg("Behavior", "ballfound Value: " + _toString(ballfound), Logger::ExtraInfo);
	
	if(wim != 0){
		if (wim->balls_size() > 0) {
			bx = wim->balls(0).relativex();
			by = wim->balls(0).relativey();
			bd = sqrt(pow(bx,2)+pow(by,2));
			bb = atan2(by,bx);
		} else {
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

void Behavior::HighHeadScanStep(float yaw_limit) {

	hmot->set_command("setHead");

	if (fabs(headpos) > yaw_limit) // 1.3
		leftright *= -1;

	headpos += 0.2 * leftright;

	hmot->set_parameter(0, headpos); //yaw
	float abspos = fabs(headpos);
	if(abspos<1.57)
		hmot->set_parameter(1, (0.145 * fabs(headpos)) - 0.752);
	else
		hmot->set_parameter(1, (-0.0698 * (fabs(headpos)-1.57)) - 0.52);
	_blk->publishSignal(*hmot, "motion");
}

void Behavior::Kick(int side) {

	if ( kickoff && (microsec_clock::universal_time() <= lastplay+seconds(30)) ) {
		if ( (mglRand()<0.5) && !obstacleFront ) {
			littleWalk(0.2, 0.0, 0.0);
		} else {
			if (side == 1) 
				amot->set_command("KickSideLeftSoft.xar");
			else 
				amot->set_command("KickSideRightSoft.xar");
			_blk->publishSignal(*amot, "motion");
		}
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

		if (teamColor == TEAM_BLUE){
			oppGoalX = yellowGoalX;
			oppGoalY = yellowGoalY;
			ownGoalX = blueGoalX;
			ownGoalY = blueGoalY;
		}
		else {
			oppGoalX = blueGoalX;
			oppGoalY = blueGoalY;
			ownGoalX = yellowGoalX;
			ownGoalY = yellowGoalY;
		}


		if (gameState == PLAYER_PLAYING) {
			if (prevGameState == PLAYER_PENALISED){
				calibrated = 0;
				_blk->publishSignal(*locReset, "behavior");
			}
			if (prevGameState == PLAYER_SET)
				lastplay = microsec_clock::universal_time();
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

void Behavior::UpdateOrientation()
{
	float ogb = anglediff2(atan2(oppGoalY - robot_y, oppGoalX - robot_x), robot_phi);

	if ((fabs(ogb) <= +45 * TO_RAD) && (fabs(ogb) > -45 * TO_RAD)) {
		orientation = 0;
	} else if ((fabs(ogb) > +45 * TO_RAD) && (fabs(ogb) <= +135 * TO_RAD)) {
		orientation = 1;
	} else if ((fabs(ogb) > +135 * TO_RAD) || (fabs(ogb) <= -135 * TO_RAD)) {
		orientation = 2;
	} else if ((fabs(ogb) <= -45 * TO_RAD) && (fabs(ogb) > -135 * TO_RAD)) {
		orientation = 3;
	}
	Logger::Instance().WriteMsg("Behavior", "OPPGOALX " + _toString(oppGoalX) + "OPPGOALY " + _toString(oppGoalY) + "OGB " + _toString(ogb)+ "ORIENTATION " + _toString(orientation), Logger::Info);
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


bool Behavior::LoadFeaturesXML(const std::string& file_name) {
	TiXmlDocument doc2(file_name.c_str());

	bool loadOkay = doc2.LoadFile();
	if (loadOkay) {
		printf("\n%s:\n", file_name.c_str());
		//dump_to_stdout(&doc2); // defined later in the tutorial
	} else {
		printf("Failed to load file \"%s\"\n", file_name.c_str());
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

	return true;
}

void Behavior::GetPosition() {
	if(wim.get() != 0){
		robot_x = wim->myposition().x();
		robot_y = wim->myposition().y();
		robot_phi = wim->myposition().phi();
		robot_confidence = wim->myposition().confidence();

		Logger::Instance().WriteMsg("Behavior",  "MY POSITION x:" + _toString(robot_x) + " y:" + _toString(robot_y) + " phi:" + _toString(robot_phi/TO_RAD), Logger::Info);
	}else{
		Logger::Instance().WriteMsg("Behavior",  "No WorldInfo Message" , Logger::Error);
		return;
	}
}
//Goes to position directly
void Behavior::gotoPosition(float target_x,float target_y, float target_phi)
{

	HighHeadScanStep(2.08);

	//Go to target simple behavior
	float Robot2Target_bearing = anglediff2(atan2(target_y - robot_y, target_x - robot_x), robot_phi);
	float Distance2Target = sqrt((target_x-robot_x)*(target_x-robot_x)+(target_y-robot_y)*(target_y-robot_y));

	cout<<"Distance2Target:"<<Distance2Target<<endl;

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


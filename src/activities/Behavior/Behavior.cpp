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
	hmot->add_parameter(0.0f);
	hmot->add_parameter(0.0f);

	amot = new MotionActionMessage();

	ballfound = 0;
	scanforball = true;
	startscan = true;
	calibrated = 0;

	play = false;
	kickoff = false;
	cX=0.0;cY=0.0;ct=0.0;
	bd=0.0; bb=0.0; bx=0.0; by=0.0; posx=0.0; posy=0.0;
	side = +1;

	readytokick = false;
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
		
		if (ballfound==1) {

			posx=0.12, posy=0.03; // Desired ball position for kick
			double epsx = 0.015, epsy = 0.015; // Desired precision
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
			ballfound = 1;
		}
		else {
			if ( lastball+seconds(2) < microsec_clock::universal_time() )
				ballfound = 0;
		} 
	}
	Logger::Instance().WriteMsg("Behavior", "ballfound Value: " + _toString(ballfound), Logger::ExtraInfo);
	
	if ( (obsm != 0) && obsm->has_ball() ) {
		bd = obsm->ball().dist();
		bb = obsm->ball().bearing();
		bx = obsm->ball().dist() * cos( obsm->ball().bearing() ) ;
		by = obsm->ball().dist() * sin( obsm->ball().bearing() ) ;
		side = (bb > 0) ? 1 : -1;
		Logger::Instance().WriteMsg("Behavior", "Ball - Distance: " + _toString(bd) + "  Bearing: " + _toString(bb) + "  BX: " + _toString(bx) + "  BY: " + _toString(by), Logger::Info);
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
	
	obstacleFront = false;
	if (om!=0)
		if (om->direction() == 0)
			obstacleFront = true;

	if ( kickoff ) {
		if ( (mglRand()<0.5) && !obstacleFront ) {
			littleWalk(0.2, 0.0, 0.0);
		} else {
			if (side == 1) 
				amot->set_command("LeftSideKickFast.xar");
			else 
				amot->set_command("RightSideKickFast.xar");
			_blk->publishSignal(*amot, "motion");
		}
		kickoff = false;
	}
	else {
		if (orientation == 0) {
			if (by > 0.0)
				amot->set_command("LeftKick");
			else
				amot->set_command("RightKick");
		} else if (orientation == 3) {
			amot->set_command("HardLeftSideKick");
			direction = -1;
		} else if (orientation == 1) {
			amot->set_command("HardRightSideKick");
			direction = +1;
		} else if (orientation == 2) {
			if (by > 0.0)
				amot->set_command("LeftBackHigh.xar");
			else
				amot->set_command("RightBackHigh.xar");
		} else {
			if (by > 0.0)
				amot->set_command("KickSideLeft.xar");
			else
				amot->set_command("KickSideRight.xar");
		}
		_blk->publishSignal(*amot, "motion");
		
		//if (side == 1)
			//amot->set_command("LeftKick");
		//else
			//amot->set_command("RightKick");
		//_blk->publishSignal(*amot, "motion");
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
	obsm = _blk->readSignal<ObservationMessage> ("vision");
	om   = _blk->readSignal<ObstacleMessage> ("obstacle");

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
	if (obsm != 0) {

		if (obsm->regular_objects_size() > 0) {
			if (((obsm->regular_objects(0).object_name().find("Skyblue",0) != std::string::npos) && (teamColor == TEAM_RED)) || 
			    ((obsm->regular_objects(0).object_name().find("Yellow",0) != std::string::npos) && (teamColor == TEAM_BLUE))) {
				double ogb = obsm->regular_objects(0).bearing();
				if (fabs(ogb) < +45 * TO_RAD) {
					orientation = 0;
				} else if (ogb > +45 * TO_RAD) {
					orientation = 1;
				} else if (ogb < -45 * TO_RAD) {
					orientation = 3;
				}
			}

			if (((obsm->regular_objects(0).object_name().find("Yellow",0) != std::string::npos) && (teamColor == TEAM_RED)) || 
			    ((obsm->regular_objects(0).object_name().find("Skyblue",0) != std::string::npos) && (teamColor == TEAM_BLUE))) {
				double mgb = obsm->regular_objects(0).bearing();
				if (fabs(mgb) < +45 * TO_RAD) {
					orientation = 2;
				} else if (mgb > +45 * TO_RAD) {
					orientation = 3;
				} else if (mgb < -45 * TO_RAD) {
					orientation = 1;
				}
			}
			Logger::Instance().WriteMsg("Behavior", "Observation: " + obsm->regular_objects(0).object_name() + " Team Color " + _toString(teamColor) + " Orientation: " + _toString(orientation), Logger::Info);
		}
	}
}


bool Behavior::readConfiguration(const std::string& file_name) {
	XMLConfig config(file_name);

	playerNumber = 3;
	if (!config.QueryElement("player", playerNumber))
		Logger::Instance().WriteMsg("Behavior", "Configuration file has no player number, setting to default value: " + _toString(playerNumber), Logger::Error);

	return true;
}

#include "BehaviorX.h"

#include <math.h>
#include "tools/logger.h"
#include "tools/toString.h"
#include "messages/RoboCupGameControlData.h"
#include "hal/robot/generic_nao/robot_consts.h"

using namespace boost::posix_time;
namespace {
	ActivityRegistrar<BehaviorX>::Type temp("BehaviorX");
}
using namespace std;

BehaviorX::BehaviorX() {

}

double mglRand()
{
    //return rand() / double(RAND_MAX);
    return (rand()%100) / 100.0;
}


/* BehaviorX Initialization */

void BehaviorX::UserInit() {
	readConfiguration(ArchConfig::Instance().GetConfigPrefix() + "/team_config.xml");

	_blk->updateSubscription("vision", msgentry::SUBSCRIBE_ON_TOPIC);
	_blk->updateSubscription("sensors", msgentry::SUBSCRIBE_ON_TOPIC);
	_blk->updateSubscription("worldstate", msgentry::SUBSCRIBE_ON_TOPIC);
	_blk->updateSubscription("obstacle", msgentry::SUBSCRIBE_ON_TOPIC);

	wmot = new MotionWalkMessage();
	wmot->add_parameter(0.0f);
	wmot->add_parameter(0.0f);
	wmot->add_parameter(0.0f);
	wmot->add_parameter(0.0f);

	hmot = new MotionHeadMessage();
	hmot->set_command("setHead");
	hmot->add_parameter(0.0f);
	hmot->add_parameter(-0.66322512);

	amot = new MotionActionMessage();

	locReset = new LocalizationResetMessage();

	readRobotConf = false;

	leftright = 1;
	headpos = 0;

	ballfound = 0;
	scanforball = true;
	startscan = true;
	scanOK = true;
	calibrated = 0;

	forball = 0;
	forpost = 0;

	kickoff = false;
	for (int i = 0; i < 2; i++)     	// i: kick-off
		for (int j = 0; j < 2; j++){	// j: team color
			initX[i][j] = 0.0;
			initY[i][j] = 0.0;
			initPhi[i][j] = 0.0;
		}
	for (int j = 0; j < 2; j++){	// j: team color
		ownGoalX[j] = 0.0; ownGoalY[j] = 0.0; oppGoalX[j] = 0.0; oppGoalY[j] = 0.0;
		ownGoalLeftX[j] = 0.0; ownGoalLeftY[j] = 0.0; ownGoalRightX[j] = 0.0; ownGoalRightY[j] = 0.0;
		oppGoalLeftX[j] = 0.0; oppGoalLeftY[j] = 0.0; oppGoalRightX[j] = 0.0; oppGoalRightY[j] = 0.0;
	}
	cX=0.0; cY=0.0; ct=0.0;
	bd=0.0; bb=0.0; bx=0.0; by=0.0; posx=0.0; posy=0.0;
	side = +1;
	robot_x = 0.0; robot_y = 0.0; robot_phi = 0.0; robot_confidence = 1.0;

	readytokick = false;
	back = 0;
	direction = 1;
	orientation = 0;

	gameState = PLAYER_INITIAL;
	teamColor = TEAM_BLUE;
	playerNumber = 1;

	readConfiguration(ArchConfig::Instance().GetConfigPrefix() + "/team_config.xml");		// reads playerNumber, teamColor
	readRobotConfiguration(ArchConfig::Instance().GetConfigPrefix() + "/robotConfig.xml");	// reads initX, initY, initPhi
	readGoalConfiguration(ArchConfig::Instance().GetConfigPrefix() + "/Features.xml");		// reads blueGoal*, yellowGoal*

	srand(time(0));
	lastmove = microsec_clock::universal_time();
	lastball = microsec_clock::universal_time();
	lastwalk = microsec_clock::universal_time();
	lastplay = microsec_clock::universal_time();
	ballseen = microsec_clock::universal_time();

	currentKick = "DoNothing";

	Logger::Instance().WriteMsg("BehaviorX", "Initialized: My number is " + _toString(playerNumber) + " and my color is " + _toString(teamColor), Logger::Info);
}


/* BehaviorX Main Execution Function */

int BehaviorX::Execute() {

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
		UpdateOrientationPlus();

		readytokick = false;

		if (ballfound==1) {
			side = (bb > 0) ? 1 : -1;
			posx=0.12, posy=0.03; // Desired ball position for kick
			double epsx = 0.025, epsy = 0.025; // Desired precision
			KickDecision(side);
			if ( (fabs( bx-posx ) < epsx)  && (fabs( by-(side*posy) ) < epsy) && (bmsg != 0) && (bmsg->radius() > 0) ) {
				readytokick = true;
				direction = (side == +1)?-1:+1;
			}

			if (!readytokick) {
				if (orientation == 1)
					side = -1;
				else if (orientation == 3)
					side = +1;
				approachBallNewWalk(posx, side*posy);
				if (scanOK)
					HeadScanStepSmart();
			}
			else {
				amot->set_command(currentKick);
				_blk->publishSignal(*amot, "motion");
			}
		}

		if (ballfound==0) {

			/* New exploration */
			if (!scanforball) {
				startscan = true;
				scanforball = true;
			}
			velocityWalk(bx*0.1, by*0.1, 0.3*(bb>0?+1:-1), 0.4);
			/* End of New Exploration */

			/* Old exploration */
			//if (!scanforball) {
				//startscan = true;
				//scanforball = true;
				//velocityWalk(0.0,0.0,0.0,1.0);
				//lastmove = microsec_clock::universal_time();
			//}
			//if (lastmove + seconds(5) < microsec_clock::universal_time()) {
				//littleWalk(direction * 0.20, 0.0, direction * M_PI_4);
				//lastmove = microsec_clock::universal_time();
			//}
			/* End of Old Exploration */
			HeadScanStepSmart();
		}
	}
	else if (gameState == PLAYER_READY) {
		if (calibrated == 0)
			calibrate();
		int p = (kickoff) ? 0 : 1;
		//gotoPosition( initX[p][teamColor], initY[p][teamColor], initPhi[p][teamColor] );
		HeadScanStepSmart();
		return 0;
	}
	else if (gameState == PLAYER_SET) {
		CheckForBall();
		if (ballfound == 0)
			HeadScanStepSmart();
	}

	return 0;
}


/* Read Incoming Messages */

void BehaviorX::read_messages()
{
	gsm  = _blk->readState<GameStateMessage> ("worldstate");
	bmsg = _blk->readSignal<BallTrackMessage> ("vision");
	allsm = _blk->readData<AllSensorValuesMessage> ("sensors");
	om   = _blk->readState<ObstacleMessageArray> ("obstacle");
	wim  = _blk->readData<WorldInfo> ("worldstate");

	//Logger::Instance().WriteMsg("BehaviorX", "read_messages ", Logger::ExtraExtraInfo);
	boost::shared_ptr<const CalibrateCam> c= _blk->readState<CalibrateCam> ("vision");
	if (c != NULL) {
		if (c->status() == 1)
			calibrated = 2;
	}
}


/* Information Gathering Functions */

void BehaviorX::GetGameState()
{
	if (gsm != 0) {
		//Logger::Instance().WriteMsg("BehaviorX", " Player_state " + _toString(gsm->player_state()), Logger::ExtraExtraInfo);
		int prevGameState = gameState;
		gameState = gsm->player_state();
		teamColor = gsm->team_color();

		if (gameState == PLAYER_PLAYING) {
			if (prevGameState == PLAYER_PENALISED){
				direction = 1;
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


void BehaviorX::GetPosition() {
	if(wim.get() != 0){
		robot_x = wim->myposition().x();
		robot_y = wim->myposition().y();
		robot_phi = wrapToPi( wim->myposition().phi() );
		robot_confidence = wim->myposition().confidence();
	}
	return;
}


void BehaviorX::UpdateOrientation()
{
	float ogb = anglediff2(atan2(oppGoalY[teamColor] - robot_y, oppGoalX[teamColor] - robot_x), robot_phi);

	if ((fabs(ogb) <= +45 * TO_RAD) && (fabs(ogb) > -45 * TO_RAD)) {
		orientation = 0;
	} else if ((fabs(ogb) > +45 * TO_RAD) && (fabs(ogb) <= +135 * TO_RAD)) {
		orientation = 1;
	} else if ((fabs(ogb) > +135 * TO_RAD) || (fabs(ogb) <= -135 * TO_RAD)) {
		orientation = 2;
	} else if ((fabs(ogb) <= -45 * TO_RAD) && (fabs(ogb) > -135 * TO_RAD)) {
		orientation = 3;
	}
	//Logger::Instance().WriteMsg("BehaviorX", "OPPGOALX " + _toString(oppGoalX[teamColor]) + "OPPGOALY " + _toString(oppGoalY[teamColor]) + "OGB " + _toString(ogb)+ "ORIENTATION " + _toString(orientation), Logger::Info);
}


void BehaviorX::UpdateOrientationPlus()
{
	float loppgb = anglediff2(atan2(oppGoalLeftY[teamColor] - robot_y, oppGoalLeftX[teamColor] - robot_x), robot_phi);
	float roppgb = anglediff2(atan2(oppGoalRightY[teamColor] - robot_y, oppGoalRightX[teamColor] - robot_x), robot_phi);
	float oppgb = wrapToPi( (wrapTo0_2Pi(loppgb) + wrapTo0_2Pi(roppgb)) / 2.0 );

	float lowngb = anglediff2(atan2(ownGoalLeftY[teamColor] - robot_y, ownGoalLeftX[teamColor] - robot_x), robot_phi);
	float rowngb = anglediff2(atan2(ownGoalRightY[teamColor] - robot_y, ownGoalRightX[teamColor] - robot_x), robot_phi);
	float owngb = wrapToPi( (wrapTo0_2Pi(lowngb) + wrapTo0_2Pi(rowngb)) / 2.0 );

	if ( (fabs(oppgb) <= M_PI_4) && (fabs(oppgb) > -M_PI_4) ) {
		orientation = 0;
	} else if ( (fabs(oppgb) > M_PI_4) && (fabs(oppgb) <= (M_PI-M_PI_4) ) ) {
		orientation = 1;
	} else if ( (fabs(oppgb) > (M_PI-M_PI_4) ) || (fabs(oppgb) <= -(M_PI-M_PI_4) ) ) {
		orientation = 2;
	} else if ( (fabs(oppgb) <= -M_PI_4 ) && (fabs(oppgb) > -(M_PI-M_PI_4) ) ) {
		orientation = 3;
	}
}


void BehaviorX::CheckForBall() {

	double closeToBall = 1.0;

	if(wim != 0){
		if (wim->balls_size() > 0) {
			bx = wim->balls(0).relativex()+wim->balls(0).relativexspeed()*0.200;
			by = wim->balls(0).relativey()+wim->balls(0).relativeyspeed()*0.200;
			bd = sqrt(pow(bx,2)+pow(by,2));
			bb = atan2(by,bx);
		} else {
			ballfound = 0;
			bx = 0.0;
			by = 1.1;
			bd = 1.1;
			bb = 0.0; // M_PI
		}
	}

	if (bmsg != 0) {
		if (bmsg->radius() > 0) {
			if ( (bd < closeToBall) || (microsec_clock::universal_time()<ballseen+milliseconds(1500)) || (scanforball) ) {
				MakeTrackBallAction();
				if (scanforball == true) {
					scanforball = false;
					ballseen = microsec_clock::universal_time();
				}
				scanOK = false;
			}
			else {
				scanOK = true;
			}
			lastball = microsec_clock::universal_time();
			ballfound = 1;
		} else {
			if (bd < closeToBall) {
				if (lastball+milliseconds(1000)<microsec_clock::universal_time())
					ballfound = 0;
			} else {
				if (lastball+seconds(3)<microsec_clock::universal_time())
					ballfound = 0;
			}
		}
	}

	return;
}


int BehaviorX::MakeTrackBallAction() {

	hmot->set_command("setHead");
	hmot->set_parameter(0, bmsg->referenceyaw());
	hmot->set_parameter(1,  bmsg->referencepitch());
	_blk->publishSignal(*hmot, "motion");

	return 1;
}


/* Head Scanning Functions */

void BehaviorX::HeadScanStepRaster() {

	static float s = (YAWMIN-YAWMAX) / (PITCHMIN-PITCHMAX);
	HeadYaw = allsm->jointdata(KDeviceLists::HEAD+KDeviceLists::YAW);
	HeadPitch = allsm->jointdata(KDeviceLists::HEAD+KDeviceLists::PITCH);

	if (startscan) {
		ysign = HeadYaw.sensorvalue() > 0 ? +1 : -1; //Side
		targetPitch = HeadPitch.sensorvalue();
		targetYaw = HeadYaw.sensorvalue();
		targetPitch = (targetPitch >= PITCHMAX) ? PITCHMAX : targetPitch;
		targetPitch = (targetPitch <= PITCHMIN) ? PITCHMIN : targetPitch;
		float yawlim = s * (targetPitch - PITCHMAX) + YAWMAX;
		targetYaw += ysign * YAWSTEP;
		targetYaw = fabs(targetYaw) >= yawlim ? ysign * yawlim : targetYaw;
		if ( fabs(targetYaw) >= yawlim )
			ysign = -ysign;
		psign = 1; //Down
		hmot->set_command("setHead");
		hmot->set_parameter(0, targetYaw);
		hmot->set_parameter(1, targetPitch);
		_blk->publishSignal(*hmot, "motion");
		waiting = 0;
		startscan = false;
		return;
	}

	waiting++;

	if ( ( (fabs(targetPitch - HeadPitch.sensorvalue()) <= OVERSH) && (fabs(targetYaw - HeadYaw.sensorvalue()) <= OVERSH) ) || (waiting >= WAITFOR) ) {
		waiting = 0;
		float yawlim = s * (targetPitch - PITCHMAX) + YAWMAX;
		if (fabs(fabs(targetYaw)-yawlim) <= OVERSH) {
			targetPitch += psign * PITCHSTEP;
			targetPitch = (targetPitch >= PITCHMAX) ? PITCHMAX : targetPitch;
			targetPitch = (targetPitch <= PITCHMIN) ? PITCHMIN : targetPitch;
			if (targetPitch >= PITCHMAX)
				psign = -1;
			else if (targetPitch <= PITCHMIN)
				psign = +1;
		}
		else {
			targetYaw += ysign * YAWSTEP;
			targetYaw = fabs(targetYaw) >= yawlim ? ysign * yawlim : targetYaw;
			if (fabs(targetYaw) >= yawlim)
				ysign = -ysign;
		}
		hmot->set_command("setHead");
		hmot->set_parameter(0, targetYaw);
		hmot->set_parameter(1, targetPitch);
		_blk->publishSignal(*hmot, "motion");
	}
	return;
}


void BehaviorX::HeadScanStepHigh(float yaw_limit) {

	hmot->set_command("setHead");

	if (fabs(headpos) > yaw_limit) // 1.3
		leftright *= -1;
	headpos += 0.2 * leftright;
	hmot->set_parameter(0, headpos); //yaw

	float abspos = fabs(headpos);
	if (abspos < 1.57)
		hmot->set_parameter(1, (0.145 * fabs(headpos)) - 0.752);
	else
		hmot->set_parameter(1, (-0.0698 * (fabs(headpos) - 1.57)) - 0.52);

	_blk->publishSignal(*hmot, "motion");
}


void BehaviorX::HeadScanStepSmart() {

	float  blue1y, blue1p, blue2y, blue2p;
	blue1y = +0.75;
	blue1p = +0.38;
	blue2y = +0.00;
	blue2p = -0.55;
	float green1y, green1p, green2y, green2p;
	green1y = +1.45;
	green1p = -0.42;
	green2y = +0.00;
	green2p = +0.35;
	float red1y, red1p, red2y, red2p;
	red1y = +1.80;
	red1p = -0.39;
	red2y = +0.00;
	red2p = -0.60;
	static enum {BLUE, RED, GREEN} state = BLUE;
	static enum {START, MIDDLE, END} phase = START;

	HeadYaw = allsm->jointdata(KDeviceLists::HEAD+KDeviceLists::YAW);
	HeadPitch = allsm->jointdata(KDeviceLists::HEAD+KDeviceLists::PITCH);

	if (startscan) {
		ysign = HeadYaw.sensorvalue() > 0 ? +1 : -1; //Side
		targetYaw = blue1y * ysign;
		targetPitch = blue1p;
		state = BLUE;
		phase = START;
		hmot->set_command("setHead");
		hmot->set_parameter(0, targetYaw);
		hmot->set_parameter(1, targetPitch);
		_blk->publishSignal(*hmot, "motion");
		waiting = 0;
		startscan = false;
		return;
	}

	waiting++;

	if ( ( (fabs(targetPitch - HeadPitch.sensorvalue()) <= OVERSH) && (fabs(targetYaw - HeadYaw.sensorvalue()) <= OVERSH) ) || (waiting >= WAITFOR) ) {
		waiting = 0;
		if (phase == START) {
			phase = MIDDLE;
			switch (state) {
			case BLUE:
				targetYaw = blue2y;
				targetPitch = blue2p;
				break;
			case GREEN:
				targetYaw = green2y;
				targetPitch = green2p;
				break;
			case RED:
				targetYaw = red2y;
				targetPitch = red2p;
				break;
			}
		}
		else if (phase == MIDDLE) {
			ysign = -ysign;
			phase = END;
			switch (state) {
			case BLUE:
				targetYaw = blue1y*ysign;
				targetPitch = blue1p;
				break;
			case GREEN:
				targetYaw = green1y * ysign;
				targetPitch = green1p;
				break;
			case RED:
				targetYaw = red1y*ysign;
				targetPitch = red1p;
				break;
			}
		}
		else {
			phase = START;
			switch (state) {
			case BLUE:
				state = GREEN;
				targetYaw = green1y * ysign;
				targetPitch = green1p;
				break;
			case GREEN:
				state = RED;
				targetYaw = red1y * ysign;
				targetPitch = red1p;
				break;
			case RED:
				state = BLUE;
				targetYaw = blue1y * ysign;
				targetPitch = blue1p;
				break;
			}
		}

		hmot->set_command("setHead");
		hmot->set_parameter(0, targetYaw);
		hmot->set_parameter(1, targetPitch);
		_blk->publishSignal(*hmot, "motion");
	}
	return;
}


void BehaviorX::HeadScanStepIntelligent() {

	float bearing;
	static enum {BALL1, OPPG, BALL2, OWNG} state = BALL1;

	HeadYaw = allsm->jointdata(KDeviceLists::HEAD+KDeviceLists::YAW);
	HeadPitch = allsm->jointdata(KDeviceLists::HEAD+KDeviceLists::PITCH);

	waiting++;

	if ( ( (fabs(targetPitch - HeadPitch.sensorvalue()) <= OVERSH) && (fabs(targetYaw - HeadYaw.sensorvalue()) <= OVERSH) ) || (waiting >= WAITFOR) ) {
		waiting = 0;
		switch (state) {
			case BALL1:
				targetYaw = lookAtPointYaw(robot_x+bx, robot_y+by);
				targetPitch = lookAtPointPitch(robot_x+bx, robot_y+by);
				break;
			case OPPG:
				targetYaw = lookAtPointYaw(oppGoalX[teamColor], oppGoalY[teamColor]);
				targetPitch = lookAtPointPitch(oppGoalX[teamColor], oppGoalY[teamColor]);
				//if (targetYaw < 1.57)
					//targetPitch = (0.145 * fabs(headpos)) - 0.752;
				//else
					//targetPitch = (-0.0698 * (fabs(headpos) - 1.57)) - 0.52;
				break;
			case BALL2:
				targetYaw = lookAtPointYaw(robot_x+bx, robot_y+by);
				targetPitch = lookAtPointPitch(robot_x+bx, robot_y+by);
				break;
			case OWNG:
				targetYaw = lookAtPointYaw(ownGoalX[teamColor], ownGoalY[teamColor]);
				targetPitch = lookAtPointPitch(ownGoalX[teamColor], ownGoalY[teamColor]);
				//if (targetYaw < 1.57)
					//targetPitch = (0.145 * fabs(headpos)) - 0.752;
				//else
					//targetPitch = (-0.0698 * (fabs(headpos) - 1.57)) - 0.52;
				break;
		}
		hmot->set_command("setHead");
		hmot->set_parameter(0, targetYaw);
		hmot->set_parameter(1, targetPitch);
		_blk->publishSignal(*hmot, "motion");

		switch (state) {
			case BALL1:
				bearing = lookAtPointYaw(oppGoalX[teamColor], oppGoalY[teamColor]);
				//if ( (-M_PI_2 < bearing) && (bearing < M_PI_2) )
					//state = OPPG;
				//else
					state = BALL2;
				break;
			case OPPG:
				state = BALL2;
				break;
			case BALL2:
				bearing = lookAtPointYaw(ownGoalX[teamColor], ownGoalY[teamColor]);
				//if ( (-M_PI_2 < bearing) && (bearing < M_PI_2) )
					//state = OWNG;
				//else
					state = BALL1;
				break;
			case OWNG:
				state = BALL1;
				break;
		}

	}
	return;
}


float BehaviorX::lookAtPointYaw(float x, float y)
{
	return anglediff2(atan2(y - robot_y, x - robot_x), robot_phi);
}


float BehaviorX::lookAtPointPitch(float x, float y)
{
	return (50.0 * TO_RAD) - atan2f( sqrt((x-robot_x)*(x-robot_x)+(y-robot_y)*(y-robot_y)), 0.45 );
}


/* Kicking */

void BehaviorX::KickDecision(int side) {

	//if ( kickoff && (microsec_clock::universal_time() <= lastplay+seconds(30)) && (sqrt(robot_x*robot_x + robot_y*robot_y) < 0.5) ) {
	if ( kickoff && (microsec_clock::universal_time() <= lastplay+seconds(25)) ) {
		if (mglRand() < 0.75) {
			littleWalk(0.2, 0.0, 0.0);
		} else {
			if (side == 1)
				currentKick = "KickSideLeftSoft.xar";
			else
				currentKick = "KickSideRightSoft.xar";
		}
	}
	else {
		if (orientation == 0) {
			if (by > 0.0)
				currentKick = "KickForwardLeft.kmex";  //LeftKick
			else
				currentKick = "KickForwardRight.kmex"; //RightKick
		} else if (orientation == 3) {
			currentKick = "KickSideLeftSoft.xar"; //"HardLeftSideKick" KickSideLeftFast
			direction = -1;
		} else if (orientation == 1) {
			currentKick = "KickSideRightSoft.xar"; //"HardRightSideKick" KickSideRightFast
			direction = +1;
		} else if (orientation == 2) {
			if (by > 0.0)
				currentKick = "KickBackLeftED.kme"; //LeftBackHigh_carpet KickBackLeft KickBackLeftPierris
			else
				currentKick = "KickBackRightED.kme"; //RightBackHigh_carpet KickBackRight KickBackRightPierris
		} else {
			if (by > 0.0)
				currentKick = "KickSideLeftSoft.xar";
			else
				currentKick = "KickSideRightSoft.xar";
		}
	}
}


/* Locomotion Functions */

void BehaviorX::velocityWalk(double ix, double iy, double it, double f)
{
	double x, y, t;
	x = ix;
	y = iy;
	t = it;

	/* BEGIN - Basic Obstacle Avoidance Code */
	if ( (om!=0) && (playerNumber == 2) ) {
		if ( (om->distance(2) <= 0.4) && (om->distance(0) <= 0.4) ) {
			if (x > 0.0) {
				x = 0.0;
				t = 0.0;
			}
		}
		else if (om->distance(2) <= 0.4) {
			if (x > 0.0) {
				x = 0.0;
				y = 0.5;
				t = 0.0;
			}
		}
		else if (om->distance(0) <= 0.4) {
			if (x > 0.0) {
				x = 0.0;
				y = -0.5;
				t = 0.0;
			}
		}
	}
	/* END - Basic Obstacle Avoidance Code */

	wmot->set_command("setWalkTargetVelocity");

	if ( (x==0.0) && (y==0.0) && (t==0.0) ) {
		cX = 0.0;
		cY = 0.0;
		ct = 0.0;
	}
	else {
		if( lastwalk + milliseconds(200) > microsec_clock::universal_time() )
			return;
		x = (x>+1.0) ? +1.0 : x;
		x = (x<-1.0) ? -1.0 : x;
		y = (y>+1.0) ? +1.0 : y;
		y = (y<-1.0) ? -1.0 : y;
		t = (t>+1.0) ? +1.0 : t;
		t = (t<-1.0) ? -1.0 : t;
		cX = 0.25 * cX + 0.75 * x;
		cY = 0.25 * cY + 0.75 * y;
		ct = 0.25 * ct + 0.75 * t;
	}

	wmot->set_parameter(0, cX);
	wmot->set_parameter(1, cY);
	wmot->set_parameter(2, ct);
	wmot->set_parameter(3, f);
	_blk->publishSignal(*wmot, "motion");
	lastwalk = microsec_clock::universal_time();
}


void BehaviorX::littleWalk(double x, double y, double th)
{
	wmot->set_command("walkTo");
	wmot->set_parameter(0, x);
	wmot->set_parameter(1, y);
	wmot->set_parameter(2, th);
	_blk->publishSignal(*wmot, "motion");
}


void BehaviorX::approachBall(double ballX, double ballY){

	static double X = 0.0, Y = 0.0, t = 0.0, f = 1.0, gain = 1.0;
	double maxd = fmaxf( fabs(bx-ballX), fabs(by-ballY) );
	f    = fminf(1.0, 0.4+(maxd/0.5));
	gain = fminf(1.0, 0.0+(maxd/0.5));
	X = gain * (bx-ballX)/maxd;
	Y = gain * (by-ballY)/maxd;
	t = gain * (bb/M_PI);
	velocityWalk(X, Y, t, f);
}


void BehaviorX::approachBallNewWalk(double ballX, double ballY){

	static double X = 0.0, Y = 0.0, t = 0.0, f = 1.0, gain = 1.0;
	double maxd = fmaxf( fabs(bx-ballX), fabs(by-ballY) );
	f    = fminf(1.0, 0.4+(maxd/0.5));
	gain = fminf(1.0, 0.0+(maxd/0.5));
	X = gain * (bx-ballX)/maxd;
	Y = gain * (by-ballY)/maxd;
	t = 0.55 * gain * (bb/M_PI);

	//X = 1.0;
	if (currentKick.compare(0,11,"KickForward") == 0){
		X = 1.0;
		velocityWalk(X, Y, t, f);
		if(bd<0.15){
			amot->set_command(currentKick);
			_blk->publishSignal(*amot, "motion");
		}
	}
	else
		velocityWalk(X, Y, t, f);
}


void BehaviorX::gotoPosition(float target_x,float target_y, float target_phi) {

	HeadScanStepHigh(2.08);

	float Robot2Target_bearing = anglediff2(atan2(target_y - robot_y, target_x - robot_x), robot_phi);
	float Distance2Target = sqrt((target_x-robot_x)*(target_x-robot_x)+(target_y-robot_y)*(target_y-robot_y));

	float VelX, VelY, Rot, freq;

	VelX =  cos(Robot2Target_bearing);
	VelY =  sin(Robot2Target_bearing);

	if (Distance2Target < 0.3) {
		VelX /= 2.0;
		VelY /= 2.0;
		Rot = anglediff2(target_phi, robot_phi)*0.5;
		freq = Distance2Target;
	}
	else if (fabs(Robot2Target_bearing) > 1) {
		VelX /= 4.0;
		VelY /= 4.0;
		Rot = Robot2Target_bearing*0.4;
		freq = 0.5;
	}
	else {
		Rot = Robot2Target_bearing*0.1 ;
		freq = 1.0;
	}

	velocityWalk(VelX, VelY, Rot, freq);
}


/* Vision Calibration */

void BehaviorX::calibrate()
{
	CalibrateCam v;
	v.set_status(0);
	_blk->publishState(v, "vision");
	calibrated = 1;
}


/* Read Configuration Functions */

bool BehaviorX::readConfiguration(const std::string& file_name) {

	XMLConfig config(file_name);

	if (!config.QueryElement("player", playerNumber))
		Logger::Instance().WriteMsg("BehaviorX", "Configuration file has no player, setting to default value: " + _toString(playerNumber), Logger::Error);

	std::string color;
	if (teamColor == TEAM_BLUE)
		color = "blue";
	else if (teamColor == TEAM_RED)
		color = "red";
	if (!config.QueryElement("default_team_color", color))
		Logger::Instance().WriteMsg("BehaviorX", "Configuration file has no team_color, setting to default value: " + color, Logger::Error);
	if (color == "blue")
		teamColor = TEAM_BLUE;
	else if (color == "red")
		teamColor = TEAM_RED;
	else
		Logger::Instance().WriteMsg("BehaviorX", "Undefined color in configuration, setting to default value: " + color, Logger::Error);

	return true;
}


bool BehaviorX::readRobotConfiguration(const std::string& file_name) {

	if ( (playerNumber < 1) || (4 < playerNumber) ) {
		Logger::Instance().WriteMsg("BehaviorX",  " readRobotConfiguration: Invalid player number "  , Logger::Error);
		return false;
	}

	readRobotConf = true;
	XML config(file_name);
	typedef std::vector<XMLNode<std::string, float, std::string> > NodeCont;
	NodeCont teamPositions, robotPosition ;
	Logger::Instance().WriteMsg("BehaviorX",  " readRobotConfiguration "  , Logger::Info);

	for (int i = 0; i < 2; i++) {
		string kickoff = (i==0)?"KickOff":"noKickOff";	//KICKOFF==0, NOKICKOFF == 1
		bool found = false;
		teamPositions = config.QueryElement<std::string, float, std::string>(kickoff);

		if (teamPositions.size() != 0)
			robotPosition = config.QueryElement<std::string, float, std::string>("robot", &(teamPositions[0]));

		for (NodeCont::iterator it = robotPosition.begin(); it != robotPosition.end(); it++) {
			if (it->attrb["number"] == playerNumber) {
				initPhi[i][TEAM_BLUE] = 0.0;
				initX[i][TEAM_BLUE] = -1.0*(it->attrb["posx"]);
				initY[i][TEAM_BLUE] = -1.0*(it->attrb["posy"]);

				initPhi[i][TEAM_RED] = M_PI;
				initX[i][TEAM_RED] = (it->attrb["posx"]);
				initY[i][TEAM_RED] = (it->attrb["posy"]);

				Logger::Instance().WriteMsg("BehaviorX", " readConf TEAM_BLUE INIT X "+ kickoff + " "+ _toString(initX[i][TEAM_BLUE]) + " INITY " + _toString(initY[i][TEAM_BLUE]) + " INITPHI " + _toString(initPhi[i][TEAM_BLUE]), Logger::Info);
				Logger::Instance().WriteMsg("BehaviorX", " readConf TEAM_RED INIT X "+ kickoff + " "+ _toString(initX[i][TEAM_RED]) + " INITY " + _toString(initY[i][TEAM_RED]) + " INITPHI " + _toString(initPhi[i][TEAM_RED]), Logger::Info);

				found = true;
			}
		}
		if (!found) {
			Logger::Instance().WriteMsg("BehaviorX",  " readRobotConfiguration: Unable to find initial " + kickoff+ " position for player number " + _toString(playerNumber) , Logger::Error);
			readRobotConf = false;
		}
	}
	return readRobotConf;
}


bool BehaviorX::readGoalConfiguration(const std::string& file_name) {

	TiXmlDocument doc2(file_name.c_str());
	bool loadOkay = doc2.LoadFile();
	if (!loadOkay) {
		Logger::Instance().WriteMsg("BehaviorX",  " readGoalConfiguration: cannot read file " + file_name , Logger::Info);
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
			ownGoalX[TEAM_BLUE] = x/1000.0;
			ownGoalY[TEAM_BLUE] = y/1000.0;
			oppGoalX[TEAM_RED] = x/1000.0;
			oppGoalY[TEAM_RED] = y/1000.0;
		}
		if (ID == "SkyblueLeft"){
			ownGoalLeftX[TEAM_BLUE] = x/1000.0;
			ownGoalLeftY[TEAM_BLUE] = y/1000.0;
			oppGoalLeftX[TEAM_RED] = x/1000.0;
			oppGoalLeftY[TEAM_RED] = y/1000.0;
		}
		if (ID == "SkyblueRight"){
			ownGoalRightX[TEAM_BLUE] = x/1000.0;
			ownGoalRightY[TEAM_BLUE] = y/1000.0;
			oppGoalRightX[TEAM_RED] = x/1000.0;
			oppGoalRightY[TEAM_RED] = y/1000.0;
		}
		if (ID == "YellowGoal"){
			oppGoalX[TEAM_BLUE] = x/1000.0;
			oppGoalY[TEAM_BLUE] = y/1000.0;
			ownGoalX[TEAM_RED] = x/1000.0;
			ownGoalY[TEAM_RED] = y/1000.0;
		}
		if (ID == "YellowLeft"){
			oppGoalLeftX[TEAM_BLUE] = x/1000.0;
			oppGoalLeftY[TEAM_BLUE] = y/1000.0;
			ownGoalLeftX[TEAM_RED] = x/1000.0;
			ownGoalLeftY[TEAM_RED] = y/1000.0;
		}
		if (ID == "YellowRight"){
			oppGoalRightX[TEAM_BLUE] = x/1000.0;
			oppGoalRightY[TEAM_BLUE] = y/1000.0;
			ownGoalRightX[TEAM_RED] = x/1000.0;
			ownGoalRightY[TEAM_RED] = y/1000.0;
		}
	}

	return true;
}



/* Test Function */

void BehaviorX::test() {


	/* OPEN CHALLENGE 2011 - PROJECTION KICK - START */

	if(wim != 0){
		if (wim->balls_size() > 0) {
			bx = wim->balls(0).relativex()+wim->balls(0).relativexspeed()*0.200;
			by = wim->balls(0).relativey()+wim->balls(0).relativeyspeed()*0.200;
			bd = sqrt(pow(bx,2)+pow(by,2));
			bb = atan2(by,bx);
		}
		else {
			ballfound = 0;
		}
	}

	if (bmsg != 0) {
		if (bmsg->radius() > 0) {
			MakeTrackBallAction();
			lastball = microsec_clock::universal_time();
			ballfound = 1;
		} else {
			if (lastball+seconds(3)<microsec_clock::universal_time())
				ballfound = 0;
		}
	}

	if (ballfound==1) {
		Logger::Instance().WriteMsg("BehaviorX",  " timestamp " + to_simple_string(microsec_clock::universal_time()), Logger::Info);
		float kickslope = -4;
		float x1, y1;
		float ubx, uby;
		float x, y, slope, t;

		if(wim != 0){
			if (wim->balls_size() > 0) {
				x1 = wim->balls(0).relativex();	//the last b observation's x position
				y1 = wim->balls(0).relativey();	//the last but one observation's y position
				ubx = wim->balls(0).relativexspeed();
				uby = wim->balls(0).relativeyspeed();
				Logger::Instance().WriteMsg("BehaviorX",  "OP: x1 " + _toString(x1) + " y1 " + _toString(y1), Logger::Info);
				Logger::Instance().WriteMsg("BehaviorX",  "OP: ubx " + _toString(ubx) + " uby " + _toString(uby), Logger::Info);

				if (fabs(uby) < 0.001)
					return;
				slope = ubx / uby;

				if ( fabs(kickslope-slope) < 0.001 )
					return;

				y = (x1-y1*slope) / (kickslope-slope);
				x = kickslope * y;
				t = fabs(x1-x) / fabs(ubx);
				Logger::Instance().WriteMsg("BehaviorX", "OP: x "+_toString(x) + " y " +_toString(y) + "TIME: " + _toString(t), Logger::Info);
				if ( (x >= 0.03) && (x <= 0.17) ) {
					if ( (t > 2.0) && (t < 5.0) ) {
						Logger::Instance().WriteMsg("BehaviorX",  "OP: Openchallenge FTW!!!!!!!!!!!!!! ", Logger::Info);
						amot->set_command("KickForwardRightFast.xar");
						_blk->publishSignal(*amot, "motion");
					}
				}
			}
		}
	}
	else {
		HeadScanStepSmart();
	}

	/* OPEN CHALLENGE 2011 - PROJECTION KICK - END */

	//HeadScanStepIntelligent();

	//if (om!=0) {
		//Logger::Instance().WriteMsg("BehaviorX", "L: " + _toString(om->direction(0)) + " C: " + _toString(om->direction(1)) + " R: " + _toString(om->direction(2)), Logger::Info);
		//if ( (om->direction(2) != 0) && (om->direction(0) == 0) ) {
			//velocityWalk(0.0, 0.0, 1.0, 1.0);
		//}
		//else if ( (om->direction(0) != 0) && (om->direction(2) == 0) ) {
			//velocityWalk(0.0, 0.0, -1.0, 1.0);
		//}
		//else if ( (om->direction(0) != 0) && (om->direction(2) != 0) ) {
			//velocityWalk(0.0, 0.0, 0.0, 1.0);
		//}
		//else {
			//velocityWalk(1.0, 0.0, 0.0, 1.0);
		//}
	//}
	//else
		//velocityWalk(1.0, 0.0, 0.0, 1.0);
}

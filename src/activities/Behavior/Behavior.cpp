#include "Behavior.h"

#include <math.h>
#include "tools/logger.h"
#include "tools/toString.h"
#include "messages/RoboCupGameControlData.h"
#include "hal/robot/generic_nao/robot_consts.h"
#include "tools/MathFunctions.h"

using namespace boost::posix_time;

ACTIVITY_REGISTER(Behavior);
using namespace std;


double mglRand()
{
    //return rand() / double(RAND_MAX);
    return (rand()%100) / 100.0;

}

/* Behavior Initialization */

void Behavior::UserInit() {
	readConfiguration(ArchConfig::Instance().GetConfigPrefix() + "/team_config.xml");

	_blk.updateSubscription("vision", msgentry::SUBSCRIBE_ON_TOPIC);
	_blk.updateSubscription("sensors", msgentry::SUBSCRIBE_ON_TOPIC);
	_blk.updateSubscription("worldstate", msgentry::SUBSCRIBE_ON_TOPIC);
	_blk.updateSubscription("obstacle", msgentry::SUBSCRIBE_ON_TOPIC);

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

	pprm = new PathPlanningRequestMessage();

    fom = new ObstacleMessage();

	readRobotConf = false;

	leftright = 1;
	headpos = 0;

	ballfound = 0;
	scanforball = true;
	startscan = true;
	scanOK = true;
	pathOK = true;
	calibrated = 0;

	forball = 0;
	forpost = 0;

	kickoff = false;
	toReadyFromGoal = false;
	for (int i = 0; i < 2; i++) {		// i: kick-off
		initX[i] = 0.0;
		initY[i] = 0.0;
		initPhi[i] = 0.0;
	}

	ownGoalX = 0.0; ownGoalY = 0.0; oppGoalX = 0.0; oppGoalY = 0.0;
	ownGoalLeftX = 0.0; ownGoalLeftY = 0.0; ownGoalRightX = 0.0; ownGoalRightY = 0.0;
	oppGoalLeftX = 0.0; oppGoalLeftY = 0.0; oppGoalRightX = 0.0; oppGoalRightY = 0.0;

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
	role = ATTACKER;


	readConfiguration(ArchConfig::Instance().GetConfigPrefix() + "/team_config.xml");		// reads playerNumber, teamColor
	readRobotConfiguration(ArchConfig::Instance().GetConfigPrefix() + "/robotConfig.xml");	// reads initX, initY, initPhi
	readGoalConfiguration(ArchConfig::Instance().GetConfigPrefix() + "/Features.xml");		// reads blueGoal*, yellowGoal*

	srand(time(0));
	lastmove = microsec_clock::universal_time();
	lastball = microsec_clock::universal_time();
	lastwalk = microsec_clock::universal_time();
	lastplay = microsec_clock::universal_time();
	lastpenalized = microsec_clock::universal_time();
	ballseen = microsec_clock::universal_time();

	Logger::Instance().WriteMsg("Behavior", "Initialized: My number is " + _toString(playerNumber) + " and my color is " + _toString(teamColor), Logger::Info);
}


/* Behavior Main Execution Function */

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

		if (lastpenalized+seconds(12)>microsec_clock::universal_time()) {
			HeadScanStepHigh(2.08);
			return 0;
		}

		CheckForBall();
		UpdateOrientationPlus();

		readytokick = false;

		if (ballfound==1) {
			side = (bb > 0) ? 1 : -1;
			posx=0.12, posy=0.03; // Desired ball position for kick
			double epsx = 0.025, epsy = 0.025; // Desired precision
			if ( (fabs( bx-posx ) < epsx)  && (fabs( by-(side*posy) ) < epsy) && (bmsg != 0) && (bmsg->radius() > 0) ) {
				readytokick = true;
				Kick(side);
				direction = (side == +1)?-1:+1;
			}

			if (!readytokick) {
			    //Define roles
                if(microsec_clock::universal_time() > lastrolechange + milliseconds(500)){
                    if(ClosestRobot()){
                        role = ATTACKER;
                    }
                    else{
                        role = CENTER_FOR;
                    }
                    lastrolechange = microsec_clock::universal_time();
                }
	//		    Logger::Instance().WriteMsg("BehaviorTest", "Role: " + _toString(role), Logger::Info);
                approachBallRoleDependent(bx, by);

				if (scanOK)
                   HeadScanStepIntelligent();
//					HeadScanStepSmart();
			}
		}

		if (ballfound==0) {

			/* New exploration */
			if (!scanforball) {
				startscan = true;
				scanforball = true;
			}
			if ( (fabs(robot_x) < 2.0) && (fabs(robot_y) < 2.0) )
				pathPlanningRequestAbsolute(0.45, 0.45*direction, M_PI_4*direction);
			else
				pathPlanningRequestAbsolute(0.1, 0.1*direction, M_PI_4*direction);
			/* End of New Exploration */

			HeadScanStepSmart();
		}
	}
	else if (gameState == PLAYER_READY) {
		//HeadScanStepSmart();
		HeadScanStepHigh(2.08);
		int p = (kickoff) ? 0 : 1;
		gotoPosition( initX[p], initY[p], initPhi[p] );
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

void Behavior::read_messages()
{
	gsm  = _blk.readState<GameStateMessage> ("worldstate");
	bmsg = _blk.readSignal<BallTrackMessage> ("vision");
	allsm = _blk.readData<AllSensorValuesMessage> ("sensors");
	om   = _blk.readState<ObstacleMessageArray> ("obstacle");
	wim  = _blk.readData<WorldInfo> ("worldstate");
    swim = _blk.readData<SharedWorldInfo> ("worldstate");

	//Logger::Instance().WriteMsg("Behavior", "read_messages ", Logger::ExtraExtraInfo);
	boost::shared_ptr<const KCalibrateCam> c= _blk.readState<KCalibrateCam> ("vision");
	if (c != NULL) {
		if (c->status() == 1)
			calibrated = 2;
	}
}


/* Information Gathering Functions */

void Behavior::GetGameState()
{
	if (gsm != 0) {
		//Logger::Instance().WriteMsg("Behavior", " Player_state " + _toString(gsm->player_state()), Logger::ExtraExtraInfo);
		int prevGameState = gameState;
		gameState = gsm->player_state();
		teamColor = gsm->team_color();
		playerNumber = gsm->player_number();

		if (gameState == PLAYER_PLAYING) {
			if (prevGameState == PLAYER_PENALISED){
				direction = 1;
				calibrated = 0;
				lastpenalized = microsec_clock::universal_time();
				locReset->set_type(-1);
				locReset->set_kickoff(kickoff);
				_blk.publishSignal(*locReset, "worldstate");
			}
			if (prevGameState == PLAYER_SET) {
				lastplay = microsec_clock::universal_time();
			}
		}
		else if (gameState == PLAYER_INITIAL) {
			if (gameState != prevGameState)
				calibrated = 0;
		}
		else if (gameState == PLAYER_READY) {
			if (gameState != prevGameState) {
				stopRobot();
				calibrated = 0;
			}
			if (prevGameState == PLAYER_INITIAL) {
				locReset->set_type(playerNumber);
				locReset->set_kickoff(kickoff);
				_blk.publishSignal(*locReset, "worldstate");
			}
			if (prevGameState == PLAYER_PLAYING)
				toReadyFromGoal = true;
		}
		else if (gameState == PLAYER_SET) {
			kickoff = gsm->kickoff();
			if (gameState != prevGameState)
				stopRobot();
			if ( toReadyFromGoal ) {
				locReset->set_type(playerNumber);
				locReset->set_kickoff(kickoff);
				_blk.publishSignal(*locReset, "worldstate");
				toReadyFromGoal = false;
			}
		}
		else if (gameState == PLAYER_FINISHED) {
			;
		}
		else if (gameState == PLAYER_PENALISED) {
			kickoff = gsm->kickoff();
		}
	}
}

bool Behavior::ClosestRobot() {
    double epsx = 0.005, epsy = 0.005; // Desired precision
 	if(swim != 0)
        if(swim.get() != 0){
//            Logger::Instance().WriteMsg("SharedWorldModel", "Closest robot x: " + _toString(swim->playerclosesttoball().x()) +
//                                " y: " + _toString(swim->playerclosesttoball().y()), Logger::Info);

            double closest_robot_x = swim->playerclosesttoball().x();
            double closest_robot_y = swim->playerclosesttoball().y();
            if(((fabs(robot_x - closest_robot_x)) < epsx) && (fabsf(robot_y - closest_robot_y) < epsy))
                return true;
            else
                return false;

        }
    else
        return true;
}

void Behavior::GetPosition() {
	if(wim != 0)
        if(wim.get() != 0){
            robot_x = wim->myposition().x();
            robot_y = wim->myposition().y();
            robot_phi = wrapToPi( wim->myposition().phi() );
            robot_confidence = wim->myposition().confidence();
        }
	return;
}


void Behavior::UpdateOrientationPlus()
{
	double loppgb = anglediff2(atan2(oppGoalLeftY - robot_y, oppGoalLeftX - robot_x), robot_phi);
	double roppgb = anglediff2(atan2(oppGoalRightY - robot_y, oppGoalRightX - robot_x), robot_phi);
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
	Logger::Instance().WriteMsg("Behavior", "AGOC-OPPGOAL: " + _toString(oppGoalLeftX) + " " + _toString(oppGoalLeftY) + " " + _toString(oppGoalRightX) + " " + _toString(oppGoalRightY), Logger::Info);
	Logger::Instance().WriteMsg("Behavior", "AGOC-ROBOT: " + _toString(robot_x) + " " + _toString(robot_y) + " " + _toString(robot_phi) + " DEG: " + _toString(int(robot_phi*180.0/M_PI)), Logger::Info);
	 Logger::Instance().WriteMsg("Behavior", "AGOC-OGB: " + _toString(oppgb) + " DEG: " + _toString(int(oppgb*180.0/M_PI)) + " ORIENTATION: " + _toString(orientation), Logger::Info);

	//double lowngb = anglediff2(atan2(ownGoalLeftY - robot_y, ownGoalLeftX - robot_x), robot_phi);
	//double rowngb = anglediff2(atan2(ownGoalRightY - robot_y, ownGoalRightX - robot_x), robot_phi);
	//double owngb = wrapToPi( (wrapTo0_2Pi(lowngb) + wrapTo0_2Pi(rowngb)) / 2.0 );


}


void Behavior::CheckForBall() {

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
		    if(!scanOK && ballfound)
                if(wim != 0)
                    if (wim->balls_size() > 0){
                        trackYaw = lookAtPointRelativeYaw(bx, by);
                        trackPitch = lookAtPointRelativePitch(bx, by);
                        MakeTrackBallActionNoBmsg();
                    }
			if (bd < closeToBall) {
				if (lastball+milliseconds(1000)<microsec_clock::universal_time()){
					ballfound = 0;
				}
			} else {
				if (lastball+seconds(3)<microsec_clock::universal_time()){
					ballfound = 0;
				}
			}
		}
	}
	return;
}


int Behavior::MakeTrackBallAction() {

	hmot->set_command("setHead");
	hmot->set_parameter(0, bmsg->referenceyaw());
	hmot->set_parameter(1,  bmsg->referencepitch());
	_blk.publishSignal(*hmot, "motion");

	return 1;
}

int Behavior::MakeTrackBallActionNoBmsg() {

	hmot->set_command("setHead");
	hmot->set_parameter(0, trackYaw);
	hmot->set_parameter(1, trackPitch);
	_blk.publishSignal(*hmot, "motion");

	return 1;
}


/* Head Scanning Functions */

void Behavior::HeadScanStepRaster() {

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
		_blk.publishSignal(*hmot, "motion");
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
		_blk.publishSignal(*hmot, "motion");
	}
	return;
}


void Behavior::HeadScanStepHigh(float yaw_limit) {

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

	_blk.publishSignal(*hmot, "motion");
}


void Behavior::HeadScanStepSmart() {

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
		_blk.publishSignal(*hmot, "motion");
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
		_blk.publishSignal(*hmot, "motion");
	}
	return;
}


void Behavior::HeadScanStepIntelligent() {

    float bearing;
	static enum {BALL1, OPPG, BALL2, OWNG} state = BALL1;

	HeadYaw = allsm->jointdata(KDeviceLists::HEAD+KDeviceLists::YAW);
	HeadPitch = allsm->jointdata(KDeviceLists::HEAD+KDeviceLists::PITCH);

	waiting++;

	if ( ( (fabs(targetPitch - HeadPitch.sensorvalue()) <= OVERSH) && (fabs(targetYaw - HeadYaw.sensorvalue()) <= OVERSH) ) || (waiting >= WAITFOR) ) {
		waiting = 0;
		switch (state) {
			case BALL1:
				targetYaw = lookAtPointRelativeYaw(bx, by);
				targetPitch = lookAtPointRelativePitch(bx, by);
				break;
			case OPPG:
				targetYaw = robot_phi - lookAtPointRelativeYaw(oppGoalX-robot_x, oppGoalY-robot_y);
				targetYaw = (targetYaw<0) ? targetYaw - 0.2 : targetYaw + 0.2;
				targetPitch = lookAtPointRelativePitch(oppGoalX-robot_x, oppGoalY-robot_y);
				//if (targetYaw < 1.57)
					//targetPitch = (0.145 * fabs(headpos)) - 0.752;
				//else
					//targetPitch = (-0.0698 * (fabs(headpos) - 1.57)) - 0.52;
				break;
			case BALL2:
				targetYaw = lookAtPointRelativeYaw(bx, by);
				targetYaw = (targetYaw<0) ? targetYaw - 0.2 : targetYaw + 0.2;
				targetPitch = lookAtPointRelativePitch(bx, by);
				break;
			case OWNG:
                targetYaw = robot_phi - lookAtPointRelativeYaw(ownGoalX-robot_x, ownGoalY-robot_y);
				targetPitch = lookAtPointRelativePitch(ownGoalX-robot_x, ownGoalY-robot_y);
				//if (targetYaw < 1.57)
					//targetPitch = (0.145 * fabs(headpos)) - 0.752;
				//else
					//targetPitch = (-0.0698 * (fabs(headpos) - 1.57)) - 0.52;
				break;
		}
		cout << " OwnX: " << ownGoalX-robot_x << " OwnY: " << ownGoalY-robot_y << " OppX: " << oppGoalX-robot_x << " OppY: " << oppGoalY-robot_y << endl;

		cout << state << " Yaw: " << targetYaw << " Pitch: " << targetPitch << endl;

		hmot->set_command("setHead");
		hmot->set_parameter(0, targetYaw);
		hmot->set_parameter(1, targetPitch);
		_blk.publishSignal(*hmot, "motion");

		switch (state) {
			case BALL1:
				bearing = lookAtPointYaw(oppGoalX, oppGoalY);
				//if ( (-M_PI_2 < bearing) && (bearing < M_PI_2) )
					//state = OPPG;
				//else
					state = OPPG;
				break;
			case OPPG:
				state = BALL2;
				break;
			case BALL2:
				bearing = lookAtPointYaw(ownGoalX, ownGoalY);
				//if ( (-M_PI_2 < bearing) && (bearing < M_PI_2) )
					//state = OWNG;
				//else
					state = OWNG;
				break;
			case OWNG:
				state = BALL1;
				break;
		}

	}
	return;
}


float Behavior::lookAtPointYaw(float x, float y)
{
	return anglediff2(atan2(y - robot_y, x - robot_x), robot_phi);
}

float Behavior::lookAtPointPitch(float x, float y)
{
	return (50.0 * TO_RAD) - atan2f( sqrt((x-robot_x)*(x-robot_x)+(y-robot_y)*(y-robot_y)), 0.45 );
}

float Behavior::lookAtPointRelativeYaw(float x, float y)
{
	return atan2(y, x);
}

float Behavior::lookAtPointRelativePitch(float x, float y)
{
	return (50.0 * TO_RAD) - atan2f( sqrt((x)*(x)+(y)*(y)), 0.45 );
}


/* Kicking */

void Behavior::Kick(int side) {

	//if ( kickoff && (microsec_clock::universal_time() <= lastplay+seconds(30)) && (sqrt(robot_x*robot_x + robot_y*robot_y) < 0.5) ) {
	if ( kickoff && (microsec_clock::universal_time() <= lastplay+seconds(25)) ) {
		if (mglRand() < 0.75) {
			littleWalk(0.2, 0.0, 0.0);
		} else {
			if (side == 1)
				amot->set_command("KickSideLeftStable.xar");
			else
				amot->set_command("KickSideRightStable.xar");
			_blk.publishSignal(*amot, "motion");
		}
	}
	else {
		if (orientation == 0) {
			if (by > 0.0)
				amot->set_command("KickForwardLeft.xar"); //LeftKick
			else
				amot->set_command("KickForwardRight.xar"); //RightKick
		} else if (orientation == 3) {
			amot->set_command("KickSideLeftStable.xar"); //"HardLeftSideKick" KickSideLeftFast
			direction = -1;
		} else if (orientation == 1) {
			amot->set_command("KickSideRightStable.xar"); //"HardRightSideKick" KickSideRightFast
			direction = +1;
		} else if (orientation == 2) {
			if (by > 0.0)
				amot->set_command("KickBackLeftED.kme"); //LeftBackHigh_carpet KickBackLeft KickBackLeftPierris
			else
				amot->set_command("KickBackRightED.kme"); //RightBackHigh_carpet KickBackRight KickBackRightPierris
		} else {
			if (by > 0.0)
				amot->set_command("KickSideLeftStable.xar");
			else
				amot->set_command("KickSideRightStable.xar");
		}
		_blk.publishSignal(*amot, "motion");
	}
}


/* Locomotion Functions */

void Behavior::velocityWalk(double ix, double iy, double it, double f)
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
	_blk.publishSignal(*wmot, "motion");
	lastwalk = microsec_clock::universal_time();
}


void Behavior::littleWalk(double x, double y, double th)
{
	wmot->set_command("walkTo");
	wmot->set_parameter(0, x);
	wmot->set_parameter(1, y);
	wmot->set_parameter(2, th);
	_blk.publishSignal(*wmot, "motion");
}

void Behavior::approachBall(double ballX, double ballY){

	static double X = 0.0, Y = 0.0, t = 0.0, f = 1.0, gain = 1.0;
	double maxd = fmaxf( fabs(bx-ballX), fabs(by-ballY) );
	f    = fminf(1.0, 0.4+(maxd/0.5));
	gain = fminf(1.0, 0.0+(maxd/0.5));
	X = gain * (bx-ballX)/maxd;
	Y = gain * (by-ballY)/maxd;
	t = gain * (bb/M_PI);
	velocityWalk(X, Y, t, f);
}


void Behavior::approachBallNewWalk(double ballX, double ballY){

	static double X = 0.0, Y = 0.0, t = 0.0, f = 1.0, gain = 1.0;
	double maxd = fmaxf( fabs(bx-ballX), fabs(by-ballY) );
	f    = fminf(1.0, 0.4+(maxd/0.5));
	gain = fminf(1.0, 0.0+(maxd/0.5));
	X = gain * (bx-ballX)/maxd;
	Y = gain * (by-ballY)/maxd;
	t = 0.55 * gain * (bb/M_PI);
	velocityWalk(X, Y, t, f);
}

void Behavior::approachBallRoleDependent(double ballX, double ballY){
    if (orientation == 1)
        side = -1;
    else if (orientation == 3)
        side = +1;
    if(role == ATTACKER){
        if (pathOK && bd > 0.2){
            pathOK = false;
            int pathSide = (bb > 0) ? 1 : -1;
            pathPlanningRequestRelative(bx, by, pathSide*M_PI_2);
        }
        else
            pathPlanningRequestAbsolute(bx-posx, by-side*posy, bb);
    }
    else if(role == CENTER_FOR){
        if (bd > 0.5){
            int pathSide = (bb > 0) ? 1 : -1;
            pathPlanningRequestAbsolute(bx-posx, by-side*posy, bb);
        }
        else
            stopRobot();
    }
}

void Behavior::stopRobot() {
	//velocityWalk(0.0, 0.0, 0.0, 1.0);
	amot->set_command("InitPose.xar");
	_blk.publishSignal(*amot, "motion");
}

void Behavior::pathPlanningRequestRelative(float target_x, float target_y, float target_phi) {
	pprm->set_gotox(target_x);
	pprm->set_gotoy(target_y);
	pprm->set_gotoangle(target_phi);
	pprm->set_mode("relative");
	_blk.publishSignal(*pprm, "obstacle");
}

void Behavior::pathPlanningRequestAbsolute(float target_x, float target_y, float target_phi) {
	pprm->set_gotox(target_x);
	pprm->set_gotoy(target_y);
	pprm->set_gotoangle(target_phi);
	pprm->set_mode("absolute");
	_blk.publishSignal(*pprm, "obstacle");
}

void Behavior::gotoPosition(float target_x,float target_y, float target_phi) {

	double targetDistance = sqrt((target_x-robot_x)*(target_x-robot_x)+(target_y-robot_y)*(target_y-robot_y));
	double targetAngle = anglediff2(atan2(target_y - robot_y, target_x - robot_x), robot_phi);
	double targetOrientation = anglediff2(target_phi, robot_phi);

	if ( (targetDistance > 0.25) || (fabs(targetOrientation) > M_PI_4) )
		pathPlanningRequestAbsolute(toCartesianX(targetDistance,targetAngle),
									toCartesianY(targetDistance,targetAngle),
									targetOrientation);
	else
		stopRobot();
}


/* Vision Calibration */

void Behavior::calibrate()
{
	KCalibrateCam v;
	v.set_status(0);
	_blk.publishState(v, "vision");
	calibrated = 1;
}


/* Read Configuration Functions */

bool Behavior::readConfiguration(const std::string& file_name) {

	XMLConfig config(file_name);

	if (!config.QueryElement("player", playerNumber))
		Logger::Instance().WriteMsg("Behavior", "Configuration file has no player, setting to default value: " + _toString(playerNumber), Logger::Error);

	std::string color;
	if (teamColor == TEAM_BLUE)
		color = "blue";
	else if (teamColor == TEAM_RED)
		color = "red";
	if (!config.QueryElement("default_team_color", color))
		Logger::Instance().WriteMsg("Behavior", "Configuration file has no team_color, setting to default value: " + color, Logger::Error);
	if (color == "blue")
		teamColor = TEAM_BLUE;
	else if (color == "red")
		teamColor = TEAM_RED;
	else
		Logger::Instance().WriteMsg("Behavior", "Undefined color in configuration, setting to default value: " + color, Logger::Error);

	return true;
}


bool Behavior::readRobotConfiguration(const std::string& file_name) {

	if ( (playerNumber < 1) || (4 < playerNumber) ) {
		Logger::Instance().WriteMsg("Behavior",  " readRobotConfiguration: Invalid player number "  , Logger::Error);
		return false;
	}

	readRobotConf = true;
	XML config(file_name);
	typedef std::vector<XMLNode<std::string, float, std::string> > NodeCont;
	NodeCont teamPositions, robotPosition ;
	Logger::Instance().WriteMsg("Behavior",  " readRobotConfiguration "  , Logger::Info);

	for (int i = 0; i < 2; i++) {
		string kickoff = (i==0)?"KickOff":"noKickOff";	//KICKOFF==0, NOKICKOFF == 1
		bool found = false;
		teamPositions = config.QueryElement<std::string, float, std::string>(kickoff);

		if (teamPositions.size() != 0)
			robotPosition = config.QueryElement<std::string, float, std::string>("robot", &(teamPositions[0]));

		for (NodeCont::iterator it = robotPosition.begin(); it != robotPosition.end(); it++) {
			if (it->attrb["number"] == playerNumber) {
				initPhi[i] = 0.0;
				initX[i] = (it->attrb["posx"]);
				initY[i] = (it->attrb["posy"]);

				Logger::Instance().WriteMsg("Behavior", " readConf INIT X "+ kickoff + " "+ _toString(initX[i]) + " INITY " + _toString(initY[i]) + " INITPHI " + _toString(initPhi[i]), Logger::Info);

				found = true;
			}
		}
		if (!found) {
			Logger::Instance().WriteMsg("Behavior",  " readRobotConfiguration: Unable to find initial " + kickoff+ " position for player number " + _toString(playerNumber) , Logger::Error);
			readRobotConf = false;
		}
	}
	return readRobotConf;
}


bool Behavior::readGoalConfiguration(const std::string& file_name) {

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
		if(Ftr->ToComment() == NULL){
			Attr = Ftr->ToElement();
			Attr->Attribute("x", &x);
			Attr->Attribute("y", &y);
			ID = Attr->Attribute("ID");
			if (ID == "YellowGoal"){
				oppGoalX = x/1000.0;
				oppGoalY = y/1000.0;
				ownGoalX = -oppGoalX;
				ownGoalY = -oppGoalY;
			}
			if (ID == "YellowLeft"){
				oppGoalLeftX = x/1000.0;
				oppGoalLeftY = y/1000.0;
				ownGoalLeftX = -oppGoalLeftX;
				ownGoalLeftY = -oppGoalLeftY;
			}
			if (ID == "YellowRight"){
				oppGoalRightX = x/1000.0;
				oppGoalRightY = y/1000.0;
				ownGoalRightX = -oppGoalRightX;
				ownGoalRightY = -oppGoalRightY;
			}
		}
	}
	return true;
}

void Behavior::generateFakeObstacles(){

    float tmpX=-3.0+ObstacleRadius,tmpY=-1.1+ObstacleRadius;
    for(int j=0;j<numOfFakeObstacles;j++){
        fakeObstacles[j][0]=INIT_VALUE;
        fakeObstacles[j][1]=INIT_VALUE;
    }
    int i=0;
    while(tmpX<-2.4){
        fakeObstacles[i][0] = tmpX;
        fakeObstacles[i][1] = tmpY;

        fakeObstacles[i+1][0] = tmpX;
        fakeObstacles[i+1][1] = -tmpY;
        tmpX+=2*ObstacleRadius;
        i+=2;
    }
    tmpX=-2.4+ObstacleRadius;
    while(tmpY<1.1){
        fakeObstacles[i][0] = tmpX;
        fakeObstacles[i][1] = tmpY;
        tmpY+=2*ObstacleRadius+0.02;
        i++;
    }
}
float Behavior::dist(float x1, float y1, float x2, float y2){
    return sqrt((x1-x2) * (x1-x2) + (y1-y2) * (y1-y2));
}
void Behavior::checkForPenaltyArea(){
    float fakeDist=0.0, fakeDir=0.0;
    for(int j=0;j<numOfFakeObstacles;j++){
        if(fakeObstacles[j][0]==INIT_VALUE)
            continue;
        else
            fakeDist=dist(robot_x,fakeObstacles[j][0],robot_y,fakeObstacles[j][1]);
            if(fakeDist<MapRadius){
                //send fake obstacle message
                fakeDir = anglediff2(atan2(fakeObstacles[j][1]-robot_y,fakeObstacles[j][0]-robot_x),robot_phi);
                fom->set_direction(fakeDir);
                fom->set_distance(fakeDist);
                _blk.publishData(*fom, "obstacle");
            }
    }
}

/* Test Function */

void Behavior::test() {


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
		Logger::Instance().WriteMsg("Behavior",  " timestamp " + to_simple_string(microsec_clock::universal_time()), Logger::Info);
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
				Logger::Instance().WriteMsg("Behavior",  "OP: x1 " + _toString(x1) + " y1 " + _toString(y1), Logger::Info);
				Logger::Instance().WriteMsg("Behavior",  "OP: ubx " + _toString(ubx) + " uby " + _toString(uby), Logger::Info);

				if (fabs(uby) < 0.001)
					return;
				slope = ubx / uby;

				if ( fabs(kickslope-slope) < 0.001 )
					return;

				y = (x1-y1*slope) / (kickslope-slope);
				x = kickslope * y;
				t = fabs(x1-x) / fabs(ubx);
				Logger::Instance().WriteMsg("Behavior", "OP: x "+_toString(x) + " y " +_toString(y) + "TIME: " + _toString(t), Logger::Info);
				if ( (x >= 0.03) && (x <= 0.17) ) {
					if ( (t > 2.0) && (t < 5.0) ) {
						Logger::Instance().WriteMsg("Behavior",  "OP: Openchallenge FTW!!!!!!!!!!!!!! ", Logger::Info);
						amot->set_command("KickForwardRightFast.xar");
						_blk.publishSignal(*amot, "motion");
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
		//Logger::Instance().WriteMsg("Behavior", "L: " + _toString(om->direction(0)) + " C: " + _toString(om->direction(1)) + " R: " + _toString(om->direction(2)), Logger::Info);
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

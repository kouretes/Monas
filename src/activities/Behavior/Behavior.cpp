#include "Behavior.h"
#include "hal/robot/generic_nao/kAlBroker.h"

#include "alxplatform.h"
#include <albroker.h>
#include <alproxy.h>
#include <almemoryfastaccess.h>
#include "almotionproxy.h"
#include "almemoryproxy.h"
#include "albrokermanager.h"
#include "alvalue.h"

#include "tools/logger.h"
#include "tools/toString.h"
#include "messages/RoboCupGameControlData.h"
#include "architecture/narukom/pub_sub/filters/special_filters.h"

namespace {
	ActivityRegistrar<Behavior>::Type temp("Behavior");
}

Behavior::Behavior()        {
}

void Behavior::UserInit() {
	_com->get_message_queue()->subscribe("vision", _blk, 0);
	_com->get_message_queue()->subscribe("sensors", _blk, 0);
	_com->get_message_queue()->subscribe("behavior", _blk, 0);
	_com->get_message_queue()->subscribe("obstacle", _blk, 0);

	wmot = new MotionWalkMessage();
	wmot->set_topic("motion");
	wmot->add_parameter(0.0f);
	wmot->add_parameter(0.0f);
	wmot->add_parameter(0.0f);
	wmot->add_parameter(0.0f);

	hmot = new MotionHeadMessage();
	hmot->set_topic("motion");
	hmot->add_parameter(0.0f);
	hmot->add_parameter(0.0f);

	amot = new MotionActionMessage();
	amot->set_topic("motion");

	ballfound = 0;

	reachedlimitup = false;
	reachedlimitdown = false;
	reachedlimitleft = false;
	reachedlimitright = false;

	scanforball = true;
	startscan = true;

	calibrated = 0;

	stopped = true;
	play = false;
	kickoff = false;

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

	Logger::Instance().WriteMsg("Behavior", "Initialized", Logger::Info);
}

int Behavior::MakeTrackBallAction() {

	float overshootfix = 0.50;
	float cx = bmsg->cx();
	float cy = bmsg->cy();
	//if (fabs(cx) > 0.015 || fabs(cy) > 0.015) {
		//hmot->set_command("setHead");
		//hmot->set_parameter(0, bmsg->referenceyaw() - overshootfix * cx);
		//hmot->set_parameter(1, bmsg->referencepitch() - overshootfix * cy);
		hmot->set_command("setHead");
        hmot->set_parameter(0, bmsg->referenceyaw());
		hmot->set_parameter(1,  bmsg->referencepitch());
		_blk->publish_signal(*hmot, "motion");
		cout<<"Track step"<<endl;
	//}
	return 1;
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

	if (gsm != 0) {
		Logger::Instance().WriteMsg("Behavior", " Player_state " + _toString(gsm->player_state()), Logger::ExtraExtraInfo);
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
		}
		else if (gameState == PLAYER_INITIAL) {
			play = false;
		}
		else if (gameState == PLAYER_READY) {
			play = false;
		}
		else if (gameState == PLAYER_SET) {
			play = false;
			calibrate();
			kickoff = gsm->kickoff();
		}
		else if (gameState == PLAYER_FINISHED) {
			play = false;
		}
		else if (gameState == PLAYER_PENALISED) {
			play = false;
			//calibrate();
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

	//if (play) mgltest();
	//return 1;

	if (play) {

		if (bmsg != 0) {
			Logger::Instance().WriteMsg("Behavior", "BallTrackMessage", Logger::ExtraExtraInfo);
			if (bmsg->radius() > 0) { //This means that a ball was found
				scanforball = false; //if you are scanning for ball please stop now
				back = 0;
				MakeTrackBallAction();

				ballfound += 1;
				if (ballfound > 20)
					ballfound = 20; //Increase this value when we see the ball
			}
			else {
				if (ballfound > 0)
					ballfound -= 1; //Decrease it when we don't see the ball
			}
		}
		//return 0;
		Logger::Instance().WriteMsg("Behavior", "ballfound Value: " + _toString(ballfound), Logger::ExtraInfo);

		float X=0.0, Y=0.0, theta=0.0;
		float bd=0.0, bx=0.0, by=0.0, bb=0.0;
		 float posx=0.13, posy=0.035;

		if ((obsm != 0) && !turning) {

			scanforball = false; //be sure to stop scanning
			int side=1;
			bd = obsm->ball().dist();
			bb = obsm->ball().bearing();
			bx = obsm->ball().dist() * cos( obsm->ball().bearing() );
			by = obsm->ball().dist() * sin( obsm->ball().bearing() );
			side = (bb > 0) ? 1 : -1;
			Logger::Instance().WriteMsg("Behavior", "Measurements - Distance: " + _toString(bd) + "  Bearing: " + _toString(bb) + "  BX: " + _toString(bx) + "  BY: " + _toString(by), Logger::Info);
            readytokick==true;
			if (!readytokick&&ballfound>=2) {

				readytokick = true;

                if ( fabs( bx - posx ) > 0.015  || fabs( by - (side*posy) ) > 0.015) {
                    //Y = gainFine * ( by - (side*posy) );
                    readytokick = false;
                }


				if (!readytokick) {
                    if(bd>0.5){
                        float X=0,Y=0,th=0;
                        if(fabs(bx)>0.15) X=1;
                        if(fabs(by)>0.15) Y=side;
                        velocityWalk(X,Y,0.01*side,1);


                    }
                    else
                    {
                        float offsety=side*posy;
                        float g=0.3;
                        littleWalk((bx-posx)*g,(by-offsety)*g,0,0.6);
                    }
				}
				else {
					//velocityWalk(0.0, 0.0, 0.0, 1.0);
					return 1;
				}
			}
		}
		else {
			readytokick = false;
		}

		/* Ready to take action */
		if (readytokick && !turning) {
			obstacleFront = false;
			if (om!=0)
				if (om->direction() == 0)
					obstacleFront = true;

			if ( kickoff ) {
				//if (mglRand()<0.5) {
				if ( (mglRand()<1.0) && !obstacleFront ) {
					littleWalk(0.2, 0.0, 0.0, 2);
				} else {
					if (bb > 0.0) {
						amot->set_command("SoftLeftSideKick");
						direction = -1;
					}
					else {
						amot->set_command("SoftRightSideKick");
						direction = +1;
					}
					_blk->publish_signal(*amot, "motion");
				}
				kickoff = false;
			}
			else {
				if (mglRand()<0.6) {
				//if ( (mglRand()<1.0) && !obstacleFront ) {
					if (by > 0.0)
						amot->set_command("LeftKick");
					else
						amot->set_command("RightKick");
				}
				else if (mglRand()<0.5) {
					if (by > 0.0) {
						amot->set_command("HardLeftSideKick");
						direction = -1;
					}
					else {
						amot->set_command("HardRightSideKick");
						direction = +1;
					}
				}
				else {
					if (by > 0.0)
						amot->set_command("LeftBackKick");
					else
						amot->set_command("RightBackKick");
				}
				_blk->publish_signal(*amot, "motion");
				back = 0;
			}
			readytokick = false;
			scanforball = true;
			startscan = true;
		}

		if ( (ballfound == 0) && !readytokick && !turning ) {
			if (!scanforball) {
				startscan = true;
				velocityWalk(0.0, 0.0, 0.0, 1.0);
			}
			scanforball = true;
		}

		if (scanforball && !readytokick && !turning && (hjsm != 0) ) {
			HeadYaw = hjsm->sensordata(0);
			HeadPitch = hjsm->sensordata(1);
			HeadScanStep();
		}

	} else if (!play) {   // Non-Play state
		velocityWalk(0.0, 0.0, 0.0, 1.0);
	}

	return 0;
}




void Behavior::HeadScanStep() {

	if (startscan) {
		littleWalk(0.0, 0.0, +2*TO_RAD, 0);
		//BE CAREFULL the max sign is according to sensors values (max maybe negative! :p)
		if (HeadPitch.sensorvalue() < LIMITDOWN) { // first go down
			scandirectionpitch = 1;
		} else {
			scandirectionpitch = -1; // go up
		}
		reachedlimitup = false;
		reachedlimitdown = false;
		reachedlimitleft = false;
		reachedlimitright = false;
		scandirectionyaw = (HeadYaw.sensorvaluediff() > 0) ? 1 : -1;
		startscan = false;
	}

	//continue scan
	if (HeadPitch.sensorvalue() < LIMITUP) {
		//Logger::Instance().WriteMsg("Behavior", " LIMITUP ", Logger::ExtraExtraInfo);
		reachedlimitup = true;
		scandirectionpitch = 1;
	}
	if (HeadPitch.sensorvalue() > LIMITDOWN) {
		//Logger::Instance().WriteMsg("Behavior", " LIMITDOWN ", Logger::ExtraExtraInfo);
		reachedlimitdown = true;
		scandirectionpitch = -1;
	}
	if (HeadYaw.sensorvalue() > LIMITLEFT) {
		//Logger::Instance().WriteMsg("Behavior", "LIMITLEFT  ", Logger::ExtraExtraInfo);
		reachedlimitleft = true;
		scandirectionyaw = -1;
	}
	if (HeadYaw.sensorvalue() < LIMITRIGHT) {
		//Logger::Instance().WriteMsg("Behavior", " LIMITRIGHT  ", Logger::ExtraExtraInfo);
		reachedlimitright = true;
		scandirectionyaw = 1;
	}

	hmot->set_command("changeHead");
	hmot->set_parameter(0, scandirectionyaw * STEPHOR); // Headyaw
	hmot->set_parameter(1, 0.0); // headPitch

	if (reachedlimitleft && reachedlimitright) {
		Logger::Instance().WriteMsg("Behavior", " reachedlimitleft && reachedlimitright ", Logger::ExtraExtraInfo);
		hmot->set_parameter(1, scandirectionpitch * STEPVER); // headPitch
		reachedlimitleft = false;
		reachedlimitright = false;
	}
	_blk->publish_signal(*hmot, "motion");

	if (reachedlimitup && reachedlimitdown) { //scanning completed
		Logger::Instance().WriteMsg("Behavior", " reachedlimitup && reachedlimitdown ", Logger::ExtraExtraInfo);
		startscan = true;
		reachedlimitdown = false;
		reachedlimitup = false;
		reachedlimitright = false;
		reachedlimitleft = false;
		///we should do something;
		if (back>0) {
			littleWalk(-0.2, 0.0, 0.0, 2);
			back--;
		}
		else {
			littleWalk(0.0, 0.0, direction * 90 * TO_RAD, 1);
			//direction = - direction;
		}
	}
}

void Behavior::read_messages() {

	//if (gsm != 0) delete gsm;
	//if (bmsg != 0) delete bmsg;
	//if (hjsm != 0) delete hjsm;
	//if (obsm != 0) delete obsm;
	//if (om != 0) delete om;

	gsm  = _blk->read_state<GameStateMessage> ("GameStateMessage");
	bmsg = _blk->read_signal<BallTrackMessage> ("BallTrackMessage");
	hjsm = _blk->read_data<HeadJointSensorsMessage> ("HeadJointSensorsMessage");
	obsm = _blk->read_signal<ObservationMessage> ("ObservationMessage");
	om   = _blk->read_signal<ObstacleMessage> ("ObstacleMessage");

	Logger::Instance().WriteMsg("Behavior", "read_messages ", Logger::ExtraExtraInfo);
	boost::shared_ptr<const CalibrateCam> c= _blk->read_state<CalibrateCam> ("CalibrateCam");
	if (c != NULL) {
		if (c->status() == 1)
			calibrated = 2;
	}
}

double Behavior::mglRand()
{
    return rand() / double(RAND_MAX);
}

void Behavior::velocityWalk(double x, double y, double th, double f)
{
	wmot->set_command("setWalkTargetVelocity");
	wmot->set_parameter(0, x);
	wmot->set_parameter(1, y);
	wmot->set_parameter(2, th);
	wmot->set_parameter(3, f);
	_blk->publish_signal(*wmot, "motion");
}

void Behavior::littleWalk(double x, double y, double th, int s)
{
	wmot->set_command("walkTo");
	wmot->set_parameter(0, x);
	wmot->set_parameter(1, y);
	wmot->set_parameter(2, th);
	_blk->publish_signal(*wmot, "motion");
}

void Behavior::calibrate()
{
	CalibrateCam v;
	v.set_status(0);
	_blk->publish_signal(v, "vision");
	calibrated = 1;
}


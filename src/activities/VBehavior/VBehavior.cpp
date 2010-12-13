#include "VBehavior.h"
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
	ActivityRegistrar<VBehavior>::Type temp("VBehavior");
}

VBehavior::VBehavior()        {
}

void VBehavior::UserInit() {
	_com->get_message_queue()->subscribe("vision", _blk, 0);
	_com->get_message_queue()->subscribe("sensors", _blk, 0);
	_com->get_message_queue()->subscribe("behavior", _blk, 0);
	_com->get_message_queue()->subscribe("obstacle", _blk, 0);

	wmot = new MotionWalkMessage();
	//wmot->set_topic("motion");
	wmot->add_parameter(0.0f);
	wmot->add_parameter(0.0f);
	wmot->add_parameter(0.0f);
	wmot->add_parameter(0.0f);

	hmot = new MotionHeadMessage();
	//hmot->set_topic("motion");
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
	cX=0;cY=0;cth=0;

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

	Logger::Instance().WriteMsg("VBehavior", "Initialized", Logger::Info);
}

int VBehavior::MakeTrackBallAction() {

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


void VBehavior::mgltest() {

	if (om!=0) {
		Logger::Instance().WriteMsg("VBehavior", "Obstacle - Direction: " + _toString(om->direction()), Logger::Info);
		if (om->direction() == 0)
			velocityWalk(0.0, 0.0, 1.0, 1.0);
	}
	else
		velocityWalk(1.0, 0.0, 0.0, 1.0);
}

int VBehavior::Execute() {

	read_messages();

	if (gsm != 0) {
		Logger::Instance().WriteMsg("VBehavior", " Player_state " + _toString(gsm->player_state()), Logger::ExtraExtraInfo);
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
	static  int kickno=0;
	static ptime lastkick=microsec_clock::universal_time()+seconds(4);
	if (play) {

		if (bmsg != 0) {
			Logger::Instance().WriteMsg("VBehavior", "BallTrackMessage", Logger::ExtraExtraInfo);
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
		//eturn 0;
		Logger::Instance().WriteMsg("VBehavior", "ballfound Value: " + _toString(ballfound), Logger::ExtraInfo);

		float X=0.0, Y=0.0, theta=0.0;
		static float bd=0.0, bx=0.0, by=0.0, bb=0.0;
		float posx=0.145, posy=0.026;
		static float lastx=0,lasty=0;

		if ((obsm != 0) && !turning) {

			scanforball = false; //be sure to stop scanning
			int side=1;
			bd = obsm->ball().dist();
			bb = obsm->ball().bearing();
			bx = obsm->ball().dist() * cos( obsm->ball().bearing() )/2 + bx/2;
			by = obsm->ball().dist() * sin( obsm->ball().bearing() )/2 + by/2;
			side = (bb > 0) ? 1 : -1;
			if(kickno==0||kickno==2)	side=-1;
				else side=1;
			Logger::Instance().WriteMsg("VBehavior", "Measurements - Distance: " + _toString(bd) + "  Bearing: " + _toString(bb) + "  BX: " + _toString(bx) + "  BY: " + _toString(by), Logger::Info);
			if (ballfound>=1) {

				readytokick = true;

                if ( fabs( bx - posx ) > 0.01  || fabs( by - (side*posy) ) > 0.009) {
                    //Y = gainFine * ( by - (side*posy) );
                    readytokick = false;
                }
				float offsety=side*posy;

				if (!readytokick) {
                   // if(abs(bd>0.6))
					//{

						static float X=0,Y=0,th=0,f=0.2;
						//X=(bx-posx)*2;
						X=((bx-posx +  5*(bx-lastx)-5*cX*0.042)/0.042)*0.45;
						X=X>0?X:X-0.04;
						X=X>1?1:X;
						X=X<-1?-1:X;
						//Y=(by-offsety)*1.6;
						Y=((by-offsety+5*(by-lasty)-5*cY*0.045)/0.045)*0.45;

						lastx=bx;
						lasty=by;

						if(bd>0.26)
							th=0.1 *Y;
						else
							th=-0.15*by*(Y>0?-1:1);


						Y=Y>0?Y+0.05:Y-0.05;
						Y=Y>1?1:Y;
						Y=Y<-1?-1:Y;
						f=1;

						th=th>1?1:th;
						th=th<-1?-1:th;





                        velocityWalk(X,Y,th,f);

					/*}
                    else
                    {

                        float g=0.3;
                        littleWalk((bx-posx)*g,(by-offsety)*g,side*0.01,0);
                    }*/
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
					littleWalk(0.2, 0.0, 0.0, 1);
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
			else if(lastkick<=microsec_clock::universal_time()){

				if(kickno==3){//if (mglRand()<0.6) {
				//if ( (mglRand()<1.0) && !obstacleFront ) {
					if (by > 0.0)
						amot->set_command("LeftKick");
					else
						amot->set_command("RightKick");
					kickno--;
				}
				else if(kickno==0)
				{
					if (by > 0.0) {
						amot->set_command("LefTak3.xar");
						direction = -1;
					}
					else {
						amot->set_command("RightTak3.xar");
						direction = +1;
					}
					kickno++;

				}
				else
				{
					if (by > 0.0) {
						amot->set_command("LeftSideKickFast.xar");
						direction = -1;
					}
					else {
						amot->set_command("RightSideKickFast.xar");
						direction = +1;
					}
					kickno++;
				}
				lastkick=microsec_clock::universal_time()+seconds(4);

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

		if (scanforball && !readytokick && !turning && (allsm != 0&&allsm->has_hjsm()) ) {

			HeadYaw= allsm->hjsm().sensordata(YAW);
			HeadPitch= allsm->hjsm().sensordata(PITCH);
			HeadScanStep();
		}

	} else if (!play) {   // Non-Play state
		velocityWalk(0.0, 0.0, 0.0, 1.0);
	}

	return 0;
}




void VBehavior::HeadScanStep() {

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
		//Logger::Instance().WriteMsg("VBehavior", " LIMITUP ", Logger::ExtraExtraInfo);
		reachedlimitup = true;
		scandirectionpitch = 1;
	}
	if (HeadPitch.sensorvalue() > LIMITDOWN) {
		//Logger::Instance().WriteMsg("VBehavior", " LIMITDOWN ", Logger::ExtraExtraInfo);
		reachedlimitdown = true;
		scandirectionpitch = -1;
	}
	if (HeadYaw.sensorvalue() > LIMITLEFT) {
		//Logger::Instance().WriteMsg("VBehavior", "LIMITLEFT  ", Logger::ExtraExtraInfo);
		reachedlimitleft = true;
		scandirectionyaw = -1;
	}
	if (HeadYaw.sensorvalue() < LIMITRIGHT) {
		//Logger::Instance().WriteMsg("VBehavior", " LIMITRIGHT  ", Logger::ExtraExtraInfo);
		reachedlimitright = true;
		scandirectionyaw = 1;
	}

	hmot->set_command("changeHead");
	hmot->set_parameter(0, scandirectionyaw * STEPHOR); // Headyaw
	hmot->set_parameter(1, 0.0); // headPitch

	if (reachedlimitleft && reachedlimitright) {
		Logger::Instance().WriteMsg("VBehavior", " reachedlimitleft && reachedlimitright ", Logger::ExtraExtraInfo);
		hmot->set_parameter(1, scandirectionpitch * STEPVER); // headPitch
		reachedlimitleft = false;
		reachedlimitright = false;
	}
	_blk->publish_signal(*hmot, "motion");

	if (reachedlimitup && reachedlimitdown) { //scanning completed
		Logger::Instance().WriteMsg("VBehavior", " reachedlimitup && reachedlimitdown ", Logger::ExtraExtraInfo);
		startscan = true;
		reachedlimitdown = false;
		reachedlimitup = false;
		reachedlimitright = false;
		reachedlimitleft = false;
		///we should do something;
		if (back>0) {
			littleWalk(-0.2, 0.0, 0.0, 1);
			back--;
		}
		else {
			littleWalk(0.0, 0.0, direction * 90 * TO_RAD, 1);
			//direction = - direction;
		}
	}
}

void VBehavior::read_messages() {

	//if (gsm != 0) delete gsm;
	//if (bmsg != 0) delete bmsg;
	//if (hjsm != 0) delete hjsm;
	//if (obsm != 0) delete obsm;
	//if (om != 0) delete om;

	gsm  = _blk->read_state<GameStateMessage> ("GameStateMessage");
	bmsg = _blk->read_signal<BallTrackMessage> ("BallTrackMessage");
	allsm = _blk->read_data<AllSensorValues> ("AllSensorValues");
	obsm = _blk->read_signal<ObservationMessage> ("ObservationMessage");
	om   = _blk->read_signal<ObstacleMessage> ("ObstacleMessage");

	Logger::Instance().WriteMsg("VBehavior", "read_messages ", Logger::ExtraExtraInfo);
	boost::shared_ptr<const CalibrateCam> c= _blk->read_state<CalibrateCam> ("CalibrateCam");
	if (c != NULL) {
		if (c->status() == 1)
			calibrated = 2;
	}
}

double VBehavior::mglRand()
{
    return rand() / double(RAND_MAX);
}

void VBehavior::velocityWalk(double x, double y, double th, double f)
{
	wmot->set_command("setWalkTargetVelocity");
	x=x>1?1:x;
	x=x<-1?-1:x;
	y=y>1?1:y;
	y=y<-1?-1:y;

	th=th>1?1:th;
	th=th<-1?-1:th;
	cX=(cX+3*x)/4;
	cY=(cY+3*y)/4;
	cth=(cth+th)/2;


	wmot->set_parameter(0, cX);
	wmot->set_parameter(1, cY);
	wmot->set_parameter(2, cth);
	wmot->set_parameter(3, f);
	_blk->publish_signal(*wmot, "motion");
}

void VBehavior::littleWalk(double x, double y, double th, int s)
{
	wmot->set_command("walkTo");
	wmot->set_parameter(0, x);
	wmot->set_parameter(1, y);
	wmot->set_parameter(2, th);
	_blk->publish_signal(*wmot, "motion");
}

void VBehavior::calibrate()
{
	CalibrateCam v;
	v.set_status(0);
	_blk->publish_signal(v, "vision");
	calibrated = 1;
}


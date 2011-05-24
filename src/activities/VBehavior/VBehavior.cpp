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
using namespace std;

VBehavior::VBehavior()        {
}

void VBehavior::UserInit() {
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
	hmot->add_parameter(0.0f);
	hmot->add_parameter(0.0f);

	amot = new MotionActionMessage();

	ballfound = 0;



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

	//if (play) mgltest();
	//return 1;
	static  int kickno=0;
	static ptime lastkick=microsec_clock::universal_time()+seconds(4);
	static ptime lastball=microsec_clock::universal_time()-seconds(30);

	if (play) {

		if (bmsg != 0) {
			Logger::Instance().WriteMsg("VBehavior", "BallTrackMessage", Logger::ExtraExtraInfo);
			if (bmsg->radius() > 0) { //This means that a ball was found
				scanforball = false; //if you are scanning for ball please stop now
				back = 0;
				MakeTrackBallAction();
				lastball=microsec_clock::universal_time();
				ballfound =20;
			}
			else {
				if (lastball+seconds(1)<microsec_clock::universal_time())
					ballfound = 0; //Decrease it when we don't see the ball
			}
		}
		//return 0;
		Logger::Instance().WriteMsg("VBehavior", "ballfound Value: " + _toString(ballfound), Logger::ExtraInfo);

		//float X=0.0, Y=0.0, theta=0.0;
		float bd=0.0, bx=0.0, by=0.0, bb=0.0;
		float posx=0.115, posy=0.04;
		//static float lastx=0,lasty=0;

		if ((obsm != 0) && !turning) {

			scanforball = false; //be sure to stop scanning
			int side=1;
			bd = obsm->ball().dist();
			bb = obsm->ball().bearing();
			bx = obsm->ball().dist() * cos( obsm->ball().bearing() ) ;//+ bx/2;
			by = obsm->ball().dist() * sin( obsm->ball().bearing() ) ;// + by/2;
			side = (bb > 0) ? 1 : -1;
			Logger::Instance().WriteMsg("VBehavior", "Measurements - Distance: " + _toString(bd) + "  Bearing: " + _toString(bb) + "  BX: " + _toString(bx) + "  BY: " + _toString(by), Logger::Info);
			if (ballfound>=1) {

				readytokick = true;

                if ( fabs( bx - posx ) > 0.01  || fabs( by - (side*posy) ) > 0.06) {
                    //Y = gainFine * ( by - (side*posy) );
                    readytokick = false;
                }
				float offsety=side*posy;

				if (!readytokick) {
                   // if(abs(bd>0.6))
					//{

						static float X=0,Y=0,th=0,f=0.2;
						//X=(bx-posx)*2;
						X=(bx-posx )*3;
						X=X>0?X:X-0.001;
						X=X>1?1:X;
						X=X<-1?-1:X;
						//Y=(by-offsety)*1.6;
						Y=(by-offsety)*3;
//
//						lastx=bx;
//						lasty=by;

						if(bd>0.30)
						{
							if(bx<0)
								th=0.2 *Y;
							else
								th=0.1 *Y;

							Y=Y*2.0/3.0;
							X=X*2.0/3.0;

						}
						else
							th=-0.06*by*(Y>0?-1:1);


						Y=Y>0?Y+0.01:Y-0.01;
						Y=Y>1?1:Y;
						Y=Y<-1?-1:Y;
						f=1;

						th=th>1?1:th;
						th=th<-1?-1:th;
						velocityWalk(X,Y,th,f);

				}
			}
		}
		else {
			readytokick = false;
		}

		/* Ready to take action */
		if (readytokick && !turning) {
			if (bb > 0.0) {
				amot->set_command("KickForwardLeftCutB.xar"); //"KickSideLeftFast.xar"
				direction = -1;
			}
			else {
				amot->set_command("KickForwardRightCutB.xar"); //"KickSideRightFast.xar"
				direction = +1;
			}
			_blk->publishSignal(*amot, "motion");

			kickoff = false;

			lastkick=microsec_clock::universal_time()+seconds(4);


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




void VBehavior::HeadScanStep() {

	static float s=(YAWMIN-YAWMAX)/(PITCHMIN-PITCHMAX);

	if (startscan) {
		//BE CAREFULL the max sign is according to sensors values (max maybe negative! :p)
		ysign=HeadYaw.sensorvalue()>0?1:-1; //Side
		//Crop to limits
		targetPitch=HeadPitch.sensorvalue();
		targetYaw=HeadYaw.sensorvalue();
		targetPitch=(targetPitch>=PITCHMAX)?PITCHMAX:targetPitch;
		targetPitch=(targetPitch<=PITCHMIN)?PITCHMIN:targetPitch;


		float yawlim=s*(targetPitch-PITCHMAX)+YAWMAX;
		//if(fabs(targetPitch)<PITCHSTEP) yawlim=YAWBACK;


		targetYaw+=ysign*YAWSTEP;
		targetYaw=fabs(targetYaw)>=yawlim?ysign*yawlim:targetYaw;
				if(fabs(targetYaw)>=yawlim)
		{
			ysign=-ysign;
		}
		psign=1;//Down
		hmot->set_command("setHead");
		hmot->set_parameter(0, targetYaw);
		hmot->set_parameter(1, targetPitch);
		_blk->publishSignal(*hmot, "motion");
		waiting=0;

		startscan=false;

	}
	waiting++;
	if( (fabs(targetPitch-HeadPitch.sensorvalue())<=OVERSH &&fabs(targetYaw -HeadYaw.sensorvalue())<=OVERSH )
		|| waiting>=WAITFOR)
	{

		waiting=0;

		float yawlim=s*(targetPitch-PITCHMAX)+YAWMAX;
		//if(fabs(targetPitch)<PITCHSTEP) yawlim=YAWBACK;


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
			{
				ysign=-ysign;
			}

		}

		hmot->set_command("setHead");
		hmot->set_parameter(0, targetYaw);
		hmot->set_parameter(1, targetPitch);
		_blk->publishSignal(*hmot, "motion");

	}
	return ;

}

void VBehavior::read_messages() {

	//if (gsm != 0) delete gsm;
	//if (bmsg != 0) delete bmsg;
	//if (hjsm != 0) delete hjsm;
	//if (obsm != 0) delete obsm;
	//if (om != 0) delete om;

	gsm  = _blk->readState<GameStateMessage> ("behavior");
	bmsg = _blk->readSignal<BallTrackMessage> ("vision");
	allsm = _blk->readData<AllSensorValuesMessage> ("sensors");
	obsm = _blk->readSignal<ObservationMessage> ("vision");
	om   = _blk->readSignal<ObstacleMessage> ("obstacle");

	Logger::Instance().WriteMsg("VBehavior", "read_messages ", Logger::ExtraExtraInfo);
	boost::shared_ptr<const CalibrateCam> c= _blk->readState<CalibrateCam> ("vision");
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
	static ptime lastcommand=microsec_clock::universal_time();
	if(microsec_clock::universal_time()-lastcommand<milliseconds(200))
		return ;
	wmot->set_command("setWalkTargetVelocity");
	lastcommand=microsec_clock::universal_time();
	x=x>1?1:x;
	x=x<-1?-1:x;
	y=y>1?1:y;
	y=y<-1?-1:y;

	th=th>1?1:th;
	th=th<-1?-1:th;
	cX=(2*cX+x)/3;
	cY=(2*cY+y)/3;
	cth=(cth+th)/2;


	wmot->set_parameter(0, cX);
	wmot->set_parameter(1, cY);
	wmot->set_parameter(2, cth);
	wmot->set_parameter(3, f);
	_blk->publishSignal(*wmot, "motion");
}

void VBehavior::littleWalk(double x, double y, double th)
{
	wmot->set_command("walkTo");
	wmot->set_parameter(0, x);
	wmot->set_parameter(1, y);
	wmot->set_parameter(2, th);
	_blk->publishSignal(*wmot, "motion");
}

void VBehavior::calibrate()
{
	CalibrateCam v;
	v.set_status(0);
	_blk->publishState(v, "vision");
	calibrated = 1;
}


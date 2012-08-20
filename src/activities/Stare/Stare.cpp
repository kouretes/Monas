
#include "Stare.h"
#include "hal/robot/generic_nao/robot_consts.h"
#include "messages/Gamecontroller.pb.h"

ACTIVITY_REGISTER(Stare);
int Stare::Execute()
{
	Logger::Instance().WriteMsg("Stare",  " execute", Logger::Info);
	//obsm = _blk.readSignal<ObservationMessage> ("vision");
	wimsg = _blk.readData<WorldInfo> ("worldstate");
	velocityWalk(0.0, 0.0, 0.0, 1.0);
	LedChangeMessage leds;
	float bd = 0.0, bx = 0.0, by = 0.0, bb = 0.0;
	LedValues* l = leds.add_leds();
	headaction = BALLTRACK;
	l->set_chain("r_ear");
	l->set_color( "off");
	_blk.publishSignal(leds, "leds");

	if (wimsg.get() == 0)
	{
		//Logger::Instance().WriteMsg("Approachball",  " No OBS", Logger::Info);
		return 0;
	}
	else
	{
		if(wimsg->balls_size() != 0)
		{
			float by = 0.0, bb = 0.0;
			by = wimsg->balls(0).relativey();

			if(by != 0.0)
				bb = atan(bx / by);

			fall = toFallOrNotToFall(wimsg);

			if(penaltyMode)
			{
				if(bb >= 0 && wimsg->balls_size() > 0 && wimsg->balls(0).relativexspeed() < 0)
					fall = 1;
				else if(bb < 0 && wimsg->balls_size() > 0 && wimsg->balls(0).relativexspeed() < 0)
					fall = -1;
			}

			fm->set_fall(fall);
			_blk.publishState(*fm, "behavior");
		}
	}

	bhmsg->set_headaction(headaction);
	_blk.publishSignal(*bhmsg, "behavior");
	_blk.publish_all();
	return 0;
}

void Stare::UserInit ()
{
	fm = new FallMessage();
	_blk.updateSubscription("worldstate", msgentry::SUBSCRIBE_ON_TOPIC);
	_blk.updateSubscription("behavior", msgentry::SUBSCRIBE_ON_TOPIC);
	fall = 0;
	penaltyMode = false;	   ////////an eimaste se penalty to allazoume se true!!!!!!!!!!!!!!
	//wmot = new MotionWalkMessage();
	wmot.add_parameter(0.0f);
	wmot.add_parameter(0.0f);
	wmot.add_parameter(0.0f);
	wmot.add_parameter(0.0f);
	bhmsg = new BToHeadMessage();
	//lastWimsg = new WorldInfo();
	headaction = BALLTRACK;
}

std::string Stare::GetName ()
{
	return "Stare";
}


void Stare::velocityWalk(double x, double y, double th, double f)
{
	//Logger::Instance().WriteMsg("Approachball",  " VelocityWalk", Logger::Info);
	wmot.set_command("setWalkTargetVelocity");
	wmot.set_parameter(0, x);
	wmot.set_parameter(1, y);
	wmot.set_parameter(2, th);
	wmot.set_parameter(3, f);
	_blk.publishSignal(wmot, "motion");
}

/*this function returns 0 when the goalkeeper shouldn't extend its foot, -1 for right foot extended and
 * 1 for left foot extended in order to block the ball
 *
 * Distance in meters
 * Time in milliseconds
 * Velocity in meters/milliseconds
 * */
int Stare::toFallOrNotToFall( boost::shared_ptr<const WorldInfo>  doi)
{
	//if(doi.get()==0){	//the two last observation messages
	if(doi == 0) 	//the two last observation messages
	{
		//Logger::Instance().WriteMsg("toFallOrNotToFall",  " no obs", Logger::Info);
		return 0;
	}

	float x1, y1, temp, dk;
	float ub, ubx, ur, uby;

	if(doi->balls_size() == 0  )
		return 0;

	x1 = doi->balls(0).relativex();	//the last b observation's x position
	y1 = doi->balls(0).relativey();	//the last but one observation's y position
	ubx = doi->balls(0).relativexspeed();
	uby = doi->balls(0).relativeyspeed();
	float ds, dx, ws;

	if(ubx < 0.0)
	{
		//ds = sqrt(pow((x1 - x2),2) + pow((y1 - y2),2));	// the distance between the last two ball positions
		Logger::Instance().WriteMsg("toFallOrNotToFall",  " ubx>0", Logger::Info);
		//temp = ((x1-x2)/ds);
		//ws = acos(temp);
		dk = (ubx * y1 - uby * x1) / ubx ; // dk is the projection of the ball's route towards the robot/goalpost

		// y1 - x1/tan(θ), tan(θ)  =  ubx/uby
		//Logger::Instance().WriteMsg("toFallOrNotToFall",  " dk = " + _toString(dk), Logger::Info);
		if(fabs(dk) <= 0.3) //if dk is shorter than the robot's foot can extend
		{
			//		Logger::Instance().WriteMsg("toFallOrNotToFall",  " t1 = " + t1, Logger::Info);
			//		Logger::Instance().WriteMsg("toFallOrNotToFall",  " t2 = " + t2, Logger::Info);
			ur = 0.1 / 1.4;
			ub = sqrt(ubx * ubx + uby * uby);

			//	Logger::Instance().WriteMsg("toFallOrNotToFall",  " ur = " + _toString(ur), Logger::Info);
			//	Logger::Instance().WriteMsg("toFallOrNotToFall",  " ub = " + _toString(ub), Logger::Info);
			//Logger::Instance().WriteMsg("toFallOrNotToFall",  " ubx = " + _toString(ubx), Logger::Info);

			if(fabs(ub) > ur)
			{
				if(ubx == 0)
				{
					Logger::Instance().WriteMsg("toFallOrNotToFall",  " ubx = 0", Logger::Info);
					return 0;
				}

				long tk;
				tk = fabs((x1 / ubx)); //in seconds...................mallon

				//	Logger::Instance().WriteMsg("toFallOrNotToFall",  " time to hit the robot" , Logger::Info);
				//if(1.5<tk && tk<6.5){
				//Logger::Instance().WriteMsg("toFallOrNotToFall",  " 300<tk<4000 ", Logger::Info);
				if(dk > 0)
				{
					//Logger::Instance().WriteMsg("toFallOrNotToFall",  " left foot" + to_simple_string(boost::posix_time::microsec_clock::universal_time()), Logger::Info);
					return 1;	//left
				}
				else
				{
					//Logger::Instance().WriteMsg("toFallOrNotToFall",  " right foot" + to_simple_string(boost::posix_time::microsec_clock::universal_time()), Logger::Info);
					return -1;	//right
				}

				//}
			}
		}
	}

	//Logger::Instance().WriteMsg("toFallOrNotToFall",  " no foot", Logger::Info);
	return 0;
}

//void Stare::robotInGoalieArea()

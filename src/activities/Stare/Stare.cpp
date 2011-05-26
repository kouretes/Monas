
#include "Stare.h"

namespace {
    ActivityRegistrar<Stare>::Type temp("Stare");
}

int Stare::Execute() {
	//Logger::Instance().WriteMsg("Stare",  " execute", Logger::Info);
	obsm = _blk->readSignal<ObservationMessage> ("vision");
	//velocityWalk(0.0, 0.0,0.0,1);
	float bd = 0.0, bx = 0.0, by = 0.0, bb = 0.0;
	
	headaction = BALLTRACK;
	if (obsm.get()==0){
		//Logger::Instance().WriteMsg("Approachball",  " No OBS", Logger::Info);
		if (lastObsm==0)
			return 0;
	}else{
		if(lastObsm!=0){
		//	Logger::Instance().WriteMsg("Stare",  " prev time" + lastObsm->image_timestamp(), Logger::ExtraInfo);
			doim->mutable_prev()->CopyFrom(*lastObsm);
			lastObsm->CopyFrom(*obsm);
			doim->mutable_last()->CopyFrom(*obsm);
		//	Logger::Instance().WriteMsg("Stare",  " last time" + obsm->image_timestamp(), Logger::ExtraInfo);
			rcvObsm = boost::posix_time::microsec_clock::universal_time()+boost::posix_time::seconds(3);
			_blk->publishData(*doim, "behavior");
		}else
			lastObsm->CopyFrom(*obsm);
	}
	int side ;//= 1;
	
	bd = lastObsm->ball().dist();
	bb = lastObsm->ball().bearing();
	bx = lastObsm->ball().dist() * cos(lastObsm->ball().bearing()); //kanw tracking me to swma
	by = lastObsm->ball().dist() * sin(lastObsm->ball().bearing());
	side = (bb > 0) ? 1 : -1;

	static float X=0,Y=0,th=0,f=0.2;
	//X=(bx-posx)*2;

	//Y=(by-offsety)*1.6;
	float offsety=side*dDistBallY;
	Y=(by-offsety)*3;
	if(bd>0.26)
	{
		if(bx<0)
			th=0.2 *Y;
		else
			th=0.1 *Y;

		Y=Y/2.0;

	}
	else
		th=-0.06*by*(Y>0?-1:1);

	f=1;

	th=th>1?1:th;
	th=th<-1?-1:th;

	//if(lastMove<= boost::posix_time::microsec_clock::universal_time()){//////////////////////////////
		//velocityWalk(0,0,th,f);
	//	lastMove = boost::posix_time::microsec_clock::universal_time() + boost::posix_time::milliseconds(400);
//	}
	fm->set_fall(toFallOrNotToFall(doim));
	_blk->publishSignal(*fm, "behavior");
	bhmsg->set_headaction(headaction);
	_blk->publishSignal(*bhmsg, "behavior");
	//Logger::Instance().WriteMsg("Stare",  " end", Logger::Info);
	return 0;
}

void Stare::UserInit () {
	rcvObsm = boost::posix_time::microsec_clock::universal_time();
	lastMove = boost::posix_time::microsec_clock::universal_time();
	fm = new FallMessage();
	_blk->subscribeTo("vision", 0);
	_blk->subscribeTo("sensors", 0);
	_blk->subscribeTo("behavior", 0);

	//wmot = new MotionWalkMessage();
	wmot.add_parameter(0.0f);
	wmot.add_parameter(0.0f);
	wmot.add_parameter(0.0f);
	wmot.add_parameter(0.0f);
	bhmsg = new BToHeadMessage();
	lastObsm = new ObservationMessage();
	doim = new DoubleObsInfo();
	headaction= BALLTRACK;
}

std::string Stare::GetName () {
	return "Stare";
}
	

void Stare::velocityWalk(double x, double y, double th, double f) {
	//Logger::Instance().WriteMsg("Approachball",  " VelocityWalk", Logger::Info);
	wmot.set_command("setWalkTargetVelocity");
	wmot.set_parameter(0, x);
	wmot.set_parameter(1, y);
	wmot.set_parameter(2, th);
	wmot.set_parameter(3, f);
	_blk->publishSignal(wmot, "motion");
}

/*this function returns 0 when the goalkeeper shouldn't extend its foot, -1 for right foot extended and 
 * 1 for left foot extended in order to block the ball
 * 
 * Distance in meters
 * Time in milliseconds
 * Velocity in meters/milliseconds
 * */
//int Stare::toFallOrNotToFall(boost::shared_ptr<const DoubleObsInfo> doi){

int Stare::toFallOrNotToFall( DoubleObsInfo* doi){
	
	//if(doi.get()==0){	//the two last observation messages
	if(doi==0){	//the two last observation messages
		//Logger::Instance().WriteMsg("toFallOrNotToFall",  " no obs", Logger::Info);
		return 0;
	}
	float x1, x2, y1, y2, temp, dk;
	float ub, ubx, ur;
	x1 = doi->prev().ball().dist() * cos(doi->prev().ball().bearing());	//the last but one observation's x position
	x2 = doi->last().ball().dist() * cos(doi->last().ball().bearing()); //the last observation's x position
	y1 = doi->prev().ball().dist() * sin(doi->prev().ball().bearing());	//the last but one observation's y position
	y2 = doi->last().ball().dist() * sin(doi->last().ball().bearing());	//the last observation's y position
	std::string t1;
	std::string t2;
	long tk;
	t1 = doi->prev().image_timestamp(); //the last but one observation's timestamp
	t2 = doi->last().image_timestamp(); //the last observation's timestamp
	float ds, dx,ws;
	if(x1>x2){

		ds = sqrt(pow((x1 - x2),2) + pow((y1 - y2),2));	// the distance between the last two ball positions
		//Logger::Instance().WriteMsg("toFallOrNotToFall",  " x1 > x2, ds = " + _toString(ds), Logger::Info);
		if(ds!=0.0){
			//time difference dt
			
			long dt = ((boost::posix_time::from_iso_string(t2) - boost::posix_time::from_iso_string(t1)).total_milliseconds());
			if(dt == 0.0){
				//Logger::Instance().WriteMsg("toFallOrNotToFall",  " dt = 0", Logger::Info);
				return 0;
			}
			temp = ((x1-x2)/ds);
			ws = acos(temp);
			dk = y2-x2*(tan(ws)); // dk is the projection of the ball's route towards the robot/goalpost
			//Logger::Instance().WriteMsg("toFallOrNotToFall",  " dk = " + _toString(dk), Logger::Info);
			if(fabs(dk)<=0.3){ //if dk is shorter than the robot's foot can extend
			
		//		Logger::Instance().WriteMsg("toFallOrNotToFall",  " t1 = " + t1, Logger::Info);
		//		Logger::Instance().WriteMsg("toFallOrNotToFall",  " t2 = " + t2, Logger::Info);
				ur = 0.15/1400;
				ub = ds/dt; // ball speed
				ub  = ub - 0.3*ub;
				ubx = (x2-x1)/dt;
				ubx = ubx - 0.3*ubx;
			//	Logger::Instance().WriteMsg("toFallOrNotToFall",  " ur = " + _toString(ur), Logger::Info);
			//	Logger::Instance().WriteMsg("toFallOrNotToFall",  " ub = " + _toString(ub), Logger::Info);
			//	Logger::Instance().WriteMsg("toFallOrNotToFall",  " ubx = " + _toString(ubx), Logger::Info);
			
				if(fabs(ub)>ur){
					if(ubx==0){
						//Logger::Instance().WriteMsg("toFallOrNotToFall",  " ubx = 0", Logger::Info);
						return 0;
					}
					tk = fabs((x2/ubx)); //in milliseconds...................mallon
					//Logger::Instance().WriteMsg("toFallOrNotToFall",  " time to hit the robot" + _toString(tk), Logger::Info);
					if(1500<tk && tk<6500){
						//Logger::Instance().WriteMsg("toFallOrNotToFall",  " 300<tk<4000 ", Logger::Info);
						if(dk>0){
							Logger::Instance().WriteMsg("toFallOrNotToFall",  " right foot" + to_simple_string(boost::posix_time::microsec_clock::universal_time()), Logger::Info);
							return 1;	//right	
						}
						else{
							Logger::Instance().WriteMsg("toFallOrNotToFall",  " left foot" + to_simple_string(boost::posix_time::microsec_clock::universal_time()), Logger::Info);
							return -1;	//left
						}
					}
				}
			}
		}
	}	
	//Logger::Instance().WriteMsg("toFallOrNotToFall",  " no foot", Logger::Info);
	return 0;		
}


#include "FollowTheBall.h"
#include "tools/logger.h"
#include "tools/toString.h"

namespace {
    ActivityRegistrar<FollowTheBall>::Type temp("FollowTheBall");
}

int FollowTheBall::Execute() {
///Prepei na metonomastei se apofeugw thn mpala/ypoti8etai oti paw pros to kentro sth "8esh mou"
	if (first){
			first = false;
			return 0;
	}
	
	//Logger::Instance().WriteMsg("FollowTheball",  " Execute", Logger::Info);
	float bd = 0.0, bx = 0.0, by = 0.0, bb = 0.0;
	float posx=0.117, posy=0.03;
	obsm = _blk->readSignal<ObservationMessage> ("vision");
	hbm = _blk->readState<HeadToBMessage> ("behavior");
	scm = _blk->readState<ScanMessage> ("behavior");
	if(scm.get()!=0)
		scancompleted = scm->scancompleted();
	if( hbm.get()!=0 ){
		if( hbm->ballfound()>0){
			headaction = BALLTRACK;
			if (obsm.get()==0){
				//Logger::Instance().WriteMsg("Aproachball",  " No OBS", Logger::Info);
				if (lastObsm==0)
					return 0;
				if (rcvObsm < boost::posix_time::microsec_clock::universal_time() ){
					velocityWalk(0,0,0,1);
					headaction=SCANFORBALL;
					return 0;
				}
			}else{
				lastObsm->CopyFrom(*obsm);
				rcvObsm = boost::posix_time::microsec_clock::universal_time()+boost::posix_time::seconds(5);
			}
			int side ;//= 1;
			
			bd = lastObsm->ball().dist();
			bb = lastObsm->ball().bearing();
			bx = lastObsm->ball().dist() * cos(lastObsm->ball().bearing()); //kanw tracking me to swma
			by = lastObsm->ball().dist() * sin(lastObsm->ball().bearing());
			side = (bb > 0) ? 1 : -1;

		   
			static float X=0,Y=0,th=0,f=0.2;
			//X=(bx-posx)*2;
			X=(bx-posx )*3;
			X=X>0?X:X-0.01;
			X=X>1?1:X;
			X=X<-1?-1:X;
			//Y=(by-offsety)*1.6;
			float offsety=side*posy;
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


			Y=Y>0?Y+0.01:Y-0.01;
			Y=Y>1?1:Y;
			Y=Y<-1?-1:Y;
			f=1;

			th=th>1?1:th;
			th=th<-1?-1:th;

               velocityWalk(-X,-Y,th,f);
			//Logger::Instance().WriteMsg("FollowTheball",  " BALLTRACK", Logger::Info);
			
		}
		else{		
			if(scancompleted){
				littleWalk(0.0, 0.0, 45 * TO_RAD, 5);
				scancompleted = false;
				//ScanMessage* sm = new ScanMessage();
			//sm->set_scancompleted(false);
				//_blk->publishState(*sm, "behavior");
				//Logger::Instance().WriteMsg("ScanForBall",  " TURNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNn", Logger::Info);
			}
			else{
				velocityWalk(0.0, 0.0, 0.0, 1);		
				//Logger::Instance().WriteMsg("ScanForBall",  " DONTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT", Logger::Info);	
			}
			headaction = SCANFORBALL;
			
			//Logger::Instance().WriteMsg("FollowTheball",  " SCANFORBALL", Logger::Info);
		}
	}
	bhmsg->set_headaction(headaction);
	_blk->publishSignal(*bhmsg, "behavior");
	return 0;
}

void FollowTheBall::UserInit () {
	scancompleted = false;
	first= true;
	_blk->subscribeTo("vision", 0);
	_blk->subscribeTo("sensors", 0);
	_blk->subscribeTo("behavior", 0);
	rcvObsm = boost::posix_time::microsec_clock::universal_time();
	wmot = new MotionWalkMessage();
	wmot->add_parameter(0.0f);
	wmot->add_parameter(0.0f);
	wmot->add_parameter(0.0f);
	wmot->add_parameter(0.0f);
	bhmsg = new BToHeadMessage();
	headaction= BALLTRACK;
	lastObsm = new ObservationMessage();
}

std::string FollowTheBall::GetName () {
	return "FollowTheBall";
}

void FollowTheBall::velocityWalk(double x, double y, double th, double f) {
	wmot->set_command("setWalkTargetVelocity");
	wmot->set_parameter(0, x);
	wmot->set_parameter(1, y);
	wmot->set_parameter(2, th);
	wmot->set_parameter(3, f);
	_blk->publishSignal(*wmot, "motion");
}

void FollowTheBall::littleWalk(double x, double y, double th, int s) {
	//cout << x << " y " << y << " th " << th<<endl;
	wmot->set_command("walkTo");
	wmot->set_parameter(0, x);
	wmot->set_parameter(1, y);
	wmot->set_parameter(2, th);
	_blk->publishSignal(*wmot, "motion");
}
	

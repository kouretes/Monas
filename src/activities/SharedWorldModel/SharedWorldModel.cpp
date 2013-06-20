#include "SharedWorldModel.h"
#include "core/include/Logger.hpp"
#include "tools/toString.h"
#include "hal/robot/generic_nao/robot_consts.h"


ACTIVITY_REGISTER(SharedWorldModel);
using namespace std;

void SharedWorldModel::UserInit()
{
	_blk.updateSubscription("worldstate", msgentry::SUBSCRIBE_ON_TOPIC, msgentry::HOST_ID_ANY_HOST);
	_blk.updateSubscription("communication", msgentry::SUBSCRIBE_ON_TOPIC);
	idx = -1;

    ReadFieldConf();

    testCount = 0;
    State.zero();
    I.zero();
    P.zero();
    Q.zero();
    F.zero();
    temp.zero();
    R1.zero();
    R2.zero();
    H1.zero();
    H2.zero();

	for(int i = 0; i < numOfRobots; i++)
	{
		bd[i] = INIT_VALUE;
	}
	for(int i = 0; i < dim; i++)
	{
		P.get(i,i) = 10.0;
		I.get(i,i) = 1.0;
		if(i<numOfRobots*3){
            if(i%3==0){
                Q.get(i,i) = QRdevx;
                Q.get(i+1,i+1) = QRdevy;
                Q.get(i+2,i+2) = QRdevtheta;


                State.get(i,0) = INIT_VALUE;
                State.get(i+1,0) = INIT_VALUE;
            }
		}
	}

    //testing
    //

    //end testing

    R1.get(0,0) = Q.read(0,0);
    R1.get(1,1) = Q.read(1,1);
    R1.get(2,2) = Q.read(2,2);

    R2.get(0,0) = Q.read(0,0);
    R2.get(1,1) = Q.read(1,1);
    R2.get(2,2) = Q.read(2,2);

    now = boost::posix_time::microsec_clock::universal_time();
    last_filter_time = now;
    LogEntry(LogLevel::Info, GetName()) << "Initialized" ;
}

void SharedWorldModel::Reset(){

}

int SharedWorldModel::Execute()
{
	h = _blk.readState<KnownHosts>("communication");

	for(int i = 0; i < numOfRobots; i++)
	{
		bd[i] = INIT_VALUE;
		robot_x[i] = INIT_VALUE;
		robot_y[i] = INIT_VALUE;
		ball_x[i] = INIT_VALUE;
		ball_y[i] = INIT_VALUE;
		ball_speed_x[i] = INIT_VALUE;
		ball_speed_y[i] = INIT_VALUE;
	}
    swi.Clear();
    gotObs = false;

    predict();
    count = 0;
    wim  = _blk.readData<WorldInfo> ("worldstate");

    if(wim != 0)
    {
        if(wim.get() != 0)
        {
            gather_info();
            count++;
        }
    }


	if(!h.get() || (h && h->entrylist_size() == 0)) {
		;//Logger::Instance().WriteMsg("SharedWorldModel", "No info from other robots!", Logger::Info);
	}
	else
	{
		const ::google::protobuf::RepeatedPtrField< HostEntry >& rf = h->entrylist();
		::google::protobuf::RepeatedPtrField< HostEntry >::const_iterator fit;

		for(fit = rf.begin(); fit != rf.end(); ++fit)
		{
            wim  = _blk.readData<WorldInfo> ("worldstate", (*fit).hostid());

            if(wim != 0)
            {
                if(wim.get() != 0)
                {
                    // Logger::Instance().WriteMsg("SharedWorldModel", "Host Name: " + _toString((*fit).hostname()) + " \tRobot x: " + _toString(robot_x[id]) + " Robot y: " + 							_toString(robot_y[id]), Logger::Info);
                    gather_info();
                    count++;
                }

		    }
		}
	}

	now = boost::posix_time::microsec_clock::universal_time();
    duration = now - last_ball_update_time;
    dtBall = duration.total_microseconds() / 1000000.0f;

	if(State(dim-2)>fieldMinX-ballOffset && State(dim-2)<fieldMaxX+ballOffset && State(dim-1)>fieldMinY-ballOffset && State(dim-1)<fieldMaxY+ballOffset && dtBall<3.0f){
        GlobalBall gb;
        gb.set_x(State(dim-2));
        gb.set_y(State(dim-1));

        swi.add_globalballs();
        swi.mutable_globalballs(0)->CopyFrom(gb);
        _blk.publishData(swi, "worldstate");
    }
    else if(gotObs)
        _blk.publishData(swi, "worldstate");

}

void SharedWorldModel::gather_info(){

    int i,j=0;

    if(wim->has_playernumber())
        id = wim->playernumber()-1;
    else{
        count--;
        return;
    }

    gotObs = true;

    robot_x[id] = wim->myposition().x();
    robot_y[id] = wim->myposition().y();
    robot_phi[id] = wim->myposition().phi();
    stab = wim->stability();
    RobotPose tempPose;

    tempPose = wim->myposition();

    //Read P from local filter
    if(tempPose.var_size()>0){
        for(i=0;i<3;i++){
            for(j=0;j<3;j++){
                R2(i,j) = tempPose.var(i*3+j);
                R1(i,j) = tempPose.var(i*3+j);
            }
        }
    }


    if (wim->balls_size() > 0)
    {
        ball_x[id] = wim->balls(0).relativex();// + wim->balls(0).relativexspeed() * 0.200;
        ball_y[id] = wim->balls(0).relativey();// + wim->balls(0).relativeyspeed() * 0.200;
        ball_speed_x[id] = wim->balls(0).relativexspeed();
        ball_speed_y[id] = wim->balls(0).relativeyspeed();
        bd[id] = sqrt(pow(ball_x[id], 2) + pow(ball_y[id], 2));
        update(id);
    }
    else
    {
        updateNoObs(id);
    }

    RobotPose rPose;
    rPose.set_x(State.read(id*3,0));
    rPose.set_y(State.read(id*3+1,0));
    rPose.set_phi(State.read(id*3+2,0));

    TeammatePose tPose;
    tPose.mutable_pose()->CopyFrom(rPose);
    tPose.set_robotid(id+1);
    tPose.set_stability(stab);

    swi.add_teammateposition();
    swi.mutable_teammateposition(count)->CopyFrom(tPose);
}

void SharedWorldModel::predict()
{
    now = boost::posix_time::microsec_clock::universal_time();
    duration = now - last_filter_time;
    last_filter_time = now;
    dt = duration.total_microseconds() / 1000000.0f;
    dtsqrd = dt*dt;

    for(int i = 0; i < numOfRobots*3; i++)
	{
        Q.get(i,i) = QRdevx*dtsqrd*10;
        Q.get(i+1,i+1) = QRdevy*dtsqrd*10;
        Q.get(i+2,i+2) = QRdevtheta*dtsqrd*10;
	}

    Q.get(dim-2,dim-2) = (vara*dtsqrd*dtsqrd/4.0)/100;
    Q.get(dim-1,dim-1) = (vara*dtsqrd*dtsqrd/4.0)/100;

    R2.get(3,3) = vara*dtsqrd*dtsqrd/4.0;
    R2.get(4,4) = vara*dtsqrd*dtsqrd/4.0;

    //R2.prettyPrint();

//    F.identity();
//    P = F*P*(F.transp());
    P += Q;

//    std::cout << testCount << "+dt = " << dt << "\n";
    testCount++;
    //State.prettyPrint();
}

void SharedWorldModel::update(int rid)
{
    last_ball_update_time = boost::posix_time::microsec_clock::universal_time();
    //convert ball to global
//    float bx = robot_x[rid] + ball_x[rid] * cos(robot_phi[rid]) - ball_y[rid] * sin(robot_phi[rid]);
//    float by = robot_y[rid] + ball_x[rid] * sin(robot_phi[rid]) + ball_y[rid] * cos(robot_phi[rid]);
    float phi=State(rid*3+2);
//    float bx=State(dim-2);
//    float by=State(dim-1);
//    float rx=State(rid*3);
//    float ry=State(rid*3+1);

    //std::cout << "gbx=" << globX << "\tgby=" << globY << std::endl;

    y2.get(0,0) = robot_x[rid] ;//- State.read(rid*3,0);
    y2.get(1,0) = robot_y[rid] ;//- State.read(rid*3+1,0);
    y2.get(2,0) = robot_phi[rid] ;//- State.read(rid*3+2,0);
    y2.get(3,0) = ball_x[rid] ;//- State.read(dim-4,0);
    y2.get(4,0) = ball_y[rid];// - State.read(dim-2,0);

//std::cout << "ybx=" << y2.get(3,0) << "\tyby=" << y2.get(5,0) << " phi:"<<phi<<std::endl;

    H2.zero();
    H2.get(0,rid*3) = 1;
    H2.get(1,rid*3+1) = 1;
    H2.get(2,rid*3+2) = 1;

    H2.get(3,rid*3) = -cos(phi);
    H2.get(3,rid*3+1) = -sin(phi);
//    H2.get(3,rid*3+2) = (rx-bx)*sin(phi)+(by-ry)*cos(phi);
    H2.get(3,dim-2) = cos(-phi);
    H2.get(3,dim-1) = -sin(-phi);

    H2.get(4,rid*3) = sin(phi);
    H2.get(4,rid*3+1) = -cos(phi);
//    H2.get(4,rid*3+2) = (rx-bx)*cos(phi)+(by-ry)*sin(phi);
    H2.get(4,dim-2) = sin(-phi);
    H2.get(4,dim-1) = cos(-phi);

//    H2.prettyPrint();

    y2-=H2*State;

    S2.zero();
    S2 = H2*P*(H2.transp());
    S2 += R2;
//    S2.prettyPrint();
//    y2.prettyPrint();
    try{
        S2.fast_invert();
    }
    catch (int e){
        cout << "Inversion of S failed in update:\n";
        S2.prettyPrint();
        S2.zero();
        S2.identity();
    }

    K2 = P*(H2.transp())*S2;
//    K2.prettyPrint();
    State += K2*y2;
    P -=K2*H2*P;
//    P.prettyPrint();


}

void SharedWorldModel::updateNoObs(int rid)
{
//    std::cout << "\n" << robot_x[rid] << robot_y[rid] << robot_phi[rid] << "\n";
    y1.get(0,0) = robot_x[rid] - State.read(rid*3,0);
    y1.get(1,0) = robot_y[rid] - State.read(rid*3+1,0);
    y1.get(2,0) = robot_phi[rid] - State.read(rid*3+2,0);

    H1.zero();
    H1.get(0,rid*3) = 1;
    H1.get(1,rid*3+1) = 1;
    H1.get(2,rid*3+2) = 1;

    S1 = H1*P*(H1.transp());
    S1 += R1;
    try{
        S1.fast_invert();
    }
    catch (int e){
        cout << "Inversion of S failed:\n";
        S1.prettyPrint();
        S1.zero();
        S1.identity();
    }
    K1 = P*(H1.transp())*S1;
    State += K1*y1;
    temp = I;
    temp -= K1*H1;
    P = temp*P;
}

void SharedWorldModel::ReadFieldConf()
{
    fieldMaxX=atof(Configurator::Instance().findValueForKey("field.FieldMaxX").c_str());
    fieldMinX=atof(Configurator::Instance().findValueForKey("field.FieldMinX").c_str());
    fieldMaxY=atof(Configurator::Instance().findValueForKey("field.FieldMaxY").c_str());
    fieldMinY=atof(Configurator::Instance().findValueForKey("field.FieldMinY").c_str());
}

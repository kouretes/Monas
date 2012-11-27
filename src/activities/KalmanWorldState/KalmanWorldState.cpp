#include "KalmanWorldState.h"
#include "hal/robot/generic_nao/robot_consts.h"
#include "tools/logger.h"
#include "tools/toString.h"
#include "messages/RoboCupGameControlData.h"
#include <pthread.h>
#include <netinet/in.h>
#include <csignal>
#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>
#include <math.h>
#include "architecture/archConfig.h"
#define NO_GAME
#define MAX_TIME_TO_RESET 15 //in seconds
using namespace std;


using namespace KMath;
ACTIVITY_REGISTER(KalmanWorldState);



void KalmanWorldState::UserInit()
{
	_blk.updateSubscription("vision", msgentry::SUBSCRIBE_ON_TOPIC);
	_blk.updateSubscription("sensors", msgentry::SUBSCRIBE_ON_TOPIC);
	_blk.updateSubscription("behavior", msgentry::SUBSCRIBE_ON_TOPIC);
	_blk.updateSubscription("worldstate", msgentry::SUBSCRIBE_ON_TOPIC);

	Logger::Instance().WriteMsg("KalmanWorldState", "KalmanWorldState Initialized", Logger::Info);
	firstOdometry = true;



	//MyWorld.add_balls();
	currentRobotAction = MotionStateMessage::IDLE;

	localizationWorld.Initialize();
	ReadFeatureConf();


	//localizationWorld.setParticlesPoseUniformly();
	timeStart = boost::posix_time::microsec_clock::universal_time();

	fallBegan = true;
	last_observation_time = boost::posix_time::microsec_clock::universal_time();
	last_filter_time = boost::posix_time::microsec_clock::universal_time();

	robotmovement.type = "ratio";
	robotmovement.freshData = false;
	robotmovement.Distance.ratiomean = 1.5;// systematic error out
	robotmovement.Distance.ratiodev = 0.55;
	robotmovement.Distance.Emean = 0.0;
	robotmovement.Distance.Edev = 0.0;

	robotmovement.Direction.ratiomean = 1.0;// systematic error out
	robotmovement.Direction.ratiodev = 0.0;
	robotmovement.Direction.Emean = 0.0;// systematic error out
	robotmovement.Direction.Edev = TO_RAD(22);

	robotmovement.Rotation.ratiomean = 1.0;// systematic error out
	robotmovement.Rotation.ratiodev = 0.55;
	robotmovement.Rotation.Emean = 0.0;// systematic error out
	robotmovement.Rotation.Edev = 0.0;

}

void KalmanWorldState::Reset(){
}

int KalmanWorldState::Execute()
{
	now = boost::posix_time::microsec_clock::universal_time();

	process_messages();

	if(currentRobotAction == MotionStateMessage::FALL){
		if(fallBegan == true){
			fallBegan = false;
			localizationWorld.spreadParticlesAfterFall();
		}
		timeStart = boost::posix_time::microsec_clock::universal_time();
		return 0;
	}else
		fallBegan = true;
	if (lrm != 0){//TODO diaforetiko initialization gia otan einai gia placement kai allo gia penalty
		timeStart = boost::posix_time::microsec_clock::universal_time();
		localizationWorld.initializeParticles((int)lrm->type(),lrm->kickoff());
	}

	AgentPosition = localizationWorld.LocalizationStepSIR(robotmovement, currentObservation, currentAmbiguousObservation,0.04);
	//cout<<AgentPosition.x<<" "<< AgentPosition.y << "  "<<AgentPosition.theta <<" "<<AgentPosition.confidence<<endl;
	MyWorld.mutable_myposition()->set_x(AgentPosition.x);
	MyWorld.mutable_myposition()->set_y(AgentPosition.y);
	MyWorld.mutable_myposition()->set_phi(AgentPosition.theta);
	MyWorld.mutable_myposition()->set_confidence(AgentPosition.confidence);

	calculate_ball_estimate(robotmovement);

	_blk.publishData(MyWorld, "worldstate");
	return 0;
}

void KalmanWorldState::calculate_ball_estimate(KMotionModel const & robotModel)
{
	boost::posix_time::time_duration duration;
	boost::posix_time::ptime observation_time;
	Ball nearest_filtered_ball, nearest_nofilter_ball;
	float dt;
	bool ballseen = false;
	if (obsm.get())
	{
		observation_time = boost::posix_time::from_iso_string(obsm->image_timestamp());
		//used for removing the ball if exceeds a threshold

		if (obsm->has_ball())
		{
			ballseen = true;

			BallObject aball = obsm->ball();
			nearest_nofilter_ball.set_relativex(aball.dist() * cos(aball.bearing()));
			nearest_nofilter_ball.set_relativey(aball.dist() * sin(aball.bearing()));

			if (MyWorld.balls_size() < 1)
			{
				//Inserting new ball
				MyWorld.add_balls();
				myBall.reset(aball.dist(), 0, aball.bearing(), 0);
				MyWorld.mutable_balls(0)->CopyFrom(nearest_nofilter_ball);
				last_filter_time = now;
			} else
			{
				//Get estimate ball
				//Update
				duration = observation_time - last_filter_time;
				last_filter_time = observation_time;
				dt = duration.total_microseconds() / 1000000.0f;

				float dist_var = 1.10 - tanh(1.8 / aball.dist()); //observation ... deviation ... leme twra
				myBall.get_predicted_ball_estimate(dt,robotModel);
				nearest_filtered_ball = myBall.get_updated_ball_estimate(aball.dist(), dist_var * dist_var, aball.bearing(), 0.03);

				//Predict
				duration = now - last_filter_time;
				last_filter_time = now;
				dt = duration.total_microseconds() / 1000000.0f;
				nearest_filtered_ball = myBall.get_predicted_ball_estimate(dt, robotModel);

				float dx = nearest_filtered_ball.relativex() - nearest_nofilter_ball.relativex();
				float dy = nearest_filtered_ball.relativey() - nearest_nofilter_ball.relativey();
				float distance = distance = KMath::norm2(dx,dy);

				//Check if we must reset the ball
				duration = observation_time - last_observation_time;
				last_observation_time = observation_time;
				dt = duration.total_microseconds() / 1000000.0f;

				if (dt > MAX_TIME_TO_RESET && distance > 0.5) //etc... dt > 5sec && distance > 2 m
				{
					myBall.reset(aball.dist(), 0, aball.bearing(), 0);
					last_filter_time = now;
					//RESET
					//cout << "RESETING_BALL" << endl;
					MyWorld.mutable_balls(0)->CopyFrom(nearest_nofilter_ball);
				} else
					MyWorld.mutable_balls(0)->CopyFrom(nearest_filtered_ball);
			}
		}
	}

	if (!ballseen)
	{
		duration = now - last_observation_time;
		dt = duration.total_microseconds() / 1000000.0f;
		if (dt > MAX_TIME_TO_RESET)
		{
			//time = newtime;
			last_observation_time = now; //So it wont try to delete already delete ball
			if (MyWorld.balls_size() > 0)
				MyWorld.clear_balls();
		} else
		{
			duration = now - last_filter_time;
			last_filter_time = now;
			dt = duration.total_microseconds() / 1000000.0f;
			nearest_filtered_ball = myBall.get_predicted_ball_estimate(dt,robotModel);
			if(myBall.get_filter_variance()>4 && MyWorld.balls_size() > 0) //Std = 2m
				MyWorld.clear_balls();
			if (MyWorld.balls_size() > 0)
				MyWorld.mutable_balls(0)->CopyFrom(nearest_filtered_ball);
		}
	}
}

void KalmanWorldState::process_messages()
{
	boost::posix_time::ptime observation_time;

	gsm = _blk.readState<GameStateMessage>("worldstate");
	obsm = _blk.readSignal<ObservationMessage>("vision");
	lrm = _blk.readSignal<LocalizationResetMessage>("worldstate");
	sm = _blk.readState<MotionStateMessage>("worldstate");


	currentObservation.clear();
	currentAmbiguousObservation.clear();
	/*static int k=0;
		k++;
		if(k==10)
		{

			if(rand()%2==1)
			{
				KObservationModel	tmpOM;
				tmpOM.Distance.val = 1.9+0.1*(1.0*rand()/RAND_MAX -0.5);
				tmpOM.Distance.Emean = 0;
				tmpOM.Distance.Edev = 1.5+2.0*0.1;//The deviation is 1.5 meter plus double the precision of vision
				//Bearing
				tmpOM.Bearing.val = KMath::wrapTo0_2Pi( -1.0);
				tmpOM.Bearing.Emean = 0.0;
				tmpOM.Bearing.Edev = TO_RAD(45) + 2.0*0.1;
				tmpOM.Feature = localizationWorld.KFeaturesmap["YellowRight"];
				currentAmbiguousObservation.push_back(tmpOM);
			}
			else
			{
				KObservationModel	tmpOM;
				tmpOM.Distance.val = 5.0+0.1*(1.0*rand()/RAND_MAX -0.5);
				tmpOM.Distance.Emean = 0;
				tmpOM.Distance.Edev = 1.5+2.0*0.1;//The deviation is 1.5 meter plus double the precision of vision
				//Bearing
				tmpOM.Bearing.val = KMath::wrapTo0_2Pi( 0.5);
				tmpOM.Bearing.Emean = 0.0;
				tmpOM.Bearing.Edev = TO_RAD(45) + 2.0*0.1;
				tmpOM.Feature = localizationWorld.KFeaturesmap["YellowRight"];
				currentAmbiguousObservation.push_back(tmpOM);
			}
				k=0;
		}*/
	if(gsm != 0){
		localizationWorld.playerNumber = gsm->player_number();
	}
	if (obsm != 0)
	{
		KObservationModel tmpOM;
		//Load observations
		const ::google::protobuf::RepeatedPtrField<NamedObject>& Objects = obsm->regular_objects();
		string id;




		for (int i = 0; i < Objects.size(); i++)
		{
			id = Objects.Get(i).object_name();
			//Distance
			tmpOM.Distance.val = Objects.Get(i).distance();
			tmpOM.Distance.Emean = 0.0;
			tmpOM.Distance.Edev = 1.5+2.0*Objects.Get(i).distance_dev();//The deviation is 1.5 meter plus double the precision of vision
			//Bearing
			tmpOM.Bearing.val = KMath::wrapTo0_2Pi( Objects.Get(i).bearing());
			tmpOM.Bearing.Emean = 0.0;
			tmpOM.Bearing.Edev = TO_RAD(45) + 2.0*Objects.Get(i).bearing_dev();//The deviation is 45 degrees plus double the precision of vision
			/*Logger::Instance().WriteMsg("kofi", "---------------id = "+id+"-----------------------------------------------------------------------------------------------------", Logger::Info);
			Logger::Instance().WriteMsg("kofi", "Distance: "+_toString(tmpOM.Distance.val) + " Distance Dev: " + _toString(tmpOM.Distance.Edev), Logger::Info);
			Logger::Instance().WriteMsg("kofi", "Angle: "+_toString(tmpOM.Bearing.val) + " Angle Dev: " + _toString(tmpOM.Bearing.Edev), Logger::Info);
			Logger::Instance().WriteMsg("kofi", "--------------------------------------------------------------------------------------------------------------------------", Logger::Info);*/

			if (localizationWorld.KFeaturesmap.count(id) != 0)
			{
				//Make the feature
				if(id.find("Left")!=string::npos ||id.find("Right")!=string::npos)
				{
					tmpOM.Feature = localizationWorld.KFeaturesmap[id];
					currentObservation.push_back(tmpOM);
				}
			}else {

				if( id.find("Yellow")!=string::npos){
					tmpOM.Feature = localizationWorld.KFeaturesmap["YellowLeft"];
					currentAmbiguousObservation.push_back(tmpOM);
				}

			}
		}
		observation_time = boost::posix_time::from_iso_string(obsm->image_timestamp());
		rpsm = _blk.readData<RobotPositionMessage> ("sensors", msgentry::HOST_ID_LOCAL_HOST,NULL, &observation_time);
	}else{
		rpsm = _blk.readData<RobotPositionMessage>("sensors");
	}

	if (rpsm != 0)
	{
		PosX = rpsm->sensordata(KDeviceLists::ROBOT_X);
		PosY = rpsm->sensordata(KDeviceLists::ROBOT_Y);
		Angle = rpsm->sensordata(KDeviceLists::ROBOT_ANGLE);
		robotmovement.freshData = true;
		RobotPositionMotionModel(robotmovement);
	}else{
		SensorData s;
		s.set_sensorvalue(0);
		PosX = s;// rpsm->sensordata(KDeviceLists::ROBOT_X);
		PosY = s;//rpsm->sensordata(KDeviceLists::ROBOT_Y);
		Angle = s;//-rpsm->sensordata(KDeviceLists::ROBOT_ANGLE);
		robotmovement.freshData = true;
		//robotmovement.freshData = false;
		cout<<"bug"<<endl;
	}
	if (sm != 0){
		currentRobotAction = sm->type();
	}

}


void KalmanWorldState::RobotPositionMotionModel(KMotionModel & MModel)
{
	if (firstOdometry)
	{
		TrackPointRobotPosition.x = PosX.sensorvalue();
		TrackPointRobotPosition.y = PosY.sensorvalue();
		TrackPointRobotPosition.phi = Angle.sensorvalue();
		TrackPoint = TrackPointRobotPosition;
		firstOdometry=false;
	}
	float XA = PosX.sensorvalue();
	float YA = PosY.sensorvalue();
	float AA = Angle.sensorvalue();


	float DX = (XA - TrackPointRobotPosition.x);
	float DY = (YA - TrackPointRobotPosition.y);
	float DR = KMath::anglediff2(AA, TrackPointRobotPosition.phi);

	float robot_dist = KMath::norm2(DX, DY);
	float robot_dir = KMath::anglediff2(atan2(DY, DX), TrackPointRobotPosition.phi);
	float robot_rot = DR;

	MModel.Distance.val = robot_dist;
	MModel.Direction.val = robot_dir;
	MModel.Rotation.val = robot_rot;

	TrackPointRobotPosition.x = XA;
	TrackPointRobotPosition.y = YA;
	TrackPointRobotPosition.phi = AA;
	//If u want edev change the KKalmanLocalization file
	TrackPoint.x += DX;
	TrackPoint.y += DY;
	TrackPoint.phi += DR;
	//Logger::Instance().WriteMsg("KalmanWorldState", "Ald Direction =  "+_toString(Angle.sensorvalue()) + " Robot_dir = " + _toString(robot_dir) + " Robot_rot = " + _toString(robot_rot) + " edev at dir = " + _toString(MModel.Distance.ratiodev), Logger::Info);
}

void KalmanWorldState::ReadFeatureConf()
{
    feature temp;
    double x,y,weight;
    string ID;
	for(int i = 0; i < _xml.numberOfNodesForKey("features.ftr"); i++){
		string key = "features.ftr~" + _toString(i) + ".";

    	ID=_xml.findValueForKey(key + "ID");
    	x= atof(_xml.findValueForKey(key + "x").c_str());
    	y= atof(_xml.findValueForKey(key + "y").c_str());
    	weight= atof(_xml.findValueForKey(key + "weight").c_str());
    	temp.set(x, y, ID, weight);
    	localizationWorld.KFeaturesmap[ID]=temp;
    }
}

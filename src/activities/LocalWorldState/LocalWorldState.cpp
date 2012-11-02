#include "LocalWorldState.h"
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

ACTIVITY_REGISTER(LocalWorldState);

bool LocalWorldState::debugmode = false;
TCPSocket * LocalWorldState::sock;

void LocalWorldState::UserInit()
{
	_blk.updateSubscription("vision", msgentry::SUBSCRIBE_ON_TOPIC);
	_blk.updateSubscription("sensors", msgentry::SUBSCRIBE_ON_TOPIC);
	_blk.updateSubscription("behavior", msgentry::SUBSCRIBE_ON_TOPIC);
	_blk.updateSubscription("worldstate", msgentry::SUBSCRIBE_ON_TOPIC);

	firstOdometry = true;
	serverpid = -1;
	debugmode = false;
	fallBegan = true;

	int maxBytedataSize = 100000;
	data = new char[maxBytedataSize]; //## TODO  FIX THIS BETTER

	currentRobotAction = MotionStateMessage::IDLE;

	//time variables initialization
    timeStart = boost::posix_time::microsec_clock::universal_time();
	lastObservationTime = boost::posix_time::microsec_clock::universal_time();
	lastFilterTime = boost::posix_time::microsec_clock::universal_time();

    //read xml files..set parameters for localizationWorld
    ReadLocConf();
    ReadFieldConf();
    ReadFeatureConf();
	localizationWorld.Initialize();

#ifdef NO_GAME
	sock = NULL;
	serverpid = pthread_create(&acceptthread, NULL, &LocalWorldState::StartServer, this);
	pthread_detach(acceptthread);
#endif

	robotmovement.type = "ratio";
	robotmovement.freshData = false;
	robotmovement.Distance.ratiomean = 1.0;// systematic error out
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

    Logger::Instance().WriteMsg("LocalWorldState", "LocalWorldState Initialized", Logger::Info);
}

void LocalWorldState::Reset(){
    //ReadLocConf();
    //ReadFieldConf();
    //ReadFeatureConf();
}

int LocalWorldState::Execute()
{
	now = boost::posix_time::microsec_clock::universal_time();

	ProcessMessages();

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


	AgentPosition = localizationWorld.LocalizationStepSIR(robotmovement, currentObservation, currentAmbiguousObservation);
	MyWorld.mutable_myposition()->set_x(AgentPosition.x);
	MyWorld.mutable_myposition()->set_y(AgentPosition.y);
	MyWorld.mutable_myposition()->set_phi(AgentPosition.theta);
	MyWorld.mutable_myposition()->set_confidence(0.0);

	calculate_ball_estimate(robotmovement);
	///DEBUGMODE SEND RESULTS
	if (debugmode)
	{
		LocalizationData_Load(currentObservation, robotmovement);
		Send_LocalizationData();
	}
	_blk.publishData(MyWorld, "worldstate");
#ifdef NO_GAME
	DebugDataForGUI.Clear();
	LocalizationDataForGUI_Load();
	_blk.publishSignal(DebugDataForGUI, "debug");
#endif
	return 0;
}

void LocalWorldState::calculate_ball_estimate(KMotionModel const & robotModel)
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
			nearest_nofilter_ball.set_relativexspeed(0.0);
			nearest_nofilter_ball.set_relativeyspeed(0.0);

			if (MyWorld.balls_size() < 1)
			{
				//Inserting new ball
				MyWorld.add_balls();
				myBall.reset(aball.dist(), 0, aball.bearing(), 0);
				MyWorld.mutable_balls(0)->CopyFrom(nearest_nofilter_ball);
				lastFilterTime = now;
			} else
			{
				//Get estimate ball
				//Update
				duration = observation_time - lastFilterTime;
				lastFilterTime = observation_time;
				dt = duration.total_microseconds() / 1000000.0f;

				float dist_var = 1.10 - tanh(1.8 / aball.dist()); //observation ... deviation ... leme twra
				myBall.get_predicted_ball_estimate(dt,robotModel);
				nearest_filtered_ball = myBall.get_updated_ball_estimate(aball.dist(), dist_var * dist_var, aball.bearing(), 0.03);

				//Predict
				duration = now - lastFilterTime;
				lastFilterTime = now;
				dt = duration.total_microseconds() / 1000000.0f;
				nearest_filtered_ball = myBall.get_predicted_ball_estimate(dt, robotModel);

				float dx = nearest_filtered_ball.relativex() - nearest_nofilter_ball.relativex();
				float dy = nearest_filtered_ball.relativey() - nearest_nofilter_ball.relativey();
				float distance = distance = KMath::norm2(dx,dy);

				//Check if we must reset the ball
				duration = observation_time - lastObservationTime;
				lastObservationTime = observation_time;
				dt = duration.total_microseconds() / 1000000.0f;

				if (dt > MAX_TIME_TO_RESET && distance > 0.5) //etc... dt > 5sec && distance > 2 m
				{
					myBall.reset(aball.dist(), 0, aball.bearing(), 0);
					lastFilterTime = now;
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
		duration = now - lastObservationTime;
		dt = duration.total_microseconds() / 1000000.0f;
		if (dt > MAX_TIME_TO_RESET)
		{
			//time = newtime;
			lastObservationTime = now; //So it wont try to delete already delete ball
			if (MyWorld.balls_size() > 0){
				MyWorld.clear_balls();
			}
		} else
		{
			duration = now - lastFilterTime;
			lastFilterTime = now;
			dt = duration.total_microseconds() / 1000000.0f;
			nearest_filtered_ball = myBall.get_predicted_ball_estimate(dt,robotModel);
			if(dt > 0.080 && myBall.get_filter_variance() > 4 && MyWorld.balls_size() > 0){ //Std = 2m and wait for 80 ms before deleting
				MyWorld.clear_balls();
			}
			if (MyWorld.balls_size() > 0)
				MyWorld.mutable_balls(0)->CopyFrom(nearest_filtered_ball);
		}
	}
}

void LocalWorldState::ProcessMessages()
{
	boost::posix_time::ptime observation_time;

	gsm = _blk.readState<GameStateMessage>("worldstate");
	obsm = _blk.readSignal<ObservationMessage>("vision");
	lrm = _blk.readSignal<LocalizationResetMessage>("worldstate");
	sm = _blk.readState<MotionStateMessage>("worldstate");


	currentObservation.clear();
	currentAmbiguousObservation.clear();
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
		robotmovement.freshData = false;
	}
	if (sm != 0){
		currentRobotAction = sm->type();
	}

}

void LocalWorldState::RobotPositionMotionModel(KMotionModel & MModel)
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
	//If u want edev change the klocalization file
	TrackPoint.x += DX;
	TrackPoint.y += DY;
	TrackPoint.phi += DR;
	//Logger::Instance().WriteMsg("LocalWorldState", "Ald Direction =  "+_toString(Angle.sensorvalue()) + " Robot_dir = " + _toString(robot_dir) + " Robot_rot = " + _toString(robot_rot) + " edev at dir = " + _toString(MModel.Distance.ratiodev), Logger::Info);
}

//------------------------------------------------- xml read/print functions -----------------------------------------------------


void LocalWorldState::ReadFeatureConf()
{
    feature temp;
    double x,y,weight;
    string ID;

    //YellowGoal
    ID=_xml.findValueForKey("Features.ftr~0.$ID").front();
    x= atof(_xml.findValueForKey("Features.ftr~0.$x").front().c_str());
    y= atof(_xml.findValueForKey("Features.ftr~0.$y").front().c_str());
    weight= atof(_xml.findValueForKey("Features.ftr~0.$weight").front().c_str());
    temp.set(x, y, ID, weight);
    localizationWorld.KFeaturesmap[ID]=temp;

    //YellowLeft
    ID=_xml.findValueForKey("Features.ftr~1.$ID").front();
    x= atof(_xml.findValueForKey("Features.ftr~1.$x").front().c_str());
    y= atof(_xml.findValueForKey("Features.ftr~1.$y").front().c_str());
    weight= atof(_xml.findValueForKey("Features.ftr~1.$weight").front().c_str());
    temp.set(x, y, ID, weight);
    localizationWorld.KFeaturesmap[ID]=temp;

    //YellowRight
    ID=_xml.findValueForKey("Features.ftr~2.$ID").front();
    x= atof(_xml.findValueForKey("Features.ftr~2.$x").front().c_str());
    y= atof(_xml.findValueForKey("Features.ftr~2.$y").front().c_str());
    weight= atof(_xml.findValueForKey("Features.ftr~2.$weight").front().c_str());
    temp.set(x, y, ID, weight);
    localizationWorld.KFeaturesmap[ID]=temp;
    Logger::Instance().WriteMsg("LocalWorldState", "Feature parameters loaded", Logger::Info);
}

void LocalWorldState::ReadLocConf()
{
    localizationWorld.robustmean=atoi(_xml.findValueForKey("Localizationconf.robustmean").front().c_str());
    localizationWorld.partclsNum=atoi(_xml.findValueForKey("Localizationconf.partclsNum").front().c_str());
    localizationWorld.SpreadParticlesDeviation=atof(_xml.findValueForKey("Localizationconf.SpreadParticlesDeviation").front().c_str());
    localizationWorld.rotation_deviation=atof(_xml.findValueForKey("Localizationconf.rotation_deviation").front().c_str());
    localizationWorld.PercentParticlesSpread=atoi(_xml.findValueForKey("Localizationconf.PercentParticlesSpread").front().c_str());
    localizationWorld.RotationDeviationAfterFallInDeg=atof(_xml.findValueForKey("Localizationconf.RotationDeviationAfterFallInDeg").front().c_str());
    localizationWorld.NumberOfParticlesSpreadAfterFall=atof(_xml.findValueForKey("Localizationconf.NumberOfParticlesSpreadAfterFall").front().c_str());
    Logger::Instance().WriteMsg("LocalWorldState", "Localization parameters loaded" , Logger::Info);
}

void LocalWorldState::ReadFieldConf()
{
    localizationWorld.CarpetMaxX=atof(_xml.findValueForKey("Field.CarpetMaxX").front().c_str());
    localizationWorld.CarpetMinX=atof(_xml.findValueForKey("Field.CarpetMinX").front().c_str());
    localizationWorld.CarpetMaxY=atof(_xml.findValueForKey("Field.CarpetMaxY").front().c_str());
    localizationWorld.CarpetMinY=atof(_xml.findValueForKey("Field.CarpetMinY").front().c_str());
    localizationWorld.FieldMaxX=atof(_xml.findValueForKey("Field.FieldMaxX").front().c_str());
    localizationWorld.FieldMinX=atof(_xml.findValueForKey("Field.FieldMinX").front().c_str());
    localizationWorld.FieldMaxY=atof(_xml.findValueForKey("Field.FieldMaxY").front().c_str());
    localizationWorld.FieldMinY=atof(_xml.findValueForKey("Field.FieldMinY").front().c_str());
	Logger::Instance().WriteMsg("LocalWorldState", "Field parameters loaded", Logger::Info);
}

//check if values are correct
void LocalWorldState::PrintFeatureConf()
{
    typedef map<string,feature>::const_iterator MapIterator;
    for (MapIterator iter = localizationWorld.KFeaturesmap.begin(); iter != localizationWorld.KFeaturesmap.end(); iter++)
    {
        cout << " Key: " << iter->first;
        cout << " Value x: " << iter->second.x;
        cout << " Value y: " << iter->second.y;
        cout << " Value weight: " << iter->second.weight;
        cout << " Value: " << iter->second.id << endl;
    }
}

//------------------------------------------------- Functions for the GUI-----------------------------------------------------
void LocalWorldState::Send_LocalizationData()
{
	outgoingheader.set_nextmsgbytesize(DebugData.ByteSize());
	outgoingheader.set_nextmsgname(DebugData.GetTypeName());

	int sendsize;
	int rsize = 0;
	int rs;
	//send a header
	sendsize = outgoingheader.ByteSize();
	sendsize = htonl(sendsize);

	try
	{
		sock->send(&sendsize, sizeof(uint32_t));
		sendsize = outgoingheader.ByteSize();
		outgoingheader.SerializeToArray(data, sendsize);
		while (rsize < sendsize)
		{
			rs = sock->send(data + rsize, sendsize - rsize);
			rsize += rs;
		}
		//send the image bytes
		sendsize = DebugData.ByteSize();
		std::string buf;
		DebugData.SerializePartialToString(&buf);
		sendsize = buf.length();

		rsize = 0;

		while (rsize < sendsize)
		{
			rs = sock->send((char *) buf.data() + rsize, sendsize - rsize);
			rsize += rs;
		}
	} catch (SocketException &e)
	{
		cerr << e.what() << endl;
		debugmode = false;
	}
}

int LocalWorldState::LocalizationData_Load(vector<KObservationModel>& Observation, KMotionModel & MotionModel)
{
	bool addnewptrs = false;
	//Fill the world with data!
	WorldInfo *WI = DebugData.mutable_world();

	WI->mutable_myposition()->set_x(AgentPosition.x*1000);
	WI->mutable_myposition()->set_y(AgentPosition.y*1000);
	WI->mutable_myposition()->set_phi(AgentPosition.theta);
	WI->mutable_myposition()->set_confidence(0.0);

	WI->CopyFrom(MyWorld);
	DebugData.mutable_robotposition()->set_x(TrackPoint.x*1000);
	DebugData.mutable_robotposition()->set_y(TrackPoint.y*1000);
	DebugData.mutable_robotposition()->set_phi(TrackPoint.phi);
	RobotPose prtcl;
	if ((unsigned int) DebugData.particles_size() < localizationWorld.SIRParticles.size)
		addnewptrs = true;
	for (unsigned int i = 0; i < localizationWorld.SIRParticles.size; i++)
	{
		if (addnewptrs)
			DebugData.add_particles();
		DebugData.mutable_particles(i)->set_x(localizationWorld.SIRParticles.x[i]*1000);
		DebugData.mutable_particles(i)->set_y(localizationWorld.SIRParticles.y[i]*1000);
		DebugData.mutable_particles(i)->set_phi(localizationWorld.SIRParticles.phi[i]);
		DebugData.mutable_particles(i)->set_confidence(localizationWorld.SIRParticles.Weight[i]);
	}

	if (obsm != NULL)
	{
		(DebugData.mutable_observations())->CopyFrom(*obsm);
	} else
	{
		DebugData.clear_observations();
	}
	return 1;
}

int LocalWorldState::LocalizationDataForGUI_Load()
{
	bool addnewptrs = false;

	if ((unsigned int)  DebugDataForGUI.particles_size() < localizationWorld.SIRParticles.size)
		addnewptrs = true;

	for (unsigned int i = 0; i < localizationWorld.SIRParticles.size; i++)
	{
		if (addnewptrs)
			DebugDataForGUI.add_particles();
		DebugDataForGUI.mutable_particles(i)->set_x(localizationWorld.SIRParticles.x[i]*1000);
		DebugDataForGUI.mutable_particles(i)->set_y(localizationWorld.SIRParticles.y[i]*1000);
		DebugDataForGUI.mutable_particles(i)->set_phi(localizationWorld.SIRParticles.phi[i]);
		DebugDataForGUI.mutable_particles(i)->set_confidence(localizationWorld.SIRParticles.Weight[i]);
	}

	return 1;
}

void * LocalWorldState::StartServer(void * astring)
{
	XMLConfig config(ArchConfig::Instance().GetConfigPrefix() + "/Localizationconf.xml");
	bool found = false;
	unsigned short port = 9001;
	float temp = 0;
	if (config.IsLoadedSuccessfully())
	{
		found = true;
		found &= config.QueryElement("port", temp);
	}
	if (found)
		port = temp;
	else
		Logger::Instance().WriteMsg("LocalWorldState", " No port number in : " + ArchConfig::Instance().GetConfigPrefix() + "/Localizationconf.xml", Logger::Warning);

	TCPServerSocket servSock(port);

	Logger::Instance().WriteMsg("LocalWorldState", " LocalWorldState server is ready at port: " + _toString(port), Logger::Info);
	while (true)
	{
		if (!debugmode)
		{
			if (sock != NULL)
				delete sock;

			if ((sock = servSock.accept()) < 0)
			{
				cout << " REturned null";
				return NULL;
			}
			cout << "Handling client ";
			try
			{
				cout << sock->getForeignAddress() << ":";
			} catch (SocketException e)
			{
				cerr << "Unable to get foreign address" << endl;
			}
			try
			{
				cout << sock->getForeignPort();
			} catch (SocketException e)
			{
				cerr << "Unable to get foreign port" << endl;
			}
			cout << endl;
			debugmode = true;

		} else
		{
			sleep(2);
		}
	}

	return NULL;
}

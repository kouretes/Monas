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

    actionKick=false;
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
    Reset();
    ReadFieldConf();
    ReadFeatureConf();
    ReadTeamConf();
    ReadRobotConf();
	localizationWorld.Initialize();
	
	sock = NULL;
	serverpid = pthread_create(&acceptthread, NULL, &LocalWorldState::StartServer, this);
	pthread_detach(acceptthread);

	robotmovement.type = "ratio";
	robotmovement.freshData = false;

    Logger::Instance().WriteMsg("LocalWorldState", "LocalWorldState Initialized", Logger::Info);
}

void LocalWorldState::Reset(){
    ReadLocConf();
	gameMode = atoi(_xml.findValueForKey("teamConfig.game_mode").c_str()) == 1 ? true : false;
}

int LocalWorldState::Execute()
{
	now = boost::posix_time::microsec_clock::universal_time();

	ProcessMessages();

	if(currentRobotAction == MotionStateMessage::FALL){
		if(fallBegan == true){
			fallBegan = false;
			localizationWorld.SpreadParticlesAfterFall();
		}
		timeStart = boost::posix_time::microsec_clock::universal_time();
		return 0;
	}else
		fallBegan = true;
	if (lrm != 0){
		timeStart = boost::posix_time::microsec_clock::universal_time();
		localizationWorld.InitializeParticles((int)lrm->type(), lrm->kickoff(), lrm->xpos(), lrm->ypos(), lrm->phipos());
	}

	AgentPosition = localizationWorld.LocalizationStepSIR(robotmovement, currentObservation, currentAmbiguousObservation);
	MyWorld.mutable_myposition()->set_x(AgentPosition.x);
	MyWorld.mutable_myposition()->set_y(AgentPosition.y);
	MyWorld.mutable_myposition()->set_phi(AgentPosition.phi);
	MyWorld.mutable_myposition()->set_confidence(0.0);

	calculate_ball_estimate(robotmovement);
	_blk.publishData(MyWorld, "worldstate");
	if(gameMode == false){
		LocalizationDataForGUI_Load();
		_blk.publishSignal(DebugDataForGUI, "debug");
		///DEBUGMODE SEND RESULTS
		if (debugmode)
		{
			LocalizationData_Load(currentObservation, robotmovement);
			Send_LocalizationData();
		}
	}

	return 0;
}

void LocalWorldState::calculate_ball_estimate(KLocalization::KMotionModel const & robotModel)
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
		KLocalization::KObservationModel tmpOM;
		observation_time = boost::posix_time::from_iso_string(obsm->image_timestamp());

		//Load observations
		const ::google::protobuf::RepeatedPtrField<NamedObject>& Objects = obsm->regular_objects();
		string id;

		for (int i = 0; i < Objects.size(); i++)
		{
			id = Objects.Get(i).object_name();
			//Distance
			tmpOM.Distance.val = Objects.Get(i).distance();
			tmpOM.Distance.Emean = 0.0;
			tmpOM.Distance.Edev = 1.5 + 2.0*Objects.Get(i).distance_dev();//The deviation is 1.5 meter plus float the precision of vision
			//Bearing
			tmpOM.Bearing.val = KMath::wrapTo0_2Pi( Objects.Get(i).bearing());
			tmpOM.Bearing.Emean = 0.0;
			tmpOM.Bearing.Edev = TO_RAD(20) + 2.0*Objects.Get(i).bearing_dev();//The deviation is 45 degrees plus float the precision of vision
			tmpOM.observationTime=observation_time;

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

void LocalWorldState::RobotPositionMotionModel(KLocalization::KMotionModel & MModel)
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

    //robot orientation change because of action (kick)
    localizationWorld.actionOdError=0.f;
    if (sm != 0){ 
        if (currentRobotAction==MotionStateMessage::ACTION && actionKick==false){
            actionKick=true;
            localizationWorld.actionOdError=atof(_xml.findValueForKey(_xml.keyOfNodeForSubvalue("actionOdometry.action",".name",sm->detail())+".phi").c_str());   
             }
        else if (currentRobotAction!=MotionStateMessage::ACTION){
            actionKick=false;           
        }        
    }

	//Logger::Instance().WriteMsg("LocalWorldState", "Ald Direction =  "+_toString(Angle.sensorvalue()) + " Robot_dir = " + _toString(robot_dir) + " Robot_rot = " + _toString(robot_rot) + " edev at dir = " + _toString(MModel.Distance.ratiodev), Logger::Info);
}

//------------------------------------------------- xml read functions -----------------------------------------------------


void LocalWorldState::ReadFeatureConf()
{
    KLocalization::feature temp;
    float x,y,weight;
    string ID;
	for(int i = 0; i < _xml.numberOfNodesForKey("features.ftr"); i++){
		string key = "features.ftr~" + _toString(i) + ".";

    	ID=_xml.findValueForKey(key + "ID");
    	x= atof(_xml.findValueForKey(key + "x").c_str());
    	y= atof(_xml.findValueForKey(key + "y").c_str());
    	weight= atof(_xml.findValueForKey(key + "weight").c_str());
    	temp.set(x, y, ID, weight);
    	localizationWorld.KFeaturesmap[ID] = temp;
    }
}

void LocalWorldState::ReadLocConf()
{
    localizationWorld.partclsNum=atoi(_xml.findValueForKey("localizationConfig.partclsNum").c_str());
    localizationWorld.spreadParticlesDeviation=atof(_xml.findValueForKey("localizationConfig.SpreadParticlesDeviation").c_str());
    localizationWorld.rotationDeviation=atof(_xml.findValueForKey("localizationConfig.rotation_deviation").c_str());
    localizationWorld.percentParticlesSpread=atoi(_xml.findValueForKey("localizationConfig.PercentParticlesSpread").c_str());
    localizationWorld.rotationDeviationAfterFallInDeg=atof(_xml.findValueForKey("localizationConfig.RotationDeviationAfterFallInDeg").c_str());
    localizationWorld.numberOfParticlesSpreadAfterFall=atof(_xml.findValueForKey("localizationConfig.NumberOfParticlesSpreadAfterFall").c_str());

    //Odometry motion model parameters
    robotmovement.Distance.ratiomean = atof(_xml.findValueForKey("localizationConfig.OdometryModel.Distance.ratiomean").c_str());
	robotmovement.Distance.ratiodev = atof(_xml.findValueForKey("localizationConfig.OdometryModel.Distance.ratiodev").c_str());
	robotmovement.Distance.Emean = atof(_xml.findValueForKey("localizationConfig.OdometryModel.Distance.Emean").c_str());
	robotmovement.Distance.Edev = atof(_xml.findValueForKey("localizationConfig.OdometryModel.Distance.Edev").c_str());

	robotmovement.Direction.ratiomean = atof(_xml.findValueForKey("localizationConfig.OdometryModel.Direction.ratiomean").c_str());
	robotmovement.Direction.ratiodev = atof(_xml.findValueForKey("localizationConfig.OdometryModel.Direction.ratiodev").c_str());
	robotmovement.Direction.Emean = atof(_xml.findValueForKey("localizationConfig.OdometryModel.Direction.Emean").c_str());
	robotmovement.Direction.Edev =atof(_xml.findValueForKey("localizationConfig.OdometryModel.Direction.Edev").c_str());

	robotmovement.Rotation.ratiomean = atof(_xml.findValueForKey("localizationConfig.OdometryModel.Rotation.ratiomean").c_str());
	robotmovement.Rotation.ratiodev = atof(_xml.findValueForKey("localizationConfig.OdometryModel.Rotation.ratiodev").c_str());
	robotmovement.Rotation.Emean = atof(_xml.findValueForKey("localizationConfig.OdometryModel.Rotation.Emean").c_str());
	robotmovement.Rotation.Edev = atof(_xml.findValueForKey("localizationConfig.OdometryModel.Rotation.Edev").c_str());

    //Sensor resetting
    localizationWorld.augMCL.aslow = atof(_xml.findValueForKey("localizationConfig.Resetting.aslow").c_str());
    localizationWorld.augMCL.afast = atof(_xml.findValueForKey("localizationConfig.Resetting.afast").c_str());
    localizationWorld.augMCL.winDuration = atof(_xml.findValueForKey("localizationConfig.Resetting.winDuration").c_str());
    localizationWorld.augMCL.enable = atof(_xml.findValueForKey("localizationConfig.Resetting.enable").c_str());
}

void LocalWorldState::ReadFieldConf()
{
    localizationWorld.fieldMaxX=atof(_xml.findValueForKey("field.FieldMaxX").c_str());
    localizationWorld.fieldMinX=atof(_xml.findValueForKey("field.FieldMinX").c_str());
    localizationWorld.fieldMaxY=atof(_xml.findValueForKey("field.FieldMaxY").c_str());
    localizationWorld.fieldMinY=atof(_xml.findValueForKey("field.FieldMinY").c_str());
}

void LocalWorldState::ReadTeamConf()
{
    localizationWorld.playerNumber=atoi(_xml.findValueForKey("teamConfig.player").c_str());
}

void LocalWorldState::ReadRobotConf()
{
    //Xml index starts at 0 
    int pNumber=localizationWorld.playerNumber-1;
	for (int i = 0; i < 2; i++)
	{
		string kickoff = (i == 0) ? "KickOff" : "noKickOff";	//KICKOFF==0, NOKICKOFF == 1
        localizationWorld.initX[i]=atof(_xml.findValueForKey("playerConfig."+kickoff+".player~"+_toString(pNumber)+".x").c_str());
        localizationWorld.initY[i]=atof(_xml.findValueForKey("playerConfig."+kickoff+".player~"+_toString(pNumber)+".y").c_str());
        localizationWorld.initPhi[i]=TO_RAD(atof(_xml.findValueForKey("playerConfig."+kickoff+".player~"+_toString(pNumber)+".phi").c_str()));
    }

    //read ready state positions
    localizationWorld.readyX=atof(_xml.findValueForKey(
_xml.keyOfNodeForSubvalue("playerConfig.Ready.player",".number",_toString(pNumber))+".x").c_str());
    localizationWorld.readyY=atof(_xml.findValueForKey(
_xml.keyOfNodeForSubvalue("playerConfig.Ready.player",".number",_toString(pNumber))+".y").c_str());
    localizationWorld.readyPhi=atof(_xml.findValueForKey(
_xml.keyOfNodeForSubvalue("playerConfig.Ready.player",".number",_toString(pNumber))+".phi").c_str());
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

int LocalWorldState::LocalizationData_Load(vector<KLocalization::KObservationModel>& Observation,KLocalization::KMotionModel & MotionModel)
{
	bool addnewptrs = false;
	//Fill the world with data!
	WorldInfo *WI = DebugData.mutable_world();

	WI->mutable_myposition()->set_x(AgentPosition.x*1000);
	WI->mutable_myposition()->set_y(AgentPosition.y*1000);
	WI->mutable_myposition()->set_phi(AgentPosition.phi);
	WI->mutable_myposition()->set_confidence(0.0);

	WI->CopyFrom(MyWorld);
	DebugData.mutable_robotposition()->set_x(TrackPoint.x*1000);
	DebugData.mutable_robotposition()->set_y(TrackPoint.y*1000);
	DebugData.mutable_robotposition()->set_phi(TrackPoint.phi);
	RobotPose prtcl;
	if (DebugData.particles_size() < localizationWorld.SIRParticles.size)
		addnewptrs = true;
	for (int i = 0; i < localizationWorld.SIRParticles.size; i++)
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
	for (int i = 0; i < localizationWorld.SIRParticles.size; i++)
	{
		if(DebugDataForGUI.particles_size() < (int)(i+1))
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
	XMLConfig config(ArchConfig::Instance().GetConfigPrefix() + "/localizationConfig.xml");
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
		Logger::Instance().WriteMsg("LocalWorldState", " No port number in : " + ArchConfig::Instance().GetConfigPrefix() + "/localizationConfig.xml", Logger::Warning);

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

void LocalWorldState::InputOutputLogger(){
	boost::posix_time::ptime currentExecute = boost::posix_time::microsec_clock::universal_time();
	
	string YellowLeft = "", YellowRight = "", Yellow = "", RobotPosition = "", RobotMovement = "";
	
	if(!robotmovement.freshData){
		RobotMovement = "0 0 0 0";
	}else{
		RobotMovement = "1 " + _toString(robotmovement.Distance.val) + " " + _toString(robotmovement.Direction.val) + " " + _toString(robotmovement.Rotation.val);
	}
	
	if(currentAmbiguousObservation.size() == 0){
		Yellow = "0 0 0 0 0 0 0 0";
	}else{
		Yellow = "1 " + _toString(currentAmbiguousObservation[0].Feature.x) + " " + _toString(currentAmbiguousObservation[0].Feature.y) + " " +
				 _toString(currentAmbiguousObservation[0].Distance.Emean) + " " + _toString(currentAmbiguousObservation[0].Distance.Edev) + " " +
				 _toString(currentAmbiguousObservation[0].Distance.val) + " " + _toString(currentAmbiguousObservation[0].Bearing.Edev) + " " +
				 _toString(currentAmbiguousObservation[0].Bearing.val);
	}
	
	YellowLeft = "0 0 0 0 0 0 0 0";
	YellowRight = "0 0 0 0 0 0 0 0";
	for (unsigned int i = 0; i < currentObservation.size(); i++)
	{
		string temp;
		temp = "1 " + _toString(currentObservation[i].Feature.x) + " " + _toString(currentObservation[i].Feature.y) + " " +
				 _toString(currentObservation[i].Distance.Emean) + " " + _toString(currentObservation[i].Distance.Edev) + " " +
				 _toString(currentObservation[i].Distance.val) + " " + _toString(currentObservation[i].Bearing.Edev) + " " +
				 _toString(currentObservation[i].Bearing.val);
		if(currentObservation[i].Feature.id.find("Left")!=string::npos)
			YellowLeft = temp;
		else
			YellowRight = temp;
	}
	RobotPosition = _toString(AgentPosition.x) + " " + _toString(AgentPosition.y) + " " + _toString(AgentPosition.phi);

	Logger::Instance().WriteMsg("LocalWorldStateLogger", _toString(currentExecute) + " " + RobotMovement + " " + Yellow + " " + YellowLeft + " " + YellowRight + " " + RobotPosition, Logger::ExtraExtraInfo);
}

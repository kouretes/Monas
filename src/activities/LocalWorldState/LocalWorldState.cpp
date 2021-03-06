#include "LocalWorldState.h"
#include "core/architecture/time/TimeTypes.hpp"
#include "hal/robot/nao/generic_nao/robot_consts.h"
#include "core/include/Logger.hpp"
#include "messages/RoboCupGameControlData.h"
#include "messages/RoboCupGameControlData.h"

#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>
#include <math.h>

#include "tools/toString.h"
#define NO_GAME
#define MAX_TIME_TO_RESET 3 //in seconds

using namespace std;
using namespace KMath;
using namespace KSystem::Time;

ACTIVITY_REGISTER(LocalWorldState);

void LocalWorldState::UserInit()
{
	_blk.updateSubscription("vision", Messaging::MessageEntry::SUBSCRIBE_ON_TOPIC);
	_blk.updateSubscription("sensors", Messaging::MessageEntry::SUBSCRIBE_ON_TOPIC);
	_blk.updateSubscription("behavior", Messaging::MessageEntry::SUBSCRIBE_ON_TOPIC);
	_blk.updateSubscription("worldstate", Messaging::MessageEntry::SUBSCRIBE_ON_TOPIC);

    actionKick=false;
	firstOdometry = true;

	fallBegan = true;
	gameState = PLAYER_INITIAL;

	stability = 0;
	currentRobotAction = MotionStateMessage::IDLE;

	//time variables initialization
    timeStart = KSystem::Time::SystemTime::now();
	lastObservationTime = KSystem::Time::SystemTime::now();
	lastFilterTime = KSystem::Time::SystemTime::now();
    odometryMessageTime = KSystem::Time::SystemTime::now();
    debugMessageTime = KSystem::Time::SystemTime::now();
    gamePlaying = KSystem::Time::SystemTime::now();
    //read xml files..set parameters for localizationWorld
    Reset();
    ReadFieldConf();
    ReadFeatureConf();
    ReadTeamConf();
    ReadRobotConf();

    ekfLocalization.locConfig = &locConfig;
    localizationWorld.locConfig = &locConfig;

    if (locConfig.ekfEnable == true){
        ekfLocalization.Initialize();

        for (int i = 0 ; i < 9 ; i++ ){
            MyWorld.mutable_myposition()->add_var(i);
        }
    }
    else{
        localizationWorld.Initialize();
    }

	robotmovement.type = "ratio";
	robotmovement.freshData = false;
	LogEntry(LogLevel::Info,GetName())<< "Initialized" ;
}

void LocalWorldState::Reset(){
    ReadLocConf();
	gameMode = atoi(Configurator::Instance().findValueForKey("teamConfig.game_mode").c_str()) == 1 ? true : false;
}

int LocalWorldState::Execute()
{
	now = KSystem::Time::SystemTime::now();

	ProcessMessages();

	if(currentRobotAction == MotionStateMessage::FALL){
		if(fallBegan == true){
			fallBegan = false;
			stability++;
            if (locConfig.ekfEnable == true){
			    ekfLocalization.IncreaseUncertaintyAfterFall();
            }
            else{
			    localizationWorld.SpreadParticlesAfterFall();
            }
		}
		timeStart = KSystem::Time::SystemTime::now();

        MyWorld.mutable_myposition()->set_x(AgentPosition.x);
	    MyWorld.mutable_myposition()->set_y(AgentPosition.y);
	    MyWorld.mutable_myposition()->set_phi(AgentPosition.phi);

        for (int i=0;i<3;i++){
            for (int j=0;j<3;j++){
                MyWorld.mutable_myposition()->set_var(i*3+j,ekfLocalization.var(i,j));
            }
        }

	    _blk.publishData(MyWorld, "external");
	    _blk.publishData(MyWorld, "worldstate");

        return 0;
	}else
		fallBegan = true;

	if (lrm != 0){
		timeStart = KSystem::Time::SystemTime::now();

        if (locConfig.ekfEnable == true){
            ekfLocalization.InitializeHypothesis((int)lrm->type(), lrm->kickoff(), lrm->xpos(), lrm->ypos(), lrm->phipos());
            TrackPoint.x = ekfLocalization.kalmanModels[0].state(0,0);
            TrackPoint.y = ekfLocalization.kalmanModels[0].state(1,0);
            TrackPoint.phi = ekfLocalization.kalmanModels[0].state(2,0);
            _blk.publishSignal(odometryInfoM, "debug");
        }
        else{
            localizationWorld.InitializeParticles((int)lrm->type(), lrm->kickoff(), lrm->xpos(), lrm->ypos(), lrm->phipos());
            TrackPoint.x = localizationWorld.agentPosition.x;
            TrackPoint.y = localizationWorld.agentPosition.y;
            TrackPoint.phi = localizationWorld.agentPosition.phi;
            _blk.publishSignal(odometryInfoM, "debug");
        }
    }

    if (gameState == PLAYER_PLAYING && (prevGameState == PLAYER_PENALISED || prevGameState == PLAYER_SET ))
         gamePlaying =KSystem::Time::SystemTime::now();

    if (gameState == PLAYER_PENALISED && prevGameState == PLAYER_PLAYING)
         stability++;

    if (gameState !=  PLAYER_SET ) {
        if (locConfig.ekfEnable == true){
            AgentPosition = ekfLocalization.LocalizationStep(robotmovement, currentObservation, currentAmbiguousObservation);

            for (int i=0;i<3;i++){
                 for (int j=0;j<3;j++){
                    MyWorld.mutable_myposition()->set_var(i*3+j,ekfLocalization.var(i,j));
                }
            }
        }
        else{
            AgentPosition = localizationWorld.LocalizationStepSIR(robotmovement, currentObservation, currentAmbiguousObservation);
        }
    }

    MyWorld.mutable_myposition()->set_x(AgentPosition.x);
	MyWorld.mutable_myposition()->set_y(AgentPosition.y);
	MyWorld.mutable_myposition()->set_phi(AgentPosition.phi);


    _blk.publishData(MyWorld, "external");

	MyWorld.set_stability(stability);
    robotmovement.Rotation.val = ekfLocalization.kalmanModels[0].state(5,0)* robotmovement.Rotation.val + ekfLocalization.kalmanModels[0].state(4,0) * robotmovement.Distance.val;
    calculateBallEstimate(robotmovement);
	_blk.publishData(MyWorld, "worldstate");

	if(gameMode == false){
        if (locConfig.ekfEnable == true){

            if ((KSystem::Time::SystemTime::now() > debugMessageTime + KSystem::Time::TimeAbsolute::seconds(4)) || lrm != 0){
                for (int i = 0; i < ekfLocalization.numberOfModels - 1 ; i++)
	            {
		            if(ekfMHypothesis.kmodel_size() < (int)(i+1))
			            ekfMHypothesis.add_kmodel();

		            ekfMHypothesis.mutable_kmodel(i)->set_x(ekfLocalization.kalmanModels[i+1].state(0,0));
		            ekfMHypothesis.mutable_kmodel(i)->set_y(ekfLocalization.kalmanModels[i+1].state(1,0));
		            ekfMHypothesis.mutable_kmodel(i)->set_phi(ekfLocalization.kalmanModels[i+1].state(2,0));

	            }
                ekfMHypothesis.set_size(ekfLocalization.numberOfModels-1);
                _blk.publishSignal(ekfMHypothesis, "debug");
                debugMessageTime = KSystem::Time::SystemTime::now();


            }
        }
        else{
		    LocalizationDataForGUILoad();
		    _blk.publishSignal(DebugDataForGUI, "debug");
        }
	}
	return 0;
}

void LocalWorldState::calculateBallEstimate(Localization::KMotionModel const & robotModel)
{
	KSystem::Time::TimeDuration duration;
	KSystem::Time::TimeAbsolute observationTime;

	//float dt;
    float dx,dy,gx,gy;
	bool ballseen = false;

    //cout << " Ball position estimation " << endl;
	if (obsm.get() && obsm->has_ball())
	{
        //cout << " Ball observation " << endl;
	    //used for removing the ball if exceeds a threshold
		BallObject aball = obsm->ball();

        dx = aball.dist() * cos(aball.bearing());
        dy = aball.dist() * sin(aball.bearing());

        gx = AgentPosition.x + dx * cos(AgentPosition.phi) + dy * sin(AgentPosition.phi);
        gy = AgentPosition.y - dx * sin(AgentPosition.phi) + dy * cos(AgentPosition.phi);

        if ( fabs(gx) < locConfig.fieldMaxX + 2 && fabs(gy) < locConfig.fieldMaxY + 2) {
		    ballseen = true;
            observationTime = now;
		    if (MyWorld.balls_size() < 1)
		    {
                //cout << " Inserting new ball " << endl;
			    MyWorld.add_balls();
			    myBall.reset(aball.dist(), 0, aball.bearing(), 0);
                ballTimeReset = 1;
			    lastObservationTime = now;
		    } else
		    {

                if (ballTimeReset < MAX_TIME_TO_RESET)
                    ballTimeReset+= 0.5;

                //Predict
                //cout << " Ball exists.. Predict + update " << endl;
			    duration = observationTime - lastObservationTime;
			    lastObservationTime = now;

			    if (duration > KSystem::Time::TimeAbsolute::seconds(MAX_TIME_TO_RESET))
			    {
				    myBall.reset(aball.dist(), 0, aball.bearing(), 0);
			    }
                else{
                    duration = observationTime - lastFilterTime;
			       // dt = duration.total_microseconds() / 1000000.0f;
			        myBall.update(aball.dist(), 0.25 , aball.bearing(), 0.03);
                    myBall.predict(duration.toFloat(),robotModel);
                }
		    }

            lastFilterTime = now;
            //cout << " Sending ball position and velocity " << endl;
            //Create message
            MyWorld.mutable_balls(0)->set_relativex(myBall.state(0,0));
            MyWorld.mutable_balls(0)->set_relativey(myBall.state(1,0));
        	MyWorld.mutable_balls(0)->set_relativexspeed(myBall.state(2,0));
            MyWorld.mutable_balls(0)->set_relativeyspeed(myBall.state(3,0));
        }

	}


	if (!ballseen)
	{
        //cout << " ball is not being seen " << endl;
		duration = now - lastObservationTime;
		//dt = duration.total_microseconds() / 1000000.0f;

		if (duration.toFloat() > ballTimeReset)
		{
			if (MyWorld.balls_size() > 0){
                ballTimeReset = 0;
                //cout << " Reseting ball " << endl;
                myBall.reset(0, 0, 0, 0);
				MyWorld.clear_balls();
			}
		} else
		{
            duration = now - lastFilterTime;
		    //dt = duration.total_microseconds() / 1000000.0f;

            if (MyWorld.balls_size() > 0){
                //cout << " ball is not being seen .. predict movement" << endl;
	            myBall.predict(duration.toFloat(),robotModel);
                MyWorld.mutable_balls(0)->set_relativex(myBall.state(0,0));
                MyWorld.mutable_balls(0)->set_relativey(myBall.state(1,0));
            	MyWorld.mutable_balls(0)->set_relativexspeed(myBall.state(2,0));
                MyWorld.mutable_balls(0)->set_relativeyspeed(myBall.state(3,0));
            }
		}

        lastFilterTime = now;
	}

    //myBall.state.prettyPrint();
}

void LocalWorldState::ProcessMessages()
{
	KSystem::Time::TimeAbsolute observation_time;

	gsm = _blk.readState<GameStateMessage>("worldstate");
	obsm = _blk.readSignal<ObservationMessage>("vision");
	lrm = _blk.readSignal<LocalizationResetMessage>("worldstate");
	sm = _blk.readState<MotionStateMessage>("worldstate");



	currentObservation.clear();
	currentAmbiguousObservation.clear();
	if(gsm != 0){
		locConfig.playerNumber = gsm->player_number();
		MyWorld.set_playernumber(gsm->player_number());
        prevGameState = gameState;
		gameState = gsm->player_state();
	}
	if (obsm != 0)
	{
		Localization::KObservationModel tmpOM;
		observation_time.fromString(obsm->image_timestamp());

		//Load observations
		const ::google::protobuf::RepeatedPtrField<NamedObject>& Objects = obsm->regular_objects();
		string id;

		for (int i = 0; i < Objects.size(); i++)
		{
			id = Objects.Get(i).object_name();
			//Distance
			tmpOM.Distance.val = Objects.Get(i).distance();
			tmpOM.Distance.Emean = 0.0;
			tmpOM.Distance.Edev = 2 + 2.0*Objects.Get(i).distance_dev();//The deviation is 1.5 meter plus float the precision of vision
			//Bearing
			tmpOM.Bearing.val = KMath::wrapTo0_2Pi( Objects.Get(i).bearing());
			tmpOM.Bearing.Emean = 0.0;
			tmpOM.Bearing.Edev = TO_RAD(40) + 2.0*Objects.Get(i).bearing_dev();//The deviation is 45 degrees plus float the precision of vision
			tmpOM.observationTime=observation_time;

			if (locConfig.KFeaturesmap.count(id) != 0)
			{
				//Make the feature
				if(id.find("Left")!=string::npos ||id.find("Right")!=string::npos)
				{
					tmpOM.Feature = locConfig.KFeaturesmap[id];
					currentObservation.push_back(tmpOM);
				}
			}else {

				if( id.find("Yellow")!=string::npos){
					tmpOM.Feature = locConfig.KFeaturesmap["YellowLeft"];
					currentAmbiguousObservation.push_back(tmpOM);
				}
			}
		}

		rpsm = _blk.readData<RobotPositionMessage> ("sensors", Messaging::MessageEntry::HOST_ID_LOCAL_HOST,NULL, &observation_time);
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

void LocalWorldState::RobotPositionMotionModel(Localization::KMotionModel & MModel)
{
	if (firstOdometry)
	{
		TrackPointRobotPosition.x = PosX.sensorvalue();
		TrackPointRobotPosition.y = PosY.sensorvalue();
		TrackPointRobotPosition.phi = Angle.sensorvalue();
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
    TrackPoint.x += cos(TrackPoint.phi + robot_dir) * robot_dist;
	TrackPoint.y += sin(TrackPoint.phi + robot_dir) * robot_dist;
	TrackPoint.phi += DR;

    if (KSystem::Time::SystemTime::now() > odometryMessageTime +KSystem::Time::TimeAbsolute::seconds(5) ){
        odometryInfoM.set_trackpointx(TrackPoint.x);
        odometryInfoM.set_trackpointy(TrackPoint.y);
        odometryInfoM.set_trackpointphi(TrackPoint.phi);
        _blk.publishSignal(odometryInfoM, "debug");
        odometryMessageTime = KSystem::Time::SystemTime::now();
    }

    //robot orientation change because of action (kick)
    localizationWorld.actionOdError=0.f;
    if (sm != 0){
        if (currentRobotAction==MotionStateMessage::ACTION && actionKick==false){
            actionKick=true;
            localizationWorld.actionOdError=TO_RAD(atof(Configurator::Instance().findValueForKey(Configurator::Instance().keyOfNodeForSubvalue("actionOdometry.action",".name",sm->detail())+".phi").c_str()));
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
    Localization::feature temp;
    float x,y,weight;
    string ID;
	for(int i = 0; i < Configurator::Instance().numberOfNodesForKey("features.ftr"); i++){
		string key = "features.ftr~" + _toString(i) + ".";

    	ID=Configurator::Instance().findValueForKey(key + "ID");
    	x= atof(Configurator::Instance().findValueForKey(key + "x").c_str());
    	y= atof(Configurator::Instance().findValueForKey(key + "y").c_str());
    	weight= atof(Configurator::Instance().findValueForKey(key + "weight").c_str());
    	temp.set(x, y, ID, weight);
    	locConfig.KFeaturesmap[ID] = temp;
    }
}

void LocalWorldState::ReadLocConf()
{
    locConfig.partclsNum=atoi(Configurator::Instance().findValueForKey("localizationConfig.partclsNum").c_str());
    locConfig.spreadParticlesDeviation=atof(Configurator::Instance().findValueForKey("localizationConfig.SpreadParticlesDeviation").c_str());
    locConfig.rotationDeviation=atof(Configurator::Instance().findValueForKey("localizationConfig.rotation_deviation").c_str());
    locConfig.percentParticlesSpread=atoi(Configurator::Instance().findValueForKey("localizationConfig.PercentParticlesSpread").c_str());
    locConfig.spreadParticlesDeviationAfterFall=atoi(Configurator::Instance().findValueForKey("localizationConfig.SpreadParticlesDeviationAfterFall").c_str());
    locConfig.rotationDeviationAfterFallInDeg=atof(Configurator::Instance().findValueForKey("localizationConfig.RotationDeviationAfterFallInDeg").c_str());
    locConfig.numberOfParticlesSpreadAfterFall=atof(Configurator::Instance().findValueForKey("localizationConfig.NumberOfParticlesSpreadAfterFall").c_str());

    locConfig.ekfEnable = atof(Configurator::Instance().findValueForKey("localizationConfig.EKFEnable").c_str());

    //Odometry motion model parameters
    robotmovement.Distance.ratiomean = atof(Configurator::Instance().findValueForKey("localizationConfig.OdometryModel.Distance.ratiomean").c_str());
	robotmovement.Distance.ratiodev = atof(Configurator::Instance().findValueForKey("localizationConfig.OdometryModel.Distance.ratiodev").c_str());
	robotmovement.Distance.Emean = atof(Configurator::Instance().findValueForKey("localizationConfig.OdometryModel.Distance.Emean").c_str());
	robotmovement.Distance.Edev = atof(Configurator::Instance().findValueForKey("localizationConfig.OdometryModel.Distance.Edev").c_str());

	robotmovement.Direction.ratiomean = atof(Configurator::Instance().findValueForKey("localizationConfig.OdometryModel.Direction.ratiomean").c_str());
	robotmovement.Direction.ratiodev = atof(Configurator::Instance().findValueForKey("localizationConfig.OdometryModel.Direction.ratiodev").c_str());
	robotmovement.Direction.Emean = atof(Configurator::Instance().findValueForKey("localizationConfig.OdometryModel.Direction.Emean").c_str());
	robotmovement.Direction.Edev =atof(Configurator::Instance().findValueForKey("localizationConfig.OdometryModel.Direction.Edev").c_str());

	robotmovement.Rotation.ratiomean = atof(Configurator::Instance().findValueForKey("localizationConfig.OdometryModel.Rotation.ratiomean").c_str());
	robotmovement.Rotation.ratiodev = atof(Configurator::Instance().findValueForKey("localizationConfig.OdometryModel.Rotation.ratiodev").c_str());
	robotmovement.Rotation.Emean = atof(Configurator::Instance().findValueForKey("localizationConfig.OdometryModel.Rotation.Emean").c_str());
	robotmovement.Rotation.Edev = atof(Configurator::Instance().findValueForKey("localizationConfig.OdometryModel.Rotation.Edev").c_str());

    //Sensor resetting
    localizationWorld.augMCL.aslow = atof(Configurator::Instance().findValueForKey("localizationConfig.Resetting.aslow").c_str());
    localizationWorld.augMCL.afast = atof(Configurator::Instance().findValueForKey("localizationConfig.Resetting.afast").c_str());
    localizationWorld.augMCL.winDuration = atof(Configurator::Instance().findValueForKey("localizationConfig.Resetting.winDuration").c_str());
    localizationWorld.augMCL.enable = atof(Configurator::Instance().findValueForKey("localizationConfig.Resetting.enable").c_str());


}

void LocalWorldState::ReadFieldConf()
{
    locConfig.fieldMaxX=atof(Configurator::Instance().findValueForKey("field.FieldMaxX").c_str());
    locConfig.fieldMinX=atof(Configurator::Instance().findValueForKey("field.FieldMinX").c_str());
    locConfig.fieldMaxY=atof(Configurator::Instance().findValueForKey("field.FieldMaxY").c_str());
    locConfig.fieldMinY=atof(Configurator::Instance().findValueForKey("field.FieldMinY").c_str());


}

void LocalWorldState::ReadTeamConf()
{
    locConfig.playerNumber=atoi(Configurator::Instance().findValueForKey("teamConfig.player").c_str());
}

void LocalWorldState::ReadRobotConf()
{

    int pNumber=locConfig.playerNumber;

    locConfig.initX[0]=atof(Configurator::Instance().findValueForKey(
Configurator::Instance().keyOfNodeForSubvalue("playerConfig.KickOff.player",".number",_toString(pNumber))+".x").c_str());
    locConfig.initY[0]=atof(Configurator::Instance().findValueForKey(
Configurator::Instance().keyOfNodeForSubvalue("playerConfig.KickOff.player",".number",_toString(pNumber))+".y").c_str());
    locConfig.initPhi[0]=atof(Configurator::Instance().findValueForKey(
Configurator::Instance().keyOfNodeForSubvalue("playerConfig.KickOff.player",".number",_toString(pNumber))+".phi").c_str());

    locConfig.initX[1]=atof(Configurator::Instance().findValueForKey(
Configurator::Instance().keyOfNodeForSubvalue("playerConfig.noKickOff.player",".number",_toString(pNumber))+".x").c_str());
    locConfig.initY[1]=atof(Configurator::Instance().findValueForKey(
Configurator::Instance().keyOfNodeForSubvalue("playerConfig.noKickOff.player",".number",_toString(pNumber))+".y").c_str());
    locConfig.initPhi[1]=atof(Configurator::Instance().findValueForKey(
Configurator::Instance().keyOfNodeForSubvalue("playerConfig.noKickOff.player",".number",_toString(pNumber))+".phi").c_str());

    //read ready state positions
    locConfig.readyX=atof(Configurator::Instance().findValueForKey(
Configurator::Instance().keyOfNodeForSubvalue("playerConfig.Ready.player",".number",_toString(pNumber))+".x").c_str());
    locConfig.readyY=atof(Configurator::Instance().findValueForKey(
Configurator::Instance().keyOfNodeForSubvalue("playerConfig.Ready.player",".number",_toString(pNumber))+".y").c_str());
    locConfig.readyPhi=TO_RAD(atof(Configurator::Instance().findValueForKey(
Configurator::Instance().keyOfNodeForSubvalue("playerConfig.Ready.player",".number",_toString(pNumber))+".phi").c_str()));

}

//------------------------------------------------- Functions for the GUI-----------------------------------------------------


int LocalWorldState::LocalizationDataForGUILoad()
{
	for (int i = 0; i < localizationWorld.SIRParticles.size; i++)
	{
		if(DebugDataForGUI.particles_size() < (int)(i+1))
			DebugDataForGUI.add_particles();
		DebugDataForGUI.mutable_particles(i)->set_x(localizationWorld.SIRParticles.x[i]*1000);
		DebugDataForGUI.mutable_particles(i)->set_y(localizationWorld.SIRParticles.y[i]*1000);
		DebugDataForGUI.mutable_particles(i)->set_phi(localizationWorld.SIRParticles.phi[i]);
	}

	return 1;
}

void LocalWorldState::InputOutputLogger(){
	KSystem::Time::TimeAbsolute currentExecute = KSystem::Time::SystemTime::now();

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
	LogEntry(LogLevel::ExtraExtraInfo,"LocalWorldStateLogger")
			<<(currentExecute).toString() << " " << RobotMovement << " "
			<<  Yellow << " " << YellowLeft  << " " <<  YellowRight
			<< " " <<  RobotPosition;
}

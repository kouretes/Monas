#include "Localization.h"
#include "Kutils.h"
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
//#include "tools/MathFunctions.h"
#define CALIBRATE 1
#define SCANFORBALL 2
#define SCANFORPOST 3
#define BALLTRACK 4

#define MAX_TIME_TO_RESET 10 //in seconds
//#define ADEBUG
//#define COUT_ON
using namespace std;

namespace
{
	ActivityRegistrar<Localization>::Type temp("Localization");
}

Localization::Localization() :
		vprof("Localization")
{
}

bool Localization::debugmode = false;
TCPSocket * Localization::sock;

void Localization::UserInit()
{
	_blk->updateSubscription("vision", msgentry::SUBSCRIBE_ON_TOPIC);
	_blk->updateSubscription("sensors", msgentry::SUBSCRIBE_ON_TOPIC);
	_blk->updateSubscription("localization", msgentry::SUBSCRIBE_ON_TOPIC);
	_blk->updateSubscription("behavior", msgentry::SUBSCRIBE_ON_TOPIC);

	Logger::Instance().WriteMsg("Localization", "Localization Initialized", Logger::Info);

	count = 0;
	serverpid = -1;
	debugmode = false;

	int max_bytedata_size = 100000;

	data = new char[max_bytedata_size]; //## TODO  FIX THIS BETTER
	leftright = 1;
	headpos = 0;
	hmot.set_command("setHead");
	hmot.add_parameter(0.0f);
	hmot.add_parameter(-0.66322512);

	wmot.add_parameter(0.0f);
	wmot.add_parameter(0.0f);
	wmot.add_parameter(0.0f);
	wmot.add_parameter(0.0f);

	bhmsg = new BToHeadMessage();

	MyWorld.add_balls();

	KLocalization::Initialize();
	//KLocalization::setParticlesPose(SIRParticles, 0, 0, 0);
	KLocalization::setParticlesPoseUniformly(SIRParticles);
	//KLocalization::setBelief(0, 0, 0, 0.1);
	sock = NULL;
	serverpid = pthread_create(&acceptthread, NULL, &Localization::StartServer, this);
	pthread_detach(acceptthread);
	firstrun = true;
	last_observation_time = boost::posix_time::microsec_clock::universal_time();
	last_filter_time = boost::posix_time::microsec_clock::universal_time();
}

void Localization::Reset()
{
	KLocalization::setParticlesPoseUniformly(SIRParticles);
}


int Localization::DebugMode_Receive()
{
	///DEBUG MODE
	bool headerparsed = false;
	int size;
	int ssize, rsize;
	int rs;
	try
	{
		ssize = 0;
		if ((sock->recv(&size, sizeof(uint32_t))) == 0)
		{
			cout << "Stopping Debug ############# Disconnecting !!!" << endl;
			debugmode = false;
		}
		size = ntohl(size);

		//cout << "Waiting for " << size << " Bytes " << endl;

		if (size < 1)
		{
			//Something went wrong ...
			cout << " Unable to parse Disconnecting" << endl;
			debugmode = false;
			return -2;
		}
		//Receive the data header
		for (rs = rsize = 0; rsize < size; rsize += rs)
			if ((rs = sock->recv(data + rsize, size - rsize)) < 0)
			{
				cout << "receive error" << endl;
				break;
			}

		//cout << "Arrived " << rsize << " Bytes " << endl;

		incommingheader.Clear();
		headerparsed = incommingheader.ParsePartialFromArray(data, size);
		if (!headerparsed)
		{
			cout << " Unable to parse Disconnecting" << endl;
			debugmode = false;
			return -1;
		}

		incommingheader.DiscardUnknownFields();

		string command = incommingheader.nextmsgname();
		#ifdef COUT_ON
		cout << "COMMAND " << command << endl;
		#endif
		if (command == "Stop")
		{
			debugmode = false;
			cout << " Stopping Debug ########################" << endl;
			return 0;
		}
		//google::protobuf::Message * incommingmsg = &incommingheader;
		//cout << "Next message " << incommingheader.nextmsgbytesize() << endl;

		//const google::protobuf::FieldDescriptor* field = incommingmsg->GetDescriptor()->FindFieldByLowercaseName("nextmsgbytesize");
		//if(field!=NULL || incommingheader.nextmsgbytesize()>0){
		//cout << "Field !" << endl;
		//const google::protobuf::Reflection* reflection =incommingmsg->GetReflection();

		//if (((size = reflection->GetInt32(*incommingmsg,field)) > 0)|| incommingheader.nextmsgbytesize()>0) //must read next message
		if ((size = incommingheader.nextmsgbytesize()) > 0) //must read next message
		{
			#ifdef COUT_ON
			cout << "NextMessageSize " << size << endl;
			#endif
			for (rs = rsize = 0; rsize < size; rsize += rs)
				if ((rs = sock->recv(data + rsize, size - rsize)) < 0)
				{
					cout << "receive error" << endl;
					break;
				}
			#ifdef COUT_ON
			cout << "Arrived " << ssize << " $$$$$$$$$$$$$$$$%%%%%%%%%Bytes Do something" << endl;
			#endif
			//if (ticommingmsg.GetTypeName() == "RobotPose") {

			if (command == "SetBelief")
			{
				RobotPose ticommingmsg;
				ticommingmsg.ParseFromArray(data, size);
				#ifdef COUT_ON
				cout << ticommingmsg.GetTypeName() << endl;
				cout << "Incoming Pose" << endl;
				#endif
				MyWorld.mutable_myposition()->MergeFrom(ticommingmsg);
				//				AgentPosition.x = MyWorld.myposition().x();
				//				AgentPosition.y = MyWorld.myposition().y();
				//				AgentPosition.theta = MyWorld.myposition().phi();
				//				AgentPosition.weightconfidence = MyWorld.myposition().confidence();
				//
				//				TrackPoint.x = AgentPosition.x;
				//				TrackPoint.y = AgentPosition.y;
				//				TrackPoint.phi = AgentPosition.theta;

				target.x = MyWorld.myposition().x();
				target.y = MyWorld.myposition().y();
				target.phi = MyWorld.myposition().phi();
				#ifdef COUT_ON
				cout << "My World theta " << AgentPosition.theta;
				#endif
			} else if (command.find("Walk") != string::npos)/* == "Walk") */
			{
				MotionWalkMessage wmot;
				wmot.ParseFromArray(data, size);
				#ifdef COUT_ON
				cout << "Incoming WalkCommand" << endl;
				#endif
				if (command.find("Stop") == string::npos)
				{ //Reset at the beggining
					TrackPoint.x = 0;
					TrackPoint.y = 0;
					TrackPoint.phi = 0;
					AgentPosition.theta = 0;
					AgentPosition.x = 0;
					AgentPosition.y = 0;
				}
				_blk->publishSignal(wmot, "motion");
			}

			//field = incommingmsg->GetDescriptor()->FindFieldByName("nextmsgbytesize");
		}
	} catch (SocketException &e)
	{
		cerr << e.what() << endl;
		#ifdef COUT_ON
		cout << "Stopping Debug ############# Disconnecting !!!" << endl;
		#endif
		debugmode = false;
	}
	return 0;
}

void Localization::SimpleBehaviorStep()
{
	// Head Scan simple step
	bool headscan = false;
	if (headscan)
	{
		if (count % 100 == 2)
		{
			hmot.set_command("setHead");
			//hmot.set_parameter(0,0.0f);
			hmot.set_parameter(1, -0.66322512);
		}

		if (fabs(headpos) > 1.5) // 1.3
			leftright *= -1;

		headpos += 0.1 * leftright;

		hmot.set_parameter(0, headpos);
		hmot.set_parameter(1, (0.12307 * abs(headpos)) - 0.66322512);
		_blk->publishSignal(hmot, "motion");

	} else
	{
		if (count % 400 == 0)
			if (count % 100 == 0)
			{
				bhmsg->set_headaction(SCANFORBALL);
				_blk->publishState(*bhmsg, "behavior");
			} else
			{
				bhmsg->set_headaction(SCANFORPOST);
				_blk->publishState(*bhmsg, "behavior");
			}

	}
	return;
	//Go to target simple behavior
	float Robot2Target_bearing = anglediff2(atan2(target.y - AgentPosition.y, target.x - AgentPosition.x), AgentPosition.theta);
	float Distance2Target = DISTANCE(target.x,AgentPosition.x,target.y,AgentPosition.y);

	//cout << "Robot2Target_bearing*TO_DEG  " << Robot2Target_bearing * TO_DEG << endl;
	//cout << atan2(target.y - AgentPosition.y, target.x - AgentPosition.x) << endl;
	//cout << AgentPosition.theta << endl << endl;
	float speed = 0.01;

	if (AgentPosition.confidence > 20)
		speed = 1;
	else
		speed = 0.8;

	if (Distance2Target < 200)
		speed *= 0.5;

	float VelX, VelY, Rot, freq;

	//TRy to get the robot to the desired position ...

	VelX = speed * cos(Robot2Target_bearing);
	VelY = speed * sin(Robot2Target_bearing);
	Rot = anglediff2(target.phi, AgentPosition.theta) * 0.3; // Robot2Target_bearing * 0.3;
	freq = 1;

	if (Distance2Target < 300)
		freq *= Distance2Target / 300;
	//Limits checks
	if (VelX > 1)
		VelX = 1;
	if (VelY > 1)
		VelY = 1;
	if (Rot > 1)
		Rot = 1;
	if (Rot < -1)
		Rot = -1;
	if (VelY < -1)
		VelY = -1;
	if (VelX < -1)
		VelX = -1;
#ifdef COUT_ON
	cout << VelX << endl;
	cout << VelY << endl;
	cout << Rot << endl;
#endif
	wmot.set_command("setWalkTargetVelocity");
	wmot.set_parameter(0, VelX);
	wmot.set_parameter(1, VelY);
	wmot.set_parameter(2, Rot);
	wmot.set_parameter(3, freq);
	_blk->publishSignal(wmot, "motion");

}

int Localization::Execute()
{
	KPROF_SCOPE(vprof, "Localization Execute");
	now = boost::posix_time::microsec_clock::universal_time();

	process_messages();

	if (debugmode)
		DebugMode_Receive();
	if (lrm != 0){
		Reset();
		Logger::Instance().WriteMsg("Localization", "Uniform particle spread over field ", Logger::Info);
	}

	LocalizationStepSIR(robotmovement, currentObservation, currentAbigiusObservation, maxrangeleft, maxrangeright);

	//SimpleBehaviorStep();

	MyWorld.mutable_myposition()->set_x(AgentPosition.x / 1000.0);
	MyWorld.mutable_myposition()->set_y(AgentPosition.y / 1000.0);
	MyWorld.mutable_myposition()->set_phi(AgentPosition.theta);
	MyWorld.mutable_myposition()->set_confidence(AgentPosition.confidence);
	#ifdef COUT_ON
	cout<<"AgentPosition.x"<<AgentPosition.x <<"AgentPosition.y"<<AgentPosition.y<<endl;
	#endif
	calculate_ball_estimate(robotmovement);
	///DEBUGMODE SEND RESULTS
	if (debugmode)
	{
		LocalizationData_Load(AUXParticles, currentObservation, robotmovement);
		Send_LocalizationData();
	}
	_blk->publishData(MyWorld, "behavior");

	count++;

	vprof.generate_report(10);
	return 0;
}

void Localization::Send_LocalizationData()
{

	outgoingheader.set_nextmsgbytesize(DebugData.ByteSize());
	outgoingheader.set_nextmsgname(DebugData.GetTypeName());

	int sendsize; //= DebugData.ByteSize();

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
		//cout << "outgoingheader sendsize " << sendsize << endl;

		while (rsize < sendsize)
		{
			rs = sock->send(data + rsize, sendsize - rsize);
			rsize += rs;
		}
		//cout << "Sended outgoingheader " << rsize << endl;
		//send the image bytes
		sendsize = DebugData.ByteSize();

		std::string buf;
		DebugData.SerializePartialToString(&buf);
		sendsize = buf.length();

		rsize = 0;
		//cout << "Will send Data" << sendsize << " " << DebugData.GetTypeName() << endl;

		while (rsize < sendsize)
		{
			rs = sock->send((char *) buf.data() + rsize, sendsize - rsize);
			rsize += rs;
		}
		//cout << "Sended " << rsize << endl;
	} catch (SocketException &e)
	{
		cerr << e.what() << endl;
		#ifdef COUT_ON
		cout << "Disconnecting !!!!!" << endl;
		#endif
		debugmode = false;
	}
}

void Localization::calculate_ball_estimate(KMotionModel const & robotModel)
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
				//cout << "RESETING_BALL" << endl;
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
				nearest_filtered_ball = myBall.get_updated_ball_estimate(aball.dist(), dist_var * dist_var, aball.bearing(), 0.03, dt);

				//Predict
				duration = now - last_filter_time;
				last_filter_time = now;
				dt = duration.total_microseconds() / 1000000.0f;
				nearest_filtered_ball = myBall.get_predicted_ball_estimate(dt, robotModel);

				float dx = nearest_filtered_ball.relativex() - nearest_nofilter_ball.relativex();
				float dy = nearest_filtered_ball.relativey() - nearest_nofilter_ball.relativey();
				float distance = dx + dy;
				if (distance != 0)
					distance = DISTANCE_2(dx,dy);

				//Check if we must reset the ball
				duration = observation_time - last_observation_time;
				last_observation_time = observation_time;
				dt = duration.total_microseconds() / 1000000.0f;

				if (dt > MAX_TIME_TO_RESET && distance > 2) //etc... dt > 5sec && distance > 2 m
				{
					myBall.reset(aball.dist(), 0, aball.bearing(), 0);
					last_filter_time = now;
					//RESET
					//cout << "RESETING_BALL" << endl;
					MyWorld.mutable_balls(0)->CopyFrom(nearest_nofilter_ball);
				} else
					MyWorld.mutable_balls(0)->CopyFrom(nearest_filtered_ball);
			}
			//cout << " relative x " << MyWorld.mutable_balls(0)->relativex() << " relative y " << MyWorld.mutable_balls(0)->relativey() << endl;
		}
	}

	if (!ballseen)
	{
		//cout << " No ball !! reseting ?" << endl;
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
			if (MyWorld.balls_size() > 0)
				MyWorld.mutable_balls(0)->CopyFrom(nearest_filtered_ball);
		}
	}
}

void Localization::RobotPositionMotionModel(KMotionModel & MModel)
{
	if (count == 0)
	{
		TrackPointRobotPosition.x = PosX.sensorvalue() * 1000;
		TrackPointRobotPosition.y = PosY.sensorvalue() * 1000;
		TrackPointRobotPosition.phi = Angle.sensorvalue();

		TrackPoint = TrackPointRobotPosition;
	}

	float XA = PosX.sensorvalue() * 1000;
	float YA = PosY.sensorvalue() * 1000;
	float AA = Angle.sensorvalue();

	//cout << "          Robot Position X: " << XA << " Y: " << YA << " A(DEG): " << AA * TO_DEG << endl;

	float DX = (XA - TrackPointRobotPosition.x);
	float DY = (YA - TrackPointRobotPosition.y);
	float DR = anglediff2(AA, TrackPointRobotPosition.phi);

	float robot_dist = DISTANCE_2(DX, DY);
	float robot_dir = anglediff2(atan2(DY, DX), TrackPointRobotPosition.phi);
	float robot_rot = DR;

	MModel.type = "ratio";
	if (robot_dist > 500)
	{
		robot_dist = 0.0;
		robot_dir = 0.000001;
		robot_rot = 0.00001;
	}
	MModel.Distance.val = robot_dist;
	MModel.Distance.ratiomean = 1.32; // -0.0048898*robot_dist + 0.013794*robot_dir + 0.32631*robot_rot + 3.6155;
	MModel.Distance.ratiodev = abs(0.002131 * (robot_dir + robot_rot) + 0.094058*robot_dist);

	MModel.Direction.val = robot_dir;
	//cout << "MModel.Direction.val " << MModel.Direction.val;
	MModel.Direction.Emean = -0.014257 * abs(robot_dir) - 0.031725 * pow(robot_dir, 3); // -0.81684*abs(robot_dir) -1.8177*pow(robot_dir,3) + ;
	//cout << "MModel.Direction.Emean " << MModel.Direction.Emean;
	MModel.Direction.Edev = abs(-0.019 * abs(robot_dir) );
	// 0.0063971 * robot_dir + 0.090724 / abs(robot_dir);
	//cout << "MModel.Direction.Edev " << MModel.Direction.Edev;

	MModel.Rotation.val = robot_rot;
	MModel.Rotation.ratiomean = 0;//-1.9346*robot_dist + 10.041 *robot_dir + 173.24*robot_rot + 890.64;
	MModel.Rotation.ratiodev = 0.0001; //5.67 *robot_dist + -69.009 *robot_dir +-359.19*robot_rot -2561.2;
	//cout << "          Robot Position DX: " << DX << " DY: " << DY << " DR(DEG): " << DR * TO_DEG << endl;

	//cout << "Distance.val = " << MModel.Distance.val << " Distance.Edev = " << MModel.Distance.Edev << endl;
	//cout << " Direction.val(DEG) = " << MModel.Direction.val * TO_DEG << " Direction.Edev(DEG) = " << MModel.Direction.Edev * TO_DEG << endl;
	//cout << "  Rotation.val(DEG) = " << MModel.Rotation.val * TO_DEG << " Rotation.Edev(DEG) = " << MModel.Rotation.Edev * TO_DEG << endl;

	TrackPointRobotPosition.x = XA;
	TrackPointRobotPosition.y = YA;
	TrackPointRobotPosition.phi = AA;

	TrackPoint.x += DX;
	TrackPoint.y += DY;
	TrackPoint.phi += DR;

}

belief Localization::LocalizationStepSIR(KMotionModel & MotionModel, vector<KObservationModel>& Observations, vector<KObservationModel>& AmbigiusObservations, double rangemaxleft, double rangemaxright)
{
	int iterations = 1;
	int index[partclsNum];

	//SpreadParticles
//	if (Observations.empty())
//	{
//		//		cout << "No observations ... spreading" << endl;
//		//SpreadParticlesCirc(SIRParticles, 10, 0, 2);
//	}
	//	if (depletions_counter > 1) {
	//		cout << "Depletion Counter " << depletions_counter << endl;
	//		SpreadParticlesCirc(SIRParticles, 100.0 * depletions_counter, 30 * TO_RAD, 50);
	//	}

	SpreadParticlesCirc(SIRParticles, SpreadParticlesDeviation, rotation_deviation, PercentParticlesSpread);
	//	if (Observations.size() > 1)
	//		ObservationParticles(Observations, SIRParticles, 6000, 4000, 200, rangemaxleft, rangemaxright);

#ifdef ADEBUG
	cout << "\nPredict Iterations " << iterations << endl;
	for (int i = 0; i < partclsNum / 10.0; i++)
	cout << SIRParticles.Weight[i] << "  ";
#endif
	//SIR Filter

	//Predict - Move particles according the Prediction Model

	for (int i = 0; i < iterations; i++)
		Predict(SIRParticles, MotionModel);

	//Set semi-optimal bearing angle as the average bearing angle to the observations
	//ForceBearing(SIRParticles, Observations);

	//Create some particles using Observation Intersection
	CircleIntersectionPossibleParticles(Observations, SIRParticles, 4);

	//Update - Using incoming observation
	Update(SIRParticles, Observations, MotionModel, partclsNum, rangemaxleft, rangemaxright);
//	if(AmbigiusObservations.size()>0)
//		Update_Ambigius(SIRParticles,AmbigiusObservations,partclsNum);

#ifdef ADEBUG
	cout << "\nUnnormalized SIR particles " << endl;
	for (int i = 0; i < partclsNum / 5.0; i++)
	cout << SIRParticles.Weight[i] << " \n ";
#endif

	//Normalize Particles  Weight in order to Resample later
	float ESS = normalize(SIRParticles.Weight, partclsNum);
	//cout << "\033[01;32m \n ESS " << ESS << " Beta " << Beta << " Beta 2 " << *Beta2 << "\033[0m" << endl;

	if (ESS < Beta * 0.8)
	{
		//cerr << "\033[01;31m \nOups SIRParticles Population Dissapeared Maybe the Robot have changed position\033[0m" << endl;
		depletions_counter++;
	} else
	{
		depletions_counter = 0;
	}
#ifdef ADEBUG
	cout << "\nNormalized SIR particles  " << endl;
	for (int i = 0; i < partclsNum / 10.0; i++)
		cout << SIRParticles.Weight[i] << " \n ";
#endif
	//Maybe Usefull for others
	memcpy(AUXParticles.x, SIRParticles.x, partclsNum * sizeof(double));
	memcpy(AUXParticles.y, SIRParticles.y, partclsNum * sizeof(double));
	memcpy(AUXParticles.phi, SIRParticles.phi, partclsNum * sizeof(double));
	memcpy(AUXParticles.Weight, SIRParticles.Weight, partclsNum * sizeof(double));
#ifdef ADEBUG
	cout << "\nAUX particles Before, SIR before Resample after memcpy" << endl;
	for (int i = 0; i < partclsNum / 10.0; i++)
		cout << AUXParticles.Weight[i];
#endif
	////#########################

	//extract estimation
	partcl maxprtcl;

	float max_weight=-1;
	int max_weight_particle_index=0;
	//Find Max Weight
//	maxprtcl.Weight = SIRParticles.Weight[0];
//	maxprtcl.x = SIRParticles.x[0];
//	maxprtcl.y = SIRParticles.y[0];
//	maxprtcl.phi = SIRParticles.phi[0];
	//SIRParticles.WeightSum = SIRParticles.Weight[0];
	for (unsigned int i = 0; i < SIRParticles.size; i++)
	{
		if (SIRParticles.Weight[i] > max_weight)
		{
//			maxprtcl.x = SIRParticles.x[i];
//			maxprtcl.y = SIRParticles.y[i];
//			maxprtcl.phi = SIRParticles.phi[i];
//			maxprtcl.Weight = SIRParticles.Weight[i];
			max_weight = SIRParticles.Weight[i];
			max_weight_particle_index = i;
		}
	}

	//Swap best particle to the
	maxprtcl.x = SIRParticles.x[0];
	maxprtcl.y = SIRParticles.y[0];
	maxprtcl.phi = SIRParticles.phi[0];
	maxprtcl.Weight = SIRParticles.Weight[0];

	SIRParticles.x[0] = SIRParticles.x[max_weight_particle_index];
	SIRParticles.y[0] = SIRParticles.y[max_weight_particle_index];
	SIRParticles.phi[0] = SIRParticles.phi[max_weight_particle_index];
	SIRParticles.Weight[0] = SIRParticles.Weight[max_weight_particle_index];

	SIRParticles.x[max_weight_particle_index] = maxprtcl.x;
	SIRParticles.y[max_weight_particle_index] = maxprtcl.y;
	SIRParticles.phi[max_weight_particle_index] = maxprtcl.phi;
	SIRParticles.Weight[max_weight_particle_index] = maxprtcl.Weight;

	AgentPosition.x =  SIRParticles.x[0];// maxprtcl.x;
	AgentPosition.y = SIRParticles.y[0];//maxprtcl.y;
	AgentPosition.theta = SIRParticles.phi[0];//maxprtcl.phi;


	//cout << "Probable agents position " << AgentPosition.x << ", " << AgentPosition.y << " maxprtcl W: " << maxprtcl.Weight << endl;
	AgentPosition = RobustMean(SIRParticles, 10);
	#ifdef COUT_ON
	cout << "Probable agents position " << AgentPosition.x << ", " << AgentPosition.y << ", " << AgentPosition.theta << endl;
	#endif
	AgentPosition = RobustMean(SIRParticles, 10);

	//TODO only one value to determine confidance, Now its only distance confidence
	AgentPosition.confidence = CalculateConfidence(SIRParticles, AgentPosition);



	//Complete the SIR
	//Check last position confidence
	if (ESS > 0 && (ESS < Beta ))
	{
		Resample(SIRParticles, index, 0);
		Propagate(SIRParticles, index);
		if (depletions_counter > 4)
			depletions_counter -= 2;
	} else
	{
		; //cout << "NO need of resampling" << endl;
	}



	//cout << "Agent Confidence " << AgentPosition.confidence << endl;

	//cin.ignore(10, '\n');
	//cin.clear();
	//	cout << "write something to display belief " << endl;
	//cin >> c;



	return AgentPosition;

}

void Localization::process_messages()
{
	boost::posix_time::ptime observation_time;

	gsm = _blk->readState<GameStateMessage>("behavior");
	obsm = _blk->readSignal<ObservationMessage>("vision");
	lrm = _blk->readSignal<LocalizationResetMessage>("behavior");


	currentObservation.clear();
	currentAbigiusObservation.clear();
	if (obsm != 0)
	{
		KObservationModel tmpOM;
		//Load observations
		const ::google::protobuf::RepeatedPtrField<NamedObject>& Objects = obsm->regular_objects();
		string id;

		maxrangeleft = obsm->bearing_limit_left();
		maxrangeright = obsm->bearing_limit_right();
		//cout << "Range Angles maxleft: " << maxrangeleft << " max right: " << maxrangeright << endl;

		for (int i = 0; i < Objects.size(); i++)
		{
			id = Objects.Get(i).object_name();

			//Distance
			tmpOM.Distance.val = Objects.Get(i).distance() * 1000;
			tmpOM.Distance.Emean = 0;
			tmpOM.Distance.Edev = 10*sqrt(sqrt(Objects.Get(i).distance_dev())) * 1000 + 30;

			//Bearing
			tmpOM.Bearing.val = wrapTo0_2Pi( Objects.Get(i).bearing());
			tmpOM.Bearing.Emean = 0;
			tmpOM.Bearing.Edev = sqrt(Objects.Get(i).bearing_dev()) * 360;


			if ((this)->KFeaturesmap.count(id) != 0)
			{
				//Make the feature
				if(id.find("Left")!=string::npos ||id.find("Right")!=string::npos)
				{
					tmpOM.Feature = (this)->KFeaturesmap[id];
					currentObservation.push_back(tmpOM);
				}
				//cout << "Feature seen " << tmpOM.Feature.id << " Distance " << tmpOM.Distance.val << " Bearing " << tmpOM.Bearing.val << endl;
				//cout << " DistanceDev " << tmpOM.Distance.Edev << " BearingDev " << tmpOM.Bearing.Edev << endl;
			}else {
				if( id.find("Skyblue")!=string::npos)
				{
					tmpOM.Feature = (this)->KFeaturesmap["SkyblueLeft"];
					currentAbigiusObservation.push_back(tmpOM);
				}

				if( id.find("Yellow")!=string::npos){
					tmpOM.Feature = (this)->KFeaturesmap["YellowLeft"];
					currentAbigiusObservation.push_back(tmpOM);
				}

				Logger::Instance().WriteMsg("Localization", "Unmatched Observation: "+id, Logger::Info);
			}
		}
		//		if(AgentPosition.confidence > 0.5){
		//			tmpOM.Feature = "OldPose";
		//			tmpOM.Distance.val = DISTANCE(AgentPosition.x,Particles.x[p],AgentPosition.y,Particles.y[p]);
		//			tmpOM.Bearing.val = atan2(Particles .y[p] - AgentPosition.y, Particles.x[p] - AgentPosition.x);
		//		}

		observation_time = boost::posix_time::from_iso_string(obsm->image_timestamp());
		rpsm = _blk->readData<RobotPositionMessage> ("sensors", msgentry::HOST_ID_LOCAL_HOST,NULL, &observation_time);
	}else{
		rpsm = _blk->readData<RobotPositionMessage>("sensors");
	}

	if (rpsm != 0)
	{
		PosX = rpsm->sensordata(KDeviceLists::ROBOT_X);
		PosY = rpsm->sensordata(KDeviceLists::ROBOT_Y);
		Angle = rpsm->sensordata(KDeviceLists::ROBOT_ANGLE);

		RobotPositionMotionModel(robotmovement);
	}

}

int Localization::LocalizationData_Load(parts & Particles, vector<KObservationModel>& Observation, KMotionModel & MotionModel)
{
	bool addnewptrs = false;
	//Fill the world with data!
	WorldInfo *WI = DebugData.mutable_world();

	//Setting my position
	WI->mutable_myposition()->set_x(AgentPosition.x);
	WI->mutable_myposition()->set_y(AgentPosition.y);
	WI->mutable_myposition()->set_phi(AgentPosition.theta);
	WI->mutable_myposition()->set_confidence(AgentPosition.confidence);

	WI->CopyFrom(MyWorld);
	//Setting robotPositionField X = DX, Y = DY, phi = DF

	//	DebugData.mutable_robotposition()->set_x(MotionModel.Distance.val);
	//	DebugData.mutable_robotposition()->set_y(MotionModel.Direction.val);
	//	DebugData.mutable_robotposition()->set_phi(MotionModel.Rotation.val);
	DebugData.mutable_robotposition()->set_x(TrackPoint.x);
	DebugData.mutable_robotposition()->set_y(TrackPoint.y);
	DebugData.mutable_robotposition()->set_phi(TrackPoint.phi);

	//	DebugData.mutable_myposition()->set_confidence(AgentPosition.confidence );

	RobotPose prtcl;

	if ((unsigned int) DebugData.particles_size() < Particles.size)
		addnewptrs = true;
	for (unsigned int i = 0; i < Particles.size; i++)
	{
		if (addnewptrs)
			DebugData.add_particles();
		DebugData.mutable_particles(i)->set_x(Particles.x[i]);
		DebugData.mutable_particles(i)->set_y(Particles.y[i]);
		DebugData.mutable_particles(i)->set_phi(Particles.phi[i]);
		DebugData.mutable_particles(i)->set_confidence(Particles.Weight[i]);
	}
	//cout << " added " << Particles.size << endl;
	if (obsm != NULL)
	{
		(DebugData.mutable_observations())->CopyFrom(*obsm);
	} else
	{
		DebugData.clear_observations();
	}
	return 1;
}

void * Localization::StartServer(void * astring)
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
		Logger::Instance().WriteMsg("Localization", " No port number in : " + ArchConfig::Instance().GetConfigPrefix() + "/Localizationconf.xml", Logger::Warning);

	TCPServerSocket servSock(port);

	Logger::Instance().WriteMsg("Localization", " Localization server is ready at port: " + _toString(port), Logger::Info);
	while (true)
	{
		if (!debugmode)
		{
			if (sock != NULL
			)
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

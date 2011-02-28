#include "Localization.h"
#include "Kutils.h"

#include "tools/logger.h"
#include "tools/toString.h"
#include "messages/RoboCupGameControlData.h"
#include <pthread.h>
#include <netinet/in.h>
#include <csignal>
#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>

#include "architecture/archConfig.h"

#define SCANFORBALL 2
#define SCANFORPOST 3

//#define ADEBUG
using namespace std;
namespace
{
	ActivityRegistrar<Localization>::Type temp("Localization");
}

Localization::Localization()
{
}

bool Localization::debugmode = false;
TCPSocket * Localization::sock;

void Localization::UserInit()
{

	_blk->subscribeTo("vision", ON_TOPIC);
	_blk->subscribeTo("sensors", ON_TOPIC);
	_blk->subscribeTo("localization", ON_TOPIC);
	_blk->subscribeTo("behavior", ON_TOPIC);

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

	KLocalization::Initialize(); //TODO PUT IT BACK TO KLOCALIZATION!
	KLocalization::setParticlesPose(SIRParticles, 0, 0, 0);
	KLocalization::setParticlesPoseUniformly(SIRParticles);
	//KLocalization::setBelief(0, 0, 0, 0.1);
	sock = NULL;
	serverpid = pthread_create(&acceptthread, NULL, &Localization::StartServer, this);
	pthread_detach(acceptthread);
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
		cout << "COMMAND " << command << endl;
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
			cout << "NextMessageSize " << size << endl;
			for (rs = rsize = 0; rsize < size; rsize += rs)
				if ((rs = sock->recv(data + rsize, size - rsize)) < 0)
				{
					cout << "receive error" << endl;
					break;
				}

			cout << "Arrived " << ssize << " $$$$$$$$$$$$$$$$%%%%%%%%%Bytes Do something" << endl;
			//if (ticommingmsg.GetTypeName() == "RobotPose") {

			if (command == "SetBelief")
			{
				RobotPose ticommingmsg;
				ticommingmsg.ParseFromArray(data, size);
				cout << ticommingmsg.GetTypeName() << endl;
				cout << "Incoming Pose" << endl;
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
				cout << "My World theta " << AgentPosition.theta;
			} else if (command.find("Walk") != string::npos)/* == "Walk") */
			{
				MotionWalkMessage wmot;
				wmot.ParseFromArray(data, size);
				cout << "Incoming WalkCommand" << endl;
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
		cout << "Stopping Debug ############# Disconnecting !!!" << endl;
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
		if (count % 100 == 0)
		{
			bhmsg->set_headaction(SCANFORPOST);
			_blk->publishState(*bhmsg, "behavior");
		}
		if (count % 200 == 0)
		{
			bhmsg->set_headaction(SCANFORBALL);
			_blk->publishState(*bhmsg, "behavior");
		}
	}
	//Go to target simple behavior
	float Robot2Target_bearing = anglediff2(atan2(target.y - AgentPosition.y, target.x - AgentPosition.x), AgentPosition.theta);
	float Distance2Target = DISTANCE(target.x,AgentPosition.x,target.y,AgentPosition.y);

	cout << "Robot2Target_bearing*TO_DEG  " << Robot2Target_bearing * TO_DEG << endl;
	cout << atan2(target.y - AgentPosition.y, target.x - AgentPosition.x) << endl;
	cout << AgentPosition.theta << endl << endl;
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
	Rot = anglediff2(target.phi, AgentPosition.theta) * 0.3;// Robot2Target_bearing * 0.3;
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

	cout << VelX << endl;
	cout << VelY << endl;
	cout << Rot << endl;

	wmot.set_command("setWalkTargetVelocity");
	wmot.set_parameter(0, VelX);
	wmot.set_parameter(1, VelY);
	wmot.set_parameter(2, Rot);
	wmot.set_parameter(3, freq);
	_blk->publishSignal(wmot, "motion");

}

int Localization::Execute()
{

	process_messages();

	if (debugmode)
		DebugMode_Receive();

	LocalizationStepSIR(robotmovement, currentObservation, maxrangeleft, maxrangeright);

	SimpleBehaviorStep();

	MyWorld.mutable_myposition()->set_x(AgentPosition.x);
	MyWorld.mutable_myposition()->set_y(AgentPosition.y);
	MyWorld.mutable_myposition()->set_phi(AgentPosition.theta);
	MyWorld.mutable_myposition()->set_confidence(AgentPosition.confidence);

	_blk->publishData(MyWorld, "behavior"); //Signal(wmot, "motion");

	///DEBUGMODE SEND RESULTS
	if (debugmode)
	{
		LocalizationData_Load(AUXParticles, currentObservation, robotmovement);
		Send_LocalizationData();
	}
	count++;
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
		cout << "outgoingheader sendsize " << sendsize << endl;

		while (rsize < sendsize)
		{
			rs = sock->send(data + rsize, sendsize - rsize);
			rsize += rs;
		}
		//cout << "Sended outgoingheader " << rsize << endl;
		//send the image bytes
		sendsize = DebugData.ByteSize();

		std::string buf;
		DebugData.SerializeToString(&buf);
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
		cout << "Disconnecting !!!!!" << endl;
		debugmode = false;
	}
}

void Localization::RobotPositionMotionModel(KMotionModel & MModel)
{

	if (count == 0)
	{

		TrackPointRobotPosition.x = PosX.sensorvalue() * 1000;
		TrackPointRobotPosition.y = PosY.sensorvalue() * 1000;
		TrackPointRobotPosition.phi = Angle.sensorvalue();
		//Gui->KfieldInitTrackLine(TrackPointRobotPosition);

		//cout << "Robots angle " << mypos.theta << endl;
		//Gui->addTrackLine(mypos);
		TrackPoint = TrackPointRobotPosition;
	}
	//TrackPoint RobotPositionAfter;
	//std::vector<float> RobotPos = motion->getRobotPosition(true);
	float XA = PosX.sensorvalue() * 1000;//-RobotPos[0] * 1000;
	float YA = PosY.sensorvalue() * 1000;//-RobotPos[1] * 1000;
	float AA = Angle.sensorvalue(); //RobotPos[2]+M_PI;

	//	mypos.x = XA * 1000;
	//	mypos.y = YA * 1000;
	//	mypos.theta = AA;
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
		robot_dist = 0.1;
		robot_dir = 0.000001;
		robot_rot = 0.00001;
	}
	MModel.Distance.val = robot_dist;
	MModel.Distance.ratiomean = 1.32;// -0.0048898*robot_dist + 0.013794*robot_dir + 0.32631*robot_rot + 3.6155;
	MModel.Distance.ratiodev = abs(0.002131 * (robot_dir + robot_rot) + 0.094058);

	MModel.Direction.val = robot_dir;
	cout << "MModel.Direction.val " << MModel.Direction.val;
	MModel.Direction.Emean = -0.014257 * abs(robot_dir) - 0.031725 * pow(robot_dir, 3) + 0.1086; // -0.81684*abs(robot_dir) -1.8177*pow(robot_dir,3) + ;
	cout << "MModel.Direction.Emean " << MModel.Direction.Emean;
	MModel.Direction.Edev = abs(-0.019 * abs(robot_dir) + 0.059824);
	// 0.0063971 * robot_dir + 0.090724 / abs(robot_dir);
	cout << "MModel.Direction.Edev " << MModel.Direction.Edev;

	MModel.Rotation.val = robot_rot;
	MModel.Rotation.ratiomean = 0;// -1.9346*robot_dist + 10.041 *robot_dir + 173.24*robot_rot + 890.64;
	MModel.Rotation.ratiodev = 0.0001;//5.67 *robot_dist + -69.009 *robot_dir +-359.19*robot_rot -2561.2;
	//cout << "          Robot Position DX: " << DX << " DY: " << DY << " DR(DEG): " << DR * TO_DEG << endl;

	cout << "Distance.val = " << MModel.Distance.val << " Distance.Edev = " << MModel.Distance.Edev << endl;
	//cout << " Direction.val(DEG) = " << MModel.Direction.val * TO_DEG << " Direction.Edev(DEG) = " << MModel.Direction.Edev * TO_DEG << endl;
	//cout << "  Rotation.val(DEG) = " << MModel.Rotation.val * TO_DEG << " Rotation.Edev(DEG) = " << MModel.Rotation.Edev * TO_DEG << endl;

	TrackPointRobotPosition.x = XA;
	TrackPointRobotPosition.y = YA;
	TrackPointRobotPosition.phi = AA;

	TrackPoint.x += DX;
	TrackPoint.y += DY;
	TrackPoint.phi += DR;

}

belief Localization::LocalizationStepSIR(KMotionModel & MotionModel, vector<KObservationModel>& Observations, double rangemaxleft, double rangemaxright)
{

	cout << "SelfLocalize SIR" << endl;
	int iterations = 1;

	int index[partclsNum];
	//Simple initialization

	//cin.ignore(10, '\n');
	//cin.clear();
	//cout << "write something to predict particles " << endl;
	//cin >> c;
	//SpreadParticles
	if (Observations.empty())
	{
		//		cout << "No observations ... spreading" << endl;
		//SpreadParticlesCirc(SIRParticles, 10, 0, 2);
	}
	//	if (depletions_counter > 1) {
	//		cout << "Depletion Counter " << depletions_counter << endl;
	//		SpreadParticlesCirc(SIRParticles, 100.0 * depletions_counter, 30 * TO_RAD, 50);
	//	}

	//SpreadParticlesCirc(SIRParticles, 10.0 * depletions_counter, 10 * TO_RAD, 20);
	//SpreadParticlesCirc(SIRParticles, 20.0 * depletions_counter, 1 * TO_RAD, 100);
	//SpreadParticlesCirc(SIRParticles, 20.0 * depletions_counter, 45 * TO_RAD, 5);
	SpreadParticlesCirc(SIRParticles, SpreadParticlesDeviation, rotation_deviation, PercentParticlesSpread);
	//	if (Observations.size() > 1)
	//		ObservationParticles(Observations, SIRParticles, 6000, 4000, 200, rangemaxleft, rangemaxright);

#ifdef ADEBUG
	cout << "\nPredict Iterations " << iterations << endl;
	for (int i = 0; i < partclsNum / 10.0; i++)
	cout << SIRParticles.Weight[i] << "  ";
#endif
	//SIR Filter


	//sleep(1);
	//Predict - Move particles according the Prediction Model
	//	if (MotionModelptr != NULL) {
	for (int i = 0; i < iterations; i++)
		Predict(SIRParticles, MotionModel);
	//	} else {
	//		SpreadParticlesCirc(SIRParticles, iterations * 100, 30 * TO_RAD, 100);
	//	}
	////#########################
	//

	//Set semi-optimal bearing angle as the average bearing angle to the observations
	if (Observations.size() > 0)
		ForceBearing(SIRParticles, Observations);

	cout << CircleIntersectionPossibleParticles(Observations, SIRParticles, 4) << endl;

	//Update - Using incoming observation
	Update(SIRParticles, Observations, MotionModel, partclsNum, rangemaxleft, rangemaxright);

#ifdef ADEBUG
	cout << "\nUnnormalized SIR particles " << endl;
	for (int i = 0; i < partclsNum / 5.0; i++)
	cout << SIRParticles.Weight[i] << " \n ";
#endif

	//Normalize Particles  Weight in order to Resample later
	float ESS = normalize(SIRParticles.Weight, partclsNum);
	cout << "\033[01;32m \n ESS " << ESS << " Beta " << Beta << " Beta 2 " << *Beta2 << "\033[0m" << endl;

	if (ESS < Beta * 0.8)
	{
		cerr << "\033[01;31m \nOups SIRParticles Population Dissapeared Maybe the Robot have changed position\033[0m" << endl;
		depletions_counter++;

		cout << "Depletion Counter " << depletions_counter << endl;
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

	//Find Max Weight
	maxprtcl.Weight = SIRParticles.Weight[0];
	maxprtcl.x = SIRParticles.x[0];
	maxprtcl.y = SIRParticles.y[0];
	maxprtcl.phi = SIRParticles.phi[0];
	SIRParticles.WeightSum = SIRParticles.Weight[0];
	for (unsigned int i = 0; i < SIRParticles.size; i++)
	{
		//Particles_cx += SIRParticles.x[i];
		//Particles_cy += SIRParticles.y[i];

		if (SIRParticles.Weight[i] > maxprtcl.Weight)
		{
			maxprtcl.x = SIRParticles.x[i];
			maxprtcl.y = SIRParticles.y[i];
			maxprtcl.phi = SIRParticles.phi[i];
			maxprtcl.Weight = SIRParticles.Weight[i];
		}
	}

	//Find Center of particles population
	//	double Particles_cx = 0;
	//	double Particles_cy = 0;
	//
	//	Particles_cx = Particles_cx / SIRParticles.size;
	//	Particles_cy = Particles_cy / SIRParticles.size;

	AgentPosition.x = maxprtcl.x;
	AgentPosition.y = maxprtcl.y;
	AgentPosition.theta = maxprtcl.phi;
	//TODO only one value to determine confidance, Now its only distance confidence
	//AgentPosition.confidence = CalculateConfidence(SIRParticles, AgentPosition);

	cout << "Probable agents position " << AgentPosition.x << ", " << AgentPosition.y << " maxprtcl W: " << maxprtcl.Weight << endl;
	//AgentPosition = RobustMean(SIRParticles, 2);
	//Complete the SIR

	if ((ESS < Beta || AgentPosition.confidence > 150))
	{
		Resample(SIRParticles, index, 0);
		Propagate(SIRParticles, index);
		if (depletions_counter > 4)
			depletions_counter -= 2;
	} else
	{
		cout << "NO need of resampling" << endl;
	}
	//TODO only one value to determine confidance, Now its only distance confidence
	AgentPosition.confidence = CalculateConfidence(SIRParticles, AgentPosition);

	cout << "Agent Confidence " << AgentPosition.confidence << endl;

	//cin.ignore(10, '\n');
	//cin.clear();
	//	cout << "write something to display belief " << endl;
	//cin >> c;

	return AgentPosition;

}

void Localization::process_messages()
{
	_blk->process_messages();

	gsm = _blk->readState<GameStateMessage> ("behavior");
	rpsm = _blk->readData<RobotPositionSensorMessage> ("sensors");
	obsm = _blk->readSignal<ObservationMessage> ("vision");

	if (rpsm != 0)
	{
		PosX = rpsm->sensordata(0);
		PosY = rpsm->sensordata(1);
		Angle = rpsm->sensordata(2);

		RobotPositionMotionModel(robotmovement);
	}

	if (obsm != 0)
	{
		KObservationModel tmpOM;
		currentObservation.clear();
		//Load observations

		const ::google::protobuf::RepeatedPtrField<NamedObject>& Objects = obsm->regular_objects();
		string id;

		maxrangeleft = obsm->bearing_limit_left();
		maxrangeright = obsm->bearing_limit_right();
		//cout << "Range Angles maxleft: " << maxrangeleft << " max right: " << maxrangeright << endl;

		for (int i = 0; i < Objects.size(); i++)
		{
			id = Objects.Get(i).object_name();
			//			if (id[0] == 'S')
			//				continue;
			if ((this)->KFeaturesmap.count(id) != 0)
			{
				//Make the feature
				tmpOM.Feature = (this)->KFeaturesmap[id];
				//Distance
				tmpOM.Distance.val = Objects.Get(i).distance() * 1000;
				tmpOM.Distance.Emean = 0;
				tmpOM.Distance.Edev = sqrt(Objects.Get(i).distance_dev()) * 1000 + 30;

				tmpOM.Bearing.val = Objects.Get(i).bearing();
				tmpOM.Bearing.Emean = 0;
				tmpOM.Bearing.Edev = sqrt(Objects.Get(i).bearing_dev()) * 560;

				currentObservation.push_back(tmpOM);
				cout << "Feature seen " << tmpOM.Feature.id << " Distance " << tmpOM.Distance.val << " Bearing " << tmpOM.Bearing.val << endl;
				cout << " DistanceDev " << tmpOM.Distance.Edev << " BearingDev " << tmpOM.Bearing.Edev << endl;
			}

			//			else {
			//				if (id.compare("HeadYawAngles") == 0) {
			//					maxrangeleft = Objects->Get(i)->bearing_limit_left;//(float) ret[i + 1];
			//					maxrangeright = Objects->Get(i)->bearing_limit_right;//(float) ret[i + 2];
			//					cout << "Range Angles maxleft: " << maxrangeleft << " max right: " << maxrangeright << endl;
			//				} else
			//					cout << "String i " << i << " Name " << (string) ret[i] << " unknown " << endl;
			//			}
		}
		//		if(AgentPosition.confidence > 0.5){
		//			tmpOM.Feature = "OldPose";
		//			tmpOM.Distance.val = DISTANCE(AgentPosition.x,Particles.x[p],AgentPosition.y,Particles.y[p]);
		//			tmpOM.Bearing.val = atan2(Particles .y[p] - AgentPosition.y, Particles.x[p] - AgentPosition.x);
		//		}
	}

	Logger::Instance().WriteMsg("Localization", "process_messages ", Logger::ExtraExtraInfo);

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
	cout << " added " << Particles.size << endl;
	if (obsm != NULL)
	{
		(DebugData.mutable_observations())->CopyFrom(*obsm);
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
			sleep(5);
		}
	}

	return NULL;
}

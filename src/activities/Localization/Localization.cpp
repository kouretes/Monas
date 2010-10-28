#include "Localization.h"
#include "tools/logger.h"
#include "tools/toString.h"
#include "messages/RoboCupGameControlData.h"
#include <pthread.h>

using namespace std;
namespace {
	ActivityRegistrar<Localization>::Type temp("Localization");
}

Localization::Localization()  {
}
bool Localization::debugmode = false;
UDTSOCKET Localization::recver;
void Localization::UserInit() {

	//_com->get_message_queue()->add_publisher(this);
	//_com->get_message_queue()->add_subscriber(_blk);
	_com->get_message_queue()->subscribe("vision", _blk, 0);
	_com->get_message_queue()->subscribe("sensors", _blk, 0);
	_com->get_message_queue()->subscribe("Localization", _blk, 0);

	Logger::Instance().WriteMsg("Localization", "Localization Initialized", Logger::Info);

	/*obsm = 0;
	gsm = 0;
	rpsm = 0;
	*/
	count = 0;

	debugmode = false;

	pthread_create(&acceptthread, NULL, &Localization::StartServer, this);
	pthread_detach( acceptthread);

	int max_bytedata_size = 100000;

	data = new char[max_bytedata_size]; //## TODO  FIX THIS BETTER

	KLocalization::Initialize(); //TODO PUT IT BACK TO KLOCALIZATION!
}
//void CallStarTServer(Localization * obj)

int Localization::Execute() {

	read_messages();
	bool headerparsed = false;
	if (debugmode) {
		while (!headerparsed) {
			int ssize;
			int ss;
			ssize = 0;
			size = 64; //////////#################################################################

			incommingheader.Clear();
			//cout << "Waiting for " << size << " Bytes " << endl;
			while (ssize < size) {
				if (UDT::ERROR == (ss = UDT::recv(recver, data + ssize, size - ssize, 0))) {
					cout << "recv:" << UDT::getlasterror().getErrorMessage() << endl;
					break;
				}
				ssize += ss;
			}
			cout << "Arrived " << ssize << " Bytes " << endl;
			headerparsed = incommingheader.ParseFromArray(data, size);
			if (!headerparsed)
				continue;

			incommingheader.DiscardUnknownFields();
			int alreadyparsedbytes = incommingheader.ByteSize();

			cout << "alreadyparsedbytes " << alreadyparsedbytes << " Bytes" << endl;
		}

		string command = incommingheader.nextmsgname();
		if (command == "Stop")
			debugmode = false;
		if ((size = incommingheader.nextmsgbytesize()) > 0) //must read next message
		{
			int ssize;
			int ss;
			ssize = 0;
			while (ssize < size) {
				if (UDT::ERROR == (ss = UDT::recv(recver, data + ssize, size - ssize, 0))) {
					cout << "recv:" << UDT::getlasterror().getErrorMessage() << endl;
					break;
				}
				ssize += ss;
			}
			cout << "Arrived " << ssize << " Bytes Do something" << endl;
		}

	}

	//LocalizationStepSIR(robotmovement,currentObservation, maxrangeleft, maxrangeright);

	MyWorld.mutable_myposition()->set_x(AgentPosition.x);
	MyWorld.mutable_myposition()->set_y(AgentPosition.y);
	MyWorld.mutable_myposition()->set_phi(AgentPosition.theta);
	MyWorld.mutable_myposition()->set_confidence(AgentPosition.confidence);


	if (debugmode) {
		LocalizationData_Load(SIRParticles, currentObservation, robotmovement);
		Send_LocalizationData();
	}

	return 0;
}

void Localization::Send_LocalizationData() {

	outgoingheader.set_nextmsgbytesize(DebugData.ByteSize());
	outgoingheader.set_nextmsgname(DebugData.GetTypeName());

	int sendsize; //= DebugData.ByteSize();

	int rsize = 0;
	int rs;
	//send a header
	sendsize = outgoingheader.ByteSize();
	outgoingheader.SerializeToArray(data, sendsize);

	//cout << "imgheader.ByteSize() sendsize " << sendsize << endl;

	while (rsize < sendsize) {
		if (UDT::ERROR == (rs = UDT::send(recver, data + rsize, sendsize - rsize, 0))) {
			cout << "recv:" << UDT::getlasterror().getErrorMessage() << endl;
			break;
		}
		rsize += rs;
	}

	//send the image bytes
	sendsize = DebugData.ByteSize();

	std::string buf;
	DebugData.SerializeToString(&buf);
	sendsize = buf.length();

	rsize = 0;
	cout << "Will send Data" << sendsize << " " << DebugData.GetTypeName() << endl;

	while (rsize < sendsize) {
		if (UDT::ERROR == (rs = UDT::send(recver, (char *) buf.data(), sendsize - rsize, 0))) {
			cout << "recv:" << UDT::getlasterror().getErrorMessage() << endl;
			break;
		}
		rsize += rs;
	}
	cout << "Sended " << rsize << endl;
}

void Localization::RobotPositionMotionModel(KMotionModel & MModel) {

	if (count == 0) {

		TrackPointRobotPosition.x = PosX.sensorvalue() * 1000;
		TrackPointRobotPosition.y = PosY.sensorvalue() * 1000;
		TrackPointRobotPosition.phi = Angle.sensorvalue();
		//Gui->KfieldInitTrackLine(TrackPointRobotPosition);

		//cout << "Robots angle " << mypos.theta << endl;
		//Gui->addTrackLine(mypos);
	}
	//TrackPoint RobotPositionAfter;
	//std::vector<float> RobotPos = motion->getRobotPosition(true);
	float XA = PosX.sensorvalue() * 1000;//-RobotPos[0] * 1000;
	float YA = PosY.sensorvalue() * 1000;//-RobotPos[1] * 1000;
	float AA = Angle.sensorvalue(); //RobotPos[2]+M_PI;

	//	mypos.x = XA * 1000;
	//	mypos.y = YA * 1000;
	//	mypos.theta = AA;
	cout << "          Robot Position X: " << XA << " Y: " << YA << " A(DEG): " << AA * TO_DEG << endl;

	float DX = (XA - TrackPointRobotPosition.x);
	float DY = (YA - TrackPointRobotPosition.y);
	float DR = anglediff2(AA, TrackPointRobotPosition.phi);

	MModel.Distance.val = DISTANCE_2(DX, DY);
	MModel.Direction.Edev = pow(MModel.Distance.val, 2);

	MModel.Direction.val = anglediff2(atan2(DY, DX), TrackPointRobotPosition.phi);
	MModel.Direction.Edev = 1 * TO_RAD;

	MModel.Rotation.val = DR;
	MModel.Rotation.Edev = 0.5 * TO_RAD;
	cout << "          Robot Position DX: " << DX << " DY: " << DY << " DR(DEG): " << DR * TO_DEG << endl;

	cout << "Distance.val = " << MModel.Distance.val << " Distance.Edev = " << MModel.Distance.Edev << endl;
	cout << " Direction.val(DEG) = " << MModel.Direction.val * TO_DEG << " Direction.Edev(DEG) = " << MModel.Direction.Edev * TO_DEG << endl;
	cout << "  Rotation.val(DEG) = " << MModel.Rotation.val * TO_DEG << " Rotation.Edev(DEG) = " << MModel.Rotation.Edev * TO_DEG << endl;

	TrackPointRobotPosition.x = XA;
	TrackPointRobotPosition.y = YA;
	TrackPointRobotPosition.phi = AA;

}

belief Localization::LocalizationStepSIR(KMotionModel & MotionModel, vector<KObservationModel> & Observations, double rangemaxleft, double rangemaxright) {

	cout << "SelfLocalize SIR" << endl;
	char c;
	int iterations = 1;
	//KMotionModel *MotionModelptr = findBestMotionModel(steps, MotionType, KouretesMotions, &iterations);

	//if (MotionModelptr == NULL) {
	//cout << "Warning No maching Motion Model!!!!!!!!!" << endl;
	//} else {
	//KMotionModel MotionModel = *MotionModelptr;
	//cout << "LocalizationStep MotionModel.Distance.Emean:  " << MotionModel.Distance.Emean << " MotionModel.Distance.Edev " << MotionModel.Distance.Edev << endl;
	//}

	int index[partclsNum];
	//Simple initialization

	//cin.ignore(10, '\n');
	//cin.clear();
	cout << "write something to predict particles " << endl;
	//cin >> c;
	//SpreadParticles
	if (Observations.empty()) {
		cout << "No observations ... spreading" << endl;
		SpreadParticlesCirc(SIRParticles, SpreadParticlesDeviation, rotation_deviation, 10);
	}
	if (depletions_counter > 1) {
		cout << "Depletion Counter " << depletions_counter << endl;
		SpreadParticlesCirc(SIRParticles, 100.0 * depletions_counter, 30 * TO_RAD, 50);
	}

	SpreadParticlesCirc(SIRParticles, 10.0 * depletions_counter, 10 * TO_RAD, 20);
	SpreadParticlesCirc(SIRParticles, 20.0 * depletions_counter, 2 * TO_RAD, 100);
	SpreadParticlesCirc(SIRParticles, 20.0 * depletions_counter, 45 * TO_RAD, 5);

	//	if (Observations.size() > 1)
	//		ObservationParticles(Observations, SIRParticles, 6000, 4000, 200, rangemaxleft, rangemaxright);

#ifdef ADEBUG
	for (int i = 0; i < partclsNum / 10.0; i++)
	cout << SIRParticles.Weight[i] << "  ";
#endif
	//SIR Filter
	cout << "\nPredict Iterations " << iterations << endl;

	//sleep(1);
	//Predict - Move particles according the Prediction Model
	//	if (MotionModelptr != NULL) {
	for (int i = 0; i < iterations; i++)
		Predict(SIRParticles, MotionModel);
	//	} else {
	//		SpreadParticlesCirc(SIRParticles, iterations * 100, 30 * TO_RAD, 100);
	//	}
	////#########################
	//Gui->display_particles((*this).SIRParticles);
	//Gui->save_field_to_png(count++);
	//

	//cin.ignore(10, '\n');
	//cin.clear();
	cout << "write something to display weighted - updated particles " << endl;
	//cin >> c;
	//	SleepMs(6500);
	//
	//Set semi-optimal bearing angle as the average bearing angle to the observations
	if (Observations.size() > 0)
		ForceBearing(SIRParticles, Observations);

	//Update - Using incoming observation

	Update(SIRParticles, Observations, MotionModel, partclsNum, rangemaxleft, rangemaxright);
	////#########################
	//	Gui->RevertBackupField();
	//	Gui->display_particles((*this).SIRParticles, true);
	//	Gui->save_field_to_png(count++);
	//SleepMs(200);
	//cin.ignore(10, '\n');
	//cin.clear();
	cout << "write something to display display normalized particles " << endl;
	//cin >> c;

#ifdef ADEBUG
	cout << "\nUnnormalized SIR particles " << endl;
	for (int i = 0; i < partclsNum / 10.0; i++)
	cout << SIRParticles.Weight[i] << "  ";
#endif

	//Normalize Particles  Weight in order to Resample later
	float ESS = normalize(SIRParticles.Weight, partclsNum);
	cout << "\033[01;32m \n ESS " << ESS << " Beta " << Beta << " Beta 2 " << *Beta2 << "\033[0m" << endl;

	if (ESS < Beta * 0.8) {
		cerr << "\033[01;31m \nOups SIRParticles Population Dissapeared Maybe the Robot have changed position\033[0m" << endl;
		depletions_counter++;

		cout << "Depletion Counter " << depletions_counter << endl;
	} else {
		depletions_counter = 0;
	}
#ifdef ADEBUG
	cout << "\nNormalized SIR particles  " << endl;
	for (int i = 0; i < partclsNum / 10.0; i++)
	cout << SIRParticles.Weight[i];
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
	//	Gui->RevertBackupField();
	//	Gui->display_particles((*this).SIRParticles);
	//	Gui->save_field_to_png(count++);

	//extract estimation
	partcl maxprtcl;

	//Find Max Weight
	maxprtcl.Weight = SIRParticles.Weight[0];
	maxprtcl.x = SIRParticles.x[0];
	maxprtcl.y = SIRParticles.y[0];
	maxprtcl.phi = SIRParticles.phi[0];
	SIRParticles.WeightSum = SIRParticles.Weight[0];
	for (unsigned int i = 0; i < SIRParticles.size; i++) {
		//Particles_cx += SIRParticles.x[i];
		//Particles_cy += SIRParticles.y[i];

		if (SIRParticles.Weight[i] > maxprtcl.Weight) {
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
	AgentPosition.confidence = CalculateConfidence(SIRParticles, AgentPosition);

	cout << "Probable agents position " << AgentPosition.x << ", " << AgentPosition.y << " maxprtcl W: " << maxprtcl.Weight << endl;

	//AgentPosition = RobustMean(SIRParticles, 2);
	//Complete the SIR

	//cin.ignore(10, '\n');
	//cin.clear();
	cout << "write something to resample+propagate " << endl;
	//cin >> c;
	if ((ESS < Beta || AgentPosition.confidence > 150) && !Observations.empty()) {
		Resample(SIRParticles, index, 0);
		Propagate(SIRParticles, index);
		if (depletions_counter > 4)
			depletions_counter -= 2;
	} else {
		cout << "NO need of resampling" << endl;
	}
	//TODO only one value to determine confidance, Now its only distance confidence
	AgentPosition.confidence = CalculateConfidence(SIRParticles, AgentPosition);
	////#########################
	//	Gui->RevertBackupField();
	//	Gui->display_particles((*this).SIRParticles);
	//	Gui->save_field_to_png(count++);

	//cin.ignore(10, '\n');
	//cin.clear();
	cout << "write something to display belief " << endl;
	//cin >> c;
	////#########################
	//Gui->display_belief(AgentPosition, rangemaxleft, rangemaxright, count);
	//Gui->save_field_to_png(count++);

	//cin.ignore(10, '\n');
	//cin.clear();
	cout << "write something to continue " << endl;
	//cin >> c;
	return AgentPosition;
	//AgentPosition
}

void Localization::read_messages() {
	_blk->process_messages();
/*
	if (gsm != 0)
		delete gsm;
	if (rpsm != 0)
		delete rpsm;
	if (obsm != 0)
		delete obsm;
*/
	gsm = _blk->read_state<GameStateMessage> ("GameStateMessage");
	rpsm = _blk->read_data<RobotPositionSensorMessage> ("RobotPositionSensorMessage");
	obsm = _blk->read_signal<ObservationMessage> ("ObservationMessage");

	if (rpsm != 0) {
		PosX = rpsm->sensordata(0);
		PosY = rpsm->sensordata(1);
		Angle = rpsm->sensordata(2);

		RobotPositionMotionModel( robotmovement);
	}

	if (obsm != 0) {
		KObservationModel tempOM;
		currentObservation.clear();
		//Load observations
		//ALValue ret = KObserver->call<ALValue> ("Observe");

        const ::google::protobuf::RepeatedPtrField< ::NamedObject >& Objects = obsm->regular_objects();
		string id;

		maxrangeleft = obsm->bearing_limit_left();//(float) ret[i + 1];
		maxrangeright = obsm->bearing_limit_right();//(float) ret[i + 2];
		cout << "Range Angles maxleft: " << maxrangeleft << " max right: " << maxrangeright << endl;

		for (int i = 0; i < Objects.size(); i++) {
			id = Objects.Get(i).object_name();//  (string) ret[i];
			if ((this)->KFeaturesmap.count(id) != 0) {
				tempOM.Feature = (this)->KFeaturesmap[id];

				//Make the feature

				//tempOM.Feature =
				tempOM.Distance.val = Objects.Get(i).distance();//ALValue::TALValueDouble(ret[i + 1]);
				tempOM.Bearing.val = Objects.Get(i).bearing();//ALValue::TALValueDouble(ret[i + 2]);
				currentObservation.push_back(tempOM);
				cout << "Feature seen " << tempOM.Feature.id << " Distance " << tempOM.Distance.val << " Bearing " << tempOM.Bearing.val << endl;
				//			cout << "feature"
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
	}

	Logger::Instance().WriteMsg("Localization", "read_messages ", Logger::ExtraExtraInfo);

}

int Localization::LocalizationData_Load(parts & Particles, vector<KObservationModel> & Observation, KMotionModel & MotionModel) {
	bool addnewptrs = false;
	//Fill the world with data!
	WorldInfo *WI = DebugData.mutable_world();

	//Setting my position
	WI->mutable_myposition()->set_x(AgentPosition.x);
	WI->mutable_myposition()->set_y(AgentPosition.y);
	WI->mutable_myposition()->set_phi(AgentPosition.theta);
	WI->mutable_myposition()->set_confidence(AgentPosition.confidence);

	//Setting robotPositionField X = DX, Y = DY, phi = DF

	DebugData.mutable_robotposition()->set_x(MotionModel.Distance.val);
	DebugData.mutable_robotposition()->set_y(MotionModel.Direction.val);
	DebugData.mutable_robotposition()->set_phi(MotionModel.Rotation.val);
	//	DebugData.mutable_myposition()->set_confidence(AgentPosition.confidence );

	RobotPose prtcl;

	if (DebugData.particles_size() < Particles.size)
		addnewptrs = true;
	for (unsigned int i = 0; i < Particles.size; i++) {
		if (addnewptrs)
			DebugData.add_particles();
		DebugData.mutable_particles(i)->set_x(Particles.x[i]);
		DebugData.mutable_particles(i)->set_y(Particles.y[i]);
		DebugData.mutable_particles(i)->set_phi(Particles.phi[i]);
		DebugData.mutable_particles(i)->set_confidence(Particles.Weight[i]);
	}


//	if(osbm!=null){
//		DebugData.Observations = obsm;
//	}
	return 1;
}

void * Localization::StartServer(void * astring) {
	// use this function to initialize the UDT library
	UDT::startup();

	addrinfo hints;
	addrinfo* res;

	memset(&hints, 0, sizeof(struct addrinfo));

	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	//hints.ai_socktype = SOCK_DGRAM;

	string service("9000");

	if (0 != getaddrinfo(NULL, service.c_str(), &hints, &res)) {
		cout << "illegal port number or port is busy.\n" << endl;
		return NULL;
	}

	UDTSOCKET serv = UDT::socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	// UDT Options
	//UDT::setsockopt(serv, 0, UDT_CC, new CCCFactory<CUDPBlast> , sizeof(CCCFactory<CUDPBlast> ));
	//iUDT::setsockopt(serv, 0, UDT_MSS, new int(9000), sizeof(int));
	//UDT::setsockopt(serv, 0, UDT_RCVBUF, new int(10000000), sizeof(int));
	//UDT::setsockopt(serv, 0, UDP_RCVBUF, new int(10000000), sizeof(int));

	if (UDT::ERROR == UDT::bind(serv, res->ai_addr, res->ai_addrlen)) {
		cout << "bind: " << UDT::getlasterror().getErrorMessage() << endl;
		return NULL;
	}

	freeaddrinfo(res);

	cout << "Localization server is ready at port: " << service << endl;

	if (UDT::ERROR == UDT::listen(serv, 10)) {
		cout << "listen: " << UDT::getlasterror().getErrorMessage() << endl;
		return NULL;
	}

	sockaddr_storage clientaddr;
	int addrlen = sizeof(clientaddr);

	//UDTSOCKET recver;

	while (true) {
		if (!debugmode) {
			if (UDT::INVALID_SOCK == (recver = UDT::accept(serv, (sockaddr*) &clientaddr, &addrlen))) {
				cout << "accept: " << UDT::getlasterror().getErrorMessage() << endl;
				return NULL;
			}

			char clienthost[NI_MAXHOST];
			char clientservice[NI_MAXSERV];
			getnameinfo((sockaddr *) &clientaddr, addrlen, clienthost, sizeof(clienthost), clientservice, sizeof(clientservice), NI_NUMERICHOST | NI_NUMERICSERV);
			cout << "new connection: " << clienthost << ":" << clientservice << endl;
		} else {
			sleep(5);
		}
		//#ifndef WIN32
		//		pthread_t rcvthread;
		//		pthread_create(&rcvthread, NULL, senddata, new UDTSOCKET(recver));
		//		pthread_detach(rcvthread);
		//#else
		//		CreateThread(NULL, 0, senddata, new UDTSOCKET(recver), 0, NULL);
		//#endif
	}

	UDT::close(serv);

	// use this function to release the UDT library
	UDT::cleanup();

	return NULL;
}

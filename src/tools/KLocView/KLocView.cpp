//============================================================================
// Name        : KRobotView.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C, Ansi-style
//============================================================================

#ifndef WIN32
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <netdb.h>
#else
#include <winsock2.h>
#include <ws2tcpip.h>
#include <wspiapi.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#include "../../messages/Kimage.pb.h"
#include "../../messages/VisionObservations.pb.h"
#include "../../messages/WorldInfo.pb.h"
#include "../../messages/motion.pb.h"
#include <netinet/in.h>

#include <iostream>
#include <fstream>
#include "KfieldGui.h"
//#include "KLocalization.h"
#include "../../../external/PracticalSocket/PracticalSocket.h"

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "opencv/cvaux.hpp"

#define TO_RAD 0.01745329f
#define TO_DEG 1.0/TO_RAD

using namespace std;
ofstream myfile;
parts Particles;
vector<KObservationModel> currentObservation;
char fname[80];
bool initialization = true;
KfieldGui* KLocView;
partcl robotposition;

int update_field(LocalizationData & DebugData) {
	WorldInfo *WI = DebugData.mutable_world();
	belief Belief;
	KLocView->CleanField();
	//Setting my position
	Belief.x = WI->myposition().x();
	Belief.y = WI->myposition().y();
	Belief.theta = WI->myposition().phi();
	Belief.confidence = WI->myposition().confidence();
	//Setting robotPositionField X = DX, Y = DY, phi = DF

	if(WI->balls_size()>0){
		KLocView->draw_ball2(Belief, WI->balls(0));
	}

	robotposition.x = DebugData.robotposition().x();
	robotposition.y = DebugData.robotposition().y();
	robotposition.phi = DebugData.robotposition().phi();
	//cout << " X " << robotposition.x << " X " << robotposition.y << " X " << robotposition.phi << endl;
	if (initialization) {

		KLocView->KfieldInitTrackLine(Belief);
		robotposition.x = Belief.x;
		robotposition.y = Belief.y;
		robotposition.phi = Belief.theta;
		KLocView->KfieldInitTrackLine(robotposition);
		initialization = false;
	} else {
		KLocView->addTrackLine(robotposition);
		KLocView->addTrackLine(Belief);
	}


	//Get The observations
	if (DebugData.has_observations()) {
		cout << "We have observations" << endl;
		if (DebugData.observations().has_ball())
		{
			cout << "We have ball" << endl;
			KLocView->draw_ball(Belief, DebugData.observations().ball());
		}else{
			cout << "No ball" << endl;
		}
		//else
		if (DebugData.observations().regular_objects_size() > 0) {
			cout << "We have observations " << DebugData.observations().regular_objects_size() << endl;
			vector<KObservationModel> currentObservation;
			KObservationModel observation;
			for (int o = 0; o < DebugData.observations().regular_objects_size(); o++) {
				NamedObject *obj = DebugData.mutable_observations()->mutable_regular_objects(o);
				cout << obj->object_name() << endl;
				if (obj->object_name() == "YellowLeft") {// "YellowLeft"
					observation.Feature.x = 3000;
					observation.Feature.y = 700;
				} else if (obj->object_name() == "YellowRight") { //"YellowRight"
					observation.Feature.x = 3000;
					observation.Feature.y = -700;
				} else if (obj->object_name() == "Yellow") {
					observation.Feature.x = 3000;
					observation.Feature.y = 0;
				} else if (obj->object_name() == "SkyblueLeft") {// "YellowLeft"
					observation.Feature.x = -3000;
					observation.Feature.y = -700;
				} else if (obj->object_name() == "SkyblueRight") { //"YellowRight"
					observation.Feature.x = -3000;
					observation.Feature.y = 700;
				} else if (obj->object_name() == "Skyblue") {
					observation.Feature.x = -3000;
					observation.Feature.y = 0;
				} else {
					continue;
				}

				observation.Feature.id = obj->object_name();
				observation.Bearing.val = obj->bearing();
				observation.Distance.val = obj->distance() * 1000;
				observation.Distance.Edev = sqrt(obj->distance_dev()) * 1000;
				currentObservation.push_back(observation);
			}
			KLocView->DrawObservations(Belief, currentObservation);
		}
		//KLocView->CleanField();
	}else{
		cout << "No Observations" <<endl;
	}
	//KLocView->BackupField();
	KLocView->draw_Trackpoint(robotposition, 20 * TO_RAD, -20 * TO_RAD);
	KLocView->draw_belief(Belief, 20 * TO_RAD, -20 * TO_RAD, 0);

	//Get the particles !!!
	if (DebugData.particles_size() > 0) {

		if ((unsigned int) DebugData.particles_size() != Particles.size) {
			if (Particles.x)
				delete [] Particles.x;
			if (Particles.y)
				delete [] Particles.y;
			if (Particles.phi)
				delete [] Particles.phi;
			if (Particles.Weight)
				delete [] Particles.Weight;
			Particles.size = DebugData.particles_size();

			//		Particles.size = 4;
			Particles.x = new double[Particles.size];
			Particles.y = new double[Particles.size];
			Particles.Weight = new double[Particles.size];
			Particles.phi = new double[Particles.size];

		}

		for (unsigned int i = 0; i < Particles.size; i++) {
			Particles.x[i] = DebugData.particles(i).x();
			Particles.y[i] = DebugData.particles(i).y();
			Particles.phi[i] = DebugData.particles(i).phi();
			Particles.Weight[i] =  DebugData.particles(i).confidence();
		}
		KLocView->draw_particles(Particles);
	}

	return 0;
}

int receive_and_send_loop(TCPSocket *sock) {

	header incommingheader;
	header outgoingheader;
	LocalizationData DebugData;

	MotionWalkMessage wmot;
	wmot.set_command("setWalkTargetVelocity");
	wmot.add_parameter(0.0f);
	wmot.add_parameter(0.0f);
	wmot.add_parameter(0.0f);
	wmot.add_parameter(0.0f);

	KfieldGui::wmot.CopyFrom(wmot);
	int size;
	int ssize, rsize;
	int ss, rs;
	//int alreadyparsedbytes = 0;
	//char response;
	//int lastmotioncounter = 0;
	char * data = new char[993604];
	int count = 0;
	bool sendmotion = true;
	float x, y, th, f;

//	Particles.size = 2;
//	Particles.x = new double(Particles.size);
//	Particles.y = new double(Particles.size);
//	Particles.Weight = new double(Particles.size);
//	Particles.phi = new double(Particles.size);

	while (true) {
		sendmotion = false;
		//Send A command First
		//Get key pressed and decide witch command to send! ...
		outgoingheader.Clear();
		outgoingheader.set_nextmsgname("Just A command");
		outgoingheader.set_nextmsgbytesize(-1); //-1 means nothing to send

		if (KLocView->state == READY_TO_SEND_SINGLE) {
			outgoingheader.set_nextmsgname("SetBelief");
			outgoingheader.set_nextmsgbytesize(KfieldGui::pose1.ByteSize());
		} else {

			if (KfieldGui::keypressed == 27)
				outgoingheader.set_nextmsgname("Stop");

			if (KfieldGui::state == FINISH) {
				if (KfieldGui::keypressed == 'm') {
					outgoingheader.set_nextmsgname("Walk");
					std::cin.clear();
					//char nextChar;

					//					while (nextChar != '\n' && nextChar != EOF) {
					//						nextChar = cin.get();
					//					}
					cout << " Do you want to repeat the last motion " << endl;
					cout << " X: " << x << " Y: " << y << " PHI " << th << " F " << f << " (type Y to repeat)" << endl;
					string s;
					cin >> ws; // skip any leading whitespace
					getline(cin, s);

					if (!(toupper(s[0]) == 'Y')) {
						cout << "Please Give X Y Theta Frequency : " << flush;
						cin >> x >> y >> th >> f;
					}
					cout << "X " << x << " Y " << y << " Th " << th << endl;
					wmot.set_parameter(0, x);
					wmot.set_parameter(1, y);
					wmot.set_parameter(2, th);
					wmot.set_parameter(3, f); //// frequency 1?
					outgoingheader.set_nextmsgbytesize(wmot.ByteSize());
					partcl tmp(0, 0, 0, 0);

					KLocView->KfieldInitTrackLine(tmp);
					KLocView->draw_Trackpoint(tmp, 0.5, -0.5);
					robotposition = tmp;
					KfieldGui::robotStartpose = tmp;
					KfieldGui::wmot.CopyFrom(wmot);
					sendmotion = true;

				}
				if (KfieldGui::keypressed == ' ') {
					outgoingheader.set_nextmsgname("StopWalk");
					wmot.set_parameter(0, 0);
					wmot.set_parameter(1, 0);
					wmot.set_parameter(2, 0);
					wmot.set_parameter(3, 0);
					outgoingheader.set_nextmsgbytesize(wmot.ByteSize());
					KfieldGui::robotEndpose = robotposition;
					sendmotion = true;
				}
			}
		}
		///Send header size in network byte order
		size = outgoingheader.ByteSize();
		int sendsize = htonl(size);
		sock->send(&sendsize, sizeof(uint32_t));
		cout << " header size " << size << endl;
		outgoingheader.SerializeToArray(data, size);
		cout << " key " << (char) KfieldGui::keypressed << " State " << KfieldGui::state << endl;
		if (KfieldGui::state == READY_TO_SEND_SINGLE) {
			KfieldGui::pose1.SerializeToArray(data + size, KfieldGui::pose1.ByteSize());
			size += KfieldGui::pose1.ByteSize();
			KfieldGui::state = FINISH;
			KLocView->KfieldInitTrackLine(KLocView->tempparticl);
			KLocView->draw_Trackpoint(KLocView->tempparticl, 0.5, -0.5);
			robotposition = KLocView->tempparticl;
		} else {
			if (KfieldGui::state == FINISH)
				if (sendmotion) {
					wmot.SerializeToArray(data + size, wmot.ByteSize());
					size += wmot.ByteSize();
					cout << " Send additionaly bytes " << wmot.ByteSize() << endl;
					if (KfieldGui::keypressed == ' ')
						KfieldGui::state = TWOPOINTSELECTION_1;
				}

		}

		for (ss = ssize = 0; ssize < size; ssize += ss)
			if ((ss = sock->send(data + ssize, size - ssize)) < 0) {
				cout << " Send  error " << endl;
				break;
			}

		if (KfieldGui::keypressed == 27)
			return 0;

		KfieldGui::keypressed = -1;

		//cout << "Sended mesage Size " << ssize << endl;

		//Now its time to receive!

		incommingheader.Clear();
		sock->recv(&size, sizeof(uint32_t));
		size = ntohl(size);

		for (rs = rsize = 0; rsize < size; rsize += rs)
			if ((rs = sock->recv(data + rsize, size - rsize)) < 0) {
				cout << "receive error" << endl;
				break;
			}

		incommingheader.ParseFromArray(data, size);
		size = incommingheader.nextmsgbytesize();

		for (rs = rsize = 0; rsize < size; rsize += rs)
			if ((rs = sock->recv(data + rsize, size - rsize)) < 0) {
				cout << "receive error" << endl;
				break;
			}

		DebugData.Clear();
		if (!DebugData.ParsePartialFromArray(data, incommingheader.nextmsgbytesize())) {
			cout << "Unable to parse " << incommingheader.nextmsgbytesize() << endl;
		} else {
			//			cout << " Parsed message " << DebugData.ByteSize() << " bytes " << endl;
			update_field(DebugData);
		}

		if (count < 1)
			;

		count++;
	}

	delete[] data;

	return 1;

}

int main(int argc, char* argv[]) {
	if (2 > argc) {
		cout << "usage: " << argv[0] << " IP (PORT)" << endl;
		return 0;
	}
	string servAddress = argv[1];
	//char *echoString = argv[2];   // Second arg: string to echo
	//int echoStringLen = strlen(echoString);   // Determine input length
	unsigned short echoServPort = ((argc == 3) ? atoi(argv[2]) : 9001);

	try {
		// Establish connection with the echo server
		TCPSocket sock(servAddress, echoServPort);
		KLocView = new KfieldGui();

		KLocView->display_Gui();//Start the Gui
		KLocView->BackupField();
		receive_and_send_loop(&sock);
	} catch (SocketException &e) {
		cerr << e.what() << endl;
		exit(1);
	}

	return EXIT_SUCCESS;
}


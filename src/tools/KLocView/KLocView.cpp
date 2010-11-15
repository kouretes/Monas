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
#ifndef WIN32
void* monitor(void*);
#else
DWORD WINAPI monitor(LPVOID);
#endif

using namespace std;
ofstream myfile;
parts Particles;
vector<KObservationModel> currentObservation;
char fname[80];
bool initialization = true;
KfieldGui* KLocView;

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
	partcl robotposition;
	robotposition.x = DebugData.robotposition().x();
	robotposition.y = DebugData.robotposition().y();
	robotposition.phi = DebugData.robotposition().phi();

	if (initialization) {
		KLocView->KfieldInitTrackLine(robotposition);
		KLocView->KfieldInitTrackLine(Belief);
	} else {
		KLocView->addTrackLine(robotposition);
		KLocView->addTrackLine(Belief);
	}
	//Get The observations
	if (DebugData.has_observations()) {
		if (DebugData.observations().has_ball())
			KLocView->draw_ball(Belief, DebugData.observations().ball());
		//else
		//KLocView->CleanField();
	}
	//KLocView->BackupField();
	KLocView->draw_Trackpoint(robotposition, 20 * TO_RAD, -20 * TO_RAD);
	KLocView->draw_belief(Belief, 20 * TO_RAD, -20 * TO_RAD, 0);

	//Get the particles !!!
	if (DebugData.particles_size() > 0) {
		if (DebugData.particles_size() != Particles.size) {
			if (Particles.x)
				delete Particles.x;
			if (Particles.y)
				delete Particles.y;
			if (Particles.phi)
				delete Particles.phi;
			if (Particles.Weight)
				delete Particles.Weight;

			Particles.x = new double(Particles.size);
			Particles.y = new double(Particles.size);
			Particles.Weight = new double(Particles.size);
			Particles.phi = new double(Particles.size);
		}

		for (unsigned int i = 0; i < Particles.size; i++) {
			Particles.x[i] = DebugData.particles(i).x();
			Particles.y[i] = DebugData.particles(i).y();
			Particles.phi[i] = DebugData.particles(i).phi();
			Particles.Weight[i] = DebugData.particles(i).confidence();
		}
	}

	return 0;
}

int receive_and_send_loop(TCPSocket *sock) {

	header incommingheader;
	header outgoingheader;
	LocalizationData DebugData;

	int size;
	int ssize, rsize;
	int ss,rs;

	//int maxheadersize = 200;
	//int headersize = maxheadersize;
	int alreadyparsedbytes = 0;

	char * data = new char[993604];

	int count = 0;
	while (true) {


		//Send A command First
		//Get key pressed and decide witch command to send! ...

		outgoingheader.set_nextmsgname("Just A command");

		if (KLocView->state == READY_TO_SEND_SINGLE) {
			outgoingheader.set_nextmsgname("SetBelief");
			outgoingheader.set_nextmsgbytesize(KfieldGui::pose1.ByteSize());
		} else {
			outgoingheader.set_nextmsgbytesize(-1); //-1 means nothing to send
		}

		if (KLocView->keypressed == 27)
			outgoingheader.set_nextmsgname("Stop");

		if (KLocView->keypressed == 'o')
			KLocView->state = ONEPOINTSELECTION;
		if (KLocView->keypressed == 't')
			KLocView->state = TWOPOINTSELECTION_1;

		size = outgoingheader.ByteSize();
		cout << "Sended header mesage Size " << size << endl;
		int sendsize = htonl(size);
		sock->send(&sendsize, sizeof(uint32_t));
		outgoingheader.SerializeToArray(data, size);
		if (KfieldGui::state == READY_TO_SEND_SINGLE) {
			KfieldGui::pose1.SerializeToArray(data + size, KfieldGui::pose1.ByteSize());
			size += KfieldGui::pose1.ByteSize();
			KfieldGui::state = FINISH;
			KLocView->KfieldInitTrackLine(KLocView->tempparticl);
			KLocView->draw_Trackpoint(KLocView->tempparticl, 0.5, -0.5);
		}

		for (ss = ssize = 0; ssize < size; ssize += ss)
			if ((ss = sock->send(data + ssize, size - ssize)) < 0) {
				cout << " Send  error " << endl;
				break;
			}

		if (KLocView->keypressed == 27)
			return 0;

		cout << "Sended mesage Size " << ssize << endl;

		//Now its time to receive!

		incommingheader.Clear();
		sock->recv(&size, sizeof(uint32_t));
		size = ntohl(size);
		//ssize = 0;
		//while (ssize < size) {
		for (rs = rsize = 0; rsize < size; rsize += rs)
			if ((rs = sock->recv(data + rsize, size - rsize)) < 0) {
				cout << "receive error" << endl;
				break;
			}
		//ssize += ss;

		cout << "Arrived " << rsize << " Bytes header size must be " << incommingheader.mysize() << " Reparsing " << endl;

		incommingheader.ParseFromArray(data, incommingheader.mysize());
		incommingheader.DiscardUnknownFields();

		alreadyparsedbytes = incommingheader.ByteSize();

		cout << "already parsed bytes " << alreadyparsedbytes << endl;

		size = incommingheader.nextmsgbytesize();

		cout << "Waiting for " << incommingheader.nextmsgname() << " Size " << size << " Bytes :" << size << endl;

		for (rs = rsize = 0; rsize < size; rsize += rs)
			if ((rs = sock->recv(data + rsize, size - rsize)) < 0) {
				cout << "receive error" << endl;
				break;
			}

		cout << "Received " << rsize << endl;

		DebugData.Clear();
		if (!DebugData.ParseFromArray(data, incommingheader.nextmsgbytesize())) {
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
	puts("Hello World!!!");

	if ((3 != argc) || (0 == atoi(argv[2]))) {
		cout << "usage: appclient server_ip server_port" << endl;
		return 0;
	}
	string servAddress = argv[1];
	//char *echoString = argv[2];   // Second arg: string to echo
	//int echoStringLen = strlen(echoString);   // Determine input length
	unsigned short echoServPort = (argc == 3) ? atoi(argv[2]) : 9000;

	//if (0 != getaddrinfo(argv[1], argv[2], &hints, &peer)) {
	//	cout << "incorrect server/peer address. " << argv[1] << ":" << argv[2] << endl;
	//	return 0;
	//}

	// connect to the server, implict bind
	//if (UDT::ERROR == UDT::connect(client, peer->ai_addr, peer->ai_addrlen)) {
	//cout << "connect: " << UDT::getlasterror().getErrorMessage() << endl;
	//return 0;
	//}

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


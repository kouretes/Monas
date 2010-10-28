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

KfieldGui* KLocView;

int update_field(LocalizationData & DebugData) {
	WorldInfo *WI = DebugData.mutable_world();
	belief Belief;
	KLocView->RevertBackupField();
	//Setting my position
	Belief.x = WI->myposition().x();
	Belief.y = WI->myposition().y();
	Belief.theta = WI->myposition().phi();
	Belief.confidence = WI->myposition().confidence();
	KLocView->addTrackLine(Belief);
	//Setting robotPositionField X = DX, Y = DY, phi = DF
	partcl robotposition;
	robotposition.x = DebugData.robotposition().x();
	robotposition.y = DebugData.robotposition().y();
	robotposition.phi = DebugData.robotposition().phi();
	KLocView->addTrackLine(robotposition);

	//Get The observations
	if (DebugData.has_observations()) {
		;//Write some thing here
	}
	KLocView->BackupField();
	KLocView->display_Trackpoint(robotposition, 20*TO_RAD, -20*TO_RAD);
	KLocView->display_belief(Belief,20*TO_RAD, -20*TO_RAD, 0);

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

int receive_and_send_loop( TCPSocket *sock) {

	header incommingheader;
	header outgoingheader;

//	incommingheader.Clear();
//	outgoingheader.Clear();
	LocalizationData DebugData;

	int size;

	int maxheadersize = 200;
	int headersize = maxheadersize;
	int alreadyparsedbytes = 0;

	char * data = new char[253604];

	int count = 0;
	while (true) {
		int ssize;
		int ss;
		ssize = 0;

		//Send A command First
		//Get key pressed and decide witch command to send! ...

		outgoingheader.set_nextmsgname("Just A command");
		if(KLocView->keypressed==27)
			outgoingheader.set_nextmsgname("Stop");
		outgoingheader.set_nextmsgbytesize(-1); //-1 means nothing to send
		outgoingheader.set_mysize(size = outgoingheader.ByteSize());
		while (size != outgoingheader.ByteSize()) {
			outgoingheader.set_mysize(size = outgoingheader.ByteSize());
		}

		outgoingheader.SerializeToArray(data, size);

		cout << "For send mesage Size " << size << endl;
//		for (int j = 0; j < size; j++) {
//			cout << (int) data[j] << " ";
//		}
//		cout << endl;
//
//		outgoingheader.SerializeToString(&buff2);
//		cout << " Serilized string " << buff2 << "Size " << buff2.length() << endl;
		while (ssize < size) {
			if ((ss = sock->send(data + ssize, size - ssize))<0) {
				cout << " Send  error "<< endl;
				break;
			}
			ssize += ss;
		}
		if(KLocView->keypressed==27)
			return 0;
		cout << "Sended mesage Size " << ssize << endl;
		for (int j = 0; j < ssize; j++) {
			cout << (int) data[j] << " ";
		}
		cout << endl;

		//Now its time to receive!
		ssize = 0;
		size = maxheadersize;

		cout << " Waiting for header size " << maxheadersize << endl;
		incommingheader.Clear();
		while (ssize < size) {
			if ((ss=sock->recv( data + ssize, size - ssize)) < 0) {
					cout << "receive error" << endl;
				break;
			}
			ssize += ss;
			cout << "Arrived " << ssize  << "Try to parse "<< endl;
			if (incommingheader.ParsePartialFromArray(data, ssize))//.ParseFromString(string(data, ssize)))
			{
				headersize = ssize;
				break;
			}
		}
		cout << "Arrived " << ssize << " Bytes header size must be "<<incommingheader.mysize() << " Reparsing "  << endl;
		//incommingheader.ParseFromString(string(data, size))

		incommingheader.ParseFromArray(data, incommingheader.mysize());
		incommingheader.DiscardUnknownFields();

		alreadyparsedbytes = incommingheader.ByteSize();

		cout << "already parsed bytes " << alreadyparsedbytes << " Bytes, Already image bytes arrived" << (headersize - alreadyparsedbytes) << endl;

		size = incommingheader.nextmsgbytesize() - (headersize - alreadyparsedbytes);//(imgheader->width() * imgheader->height() * 2);

		cout << "Waiting for " << incommingheader.nextmsgname() << " Size " << size << " Bytes :" << size + (headersize - alreadyparsedbytes) << endl;

		bool parsed = false;

		std::string buf;

		ssize = 0;
		cout << " Needed to receive more " << size << " bytes" << endl;
		while (ssize < size) {
			if ((ss=sock->recv( data + headersize + ssize, size - ssize)) < 0) {
					cout << "receive error" << endl;
				break;
			} else {
				ssize += ss;
			}
		}

		cout << "Received " << ssize << endl;
		if (!parsed) {
			DebugData.Clear();
			if (!DebugData.ParsePartialFromString(string(data + alreadyparsedbytes, incommingheader.nextmsgbytesize()))) {
				cout << "Unable to parse " << incommingheader.nextmsgbytesize() << endl;
			} else {
				parsed = true;
				cout << " Parsed message " << DebugData.ByteSize() << " bytes " << endl;
				update_field(DebugData);
			}
		}

		//cout << "Display image size " << size << endl;

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
  } catch(SocketException &e) {
    cerr << e.what() << endl;
    exit(1);
  }


	return EXIT_SUCCESS;
}


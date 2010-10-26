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
#include "Kimage.pb.h"
#include "WorldInfo.pb.h"



#include <iostream>
#include <fstream>
#include "PracticalSocket.h"

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "opencv/cvaux.hpp"


IplImage* fIplImageHeader = NULL;
char * imageDeinterlaced = NULL;

int receive_and_send_loop( TCPSocket *sock);
using namespace std;
ofstream myfile;

char fname[80];
int Displayimage(char * dataPointer, int size, bool isYUV) {
	int count = 0;
	int width = 320;//640;//
	int height = 240;//480;//

	if (size == 921600 || size == 614400) //640*480*3
	{
		width = 640;
		height = 480;
	} else if (size == 230400 || size == 153600) {
		width = 320;
		height = 240;
	}

	int index, indexD;
	fIplImageHeader = cvCreateImageHeader(cvSize(width, height), 8, 3);

	if (isYUV == 0) {
		if (imageDeinterlaced == NULL)
			imageDeinterlaced = new char[width * height * 3];

		for (int j = 0; j < height; j++)
			for (int i = 0; i < width; i++) {
				index = j * 3 * width + i * 3;
				indexD = j * 2 * (width);
				imageDeinterlaced[index] = dataPointer[indexD + i * 2]; //Y
				imageDeinterlaced[index + 1] = dataPointer[indexD + (i / 2) * 4 + 1]; //U
				imageDeinterlaced[index + 2] = dataPointer[indexD + (i / 2) * 4 + 3]; //V
			}
		fIplImageHeader->imageData = imageDeinterlaced;
	} else {
		fIplImageHeader->imageData = dataPointer;
	}

	CvImage yuv, rgb;
	yuv.attach(fIplImageHeader, true);
	rgb = yuv.clone();

	cvCvtColor(yuv, rgb, CV_YCrCb2RGB); // simply call OpenCV functions and pass the class instances there
	//cvCvtColor(yuv, rgb, CV_BGR2RGB); // simply call OpenCV functions and pass the class instances there
	cvShowImage("Image", rgb);

#ifdef SEGCOMPLETED
	char sImage[height][width];
	//	cout << "Starting Segmentation " << endl;

	for (int j = 1; j < (height - 2); j++) {
		for (int i = 1; i < width - 1; i++) {

			sImage[j][i] = sgm->getColor(j, i, (uInt8 *) dataPointer, true);

			char k = sImage[j][i];

			switch (k) {
				//	Red, Blue, Green, Cyan, Yellow, Orange, White, Black
				case 0:
				segImage[j * 3 * width + i * 3 + 2] = 0;
				segImage[j * 3 * width + i * 3 + 1] = 0;
				segImage[j * 3 * width + i * 3] = 0;
				break;
				case 1://RED
				segImage[j * 3 * width + i * 3 + 2] = 255;
				segImage[j * 3 * width + i * 3 + 1] = 0;
				segImage[j * 3 * width + i * 3] = 0;
				break;
				case 2://BlUE
				segImage[j * 3 * width + i * 3 + 2] = 0;
				segImage[j * 3 * width + i * 3 + 1] = 0;
				segImage[j * 3 * width + i * 3] = 255;
				break;
				case 3://GREEN
				segImage[j * 3 * width + i * 3 + 2] = 0;
				segImage[j * 3 * width + i * 3 + 1] = 255;
				segImage[j * 3 * width + i * 3] = 0;
				break;
				case 4://SkyBlue
				segImage[j * 3 * width + i * 3 + 2] = 0;
				segImage[j * 3 * width + i * 3 + 1] = 107;
				segImage[j * 3 * width + i * 3] = 228;
				break;
				case 5://Yellow
				segImage[j * 3 * width + i * 3 + 2] = 255;
				segImage[j * 3 * width + i * 3 + 1] = 255;
				segImage[j * 3 * width + i * 3] = 0;
				break;
				case 6://Orange
				segImage[j * 3 * width + i * 3 + 2] = 255;
				segImage[j * 3 * width + i * 3 + 1] = 180;
				segImage[j * 3 * width + i * 3] = 0;
				break;
				case 7:
				segImage[j * 3 * width + i * 3 + 2] = 255;
				segImage[j * 3 * width + i * 3 + 1] = 255;
				segImage[j * 3 * width + i * 3] = 255;
				break;
				case 8:
				segImage[j * 3 * width + i * 3 + 2] = 0;
				segImage[j * 3 * width + i * 3 + 1] = 0;
				segImage[j * 3 * width + i * 3] = 0;
				default:
				segImage[j * 3 * width + i * 3 + 2] = 0;
				segImage[j * 3 * width + i * 3 + 1] = 0;
				segImage[j * 3 * width + i * 3] = 0;

			}
			////if (savesegmented == true)
			//	putc(sImage[j][i] * 30 + 10, fp2);
		}

	}
	//std::cout << "Segmentation Completed... pray!\n";

	//if (savesegmented == true) {
	//	fflush(fp2);
	//	fclose(fp2);
	//	savesegmented = false;
	//}

	fIplSegImageHeader = cvCreateImageHeader(cvSize(width, height), IPL_DEPTH_8U, 3);
	fIplSegImageHeader->imageData = (char *) segImage;

	CvImage seg(fIplSegImageHeader), colorseg;

	//colorseg = seg.clone();
	//cvCvtColor(seg, colorseg, CV_GRAY2RGB); // simply call OpenCV functions and pass the class instances there
	seg.show("SegmentationTest");

#endif

	char c = cvWaitKey(10);

	if ((char) c == 27) {

		cvDestroyAllWindows();
		exit(0);
	}
	switch ((char) c) {
	case 'c':
		FILE * fp;
		sprintf(fname, "saved_%02d.yuyv", count);

		printf("Writing out file %s\n", fname);

		if ((fp = fopen(fname, "wb")) == NULL) {
			printf("Could not open file %s for writing.\n", fname);
			return 0;
		}
		//
		fwrite(dataPointer, width * height * 2, sizeof(char), fp);

		//
		fflush(fp);
		fclose(fp);
		/*
		 ///Save pnm
		 sprintf(fname, "seg-%02d.pnm", count);

		 if ((fp = fopen(fname, "wb")) == NULL) {
		 //logger->error("blobengine", "could not create proxy to NaoCam")
		 //msg = msg + "Could not open file " + fname + " for writing.\n";

		 printf("Could not open file %s for writing.\n", fname);
		 return 0;
		 }
		 fprintf(fp, "P6\n%d %d\n255\n", width, height);
		 fwrite(segImage, width * height * 3, sizeof(char), fp);
		 fflush(fp);
		 fclose(fp);

		 sprintf(fname, "datasaved_%02d.txt", count++);

		 //	  myfile.open ("example.txt");
		 //ofstream myfile;
		 myfile.open(fname);
		 if (myfile.is_open()) {
		 myfile << count << " : ";
		 for (int v = 0; v < 12; v++) {
		 myfile << oldCameraValues[v] << " ";

		 }
		 cout << endl;
		 myfile.flush();
		 myfile.close();
		 } else
		 cout << "Unable to open file";
		 break;*/
	case 'r':

	default:
		;
	}

	return 0;
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





  try {
    // Establish connection with the echo server
    TCPSocket sock(servAddress, echoServPort);
		cvNamedWindow("Image", CV_WINDOW_AUTOSIZE);//Start the Gui

		receive_and_send_loop(&sock);
  } catch(SocketException &e) {
    cerr << e.what() << endl;
    exit(1);
  }


	return EXIT_SUCCESS;
}


int receive_and_send_loop( TCPSocket *sock){

	char * newdata;

	KRawImageHeader * imgheader = new KRawImageHeader();
	//KRawImageHeader * imgheader2 = new KRawImageHeader();

	KRawImage img;


	int size = (imgheader->width() * imgheader->height() * 2); //example size
	imgheader->set_bytes(size);

	int maxheadersize = 200; //max header size//imgheader->ByteSize();
	int alreadyparsedbytes = 0;
	//cout << "View imgheader->ByteSize() " << maxheadersize << endl;
	char * data = new char[253604];

	int count = 0;
	while (true) {
		int ssize;
		int ss;
		ssize = 0;
		size = maxheadersize;

		imgheader->Clear();
		//cout << "Waiting for " << size << " Bytes " << endl;
		while (ssize < size) {
			if (UDT::ERROR == (ss = UDT::recv(client, data + ssize, size - ssize, 0))) {
				cout << "recv:" << UDT::getlasterror().getErrorMessage() << endl;
				break;
			}
			ssize += ss;
		}
		cout << "Arrived " << ssize << " Bytes " << endl;
		imgheader->ParseFromString(string(data, size));
		imgheader->DiscardUnknownFields();
		alreadyparsedbytes = imgheader->ByteSize();

		cout << "alreadyparsedbytes " << alreadyparsedbytes << " Bytes, Already image bytes " << (maxheadersize-alreadyparsedbytes) << endl;

		size = imgheader->bytes() - (maxheadersize-alreadyparsedbytes);//(imgheader->width() * imgheader->height() * 2);
		cout << "View  2 imgheader->bytes() " << size << endl;
		cout << " Width " << imgheader->width() <<  " Height " << imgheader->height() <<  endl;

		cout << "Waiting for " << size << " Bytes :" <<  size + (maxheadersize-alreadyparsedbytes) <<  endl;

		bool parsed = false;

		std::string buf;

		//if (count == 0) {
		ssize = 0;
		while (ssize < size) {
			if (UDT::ERROR == (ss = UDT::recv(client, data+maxheadersize + ssize, (size - ssize), 0))) {
				cout << "recv:" << UDT::getlasterror().getErrorMessage() << endl;
				//usleep(20000);
				//	break;
			} else {
				//	cout << ssize << " dddss=" << ss << "  " << "size - ssize " << (size - ssize) << endl;
				ssize += ss;
				//cout << ssize << " eeee  " << endl;
			}

		}

		cout << "Received " << ssize << endl;
		//usleep(10000);
		if (!parsed) {
			img.Clear();
			if (!img.ParsePartialFromString(string(data+alreadyparsedbytes, imgheader->bytes() ))) {
				cout << "Unable to parse " << imgheader->bytes()  << endl;
			} else {
				parsed = true;
				cout << " Parsed message " << img.ByteSize() << " bytes " << endl;
				//cout << data << endl;
			}
		}

		//sleep(2);

		//		//	cout << "Setting camera Parameters count " << count << endl;
		//		/*if (++count == 50) {
		//		 count = 0;
		//
		//		 }*/
		//		//if (ssize < size)
		//		//	break;
		//		//}
		cout << "Display image size " << size << endl;
		if (count < 1)
			newdata = new char[imgheader->width() * imgheader->height() * 2];
		img.imagerawdata().copy(newdata, imgheader->width() * imgheader->height() * 2);
		Displayimage(newdata, size, false);

		count++;
	}
	cvDestroyWindow("Image");
	UDT::close(client);
	//UDT::close( client2);
	delete[] data;

	// use this function to release the UDT library
	UDT::cleanup();

	return EXIT_SUCCESS;
}

#ifndef WIN32
void* monitor(void* s)
#else
DWORD WINAPI monitor(LPVOID s)
#endif
{
	UDTSOCKET u = *(UDTSOCKET*) s;

	UDT::TRACEINFO perf;

	cout << "RecvRate(Mbit/s)\tRTT(ms)\tCWnd\tPktSndPeriod(us)\tSendACK\tSendNAK" << endl;

	while (true) {
#ifndef WIN32
		sleep(1);
#else
		Sleep(1000);
#endif

		if (UDT::ERROR == UDT::perfmon(u, &perf)) {
			cout << "perfmon: " << UDT::getlasterror().getErrorMessage() << endl;
			break;
		}

		cout << perf.mbpsRecvRate << "\t\t" << perf.msRTT << "\t" << perf.pktCongestionWindow << "\t" << perf.usPktSndPeriod << "\t\t\t" << perf.pktSentACK << "\t"
				<< perf.pktSentNAK << endl;
	}

#ifndef WIN32
	return NULL;
#else
	return 0;
#endif
}

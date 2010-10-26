/*
 * KfieldGui.cpp
 *
 *  Created on: 12 Μαρ 2010
 *      Author: trs
 */

#include "KfieldGui.h"
#include "Kutils.h"
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cxcore.h>

IplImage * KfieldGui::field;
char KfieldGui::wndname[10];
short KfieldGui::Waitingforkey;
short KfieldGui::keypressed;
pthread_mutex_t KfieldGui::lock;

#define NAO_2008
CvScalar KfieldGui::random_color() {
	CvRNG rng(-1);
	int icolor = cvRandInt(&rng);
	return CV_RGB(icolor & 255, (icolor >> 8) & 255, (icolor >> 16) & 255);
}
KfieldGui::KfieldGui(KLocalization & KLocObj) {
	KfieldGui();
	display_particles(KLocObj.SIRParticles);
}
KfieldGui::KfieldGui() {

	Waitingforkey = 0;
	strcpy(wndname, "KField ;)");
	// TODO Auto-generated constructor stub

	cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, hScale, vScale, 0, lineWidth, CV_AA);
	make_field(&cleanfield);
	field = cvCreateImage(cvSize(cleanfield->width, cleanfield->height), 8, 3);
	field2 = cvCreateImage(cvSize(cleanfield->width, cleanfield->height), 8, 3);
	thread_pid = -2;
	cvCopy(cleanfield, field, NULL);

	lock = PTHREAD_MUTEX_INITIALIZER;
	//cvNamedWindow(wndname, CV_WINDOW_AUTOSIZE);

	//cvShowImage(wndname, field);
	//cvWaitKey(0);
	//cvInitFont(&font, CV_FONT_HERSHEY_PLAIN, hScale, vScale, 0, lineWidth, CV_AA);

	mypospoint_old.x = (0 + (2 * 675.0 + 6050) / 2.0) / SCALE;
	mypospoint_old.y = (0 + (2 * 675.0 + 4050) / 2.0) / SCALE;
#ifdef WEBOTS

	trackpospoint_old.x = (0 + (2 * 675.0 + 6050) / 2.0) / SCALE;
	trackpospoint_old.y = (0 + (2 * 675.0 + 4050) / 2.0) / SCALE;
#endif
}

KfieldGui::~KfieldGui() {
	// TODO Auto-generated destructor stub
}
void KfieldGui::display_Gui() {
	if (thread_pid == -2) {
		cout << "Creating GUI thread" << endl;
		int *fps = new int();
		*fps = 10;
		cvNamedWindow(wndname, 1);
		thread_pid = pthread_create(new pthread_t, NULL, KfieldGui::display_field, fps);
	}
}

void KfieldGui::DrawObservations(vector<KObservationModel> & currentObservation) {
	CvPoint pt1, pt2;

	pthread_mutex_lock(&lock);
	for (unsigned int i = 0; i < currentObservation.size(); i++) {
		cout << "Plot  the " << currentObservation[i].Feature.id << "X " << currentObservation[i].Feature.x << " Y " << currentObservation[i].Feature.y << endl;
		pt1.x = (currentObservation[i].Feature.x + (2 * 675.0 + 6050) / 2.0) / SCALE;
		pt1.y = (-currentObservation[i].Feature.y + (2 * 675.0 + 4050) / 2.0) / SCALE;

		cvCircle(field, pt1, currentObservation[i].Distance.val / SCALE, (strchr(currentObservation[i].Feature.id.c_str(), 'Y')) ? CV_RGB(245, 245, 0) : CV_RGB(10, 10, 250), 0, CV_AA, 0);

		pt2.x = pt1.x - (currentObservation[i].Distance.val / SCALE) * cos(currentObservation[i].Bearing.val);
		pt2.y = pt1.y - (currentObservation[i].Distance.val / SCALE) * sin(currentObservation[i].Bearing.val);

		cvLine(field, pt1, pt2, (strchr(currentObservation[i].Feature.id.c_str(), 'Y')) ? CV_RGB(245, 245, 0) : CV_RGB(20, 20, 250), 0, CV_AA, 0);

		//grammi apo to trackpoint sto feature ... TODO

	}
	pthread_mutex_unlock(&lock);

}

void KfieldGui::KfieldInitTrackLine(belief mypos) {
	mypospoint.x = (mypos.x + (2 * 675.0 + 6050) / 2.0) / SCALE;
	mypospoint.y = (-mypos.y + (2 * 675.0 + 4050) / 2.0) / SCALE;
	mypospoint_old = mypospoint;
}

void KfieldGui::KfieldInitTrackLine(partcl TrackPoint) {
	trackpospoint.x = (TrackPoint.x + (2 * 675.0 + 6050) / 2.0) / SCALE;
	trackpospoint.y = (-TrackPoint.y + (2 * 675.0 + 4050) / 2.0) / SCALE;
	trackpospoint_old = trackpospoint;
}

void KfieldGui::addTrackLine(belief mypos) {
	mypospoint.x = (mypos.x + (2 * 675.0 + 6050) / 2.0) / SCALE;
	mypospoint.y = (-mypos.y + (2 * 675.0 + 4050) / 2.0) / SCALE;

	pthread_mutex_lock(&lock);

	cvLine(field, mypospoint_old, mypospoint, CV_RGB(250, 30, 0), 1, CV_AA, 0);
	cvLine(cleanfield, mypospoint_old, mypospoint, CV_RGB(250, 30, 0), 1, CV_AA, 0);
	mypospoint_old = mypospoint;
	pthread_mutex_unlock(&lock);

	//cout << "Adder track line\n\n\n" << "mypospoint.x" << mypospoint.x <<"mypospoint.y" << mypospoint.y << endl;
}

void KfieldGui::DisplayErrors(float DistError, float RotError) {
	pthread_mutex_lock(&lock);

	tmp = Kutils::to_string(DistError); // boost::lexical_cast<string>(Belief.confidence);
	cvPutText(field, tmp.c_str(), cvPoint((2 * 675.0 + 6050 - 200) / SCALE, (2 * 675.0 + 4050 + 650) / SCALE), &font, cvScalar(255, 255, 255));

	tmp = Kutils::to_string(RotError * TO_DEG);
	cvPutText(field, tmp.c_str(), cvPoint((2 * 675.0 + 6050 - 200) / SCALE, (2 * 675.0 + 4050 + 850) / SCALE), &font, cvScalar(255, 255, 255));
	pthread_mutex_unlock(&lock);

}

void KfieldGui::addTrackLine(partcl TrackPoint) {
	pthread_mutex_lock(&lock);
	trackpospoint.x = (TrackPoint.x + (2 * 675.0 + 6050) / 2.0) / SCALE;
	trackpospoint.y = (-TrackPoint.y + (2 * 675.0 + 4050) / 2.0) / SCALE;

	cvLine(field, trackpospoint_old, trackpospoint, CV_RGB(0, 100, 250), 1, CV_AA, 0);
	cvLine(cleanfield, trackpospoint_old, trackpospoint, CV_RGB(0, 100, 250), 1, CV_AA, 0);

	trackpospoint_old = trackpospoint;
	pthread_mutex_unlock(&lock);

	//cout << "Adder TrackPoint line\n " << "TrackPoint.x" << trackpospoint.x <<"TrackPoint.y" << trackpospoint.y << endl;
}

void KfieldGui::CleanField() {
	pthread_mutex_lock(&lock);

	cvCopy(cleanfield, field, NULL);
	pthread_mutex_unlock(&lock);

}

void KfieldGui::BackupField() {
	pthread_mutex_lock(&lock);
	cvCopy(field, field2, NULL);
	pthread_mutex_unlock(&lock);
	cout << "FieldBackuped " << endl;
}

void KfieldGui::RevertBackupField() {
	pthread_mutex_lock(&lock);

	cvCopy(field2, field, NULL);
	pthread_mutex_unlock(&lock);

}

int KfieldGui::save_field_to_png(int aanumber) {
	sprintf(fieldfilename, "Field %04u.png", aanumber);
	return cvSaveImage(fieldfilename, field);
}

void KfieldGui::display_belief(belief Belief, double maxrangeleft, double maxrangeright, int count) {
	pthread_mutex_lock(&lock);

	IplImage* image = field;
	CvPoint pt1, pt2;
	int len = 200.0 / SCALE;
	//double max = 0;
	//int green_width = (2 * 675.0 + 6050) / SCALE;
	//int green_height = (2 * 675.0 + 4050) / SCALE;

	pt1.x = (Belief.x + (2 * 675.0 + 6050) / 2.0) / SCALE;
	pt1.y = (-Belief.y + (2 * 675.0 + 4050) / 2.0) / SCALE;
	cvCircle(image, pt1, 40.0 / SCALE, CV_RGB(200, 70, 40), CV_FILLED, CV_AA, 0);
	//cvCircle(image, pt1, 4, CV_RGB(0,0,100), 0, CV_AA, 0);

	pt2.x = pt1.x + len * cos(-Belief.theta);
	pt2.y = pt1.y + len * sin(-Belief.theta);
	cvLine(image, pt1, pt2, CV_RGB(150, 100, 200), 2, CV_AA, 0);

	pt2.x = pt1.x + len * 20 * cos(-(Belief.theta + maxrangeleft));
	pt2.y = pt1.y + len * 20 * sin(-(Belief.theta + maxrangeleft));

	cvLine(image, pt1, pt2, CV_RGB(150, 100, 200), 1, CV_AA, 0);

	pt2.x = pt1.x + len * 20 * cos(-(Belief.theta + maxrangeright));
	pt2.y = pt1.y + len * 20 * sin(-(Belief.theta + maxrangeright));

	cvLine(image, pt1, pt2, CV_RGB(150, 100, 200), 1, CV_AA, 0);

	tmp = Kutils::to_string(Belief.confidence); // boost::lexical_cast<string>(Belief.confidence);
	cvPutText(image, tmp.c_str(), cvPoint((2 * 675.0 + 6050 - 200) / SCALE, (2 * 675.0 + 4050 + 250) / SCALE), &font, cvScalar(255, 255, 255));

	tmp = Kutils::to_string(Belief.weightconfidence); // boost::lexical_cast<string>(Belief.confidence);
	cvPutText(image, tmp.c_str(), cvPoint((2 * 675.0 + 6050 - 200) / SCALE, (2 * 675.0 + 4050 + 450) / SCALE), &font, cvScalar(255, 255, 255));

	char buf[20];
	sprintf(buf, "%d", count);
	tmp = Kutils::to_string((float) Belief.confidence); // boost::lexical_cast<string>(Belief.confidence);
	cvPutText(field, buf, cvPoint((2 * 675.0 + 6050 + 450) / SCALE, (2 * 675.0 + 4050 - 50) / SCALE), &font, cvScalar(250, 250, 2));
	pthread_mutex_unlock(&lock);

}

void KfieldGui::display_Trackpoint(partcl Belief, double maxrangeleft, double maxrangeright) {
	pthread_mutex_lock(&lock);

	cout << " Belief X:" << Belief.x << " Y: " << Belief.y << " Theta " << Belief.phi << endl;

	IplImage** image = &field;
	CvPoint pt1, pt2;
	int len = 200.0 / SCALE;
	double max = 0;
	//int green_width = (2 * 675.0 + 6050) / SCALE;
	//int green_height = (2 * 675.0 + 4050) / SCALE;

	pt1.x = (Belief.x + (2 * 675.0 + 6050) / 2.0) / SCALE;
	pt1.y = (-Belief.y + (2 * 675.0 + 4050) / 2.0) / SCALE;
	cvCircle(*image, pt1, 25.0 / SCALE, CV_RGB(60, 100, 200), CV_FILLED, CV_AA, 0);
	//cvCircle(image, pt1, 4, CV_RGB(0,0,100), 0, CV_AA, 0);

	pt2.x = pt1.x + len * cos(-Belief.phi);
	pt2.y = pt1.y + len * sin(-Belief.phi);
	cvLine(*image, pt1, pt2, CV_RGB(50, 100, 200), 2, CV_AA, 0);

	pt2.x = pt1.x + len * 20 * cos(-(Belief.phi + maxrangeleft));
	pt2.y = pt1.y + len * 20 * sin(-(Belief.phi + maxrangeleft));

	cvLine(*image, pt1, pt2, CV_RGB(50, 80, 200), 1, CV_AA, 0);

	pt2.x = pt1.x + len * 20 * cos(-(Belief.phi + maxrangeright));
	pt2.y = pt1.y + len * 20 * sin(-(Belief.phi + maxrangeright));

	cvLine(*image, pt1, pt2, CV_RGB(50, 80, 200), 1, CV_AA, 0);

	if (max < Belief.Weight)
		max = Belief.Weight;
	pthread_mutex_unlock(&lock);

	//

}

void KfieldGui::display_particles(parts & Particles, bool unnormilised) {
	pthread_mutex_lock(&lock);

	//field;
	CvPoint pt1, pt2;
	int len = 200.0 / SCALE;
	double max = 0;
	//int green_width = (2 * 675.0 + 6050) / SCALE;
	//int green_height = (2 * 675.0 + 4050) / SCALE;
	if (unnormilised)
		max = 1;
	else {
		for (unsigned int i = 0; i < Particles.size; i++)
			if (Particles.Weight[i] > max)
				max = Particles.Weight[i];
	}

	for (unsigned int i = 0; i < Particles.size; i++) {

		pt1.x = (Particles.x[i] + (2 * 675.0 + 6050) / 2.0) / SCALE;
		pt1.y = (-Particles.y[i] + (2 * 675.0 + 4050) / 2.0) / SCALE;
		cvCircle(field, pt1, 20.0 / SCALE, CV_RGB(230, 254 * (1 - Particles.Weight[i] / max), 48), CV_FILLED, CV_AA, 0);
		//cvCircle(image, pt1, 4, CV_RGB(0,0,100), 0, CV_AA, 0);
		//cout << "X: "<<Particles.x[i] << " Y: " << Particles.y[i] << " Phi: " << Particles.phi[i] << endl;
		//cout << "pt1.x " << pt1.x << " pt1.y "<<pt1.y << endl;
		pt2.x = pt1.x + len * cos(-Particles.phi[i]);
		pt2.y = pt1.y + len * sin(-Particles.phi[i]);
		cvLine(field, pt1, pt2, CV_RGB(238, 254 * (1 - Particles.Weight[i] / max), 48), 1, CV_AA, 0);
	}

	//CvPoint pt3;
	cout << "MAX(!?)  = " << max << endl;
	if (max != 0)
		for (unsigned int i = 0; i < Particles.size; i++) {
			pt1.x = field->width;
			pt1.y = (-Particles.y[i] + (2 * 675.0 + 4050) / 2.0) / SCALE;
			pt2.y = pt1.y;
			pt2.x = -2 + pt1.x - (Particles.Weight[i] / max) * (900.0 / SCALE);
			//if (i % 10 == 0)
			//cout << " pt2.x " << pt2.x << " Particles.Weight[" << i << "] " << Particles.Weight[i] << endl;
			cvLine(field, pt1, pt2, CV_RGB(238, 254 * (1 - Particles.Weight[i] / max), 48), 1, CV_AA, 0);

			pt1.y = field->height;
			pt1.x = (Particles.x[i] + (2 * 675.0 + 6050) / 2.0) / SCALE;
			pt2.x = pt1.x;
			pt2.y = -2 + pt1.y - (Particles.Weight[i] / max) * (900.0 / SCALE);
			cvLine(field, pt1, pt2, CV_RGB(238, 254 * (1 - Particles.Weight[i] / max), 48), 1, CV_AA, 0);
		}

	pthread_mutex_unlock(&lock);
	cout << " Particles Drawn " << endl;
}

void* KfieldGui::display_field(void * fps) {

	int * FPS = (int *) fps;
	int waittime = round(1000.0 / (double) (*FPS));
	cout << "\033[22;32m Fps " << (*FPS) << " Waittime " << waittime << "\033[0m" << endl;
	while ((keypressed = cvWaitKey(waittime)) != 27) {
		pthread_mutex_lock(&lock);
		cvShowImage(wndname, field);
		pthread_mutex_unlock(&lock);

	}
	cout << " Should Exit" << endl;
	cvDestroyWindow(wndname);
	pthread_exit( NULL);
}

void KfieldGui::make_field(IplImage** image) {
	pthread_mutex_lock(&lock);

	CvSize fieldSize = cvSize((round(7400.0 / SCALE)) + 1000.0 / SCALE, (5400.0 / SCALE) + 1000.0 / SCALE);

	*image = cvCreateImage(fieldSize, 8, 3);

	int green_width = (2 * 675.0 + 6050) / SCALE;
	int green_height = (2 * 675.0 + 4050) / SCALE;
	CvPoint pt1, pt2, pt3;
	//int width = 600 * 2, height = 400 * 2;
	pt1.x = 0;
	pt1.y = 0;
	pt2.x = green_width;
	pt2.y = green_height;
	//green field
	cvRectangle(*image, pt1, pt2, CV_RGB(50, 215, 100), CV_FILLED, CV_AA, 0);
	//white rectangle
	pt1.x += 675.0 / SCALE;
	pt1.y += 675.0 / SCALE;
	pt2.x -= 675.0 / SCALE;
	pt2.y -= 675.0 / SCALE;
	cvRectangle(*image, pt1, pt2, CV_RGB(255, 255, 255), CV_FILLED, CV_AA, 0);
	//green inside

	pt1.x += 50.0 / SCALE;
	pt1.y += 50.0 / SCALE;
	pt2.x -= 50.0 / SCALE;
	pt2.y -= 50.0 / SCALE;
	cvRectangle(*image, pt1, pt2, CV_RGB(50, 215, 100), CV_FILLED, CV_AA, 0);
	/////white rectangle

	pt3.x = green_width / 2.0;
	pt3.y = green_height / 2.0;
	//cout << "X: " << pt3.x << " Y: " << pt3.y << endl;
	cvCircle(*image, pt3, 1250.0 / (2.0 * SCALE), CV_RGB(255, 255, 255), CV_FILLED, CV_AA, 0);
	cvCircle(*image, pt3, (1250.0 - 100) / (2.0 * SCALE), CV_RGB(50, 215, 100), CV_FILLED, CV_AA, 0);

	//Vertical Line
	//pt1.x= 670.0 / SCALE;
	pt1.x = pt3.x - 25.0 / SCALE;
	pt2.x = pt3.x + 25.0 / SCALE;
	cvRectangle(*image, pt1, pt2, CV_RGB(255, 255, 255), CV_FILLED, CV_AA, 0);

	//Small areas Left
	pt1.x = 675.0 / SCALE;
	pt1.y = (675.0 + 50.0 + 425.0) / SCALE;
	pt2.x = (675.0 + 650.0) / SCALE;
	pt2.y = (675.0 + 50.0 + 425.0 + 3050.0) / SCALE;

	cvRectangle(*image, pt1, pt2, CV_RGB(255, 255, 255), CV_FILLED, CV_AA, 0);

	pt1.x += 50.0 / SCALE;
	pt1.y += 50.0 / SCALE;
	pt2.x -= 50.0 / SCALE;
	pt2.y -= 50.0 / SCALE;
	cvRectangle(*image, pt1, pt2, CV_RGB(50, 215, 100), CV_FILLED, CV_AA, 0);

	//Small areas Right
	pt1.x = green_width - (675.0 + 650.0) / SCALE;
	pt1.y = (675.0 + 50.0 + 425.0) / SCALE;
	pt2.x = green_width - (675.0) / SCALE;
	pt2.y = (675.0 + 50.0 + 425.0 + 3050.0) / SCALE;

	cvRectangle(*image, pt1, pt2, CV_RGB(255, 255, 255), CV_FILLED, CV_AA, 0);

	pt1.x += 50.0 / SCALE;
	pt1.y += 50.0 / SCALE;
	pt2.x -= 50.0 / SCALE;
	pt2.y -= 50.0 / SCALE;
	cvRectangle(*image, pt1, pt2, CV_RGB(50, 215, 100), CV_FILLED, CV_AA, 0);

#ifdef NAO_2008
	//Blue goal Posts
	pt3.x = (675.0 + (40.0 / 2.0)) / SCALE;
	pt3.y = green_height / 2.0 + 1400.0 / (2.0 * SCALE);

	cvCircle(*image, pt3, 100 / (2.0 * SCALE), CV_RGB(80, 55, 255), CV_FILLED, CV_AA, 0);
	pt3.y = green_height / 2.0 - 1400.0 / (2.0 * SCALE);

	cvCircle(*image, pt3, 100 / (2.0 * SCALE), CV_RGB(80, 55, 255), CV_FILLED, CV_AA, 0);

	//Yellow goal Posts
	pt3.x = (675.0 + 6050 + (-40.0 / 2.0)) / SCALE;
	pt3.y = green_height / 2.0 + 1400.0 / (2.0 * SCALE);

	cvCircle(*image, pt3, 100 / (2.0 * SCALE), CV_RGB(255, 255, 35), CV_FILLED, CV_AA, 0);
	pt3.y = green_height / 2.0 - 1400.0 / (2.0 * SCALE);

	cvCircle(*image, pt3, 100 / (2.0 * SCALE), CV_RGB(255, 255, 35), CV_FILLED, CV_AA, 0);

#else ///aibo
	//Blue goal Posts
	pt3.x = (675.0 - 500 + (40.0 / 2.0)) / SCALE;
	pt3.y = green_height / 2.0 + 1400.0 / (2.0 * SCALE);

	cvCircle(*image, pt3, 100 / (2.0 * SCALE), CV_RGB(80, 55, 255), CV_FILLED, CV_AA, 0);
	pt3.y = green_height / 2.0 - 1400.0 / (2.0 * SCALE);

	cvCircle(*image, pt3, 100 / (2.0 * SCALE), CV_RGB(80, 55, 255), CV_FILLED, CV_AA, 0);

	//Yellow goal Posts
	pt3.x = (675.0 + 6050 + 500 + (-40.0 / 2.0)) / SCALE;
	pt3.y = green_height / 2.0 + 1400.0 / (2.0 * SCALE);

	cvCircle(*image, pt3, 100 / (2.0 * SCALE), CV_RGB(255, 255, 35), CV_FILLED, CV_AA, 0);
	pt3.y = green_height / 2.0 - 1400.0 / (2.0 * SCALE);
	cvCircle(*image, pt3, 100 / (2.0 * SCALE), CV_RGB(255, 255, 35), CV_FILLED, CV_AA, 0);

#endif

	cvPutText(*image, "Distance Error", cvPoint((2 * 675.0 + 6050 - 200) / SCALE, (2 * 675.0 + 4050 + 550) / SCALE), &font, cvScalar(50, 50, 255));
	cvPutText(*image, "Rotation Error Deg", cvPoint((2 * 675.0 + 6050 - 200) / SCALE, (2 * 675.0 + 4050 + 750) / SCALE), &font, cvScalar(50, 50, 255));
	cvPutText(*image, "Step:", cvPoint((2 * 675.0 + 6050 + 50) / SCALE, (2 * 675.0 + 4050 - 50) / SCALE), &font, cvScalar(250, 250, 2));
	cvPutText(*image, "Dist Confidence:  ", cvPoint((2 * 675.0 + 6050 - 200) / SCALE, (2 * 675.0 + 4050 + 150) / SCALE), &font, cvScalar(155, 155, 255));
	cvPutText(*image, "Weight Sum:  ", cvPoint((2 * 675.0 + 6050 - 200) / SCALE, (2 * 675.0 + 4050 + 350) / SCALE), &font, cvScalar(155, 255, 255));

	pthread_mutex_unlock(&lock);

}

//int KfieldGui::GuiWaitKeyPress() {
//	cout << " GuiWaitKeyPress " << endl;
//	while (Waitingforkey != 2) {
//		usleep(100 * 1000);
//		cout << "e ";
//	}
//
//	int ret = cvWaitKey(0);
//	Waitingforkey = 0;
//	return ret;
//}

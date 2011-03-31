/*
 * KfieldGui.cpp
 *
 *  Created on: 12 Μαρ 2010
 *      Author: trs
 */

#include "KfieldGui.h"
#include "../../activities/Localization/Kutils.h"
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cxcore.h>

#define NAO_2010

CvFont KfieldGui::font;
pthread_mutex_t KfieldGui::lock;
partcl KfieldGui::tempparticl, KfieldGui::robotStartpose, KfieldGui::robotEndpose;
;
RobotPose KfieldGui::pose1;
RobotPose KfieldGui::pose2;
short KfieldGui::keypressed;
int KfieldGui::state;
MotionWalkMessage KfieldGui::wmot;

KfieldGui* Gui;

void * gui_redraw(void * foo) {
	return Gui->redraw_field(foo);
}

void gui_onmouse(int event, int x, int y, int flags, void* param) {
	return Gui->on_mouse(event, x, y, flags, param);
}

CvScalar KfieldGui::random_color() {
	CvRNG rng(-1);
	int icolor = cvRandInt(&rng);
	return CV_RGB(icolor & 255, (icolor >> 8) & 255, (icolor >> 16) & 255);
}
KfieldGui::KfieldGui(KLocalization & KLocObj) {
	KfieldGui();
	draw_particles(KLocObj.SIRParticles);
}

void KfieldGui::drawCursor(int x, int y) {
	//Get clean copy of image
	//pthread_mutex_lock(&lock);
	//cvCopy(field, screenfield);

	//screenexecutiondate=cvCloneImage(imagen);
	//Draw a circle where is the mouse	cvLine(screenfield, cvPoint(last.x, last.y), cvPoint(x, y), CV_RGB(300,100,100), 1, 0, 0);
	//cvCircle(screenexecutiondate, cvPoint(x,y), r, CV_RGB(0,0,0), 1, CV_AA, 0);

	if (state != TWOPOINTSELECTION_2) {
		cvLine(screenfield, cvPoint(last.x, last.y), cvPoint(x, y), color["gray"], 1, 0, 0);
		cvCircle(screenfield, cvPoint(last.x, last.y), sqrt((last.x - x) * (last.x - x) + (last.y - y) * (last.y - y)), color["gray"], 1, CV_AA, 0);
	} else {
		cvLine(screenfield, cvPoint(point1.x, point1.y), cvPoint(x, y), color["gray"], 1, 0, 0);
		string tmp = Kutils::to_string(sqrt((point1.x - x) * (point1.x - x) + (point1.y - y) * (point1.y - y)) * scale);
		cvPutText(screenfield, tmp.c_str(), cvPoint((2 * margintoline + field_width - 200) / scale, (2 * margintoline + field_height + 650) / scale), &font, color["white"]);

		//cvPutText(field, tmp.c_str(), cvPoint((2 * margintoline + field_width - 200) / scale, (2 * margintoline + field_height + 850) / scale), &font, color["white"]);
	}
	//cvCopy(imagen,screenexecutiondate);
	//pthread_mutex_unlock(&lock);
}

void KfieldGui::on_mouse(int event, int x, int y, int flags, void* param) {
	if (state == FINISH || state == READY_TO_SEND_TWO || state == READY_TO_SEND_SINGLE)
		return;
	pthread_mutex_lock(&lock);
	cvCopy(field, screenfield);

	//Select mouse Event
	if (event == CV_EVENT_LBUTTONDOWN) {

		if (state == TWOPOINTSELECTION_2) {
			cvLine(cleanfield, cvPoint(point1.x, point1.y), cvPoint(x, y), color["gray"], 1, 0, 0);
			cvLine(field, cvPoint(point1.x, point1.y), cvPoint(x, y), color["gray"], 1, 0, 0);
			state = TWOPOINTSELECTION_3;
		}
		last.x = x;
		last.y = y;
		drawing = 1;

	} else if (event == CV_EVENT_LBUTTONUP) {

		cvLine(cleanfield, cvPoint(last.x, last.y), cvPoint(x, y), color["gray"], 1, CV_AA, 0);
		cvCircle(cleanfield, cvPoint(last.x, last.y), 100 / scale/*sqrt((last.x - x) * (last.x - x) + (last.y - y) * (last.y - y))*/, CV_RGB(100, 100, 50), 1, CV_AA, 0);

		std::cout << "X " << last.x << " Y " << last.y << " angle " << atan2f(-(y - last.y), x - last.x) * 180.0 / 3.141592653589793238 << std::endl;
		if (state == ONEPOINTSELECTION || state == TWOPOINTSELECTION_1) {
			point1 = last;
			//			point1.y = last.y;

			KfieldGui::pose1.set_x(gui2world_x(last.x));//last.x * scale - (2 * margintoline + field_width) / 2.0);
			KfieldGui::pose1.set_y(gui2world_y(last.y)); //-(last.y * scale - (2 * margintoline + field_height) / 2.0));
			KfieldGui::pose1.set_phi(atan2f(-(y - last.y), x - last.x));
			KfieldGui::pose1.set_confidence(sqrt((last.x - x) * (last.x - x) + (last.y - y) * (last.y - y)));

			std::cout << "X " << pose1.x() << " Y " << pose1.y() << " angle " << atan2f(-(y - last.y), x - last.x) * 180.0 / 3.141592653589793238 << std::endl;
			tempparticl.x = pose1.x();
			tempparticl.y = pose1.y();
			tempparticl.phi = pose1.phi();
			tempparticl.Weight = pose1.confidence();

			if (state == ONEPOINTSELECTION) {
				state = READY_TO_SEND_SINGLE;
				drawing = 0;
			} else {
				state = TWOPOINTSELECTION_2;
				cout << "Select Second Point " << endl;
			}

		} else if (state == TWOPOINTSELECTION_3) {
			KfieldGui::pose2.set_x(gui2world_x(last.x));
			KfieldGui::pose2.set_y(gui2world_y(last.y));
			KfieldGui::pose2.set_phi(atan2f(-(y - last.y), x - last.x));
			KfieldGui::pose2.set_confidence(sqrt((last.x - x) * (last.x - x) + (last.y - y) * (last.y - y)));

			state = FINISH;//READY_TO_SEND_TWO;
			//			cout << " Ready to send two" << endl;
			record_odometry_data();
			drawing = 0;
		}
	} else if (event == CV_EVENT_MOUSEMOVE) {
		if (drawing)
			drawCursor(x, y);
	}
	pthread_mutex_unlock(&lock);
}

void KfieldGui::Init(int newscale) {

	pthread_mutex_lock(&lock);
	this->scale = newscale*(-0.6994) + 10.6994;
	hScale = 3.4 / scale;
	vScale = 3.4 / scale;
	int lineWidth = 1;

	if (field){
		cvReleaseImage(&field);
		field = NULL;
	}
	if (field2){
		cvReleaseImage(&field2);
		field2 = NULL;
	}
	if (cleanfield){
		cvReleaseImage(&cleanfield);
		cleanfield = NULL;
	}
	if (screenfield){
		cvReleaseImage(&screenfield);
		screenfield = NULL;
	}

	cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, hScale, vScale, 0, lineWidth, CV_AA);
	make_field(&cleanfield);

	field = cvCreateImage(cvSize(cleanfield->width, cleanfield->height), 8, 3);
	field2 = cvCreateImage(cvSize(cleanfield->width, cleanfield->height), 8, 3);
	screenfield = cvCreateImage(cvSize(cleanfield->width, cleanfield->height), 8, 3);

	cvCopy(cleanfield, field, NULL);
	drawing = 0;

	state = FINISH;
	mypospoint_old.x = (0 + (2 * margintoline + field_width) / 2.0) / scale;
	mypospoint_old.y = (0 + (2 * margintoline + field_height) / 2.0) / scale;
	pthread_mutex_unlock(&lock);
	cout << "Initialization Compelete " << scale << endl;
}

KfieldGui::KfieldGui() {
	filecreated = false;
	cleanfield = NULL;
	field = NULL;
	field2 = NULL;
	screenfield = NULL;
	Waitingforkey = 0;
	strcpy(wndname, "KField ;)");

	thread_pid = -2;

	time_t rawtime;
	struct tm * timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(executiondate, 80, "%d-%m_%I.%M%p", timeinfo);

	pthread_mutex_init(&lock, NULL);
	cout << " Starting initialization" << endl;

	color["yellow"] = CV_RGB(245,245,10);
	color["lightyellow"] = CV_RGB(245,245,100);
	color["blue"] = CV_RGB(20,20,250);
	color["skyblue"] = CV_RGB(0,100,250);

	color["lightblue"] = CV_RGB(100,100,250);
	color["darkblue"] = CV_RGB(50,50,128);

	color["fieldgreen"] = CV_RGB(50,215,100);
	color["white"] = CV_RGB(255,255,255);
	color["black"] = CV_RGB(0,0,0);
	color["purple"] = CV_RGB(150,100,200);
	color["red"] = CV_RGB(2,30,0);

	color["orange"] = CV_RGB(254,87,0);

	color["darkorange"] = CV_RGB(200,70,40); //belief
	color["blue"] = CV_RGB(20,20,250);
	color["gray"] = CV_RGB(100,100,100);

	Init(SCALE);
}

KfieldGui::~KfieldGui() {
	// TODO Auto-generated destructor stub
}
void KfieldGui::display_Gui() {
	if (thread_pid == -2) {
		cout << "Creating GUI thread" << endl;
		int *fps = new int();
		*fps = 10;
		cvNamedWindow(wndname, CV_WINDOW_AUTOSIZE);
		Gui = this;
		thread_pid = pthread_create(new pthread_t, NULL, &gui_redraw/* KfieldGui::redraw_field*/, fps);
	}
	cvSetMouseCallback(wndname, &gui_onmouse/*KfieldGcvui::on_mouse*/, 0);
}

void KfieldGui::DrawObservations(belief Belief,vector<KObservationModel> & currentObservation) {
	CvPoint pt1, pt2;
	unsigned int i;
	pthread_mutex_lock(&lock);
	for (i = 0; i < currentObservation.size(); i++) {
		cout << "Plot  the " << currentObservation[i].Feature.id << "X " << currentObservation[i].Feature.x << " Y " << currentObservation[i].Feature.y << endl;
		pt1.x = (currentObservation[i].Feature.x + (2 * margintoline + field_width) / 2.0) / scale;
		pt1.y = (-currentObservation[i].Feature.y + (2 * margintoline + field_height) / 2.0) / scale;

		cvCircle(field, pt1, currentObservation[i].Distance.val / scale, (strchr(currentObservation[i].Feature.id.c_str(), 'Y')) ? color["yellow"] : color["blue"], 0, CV_AA, 0);
		cout << " " << currentObservation[i].Distance.val + currentObservation[i].Distance.Edev << endl;
		if ((currentObservation[i].Distance.val + currentObservation[i].Distance.Edev) > 0)
			cvCircle(field, pt1, (currentObservation[i].Distance.val + currentObservation[i].Distance.Edev) / scale, (strchr(currentObservation[i].Feature.id.c_str(), 'Y')) ? color["lightyellow"] : color["lightblue"], 0, CV_AA, 0);
		if ((currentObservation[i].Distance.val - currentObservation[i].Distance.Edev) > 0)
			cvCircle(field, pt1, (currentObservation[i].Distance.val - currentObservation[i].Distance.Edev) / scale, (strchr(currentObservation[i].Feature.id.c_str(), 'Y')) ? color["lightyellow"] : color["lightblue"], 0, CV_AA, 0);
		pt2.x = pt1.x - (currentObservation[i].Distance.val / scale) * cos(-(currentObservation[i].Bearing.val+Belief.theta));
		pt2.y = pt1.y - (currentObservation[i].Distance.val / scale) * sin(-(currentObservation[i].Bearing.val+Belief.theta));

		cvLine(field, pt1, pt2, (strchr(currentObservation[i].Feature.id.c_str(), 'Y')) ? color["yellow"] : color["blue"], 0, CV_AA, 0);

		if (i == 0) {
			string tmp = Kutils::to_string(currentObservation[i].Distance.val);
			cvPutText(field, tmp.c_str(), cvPoint((2 * margintoline + field_width - 200) / scale, (2 * margintoline + field_height + 650) / scale), &font, (strchr(currentObservation[i].Feature.id.c_str(), 'Y')) ? color["lightyellow"] : color["lightblue"]);
		}

		if (i == 1) {
			string tmp = Kutils::to_string(currentObservation[i].Distance.val);
			cvPutText(field, tmp.c_str(), cvPoint((2 * margintoline + field_width - 200) / scale, (2 * margintoline + field_height + 850) / scale), &font, (strchr(currentObservation[i].Feature.id.c_str(), 'Y')) ? color["lightyellow"] : color["lightblue"]);
		}
	}
	pthread_mutex_unlock(&lock);

}

void KfieldGui::KfieldInitTrackLine(belief mypos) {
	mypospoint.x = (mypos.x + (2 * margintoline + field_width) / 2.0) / scale;
	mypospoint.y = (-mypos.y + (2 * margintoline + field_height) / 2.0) / scale;
	mypospoint_old = mypospoint;
}

void KfieldGui::KfieldInitTrackLine(partcl TrackPoint) {
	trackpospoint.x = (TrackPoint.x + (2 * margintoline + field_width) / 2.0) / scale;
	trackpospoint.y = (-TrackPoint.y + (2 * margintoline + field_height) / 2.0) / scale;
	trackpospoint_old = trackpospoint;
}

void KfieldGui::addTrackLine(belief mypos) {
	mypospoint.x = (mypos.x + (2 * margintoline + field_width) / 2.0) / scale;
	mypospoint.y = (-mypos.y + (2 * margintoline + field_height) / 2.0) / scale;

	pthread_mutex_lock(&lock);

	cvLine(field, mypospoint_old, mypospoint, color["red"], 1, CV_AA, 0);
	cvLine(cleanfield, mypospoint_old, mypospoint, color["red"], 1, CV_AA, 0);
	mypospoint_old = mypospoint;
	pthread_mutex_unlock(&lock);

	//cout << "Adder track line\n\n\n" << "mypospoint.x" << mypospoint.x <<"mypospoint.y" << mypospoint.y << endl;
}

void KfieldGui::drawErrors(float DistError, float RotError) {
	pthread_mutex_lock(&lock);

	tmp = Kutils::to_string(DistError); // boost::lexical_cast<string>(Belief.confidence);
	cvPutText(field, tmp.c_str(), cvPoint((2 * margintoline + field_width - 200) / scale, (2 * margintoline + field_height + 650) / scale), &font, color["white"]);

	tmp = Kutils::to_string(RotError * TO_DEG);
	cvPutText(field, tmp.c_str(), cvPoint((2 * margintoline + field_width - 200) / scale, (2 * margintoline + field_height + 850) / scale), &font, color["white"]);
	pthread_mutex_unlock(&lock);

}

void KfieldGui::addTrackLine(partcl TrackPoint) {
	pthread_mutex_lock(&lock);
	trackpospoint.x = (TrackPoint.x + (2 * margintoline + field_width) / 2.0) / scale;
	trackpospoint.y = (-TrackPoint.y + (2 * margintoline + field_height) / 2.0) / scale;

	cvLine(field, trackpospoint_old, trackpospoint, color["lightblue"], 1, CV_AA, 0);
	cvLine(cleanfield, trackpospoint_old, trackpospoint, color["lightblue"], 1, CV_AA, 0);

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

void KfieldGui::draw_belief(belief Belief, double maxrangeleft, double maxrangeright, int count) {
	pthread_mutex_lock(&lock);

	IplImage* image = field;
	CvPoint pt1, pt2;
	int len = 200.0 / scale;

	pt1.x = (Belief.x + (2 * margintoline + field_width) / 2.0) / scale;
	pt1.y = (-Belief.y + (2 * margintoline + field_height) / 2.0) / scale;
	cvCircle(image, pt1, 30.0 / scale, color["darkorange"], CV_FILLED, CV_AA, 0);

	pt2.x = pt1.x + len * cos(-Belief.theta);
	pt2.y = pt1.y + len * sin(-Belief.theta);
	cvLine(image, pt1, pt2, color["purple"], 2, CV_AA, 0);

	pt2.x = pt1.x + len * 20 * cos(-(Belief.theta + maxrangeleft));
	pt2.y = pt1.y + len * 20 * sin(-(Belief.theta + maxrangeleft));

	cvLine(image, pt1, pt2, color["purple"], 1, CV_AA, 0);

	pt2.x = pt1.x + len * 20 * cos(-(Belief.theta + maxrangeright));
	pt2.y = pt1.y + len * 20 * sin(-(Belief.theta + maxrangeright));

	cvLine(image, pt1, pt2, color["purple"], 1, CV_AA, 0);

	tmp = Kutils::to_string(Belief.confidence); // boost::lexical_cast<string>(Belief.confidence);
	cvPutText(image, tmp.c_str(), cvPoint((2 * margintoline + field_width - 200) / scale, (2 * margintoline + field_height + 250) / scale), &font, color["white"]);

	tmp = Kutils::to_string(Belief.weightconfidence); // boost::lexical_cast<string>(Belief.confidence);
	cvPutText(image, tmp.c_str(), cvPoint((2 * margintoline + field_width - 200) / scale, (2 * margintoline + field_height + 450) / scale), &font, color["white"]);

	char buf[20];
	sprintf(buf, "%d", count);
	tmp = Kutils::to_string((float) Belief.confidence); // boost::lexical_cast<string>(Belief.confidence);
	cvPutText(field, buf, cvPoint((2 * margintoline + field_width + 450) / scale, (2 * margintoline + field_height - 50) / scale), &font, color["yellow"]);
	pthread_mutex_unlock(&lock);

}

void KfieldGui::draw_Trackpoint(partcl Belief, double maxrangeleft, double maxrangeright) {
	pthread_mutex_lock(&lock);

	//cout << " Belief X:" << Belief.x << " Y: " << Belief.y << " Theta " << Belief.phi << endl;

	IplImage** image = &field;
	CvPoint pt1, pt2;
	int len = 200.0 / scale;
	double max = 0;

	pt1.x = (Belief.x + (2 * margintoline + field_width) / 2.0) / scale;
	pt1.y = (-Belief.y + (2 * margintoline + field_height) / 2.0) / scale;
	cvCircle(*image, pt1, 25.0 / scale, color["darkblue"], CV_FILLED, CV_AA, 0);

	pt2.x = pt1.x + len * cos(-Belief.phi);
	pt2.y = pt1.y + len * sin(-Belief.phi);
	cvLine(*image, pt1, pt2, color["blue"], 2, CV_AA, 0);

	pt2.x = pt1.x + len * 20 * cos(-(Belief.phi + maxrangeleft));
	pt2.y = pt1.y + len * 20 * sin(-(Belief.phi + maxrangeleft));

	cvLine(*image, pt1, pt2, color["lightblue"], 1, CV_AA, 0);

	pt2.x = pt1.x + len * 20 * cos(-(Belief.phi + maxrangeright));
	pt2.y = pt1.y + len * 20 * sin(-(Belief.phi + maxrangeright));

	cvLine(*image, pt1, pt2, color["lightblue"], 1, CV_AA, 0);

	if (max < Belief.Weight)
		max = Belief.Weight;
	pthread_mutex_unlock(&lock);

	//

}

void KfieldGui::draw_particles(parts & Particles, bool leaveunnormilised) {
	unsigned int i;
	pthread_mutex_lock(&lock);

	//field;
	CvPoint pt1, pt2;
	int len = 200.0 / scale;
	double max = 0;
	//int p_green_width = (2 * margintoline + field_width) / scale;
	//int p_green_height = (2 * margintoline + field_height) / scale;
	if (leaveunnormilised)
		max = 1;
	else {
		for (i = 0; i < Particles.size; i++)
			if (Particles.Weight[i] > max)
				max = Particles.Weight[i];
	}

	for (i = 0; i < Particles.size; i++) {

		pt1.x = (Particles.x[i] + (2 * margintoline + field_width) / 2.0) / scale;
		pt1.y = (-Particles.y[i] + (2 * margintoline + field_height) / 2.0) / scale;
		cvCircle(field, pt1, 20.0 / scale, CV_RGB(130, 254 * (1 - Particles.Weight[i] / max), 48), CV_FILLED, CV_AA, 0);
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
		for (i = 0; i < Particles.size; i++) {
			pt1.x = field->width;
			pt1.y = (-Particles.y[i] + (2 * margintoline + field_height) / 2.0) / scale;
			pt2.y = pt1.y;
			pt2.x = -2 + pt1.x - (Particles.Weight[i] / max) * (900.0 / scale);
			//if (i % 10 == 0)
			//cout << " pt2.x " << pt2.x << " Particles.Weight[" << i << "] " << Particles.Weight[i] << endl;
			cvLine(field, pt1, pt2, CV_RGB(238, 254 * (1 - Particles.Weight[i] / max), 48), 1, CV_AA, 0);

			pt1.y = field->height;
			pt1.x = (Particles.x[i] + (2 * margintoline + field_width) / 2.0) / scale;
			pt2.x = pt1.x;
			pt2.y = -2 + pt1.y - (Particles.Weight[i] / max) * (900.0 / scale);
			cvLine(field, pt1, pt2, CV_RGB(238, 254 * (1 - Particles.Weight[i] / max), 48), 1, CV_AA, 0);
		}

	pthread_mutex_unlock(&lock);
	cout << " Particles Drawn " << Particles.size << endl;
}

void KfieldGui::draw_ball(belief Belief, BallObject Ball) {
	pthread_mutex_lock(&lock);

	CvPoint pt1, pt2;
	int radius = Ball.ball_diameter() / 2 * 1000.0 / scale;

	//double max = 0;

	pt1.x = Belief.x + Ball.dist() * 1000 * cos((Belief.theta + Ball.bearing()));
	pt1.y = Belief.y + Ball.dist() * 1000 * sin((Belief.theta + Ball.bearing()));

	pt2.x = (pt1.x + (2 * margintoline + field_width) / 2.0) / scale;
	pt2.y = (-pt1.y + (2 * margintoline + field_height) / 2.0) / scale;

	cout << "Ball Dist" << Ball.dist() << " Ball diameter " << Ball.ball_diameter() << " Bearing " << Ball.bearing() << " pt x:" << pt1.x << " y: " << pt1.y << endl;
	cvCopy(cleanfield, field);
	cvCircle(field, pt2, radius, color["orange"], CV_FILLED, CV_AA, 0);
	tmp = Kutils::to_string(Ball.dist());
	cvPutText(field, tmp.c_str(), cvPoint((2 * margintoline + field_width - 400) / scale, (2 * margintoline + field_height + 850) / scale), &font, color["orange"]);
	pthread_mutex_unlock(&lock);
	//cout << " Ball Drawn " << endl;
}

void KfieldGui::draw_ball2(belief Belief, Ball Ball) {
	pthread_mutex_lock(&lock);

	CvPoint pt1, pt2;
	int radius = 60 / 2 * 1000.0 / scale;

	//double max = 0;

	pt1.x = Belief.x + Ball.relativex() * 1000*cos(Belief.theta);//Ball.dist() * 1000 * cos((Belief.theta + Ball.bearing()));
	pt1.y = Belief.y + Ball.relativey() * 1000*sin(Belief.theta);//Ball.dist() * 1000 * sin((Belief.theta + Ball.bearing()));

	pt2.x = (pt1.x + (2 * margintoline + field_width) / 2.0) / scale;
	pt2.y = (-pt1.y + (2 * margintoline + field_height) / 2.0) / scale;

	//cout << "Ball Dist" << Ball.dist() << " Ball diameter " << Ball.ball_diameter() << " Bearing " << Ball.bearing() << " pt x:" << pt1.x << " y: " << pt1.y << endl;
	cvCopy(cleanfield, field);
	cvCircle(field, pt2, radius, color["darkorange"], CV_FILLED, CV_AA, 0);
	//tmp = Kutils::to_string(Ball.dist());
	cvPutText(field, tmp.c_str(), cvPoint((2 * margintoline + field_width - 400) / scale, (2 * margintoline + field_height + 850) / scale), &font, color["orange"]);
	pthread_mutex_unlock(&lock);
	//cout << " Ball Drawn " << endl;
}


void* KfieldGui::redraw_field(void * fps) {
	int * FPS = (int *) fps;
	Waitingforkey = round(1000.0 / (double) (*FPS));
	cout << "\033[22;32m Fps " << (*FPS) << " Waittime " << Waitingforkey << "\033[0m" << endl;
	int key;
	cvShowImage(wndname, field);
	while ((key = cvWaitKey(Waitingforkey)) != 27) {
		if (key > 0)
			keypressed = key;
		//cout << keypressed << " " << (int)keypressed<< " " <<(char)keypressed << endl;
		pthread_mutex_lock(&lock);
		if (!drawing)
			cvShowImage(wndname, field);
		else
			cvShowImage(wndname, screenfield);
		pthread_mutex_unlock(&lock);

		if (keypressed == 'o') {
			state = ONEPOINTSELECTION;
			cout << " o " << endl;
			keypressed = -2;
		}
		if (keypressed == 't') {
			state = TWOPOINTSELECTION_1;
			cout << " t " << endl;
			keypressed = -2;
		}
		if (keypressed <= '9' && keypressed >= '1') {
			char achar = (char) keypressed;
			scale = atoi(&achar);
			keypressed = -2;
			cout << "Resizing window !!! " << endl;
			Init(scale);
		}

	}
	cout << " Should Exit" << endl;
	cvDestroyWindow(wndname);
	pthread_exit(NULL);
}

void KfieldGui::make_field(IplImage** image) {

	CvSize fieldSize = cvSize((rint(7400.0 / scale)) + 1000.0 / scale, (5400.0 / scale) + 1000.0 / scale);

	*image = cvCreateImage(fieldSize, 8, 3);
	///Values in mm
	linewidth = 50;
	margintoline = 700;
	field_width = 6000;
	field_height = 4000;

	goalarea_height = 2200;
	goalarea_width = 600;
	center_circle_diam = 1200;

	goal_width = 1400;
	verticalgoalpost_diam = 100;

	p_green_width = (2 * margintoline + field_width) / scale; //in pixels
	p_green_height = (2 * margintoline + field_height) / scale;
	CvPoint pt1, pt2, pt3;

	pt1.x = 0;
	pt1.y = 0;
	pt2.x = p_green_width;
	pt2.y = p_green_height;
	//green field
	cvRectangle(*image, pt1, pt2, CV_RGB(50, 215, 100), CV_FILLED, 0, 0);
	//white rectangle
	pt1.x += rint((margintoline - linewidth / 2.0f) / scale);
	pt1.y += rint((margintoline - linewidth / 2.0f) / scale);
	pt2.x -= rint((margintoline - linewidth / 2.0f) / scale);
	pt2.y -= rint((margintoline - linewidth / 2.0f) / scale);
	cvRectangle(*image, pt1, pt2, color["white"], CV_FILLED, 0, 0);
	//green inside
	pt1.x += rint(linewidth / scale);
	pt1.y += rint(linewidth / scale);
	pt2.x -= rint(linewidth / scale);
	pt2.y -= rint(linewidth / scale);
	cvRectangle(*image, pt1, pt2, CV_RGB(50, 215, 100), CV_FILLED, 0, 0);

	/////white rectangle
	pt3.x = rint(p_green_width / 2.0);
	pt3.y = rint(p_green_height / 2.0);

	cvCircle(*image, pt3, rint((center_circle_diam + linewidth) / (2.0 * scale)), color["white"], CV_FILLED, CV_AA, 0);
	cvCircle(*image, pt3, rint((center_circle_diam - linewidth) / (2.0 * scale)), CV_RGB(50, 215, 100), CV_FILLED, CV_AA, 0);

	//Vertical Line
	pt1.x = /*pt3.x -*/rint((margintoline + field_width / 2.0 + (linewidth / 2.0f)) / scale);
	pt2.x = /* pt3.x +*/rint((margintoline + field_width / 2.0 - (linewidth / 2.0f)) / scale);
	cvRectangle(*image, pt1, pt2, color["white"], CV_FILLED, 0, 0);

	//Small areas Left
	pt1.x = rint((margintoline - linewidth / 2.0f) / scale);
	pt1.y = rint((margintoline + (field_height - goalarea_height) / 2.0f - linewidth / 2.0f) / scale); //(675.0 + 50.0 + 425.0) / scale;
	pt2.x = rint((margintoline + goalarea_width + linewidth / 2.0f) / scale);// (675.0 + 650.0) / scale;
	pt2.y = rint((margintoline + (field_height - goalarea_height) / 2.0f + goalarea_height + linewidth / 2.0f) / scale);

	cvRectangle(*image, pt1, pt2, color["white"], CV_FILLED, 0, 0);

	//green inside
	pt1.x += rint(linewidth / scale);
	pt1.y += rint(linewidth / scale);
	pt2.x -= rint(linewidth / scale);
	pt2.y -= rint(linewidth / scale);
	cvRectangle(*image, pt1, pt2, CV_RGB(50, 215, 100), CV_FILLED, 0, 0);

	//Small areas Right
	pt1.x = rint((field_width + margintoline - goalarea_width - linewidth / 2.0f) / scale); //p_green_width - (675.0 + 650.0) / scale;//
	pt1.y = rint((margintoline + (field_height - goalarea_height) / 2.0f - linewidth / 2.0f) / scale); //(675.0 + 50.0 + 425.0) / scale;
	pt2.x = rint((field_width + margintoline + linewidth / 2.0f) / scale);
	pt2.y = rint((margintoline + (field_height - goalarea_height) / 2.0f + goalarea_height + linewidth / 2.0f) / scale);

	cvRectangle(*image, pt1, pt2, color["white"], CV_FILLED, 0, 0);

	pt1.x += rint(linewidth / scale);
	pt1.y += rint(linewidth / scale);
	pt2.x -= rint(linewidth / scale);
	pt2.y -= rint(linewidth / scale);
	cvRectangle(*image, pt1, pt2, CV_RGB(50, 215, 100), CV_FILLED, 0, 0);

	//Blue goal Posts
	pt3.x = rint(margintoline / scale);
	pt3.y = rint((margintoline + (field_height - goal_width) / 2.0) / scale);
	cvCircle(*image, pt3, rint(verticalgoalpost_diam / (2.0 * scale)), color["skyblue"], CV_FILLED, CV_AA, 0);

	pt3.y = rint((margintoline + goal_width + (field_height - goal_width) / 2.0) / scale);
	cvCircle(*image, pt3, rint(verticalgoalpost_diam / (2.0 * scale)), color["skyblue"], CV_FILLED, CV_AA, 0);

	//Yellow goal Posts
	pt3.x = rint((margintoline + field_width) / scale);//(675.0 + 6050 + (-40.0 / 2.0)) / scale;
	pt3.y = rint((margintoline + (field_height - goal_width) / 2.0) / scale);
	cvCircle(*image, pt3, rint(verticalgoalpost_diam / (2.0 * scale)), color["yellow"], CV_FILLED, CV_AA, 0);
	pt3.y = rint((margintoline + goal_width + (field_height - goal_width) / 2.0) / scale);
	cvCircle(*image, pt3, rint(verticalgoalpost_diam / (2.0 * scale)), color["yellow"], CV_FILLED, CV_AA, 0);

	//Crosses
	//center
	pt1.y = rint((margintoline + field_height / 2.0 - linewidth / 2.0f) / scale);
	pt2.y = rint((margintoline + field_height / 2.0 + linewidth / 2.0f) / scale);
	pt1.x = rint((margintoline + field_width / 2.0 + (linewidth)) / scale);
	pt2.x = rint((margintoline + field_width / 2.0 - (linewidth)) / scale);
	cvRectangle(*image, pt1, pt2, color["white"], CV_FILLED, 0, 0);

	//left cross
	//Horizontal 100mm
	pt1.y = rint((margintoline + field_height / 2.0 - linewidth / 2.0f) / scale);
	pt2.y = rint((margintoline + field_height / 2.0 + linewidth / 2.0f) / scale);
	pt1.x = rint((margintoline + (field_width / 2.0 - 1200) + (linewidth)) / scale);
	pt2.x = rint((margintoline + (field_width / 2.0 - 1200) - (linewidth)) / scale);
	cvRectangle(*image, pt1, pt2, color["white"], CV_FILLED, 0, 0);
	//Vertical 100mm
	pt1.y = rint((margintoline + field_height / 2.0 - linewidth) / scale);
	pt2.y = rint((margintoline + field_height / 2.0 + linewidth) / scale);
	pt1.x = rint((margintoline + (field_width / 2.0 - 1200) + (linewidth / 2.0f)) / scale);
	pt2.x = rint((margintoline + (field_width / 2.0 - 1200) - (linewidth / 2.0f)) / scale);
	cvRectangle(*image, pt1, pt2, color["white"], CV_FILLED, 0, 0);

	//Right cross
	//Horizontal 100mm
	pt1.y = rint((margintoline + field_height / 2.0 - linewidth / 2.0f) / scale);
	pt2.y = rint((margintoline + field_height / 2.0 + linewidth / 2.0f) / scale);
	pt1.x = rint((margintoline + (field_width - 1800) + (linewidth)) / scale);
	pt2.x = rint((margintoline + (field_width - 1800) - (linewidth)) / scale);
	cvRectangle(*image, pt1, pt2, color["white"], CV_FILLED, 0, 0);
	//Vertical 100mm
	pt1.y = rint((margintoline + field_height / 2.0 - linewidth) / scale);
	pt2.y = rint((margintoline + field_height / 2.0 + linewidth) / scale);
	pt1.x = rint((margintoline + (field_width - 1800) + (linewidth / 2.0f)) / scale);
	pt2.x = rint((margintoline + (field_width - 1800) - (linewidth / 2.0f)) / scale);
	cvRectangle(*image, pt1, pt2, color["white"], CV_FILLED, 0, 0);

	cvPutText(*image, "Distance Error", cvPoint((2 * margintoline + field_width - 200) / scale, (2 * margintoline + field_height + 550) / scale), &font, color["blue"]);
	cvPutText(*image, "Rotation Error Deg", cvPoint((2 * margintoline + field_width - 200) / scale, (2 * margintoline + field_height + 750) / scale), &font, color["blue"]);
	cvPutText(*image, "Step:", cvPoint((2 * margintoline + field_width + 50) / scale, (2 * margintoline + field_height - 50) / scale), &font, color["yellow"]);
	cvPutText(*image, "Dist Confidence:  ", cvPoint((2 * margintoline + field_width - 200) / scale, (2 * margintoline + field_height + 150) / scale), &font, color["lightblue"]);
	cvPutText(*image, "Weight Sum:  ", cvPoint((2 * margintoline + field_width - 200) / scale, (2 * margintoline + field_height + 350) / scale), &font, color["skyblue"]);

}

int KfieldGui::record_odometry_data() {

	string str = "LocalizationData_Vnew";
	str = str + executiondate + ".csv";

	ofstream stats;
	stats.open(str.c_str(), ios::app);
	if (!filecreated) {
		//add header
		stats << "wmot0,wmot1,wmot2,wmot3,Start_x ,Start_y ,Start_phi,End_x,End_y,End_phi,Click1_x,Click1_y,Click1_phi,Click2_x,Click2_y,Click2_phi" << endl;
		filecreated = true;
	}
	cout << wmot.parameter_size() << endl;
	stats << wmot.parameter(0) << ',' << wmot.parameter(1) << ',' << wmot.parameter(2) << ',' << wmot.parameter(3) << ',';
	stats << robotStartpose.x << ',' << robotStartpose.y << ',' << robotStartpose.phi << ',';
	stats << robotEndpose.x << ',' << robotEndpose.y << ',' << robotEndpose.phi << ',';
	stats << pose1.x() << ',' << pose1.y() << ',' << pose1.phi() << ',';
	stats << pose2.x() << ',' << pose2.y() << ',' << pose2.phi() << endl;

	stats.close();
	return 0;
	//	KfieldGui::pose2.set_phi(atan2f(-(y - last.y), x - last.x));
	//				KfieldGui::pose2.set_confidence(sqrt((last.x - x) * (last.x - x) + (last.y - y) * (last.y - y)));

	//mypos.confidence << ',' << mypos.weightconfidence << ',';
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

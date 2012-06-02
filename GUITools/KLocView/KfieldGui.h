/*
 * KfieldGui.h
 *
 *  Created on: 12 Μαρ 2010
 *      Author: trs
 */

#ifndef KFIELDGUI_H_
#define KFIELDGUI_H_

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cxcore.h>
#include "../../src/activities/Localization/KLocalization.h"
#include "../../src/messages/VisionObservations.pb.h"
#include "../../src/messages/WorldInfo.pb.h"
#include "../../src/messages/motion.pb.h"
#include "stdio.h"
#include <fstream>
#include <time.h>
#include <map>

#define DELAY 5
#define SCALE 5

#define gui2world_x(x) x * scale - (2 * margintoline + field_width) / 2.0f
#define gui2world_y(y) -(y * scale - (2 * margintoline + field_height) / 2.0f)
#define world2gui_x(x) (x + (2 * margintoline + field_width) / 2.0f) / scale
#define world2gui_y(x) (-y + (2 * margintoline + field_height) / 2.0f) / scale

enum states {
	ONEPOINTSELECTION, TWOPOINTSELECTION_1, TWOPOINTSELECTION_2, TWOPOINTSELECTION_3, READY_TO_SEND_SINGLE, READY_TO_SEND_TWO, FINISH
};

class KfieldGui {
	private:

		static pthread_mutex_t lock;

		static CvFont font;
		short Waitingforkey;


		char wndname[10];
		char fieldfilename[100];
		IplImage* cleanfield;
		IplImage *screenfield;
		IplImage *field;
		IplImage *field2;
		int thread_pid;
		string tmp;
		CvPoint mypospoint, mypospoint_old;
		CvPoint trackpospoint, trackpospoint_old;
		CvPoint last, point1;
		bool filecreated;
		char executiondate[80];
	public:
		void drawCursor(int, int);
		void on_mouse(int, int, int, int, void*);
		static MotionWalkMessage wmot;
		int drawing;
		static int state;
		static RobotPose pose1, pose2;
		static partcl tempparticl, robotStartpose, robotEndpose;

		double hScale ;
		double vScale ;
		double scale;
		CvScalar random_color();
		void KfieldInitTrackLine(partcl TrackPoint);
		void KfieldInitTrackLine(belief mypos);
		void addTrackLine(belief mypos);
		void addTrackLine(partcl TrackPoint);
		void drawErrors(float DistError, float RotError);

		int record_odometry_data();

		void CleanField();
		void BackupField();
		void RevertBackupField();
		void DrawObservations(belief Belief, vector<KObservationModel> & currentObservation);
		int save_field_to_png(int save_field_to_file);
		void display_Gui();
		KfieldGui();
		KfieldGui(KLocalization & KLocObj);
		virtual ~KfieldGui();
		void draw_belief(belief Belief, double maxrangeleft, double maxrangeright, int step);
		void draw_Trackpoint(partcl Belief, double maxrangeleft, double maxrangeright);
		void draw_particles(parts & Particles, bool leaveunnormilised = false);
		void draw_ball(belief Belief, BallObject Ball);
		void draw_ball2(belief Belief, Ball aBall);
		void* redraw_field(void * fps);
		void make_field(IplImage** image);
		static short keypressed;
		int GuiWaitKeyPress();

		void Init(int scale);
		int linewidth;
		int margintoline;
		int field_width;
		int field_height;

		int goalarea_height;
		int goalarea_width;
		int center_circle_diam;

		int goal_width;
		int verticalgoalpost_diam;

		int p_green_width; //in pixels
		int p_green_height;

		std::map<string, CvScalar> color;

};

#endif /* KFIELDGUI_H_ */

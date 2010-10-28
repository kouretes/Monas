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
#include "../../activities/Localization/KLocalization.h"
#include "stdio.h"

#define DELAY 5
#define SCALE 7.0

class KfieldGui {
	private:

		static pthread_mutex_t lock;

		CvFont font;
		static short Waitingforkey;

		const static int lineWidth = 1;
		static char wndname[10];
		char fieldfilename[100];

		IplImage* cleanfield;
		static IplImage *field;
		IplImage *field2;
		int thread_pid;
		string tmp;
		CvPoint mypospoint, mypospoint_old;
		CvPoint trackpospoint, trackpospoint_old;

	public:
		const static double hScale = 3.0 / SCALE;
		const static double vScale = 3.0 / SCALE;
		static CvScalar random_color();
		void KfieldInitTrackLine(partcl TrackPoint);
		void KfieldInitTrackLine(belief mypos);
		void addTrackLine(belief mypos);
		void addTrackLine(partcl TrackPoint);
		void DisplayErrors(float DistError, float RotError);

		void CleanField();
		void BackupField();
		void RevertBackupField();
		void DrawObservations(vector<KObservationModel> & currentObservation);
		int save_field_to_png(int save_field_to_file);
		void display_Gui();
		KfieldGui();
		KfieldGui(KLocalization & KLocObj);
		virtual ~KfieldGui();
		void display_belief(belief Belief, double maxrangeleft, double maxrangeright, int step);
		void display_Trackpoint(partcl Belief, double maxrangeleft, double maxrangeright);
		static void display_particles(parts & Particles, bool unnormilized = false);
		static void* display_field(void * fps);
		void make_field(IplImage** image);
		static short keypressed;
		static int GuiWaitKeyPress();
};

#endif /* KFIELDGUI_H_ */

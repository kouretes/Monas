#ifndef VISION_H
#define VISION_H
#include "KCameraTransformation.h"

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include "architecture/IActivity.h"
#include "hal/robot/generic_nao/KImageExtractor.h"
#include "KSegmentator.h"

#include "messages/VisionObservations.pb.h"
#include "messages/SensorsMessage.pb.h"
#include "messages/ObstacleAvoidanceMessage.pb.h"
#include "messages/Kimage.pb.h"
#include "messages/Gamecontroller.pb.h"
#include "messages/WorldInfo.pb.h"

#include "PracticalSocket.h"

#include <vector>
#include <iostream>
//#define DEBUGVISION


class Vision: public IActivity
{
	public:
		/**
		 * The only available constructor:
		 */
		Vision();

		void UserInit();
		void fetchAndProcess();
		int Execute();
		std::string GetName()
		{
			return "Vision";
		}

		typedef struct GoalPostdata
		{
				KVecInt2 ll, lr, tl, tr;//Corners
				KVecInt2 top, bot;
				//float angHeight;//in rads
				bool haveBot, haveTop;
				bool haveWidth;
				bool haveHeight;
				std::vector<measurement> dist, ber;
				measurement distBot, distTop;
				measurement bBot, bTop;
				int leftOrRight;

				measurement distance;
				measurement bearing;

				bool contains(KVecInt2 p) const
				{

					if (p.x + (lr.x - ll.x) < ll.x && p.x + (tr.x - tl.x) < tl.x)
						return false;
					if (p.x - (lr.x - ll.x) > lr.x && p.x - (tr.x - tl.x) > tr.x)
						return false;
					/*if(isLeft(ll,lr,p)>0)
					 return false;
					 cout<<"1111"<<endl;

					 */

					//cout<<"contains"<<endl;

					return true;

				}
				;
				static int isLeft(KVecInt2 s, KVecInt2 e, KVecInt2 t)
				{
					return (e.x - s.x) * (t.y - s.y) - (t.x - s.x) * (e.y - s.y);
				}
		} goalpostdata_t;

		typedef struct balldata
		{
				float x, y;
				float cr;
				measurement bearing;
				measurement distance;
				float ballradius;

		} balldata_t;

	private:
		struct
		{
				bool cvHighgui;
				int sensordelay;
				float Dfov;
				std::string SegmentationBottom, SegmentationTop;
				int scanV, scanH, minH, subsampling, bordersize, pixeltol;
				float skipdistance, seedistance, obstacledistance;

				float balltolerance, ballsize;
				float goalheight, goaldist, goaldiam, goalslopetolerance, widthestimateotolerance;

				float pitchoffset;

		} config;
		XMLConfig *xmlconfig;
		BallTrackMessage trckmsg;
		ObservationMessage obs;
		LedChangeMessage leds;
		//Incoming messages!
		boost::shared_ptr<const AllSensorValues> allsm;

		//Camera transformation matrix
		cpose p;//Robot pose


		//Extractor Object, to get a new image
		KImageExtractor ext;
		KCameraTranformation kinext;
		KSegmentator *seg;
		KSegmentator *segbottom;
		KSegmentator *segtop;
		int type;//Colorspace fourCC
		//Raw Input Image
		IplImage *rawImage;
		IplImage *segIpl;
		//Ball Detection related
		std::vector<KVecInt2> ballpixels;
		std::vector<KVecInt2> ygoalpost;
		std::vector<KVecInt2> bgoalpost;
		std::vector<KVecInt2> obstacles;
		KVecFloat2 Vup, Vdn, Vlt, Vrt;

		void loadXMLConfig(std::string fname);

		void gridScan(const KSegmentator::colormask_t color);

		bool calculateValidBall(balldata_t ball, KSegmentator::colormask_t c);
		bool calculateValidGoalPost(goalpostdata_t & goal, KSegmentator::colormask_t c);
		bool calculateValidGoalPostBase(const goalpostdata_t &goal, KSegmentator::colormask_t c);
		bool calculateValidGoalPostTop(goalpostdata_t &goal, KSegmentator::colormask_t c);

		balldata_t locateBall(std::vector<KVecInt2> cand);
		void publishObstacles(std::vector<KVecInt2> points);
		KVecFloat2 centerOfCircle(KVecFloat2 l, KVecFloat2 m, KVecFloat2 r);
		int locateGoalPost(std::vector<KVecInt2> cand, KSegmentator::colormask_t c);

		typedef struct traceresult_struct
		{
				KVecInt2 p;
				bool smartsuccess;
		} traceResult;

		traceResult traceline(KVecInt2 start, KVecInt2 vel, KSegmentator::colormask_t c);
		traceResult traceline(KVecInt2 start, KVecFloat2 vel, KSegmentator::colormask_t c);
		traceResult traceStrictline(KVecInt2 start, KVecFloat2 vel, KSegmentator::colormask_t c);
		traceResult traceBlobEdge(KVecInt2 start, KVecFloat2 vel, KSegmentator::colormask_t c);
		//Wrapper for seg object
		KSegmentator::colormask_t doSeg(int x, int y);
		inline bool validpixel(int x, int y);
		KVecFloat2 imageToCamera( KVecFloat2 const & imagep);
		KVecFloat2 imageToCamera( KVecInt2 const & imagep);
		KVecInt2 cameraToImage( KVecFloat2 const & c);

		void fillGoalPostHeightMeasurments(GoalPostdata & newpost);
		void fillGoalPostWidthMeasurments(GoalPostdata & newpost, KSegmentator::colormask_t c);
		//KVecFloat2 & cameraToObs(KMat::HCoords<float ,2> const& t);
		//KVecFloat2 & camToRobot(KMat::HCoords<float ,2> & t);
		KVecFloat2 camToRobot(KVecFloat2 const & t);
		void cvShowSegmented();

		//For Debug!
		static void * StartServer(void *s);
		pthread_t acceptthread;
		static TCPSocket *sock;
		void recv_and_send();
		KRawImage img;
		KRawImageHeader imgheader;
		//void Send_Image();
		int sendtype;
		static bool debugmode;

		header incommingheader;
		header outgoingheader;

		int size;
		char *data;

};

#endif

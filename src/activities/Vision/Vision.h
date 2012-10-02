#ifndef VISION_H
#define VISION_H

#include "architecture/executables/IActivity.h"
#include "KCameraTransformation.h"

#include <opencv/cv.h>
#include "KSegmentator.h"


#include "messages/Kimage.pb.h"
#include "messages/VisionObservations.pb.h"
#include "messages/SensorsMessage.pb.h"
#include "messages/ObstacleAvoidanceMessage.pb.h"

#include "messages/Gamecontroller.pb.h"
#include "messages/WorldInfo.pb.h"

//#define KPROFILING_ENABLED
#include "tools/profiler.hpp"

#include <vector>
#include <iostream>
//#define DEBUGVISION





ACTIVITY_START

class Vision: public IActivity
{
public:
	/**
	 * The only available constructor:
	 */
	ACTIVITY_VISIBLE Vision(Blackboard& b, XmlNode &x);
	ACTIVITY_VISIBLE ~Vision()
	{
		if(segtop == segbottom && segbottom != NULL)
			delete segbottom;
		else if(segbottom != NULL)
			delete segbottom;
		else if(segtop != NULL)
			delete segtop;
	};


	void ACTIVITY_VISIBLE UserInit();
	void ACTIVITY_VISIBLE Reset();
	void fetchAndProcess();
	int ACTIVITY_VISIBLE IEX_DIRECTIVE_HOT Execute();
	std::string ACTIVITY_VISIBLE  GetName()
	{
		return "Vision";
	};

	struct goalpostdata_t
	{
		KVecFloat2 ll, lr, tl, tr;//Corners
		KVecFloat2 top, bot;
		//float angHeight;//in rads
		bool haveBot, haveTop;
		bool haveWidth;
		bool haveHeight;
		std::vector<measurement> dist, ber;

		measurement distBot, distTop;
		measurement distHeight;
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
		};

	} ;

	struct balldata_t
	{
		float x, y;
		float cr;
		measurement bearing;
		measurement distance;
		float ballradius;

	};

private:
	struct
	{
		int sensordelay;
		float Dfov;
		float cameraGamma;
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
	mutable KProfiling::profiler vprof;
#ifdef DEBUGVISION
	void nullCall();
#endif
	//Incoming messages!
	boost::shared_ptr<const AllSensorValuesMessage> asvmo, asvmn; //Older and newer than requested timestamp
	boost::posix_time::ptime timeo, timen, stamp; //time of earlier sensors, later, and the image itself

	//Camera transformation matrix
	cpose p;//Robot pose


	//Extractor Object, to get a new image
	KCameraTranformation kinext;
	KSegmentator *seg;
	KSegmentator *segbottom;
	KSegmentator *segtop;

	//Raw Input Image
	KImageConst rawImage;
	//Ball Detection related
	std::vector<KVecInt2> ballpixels;
	std::vector<KVecInt2> ygoalpost;
	std::vector<KVecInt2> bgoalpost;
	std::vector<KVecInt2> obstacles;
	std::vector<KVecInt2> tobeshown;
	KVecFloat2 Vup, Vdn, Vlt, Vrt;

	void loadXMLConfig(std::string fname);

	void gridScan(const KSegmentator::colormask_t color);

	bool calculateValidBall(balldata_t const ball, KSegmentator::colormask_t c) const ;
	bool calculateValidGoalPost(goalpostdata_t & goal, KSegmentator::colormask_t c) const ;
	bool calculateValidGoalPostBase(const goalpostdata_t &goal, KSegmentator::colormask_t c) const;
	bool calculateValidGoalPostTop(goalpostdata_t &goal, KSegmentator::colormask_t c) const ;

	balldata_t locateBall(std::vector<KVecInt2> const& cand);
	void publishObstacles(std::vector<KVecInt2>  points) const;
	KVecFloat2 centerOfCircle(KVecFloat2 l, KVecFloat2 m, KVecFloat2 r) const ;
	int locateGoalPost(std::vector<KVecInt2> const& cand, KSegmentator::colormask_t c);

	typedef struct traceresult_struct
	{
		KVecInt2 p;
		bool smartsuccess;
	} traceResult;

	traceResult traceline(KVecInt2 const& start, KVecInt2 const&  vel, KSegmentator::colormask_t c) const ;
	traceResult traceline(KVecInt2 const& start, KVecFloat2 const& vel, KSegmentator::colormask_t c) const ;
	//traceResult traceStrictline(KVecInt2 start, KVecFloat2 vel, KSegmentator::colormask_t c) const;
	//traceResult traceBlobEdge(KVecInt2 const& start, KVecFloat2 const& vel, KSegmentator::colormask_t c) const ;
	//Wrapper for seg object
	KSegmentator::colormask_t doSeg(const int x, const int y, const KSegmentator::colormask_t h = 0xFF ) const;
	void prepSeg(const int x, const int y) const; //Prefetch

	inline bool validpixel(int x, int y) const;
	KVecFloat2 imageToCamera( KVecFloat2 const & imagep)const ;
	KVecFloat2 imageToCamera( KVecInt2 const & imagep) const;

	KVecFloat2 simpleRotation(KVecFloat2 const & i)const ;
	KVecFloat2 simpleRotation(KVecInt2 const & i)const ;


	KVecInt2 cameraToImage( KVecFloat2 const & c) const;
	KMath::KMat::GenMatrix<float, 2, 2> simpleRot;

	void fillGoalPostHeightMeasurments(goalpostdata_t & newpost) const;
	void fillGoalPostWidthMeasurments(goalpostdata_t & newpost, KSegmentator::colormask_t c) const;
	//KVecFloat2 & cameraToObs(KMat::HCoords<float ,2> const& t);
	//KVecFloat2 & camToRobot(KMat::HCoords<float ,2> & t);
	KVecFloat2 camToRobot(KVecFloat2 const & t) const;

};

ACTIVITY_END

#endif

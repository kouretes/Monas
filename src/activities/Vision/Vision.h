#ifndef VISION_H
#define VISION_H
#include "KCameraTransformation.h"

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "architecture/IActivity.h"
#include <iostream>
//#include "albrokermanager.h"
//#include "almemoryproxy.h"

#include "KImageExtractor.h"
#include "KSegmentator.h"

#include "architecture/narukom/pub_sub/publisher.h"
#include "architecture/narukom/pub_sub/subscriber.h"
#include "messages/VisionObservations.pb.h"
#include "messages/SensorsMessage.pb.h"
#include "messages/Gamecontroller.pb.h"
#include "messages/ObstacleAvoidanceMessage.pb.h"
#include "architecture/narukom/pub_sub/filters/type_filter.h"
#include "messages/Kimage.pb.h"
#include "messages/WorldInfo.pb.h"
#include "PracticalSocket.h"


#include <vector>

//#define DEBUGVISION


class Vision : public IActivity
{
public:
    /**
    * The only available constructor:
    */
    Vision() ;

    void UserInit();
    void fetchAndProcess();
    int Execute();
    std::string GetName() {
        return "Vision";
    }

	typedef struct GoalPostdata
    {
        CvPoint ll,lr,tl,tr;//Corners
        CvPoint	top,bot;
        //float angHeight;//in rads
        bool haveBot,haveTop;
        bool haveWidth;
        bool haveHeight;
        std::vector<measurement> dist,ber;
        measurement distBot,distTop;
        measurement bBot,bTop;
        int leftOrRight;

        measurement distance;
        measurement bearing;



        bool contains(CvPoint p) const
		{

			if(p.x+(lr.x-ll.x)<ll.x && p.x+(tr.x-tl.x)<tl.x)
				return false;
			if(p.x-(lr.x-ll.x)>lr.x && p.x-(tr.x-tl.x)>tr.x)
				return false;
			/*if(isLeft(ll,lr,p)>0)
				return false;
			cout<<"1111"<<endl;

			*/

			//cout<<"contains"<<endl;

			return true;

		};
		static int isLeft(CvPoint s, CvPoint e, CvPoint t)
		{
			 return (e.x - s.x) * (t.y - s.y)  - (t.x - s.x) * (e.y - s.y) ;
		}
    } goalpostdata_t;

	typedef struct balldata
    {
        float x,y;
        float cr;
        measurement bearing;
        measurement distance;
        float ballradius;

    } balldata_t;

private:
    //bool cvHighgui;//
    struct
    {
    	bool cvHighgui;
    	int sensordelay;
    	float Dfov;
    	std::string SegmentationBottom,SegmentationTop;
    	int scanstep,subsampling,borderskip;
    	float skipdistance,seedistance,obstacledistance;

    	float balltolerance,ballsize;
    	float goalheight,goaldist,goaldiam,goalslopetolerance,widthestimateotolerance;


    } config;
    XMLConfig *xmlconfig;
    BallTrackMessage trckmsg;
    ObservationMessage obs;
    LedChangeMessage leds;
    //Incoming messages!
    boost::shared_ptr<const InertialSensorsMessage>  im;
    boost::shared_ptr<const HeadJointSensorsMessage>  hm;
    //Camera transformation matrix
    cpose p;//Robot pose

    enum colors
    {
        bad = 0, red = 1, blue = 2, green = 3, skyblue = 4, yellow = 5, orange = 6, white = 7, black = 8
    };
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
    std::vector<CvPoint> ballpixels;
    std::vector<CvPoint> ygoalpost;
    std::vector<CvPoint> bgoalpost;
    std::vector<CvPoint> obstacles;
    CvPoint2D32f Vup,Vdn,Vlt,Vrt;

    void loadXMLConfig(std::string fname);

    void gridScan(const KSegmentator::colormask_t color);

    bool calculateValidBall(balldata_t ball, KSegmentator::colormask_t c);
    bool calculateValidGoalPost(goalpostdata_t & goal, KSegmentator::colormask_t c);
    bool calculateValidGoalPostBase(const goalpostdata_t &goal, KSegmentator::colormask_t c);
    bool calculateValidGoalPostTop(goalpostdata_t &goal, KSegmentator::colormask_t c);

    balldata_t locateBall(std::vector<CvPoint> cand);
    void publishObstacles(std::vector<CvPoint> points);
    CvPoint2D32f centerOfCircle(CvPoint l, CvPoint m, CvPoint r);
    int locateGoalPost(std::vector<CvPoint> cand, KSegmentator::colormask_t c);

    typedef struct traceresult_struct
    {
    	CvPoint p;
    	bool smartsuccess;
    } traceResult;

    traceResult traceline(CvPoint start, CvPoint vel, KSegmentator::colormask_t c);
	traceResult traceline(CvPoint start, CvPoint2D32f vel, KSegmentator::colormask_t c);
	traceResult traceStrictline(CvPoint start, CvPoint2D32f vel, KSegmentator::colormask_t c);
	traceResult traceBlobEdge(CvPoint start, CvPoint2D32f vel, KSegmentator::colormask_t c);
    //Wrapper for seg object
    KSegmentator::colormask_t doSeg(int x, int y);
    inline bool validpixel(int x,int y);
    KMat::HCoords<float,2>  imageToCamera( const KMat::HCoords<float,2>  & imagep);
    KMat::HCoords<int,2>  cameraToImage( const KMat::HCoords<float,2>  & c);

	void fillGoalPostHeightMeasurments(GoalPostdata & newpost);
	void fillGoalPostWidthMeasurments(GoalPostdata & newpost, KSegmentator::colormask_t c);
    //KMat::HCoords<float,2> & cameraToObs(KMat::HCoords<float ,2> const& t);
    //KMat::HCoords<float,2> & camToRobot(KMat::HCoords<float ,2> & t);
    KMat::HCoords<float,2>  camToRobot(KMat::HCoords<float ,2> & t);
    void cvShowSegmented();


    //For Debug!
	static void * StartServer(void * kati);
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

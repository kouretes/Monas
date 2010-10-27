#ifndef VISION_H
#define VISION_H
#include <albroker.h>
#include "alptr.h"
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
//#define DEBUGVISION

#include <vector>




class Vision : public IActivity
{
public:
    /**
    * The only available constructor: I need:
    * albroker: only to pass along to the KImageExtractor
    * MessageQueue : a messageQueue to deliver messages to
    * gui= enable debug gui? On the robot all cvhighigui calls should fail, so default is false
    */
    Vision() ;

    void UserInit();
    void testrun();
    int Execute();
    std::string GetName() {
        return "Vision";
    }

     typedef struct GoalPostdata
    {
        CvPoint ll;//Corners
        CvPoint lr;//Corners
        CvPoint bottom;
        CvPoint top;
        int height;//in pixels
        float d;//Distance
        float conf;
    } goalpostdata_t;
private:
    //bool cvHighgui;//
    XMLConfig *config;
    BallTrackMessage trckmsg;
    ObservationMessage obs;
    LedChangeMessage leds;
    //Incoming messages!
    boost::shared_ptr<const InertialSensorsMessage>  im;
    boost::shared_ptr<const HeadJointSensorsMessage>  hm;
    //Camera transformation matrix
    cpose p;//Robot pose

    //AL::ALPtr<AL::ALMemoryProxy> memory;

    //Ball Detection related
    typedef struct PointProjection
    {
        float distance;
        float bearing;
    } pointprojection_t;

    typedef struct balldata
    {
        float x,y;
        float cr;
        measurement bearing;
        measurement distance;
        float ballradius;

    } balldata_t;



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
    std::vector<CvPoint>  obstacles;

    void gridScan(const KSegmentator::colormask_t color);

    bool calculateValidBall(balldata_t ball, KSegmentator::colormask_t c);
    bool calculateValidGoalPost(goalpostdata_t goal, KSegmentator::colormask_t c);
    balldata_t locateBall(std::vector<CvPoint> cand);
    void publishObstacles(std::vector<CvPoint> points);
    goalpostdata_t locateGoalPost(std::vector<CvPoint> cand, KSegmentator::colormask_t c);
    CvPoint traceline(CvPoint start, CvPoint vel, KSegmentator::colormask_t c);
    //Wrapper for seg object
    KSegmentator::colormask_t doSeg(int x, int y);
    inline bool validpixel(int x,int y);
    KMat::HCoords<float,2>  imageToCamera( const KMat::HCoords<int,2>  & imagep);
    KMat::HCoords<int,2>  cameraToImage( const KMat::HCoords<float,2>  & c);
    //KMat::HCoords<float,2> & cameraToObs(KMat::HCoords<float ,2> const& t);
    //KMat::HCoords<float,2> & camToRobot(KMat::HCoords<float ,2> & t);
    KMat::HCoords<float,2>  camToRobot(KMat::HCoords<float ,2> & t);
    TypeFilter* type_filter;
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
